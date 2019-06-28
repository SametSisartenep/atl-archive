#include <u.h>
#include <libc.h>
#include <ctype.h>
#include <bio.h>
#include <draw.h>
#include "dat.h"
#include "fns.h"

Vector3 *verts;
Triangle3 *tris;
int nvert, ntri;
int lineno;

int
objread(char *f, Triangle3 **mesh)
{
	Biobuf *bin;
	char *s;
	int idx;
	double *coord;
	Vector3 *vert;

	bin = Bopen(f, OREAD);
	if(bin == nil)
		sysfatal("Bopen: %r");
	while((s = Brdline(bin, '\n')) != nil){
		lineno++;
		if(*s == 'v'){
			verts = erealloc(verts, ++nvert*sizeof(Vector3));
			coord = (double *)(verts+nvert-1);
			while(*s != '\n' && (Vector3 *)coord-verts < nvert){
				while(isspace(*++s))
					;
				*coord++ = strtod(s, &s);
			}
			if(coord-(double *)(verts+nvert-1) < 3){
				werrstr("%s:%d insufficient coordinates", f, lineno);
				goto error;
			}
		}
		if(*s == 'f'){
			tris = erealloc(tris, ++ntri*sizeof(Triangle3));
			vert = (Vector3 *)(tris+ntri-1);
			while(*s != '\n' && (Triangle3 *)vert-tris < ntri){
				while(isspace(*++s))
					;
				idx = strtol(s, &s, 0);
				if(idx > nvert){
					werrstr("%s:%d insufficient vertices", f, lineno);
					goto error;
				}
				*vert++ = verts[idx-1];
			}
		}
	}
	*mesh = tris;
	free(verts);
	Bterm(bin);
	return ntri;
error:
	free(verts);
	free(tris);
	Bterm(bin);
	return -1;
}
