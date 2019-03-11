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
#include <errno.h>

#define nil NULL
typedef unsigned int uint;
typedef long long vlong;
typedef unsigned long long uvlong;

enum {
	Sok		= 200,
	Spartial	= 206,
	Sbadreq		= 400,
	Sforbid		= 403,
	Snotfound	= 404,
	Snotrange	= 416,
	Sinternal	= 500,
	Snotimple	= 501,
	Swrongver	= 505,
};
char *statusmsg[] = {
 [Sok]		"OK",
 [Spartial]	"Partial Content",
 [Sbadreq]	"Bad Request",
 [Sforbid]	"Forbidden",
 [Snotfound]	"Not Found",
 [Snotrange]	"Range Not Satisfiable",
 [Sinternal]	"Internal Server Error",
 [Snotimple]	"Not Implemented",
 [Swrongver]	"HTTP Version Not Supported",
};

typedef struct Req Req;
typedef struct Res Res;
typedef struct HField HField;

struct Req {
	char *method, *target, *version;
	HField *fields;
};

struct Res {
	int status;
	HField *fields;
};

struct HField {
	char *key, *value;
	HField *next;
};

char httpver[] = "HTTP/1.1";
char srvname[] = "filmoteca";
char errmsg[] = "NO MOVIES HERE";
char listhead[] = "<!doctype html>\n<html>\n<head>\n"
	"<link rel=\"stylesheet\" href=\"/style\" media=\"all\" type=\"text/css\"/>\n"
	"<title>Filmoteca</title>\n"
	"</head>\n<body>\n"
	"<h1>Filmoteca</h1>\n";
char listfeet[] = "</body>\n</html>\n";
char portalhead[] = "<!doctype html>\n<html>\n<head>\n"
	"<link rel=\"stylesheet\" href=\"/style\" media=\"all\" type=\"text/css\"/>\n"
	"<title>Filmoteca - %s</title>\n"
	"</head>\n<body>\n"
	"<h1>%s</h1>\n";
char portalbody[] = "<a href=\"%s/cover\"><img id=\"cover\" src=\"%s/cover\"/></a>\n"
	"<table>\n"
	"\t<tr>\n"
	"\t\t<td>Release</td><td>%s</td>\n"
	"\t</tr>\n"
	"\t<tr>\n"
	"\t\t<td>Stream</td><td><a href=\"%s/video\">link</a></td>\n"
	"\t</tr>\n"
	"</table>\n";
char portalfeet[] = "</body>\n</html>\n";
char stylepath[] = "/home/pi/lib/film/style.css";
char fvicopath[] = "/home/pi/lib/film/favicon.ico";
char wdir[] = "/home/pi/films";
char **dirlist;
int ndir;
Req *req;
Res *res;

void hfatal(char *);

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
		hfatal("malloc");
	memset(p, 0, n);
	return p;
}

void *
erealloc(void *ptr, long n)
{
	void *p;

	p = realloc(ptr, n);
	if(p == nil)
		hfatal("realloc");
	return p;
}

int
urldecode(char *url, char *out, long n)
{
	char *o, *ep;
	int c;

	ep = url+n;
	for(o = out; url <= ep; o++){
		c = *url++;
		if(c == '%' &&
		   (!isxdigit(*url++) ||
		    !isxdigit(*url++) ||
		    !sscanf(url-2, "%2x", &c)))
			return -1;
		*o = c;
	}
	return o - out;
}

int
mimetype(int fd, char *mime, long len)
{
	char m[256];
	uvlong n;
	int pf[2];
	char *argv[] = {
		"sh", "-c",
		"file -i - | sed 's/^.*:\\s*//' | tr -d '\\n'",
		nil,
	};

	memset(m, 0, sizeof m);
	if(pipe(pf) < 0)
		return -1;
	switch(fork()){
	case -1: return -1;
	case 0:
		close(pf[0]);
		dup2(fd, 0);
		dup2(pf[1], 1);
		close(pf[1]);
		execv("/bin/sh", argv);
		sysfatal("execl");
	default:
		close(pf[1]);
		if((n = read(pf[0], m, sizeof(m)-1)) < 0)
			return -1;
		close(pf[0]);
		if(strcmp(req->target, "/style") == 0)
			strncpy(m, "text/css; charset=utf-8", sizeof(m)-1);
		/* file(1) is not that good at guessing. */
		if(strncmp(m, "audio", 5) == 0)
			strncpy(m, "video", 5);
		strncpy(mime, m, len);
		wait(nil);
	}
	return 0;
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
	r->status = sc;
	inserthdr(&r->fields, "Server", srvname);
	return r;
}

void
freeres(Res *r)
{
	freehdr(r->fields);
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
	snprintf(clen, sizeof clen, "%u", strlen(errmsg));
	inserthdr(&res->fields, "Content-Type", "text/plain; charset=utf-8");
	inserthdr(&res->fields, "Content-Length", clen);
	hprinthdr();
	hprint("%s", errmsg);
	hprint("");
	exit(0);
}

void
hfatal(char *ctx)
{
	hstline(Sinternal);
	hprint("Content-Type: %s", "text/plain; charset=utf-8");
	hprint("Content-Length: %u", strlen(errmsg));
	hprint("");
	hprint("%s", errmsg);
	hprint("");
	fflush(stdout);
	sysfatal(ctx);
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
	if(meth == nil || targ == nil || vers == nil)
		hfail(Sbadreq);
	if(targ[strlen(targ)-1] == '/')
		targ[strlen(targ)-1] = 0;
	req = allocreq(meth, targ, vers);
	while((n = getline(&line, &linelen, stdin)) > 0){
		if(strcmp(line, "\r\n") == 0)
			break;
		k = strtok(line, ": ");
		v = strtok(nil, " \r");
		if(k == nil || v == nil)
			hfail(Sbadreq);
		inserthdr(&req->fields, k, v);
	}
}

void
sendfile(char *path, struct stat *fst)
{
	char buf[8*1024], mime[256], *s, crstr[6+3*16+1+1+1], clstr[16];
	uvlong brange[2], n, clen;
	int f;

	n = clen = 0;
	f = open(path, O_RDONLY);
	if(f < 0)
		hfatal("sendfile: open");
	if(mimetype(f, mime, sizeof(mime)-1) < 0)
		hfatal("sendfile: mimetype");
	mime[strlen(mime)] = 0;
	clen = fst->st_size;
	if((s = lookuphdr(req->fields, "Range")) != nil){
		while(!isdigit(*++s) && *s != 0)
			;
		if(*s == 0)
			hfail(Sbadreq);
		brange[0] = strtol(s, &s, 0);
		if(*s++ != '-')
			hfail(Sbadreq);
		if(!isdigit(*s))
			brange[1] = fst->st_size-1;
		else
			brange[1] = strtol(s, &s, 0);
		if(brange[0] > brange[1] || brange[1] >= fst->st_size){
			res = allocres(Snotrange);
			snprintf(crstr, sizeof crstr, "bytes */%llu",
				fst->st_size);
		}else{
			res = allocres(Spartial);
			lseek(f, brange[0], SEEK_SET);
			clen = brange[1]-brange[0]+1;
			snprintf(crstr, sizeof crstr, "bytes %llu-%llu/%llu",
				brange[0], brange[1], fst->st_size);
		}
		inserthdr(&res->fields, "Content-Range", crstr);
	}else
		res = allocres(Sok);
	inserthdr(&res->fields, "Accept-Ranges", "bytes");
	inserthdr(&res->fields, "Content-Type", mime);
	snprintf(clstr, sizeof clstr, "%llu", clen);
	inserthdr(&res->fields, "Content-Length", clstr);
	if((s = lookuphdr(req->fields, "Connection")) != nil)
		inserthdr(&res->fields, "Connection", s);
	hprinthdr();
	if(strcmp(req->method, "HEAD") == 0)
		goto EOT;
	while(clen -= n, (n = read(f, buf, sizeof buf)) > 0 && clen > 0)
		if(write(1, buf, n) <= 0)
			break;
EOT:
	close(f);
}

void
sendlist(char *path)
{
	DIR *d;
	struct dirent *dir;
	char clstr[16];
	uvlong clen;
	int i;

	clen = 0;
	d = opendir(path);
	if(d == nil)
		hfatal("sendlist: opendir");
	clen = 0;
	clen += strlen(listhead)+5;
	while((dir = readdir(d)) != nil)
		if(strcmp(dir->d_name, ".") != 0 &&
		   strcmp(dir->d_name, "..") != 0){
			dirlist = erealloc(dirlist, ++ndir*sizeof(char *));
			dirlist[ndir-1] = strdup(dir->d_name);
			clen += 4+16+strlen(req->target)+2*strlen(dir->d_name)+5+1;
		}
	clen += 6+strlen(listfeet);
	snprintf(clstr, sizeof clstr, "%llu", clen);
	res = allocres(Sok);
	inserthdr(&res->fields, "Content-Type", "text/html; charset=utf-8");
	inserthdr(&res->fields, "Content-Length", clstr);
	hprinthdr();
	if(strcmp(req->method, "HEAD") == 0)
		return;
	printf(listhead);
	printf("<ul>\n");
	rewinddir(d);
	for(i = 0; i < ndir; i++)
		printf("<li><a href=\"%s/%s\">%s</a></li>\n",
			strcmp(req->target, "/") == 0 ? "" : req->target,
			dirlist[i], dirlist[i]);
	printf("</ul>\n");
	printf(listfeet);
	hprint("");
}

void
sendportal(char *path)
{
	char clstr[16], *title, infopath[512], date[16];
	uvlong n, clen;
	int f;

	clen = 0;
	memset(infopath, 0, sizeof infopath);
	title = strrchr(path, '/');
	if(*++title == 0)
		hfail(Sbadreq);
	snprintf(infopath, sizeof(infopath)-1, "%s/%s", path, "release");
	f = open(infopath, O_RDONLY);
	if(f < 0)
		hfatal("sendportal: open");
	if((n = read(f, date, sizeof(date)-1)) < 0)
		hfatal("sendportal: read");
	date[strlen(date)] = 0;
	close(f);
	clen += strlen(portalhead) + 2*strlen(title);
	clen += strlen(portalbody) + 2*strlen(req->target) +
		strlen(date) + 2*strlen(req->target);
	clen += strlen(portalfeet);
	snprintf(clstr, sizeof clstr, "%llu", clen);
	res = allocres(Sok);
	inserthdr(&res->fields, "Content-Type", "text/html; charset=utf-8");
	inserthdr(&res->fields, "Content-Length", clstr);
	hprinthdr();
	if(strcmp(req->method, "HEAD") == 0)
		return;
	printf(portalhead, title, title);
	printf(portalbody, req->target, req->target, date, req->target);
	printf(portalfeet);
	hprint("");
}

int
main()
{
	struct stat fst;
	char path[512];

	memset(path, 0, sizeof path);
	hparsereq();
	if(strcmp(req->method, "GET") != 0 && strcmp(req->method, "HEAD") != 0)
		hfail(Snotimple);
	if(strcmp(req->version, httpver) != 0)
		hfail(Swrongver);
	if(strcmp(req->target, "/style") == 0)
		strncpy(path, stylepath, sizeof(path)-1);
	else if(strcmp(req->target, "/favicon.ico") == 0)
		strncpy(path, fvicopath, sizeof(path)-1);
	else
		snprintf(path, sizeof(path)-1, "%s%s", wdir, req->target);
	if(urldecode(path, path, strlen(path)) < 0)
		hfail(Sbadreq);
	if(stat(path, &fst) < 0)
		switch(errno){
		case EACCES: hfail(Sforbid);
		case ENOENT: hfail(Snotfound);
		default: hfatal("stat");
		}
	if(S_ISREG(fst.st_mode))
		sendfile(path, &fst);
	else if(strstr(req->target+1, "/") == nil)
		sendlist(path);
	else
		sendportal(path);
	exit(0);
}
