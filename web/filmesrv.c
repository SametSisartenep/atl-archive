#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#include <ctype.h>

#define nil NULL
typedef unsigned int uint;
typedef long long vlong;
typedef unsigned long long uvlong;

enum {
	Sok		= 200,
	Sbadreq		= 400,
	Snotfound	= 404,
	Snotimple	= 501,
};
char *statusmsg[] = {
 [Sok]		"OK",
 [Sbadreq]	"Bad Request",
 [Snotfound]	"Not Found",
 [Snotimple]	"Not Implemented",
};

typedef struct Req Req;
typedef struct Res Res;
typedef struct HField HField;

struct Req {
	char *method, *target, *version;
	HField *fields;
};

struct Res {
	char *version;
	int status;
	HField *fields;
};

struct HField {
	char *key, *value;
	HField *next;
};

char *httpver = "HTTP/1.1";
char *srvname = "filmoteca";
char msgerr[] = "<!doctype html>"
	"<html>"
	"<head><title>ERROR</title></head>"
	"<body><h1>NO MOVIES HERE</h1></body>"
	"</html>";
char msghead[] = "<!doctype html>"
	"<html>"
	"<head><title>Filmoteca</title></head>"
	"<body><h1>Filmoteca</h1>";
char msgfeet[] = "</body></html>";
char wdir[] = "/home/pi/films";
Req *req;
Res *res;

/* a crappy mimic of the original */
void
sysfatal(char *s)
{
	perror(s);
	exit(1);
}

void *
emalloc(long n)
{
	void *p;

	p = malloc(n);
	if(p == nil)
		sysfatal("malloc");
	memset(p, 0, n);
	return p;
}

int
urldecode(char *url, char *out, long n)
{
	char *o;
	char *ep = url+n;
	int c;

	for(o = out; url <= ep; o++){
		c = *url++;
		if(c == '%' &&
		   (!isxdigit(*url++) ||
		    !isxdigit(*url++) ||
		    !sscanf(url-2, "%2x", &c)))
			return -1;
		if(out)
			*o = c;
	}
	return o - out;
}

void
mimetype(int fd, char *mime, long len)
{
	char m[256];
	int pf[2];
	char *argv[] = {
		"sh", "-c",
		"file -i - | sed 's/^.*:\\s*//' | tr -d '\\n'",
		nil,
	};

	memset(m, 0, sizeof m);
	if(pipe(pf) < 0)
		sysfatal("pipe");
	switch(fork()){
	case -1: sysfatal("fork");
	case 0:
		close(pf[0]);
		dup2(fd, 0);
		dup2(pf[1], 1);
		close(pf[1]);
		execv("/bin/sh", argv);
		sysfatal("execl");
	default:
		close(pf[1]);
		read(pf[0], m, sizeof m);
		close(pf[0]);
		if(strncmp(m, "audio", 5) == 0)
			strncpy(m, "video", 5);
		strncpy(mime, m, len);
		wait(nil);
	}
}

HField *
allochdr(char *k, char *v)
{
	HField *h;

	h = emalloc(sizeof(HField));
	h->key = strdup(k);
	h->value = strdup(v);
	return h;
}

void
freehdr(HField *h)
{
	HField *hn;

	while(h != nil){
		hn = h->next;
		free(h->value);
		free(h->key);
		free(h);
		h = hn;
	}
}

void
inserthdr(HField **h, char *k, char *v)
{
	while(*h != nil)
		h = &(*h)->next;
	*h = allochdr(k, v);
}

char *
lookuphdr(HField *h, char *k)
{
	while(h != nil){
		if(strcmp(h->key, k) == 0)
			return h->value;
		h = h->next;
	}
	return nil;
}

Req *
allocreq(char *meth, char *targ, char *vers)
{
	Req *r;

	r = emalloc(sizeof(Req));
	r->method = strdup(meth);
	r->target = strdup(targ);
	r->version = strdup(vers);
	return r;
}

void
freereq(Req *r)
{
	freehdr(r->fields);
	free(r->version);
	free(r->target);
	free(r->method);
	free(r);
}

Res *
allocres(int sc)
{
	Res *r;

	r = emalloc(sizeof(Res));
	r->version = strdup(httpver);
	r->status = sc;
	inserthdr(&r->fields, "Server", srvname);
	return r;
}

void
freeres(Res *r)
{
	freehdr(r->fields);
	free(r->version);
	free(r);
}

int
hprint(char *fmt, ...)
{
	va_list ap;
	int rc;

	va_start(ap, fmt);
	rc = vprintf(fmt, ap);
	rc += printf("\r\n");
	va_end(ap);
	return rc;
}

void
hstline(int sc)
{
	hprint("%s %d %s", httpver, sc, statusmsg[sc]);
}

void
hparsereq(void)
{
	char *line, *meth, *targ, *vers, *k, *v;
	uint n, linelen;

	n = getline(&line, &linelen, stdin);
	meth = strtok(line, " ");
	targ = strtok(nil, " ");
	vers = strtok(nil, " \r");
	req = allocreq(meth, targ, vers);
	while((n = getline(&line, &linelen, stdin)) > 0){
		if(strcmp(line, "\r\n") == 0)
			break;
		k = strtok(line, ": ");
		v = strtok(nil, " \r");
		inserthdr(&req->fields, k, v);
	}
}

void
hprinthdr(void)
{
	HField *hp;

	hstline(res->status);
	for(hp = res->fields; hp != nil; hp = hp->next)
		hprint("%s: %s", hp->key, hp->value);
	hprint("");
	fflush(stdout);
}

void
hfail(int sc)
{
	char clen[16];

	res = allocres(sc);
	snprintf(clen, sizeof clen, "%u", strlen(msgerr));
	inserthdr(&res->fields, "Content-Type", "text/html; charset=utf-8");
	inserthdr(&res->fields, "Content-Length", clen);
	hprinthdr();
	hprint("%s", msgerr);
	hprint("");
	exit(0);
}

int
main()
{
	DIR *d;
	struct dirent *dir;
	struct stat fst;
	char buf[8*1024], mime[256], path[512], *s, crstr[6+3*16+1+1+1], clstr[16];
	int f;
	uvlong brange[2], n, clen;

	n = clen = 0;
	hparsereq();
	if(strcmp(req->method, "GET") != 0 && strcmp(req->method, "HEAD") != 0)
		hfail(Snotimple);
	if(strcmp(req->version, httpver) != 0)
		hfail(Snotimple);
	snprintf(path, sizeof path, "%s%s", wdir, req->target);
	if(urldecode(path, path, strlen(path)) < 0)
		sysfatal("urldecode");
	stat(path, &fst);
	if(S_ISREG(fst.st_mode)){
		f = open(path, O_RDONLY);
		if(f < 0)
			sysfatal("open");
		mimetype(f, mime, sizeof mime);
		res = allocres(Sok);
		inserthdr(&res->fields, "Accept-Ranges", "bytes");
		inserthdr(&res->fields, "Content-Type", mime);
		clen = fst.st_size;
		if((s = lookuphdr(req->fields, "Range")) != nil){
			while(!isdigit(*s++))
				;
			brange[0] = strtol(s, &s, 0);
			if(*s++ != '-')
				hfail(Sbadreq);
			if(!isdigit(*s))
				brange[1] = fst.st_size-1;
			else
				brange[1] = strtol(s, &s, 0);
			if(brange[0] > brange[1] || brange[1] >= fst.st_size)
				hfail(Sbadreq);
			lseek(f, brange[0], SEEK_SET);
			clen = brange[1]-brange[0]+1;
			snprintf(crstr, sizeof crstr, "bytes %llu-%llu/%llu",
				brange[0], brange[1], fst.st_size);
			inserthdr(&res->fields, "Content-Range", crstr);
		}
		snprintf(clstr, sizeof clstr, "%llu", clen);
		inserthdr(&res->fields, "Content-Length", clstr);
		if((s = lookuphdr(req->fields, "Connection")) != nil &&
		    strcmp(s, "close") == 0)
			inserthdr(&res->fields, "Connection", s);
		hprinthdr();
		if(strcmp(req->method, "HEAD") == 0){
			close(f);
			exit(0);
		}
		while(clen -= n, (n = read(f, buf, sizeof buf)) > 0 && clen > 0)
			write(1, buf, n);
		close(f);
		goto end;
	}
	d = opendir(path);
	if(d == nil)
		hfail(Snotfound);
	clen = 0;
	clen += strlen(msghead)+4;
	while((dir = readdir(d)) != nil)
		if(strcmp(dir->d_name, ".") != 0 &&
		   strcmp(dir->d_name, "..") != 0 &&
		   dir->d_type & DT_DIR|DT_LNK)
			clen += 4+16+strlen(req->target)+2*strlen(dir->d_name)+5;
	clen += 5+strlen(msgfeet);
	snprintf(clstr, sizeof clstr, "%llu", clen);
	res = allocres(Sok);
	inserthdr(&res->fields, "Content-Type", "text/html; charset=utf-8");
	inserthdr(&res->fields, "Content-Length", clstr);
	hprinthdr();
	if(strcmp(req->method, "HEAD") == 0)
		exit(0);
	printf(msghead);
	printf("<ul>");
	rewinddir(d);
	while((dir = readdir(d)) != nil)
		if(strcmp(dir->d_name, ".") != 0 &&
		   strcmp(dir->d_name, "..") != 0 &&
		   dir->d_type & DT_DIR|DT_LNK)
			printf("<li><a href=\"%s/%s\">%s</a></li>",
				strcmp(req->target, "/") ? req->target : "",
				dir->d_name, dir->d_name);
	printf("</ul>");
	printf(msgfeet);
end:
	hprint("");
	exit(0);
}
