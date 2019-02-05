#include <u.h>
#include <libc.h>
#include <bio.h>

typedef struct Coord Coord;
typedef struct BBox BBox;
typedef struct MFHeader MFHeader;
typedef struct Tag Tag;

struct Coord {
	double	lat;
	double	lon;
};

struct BBox {
	Coord	min;
	Coord	max;
};

struct Tag {
	int	key;
	char	*val;
};

struct MFHeader {
	char	magic[21];	/* magic sequence */
	uint	size;		/* header size */
	uint	fversion;	/* file version */
	uvlong	fsize;		/* file size */
	uvlong	dob;		/* date of creation */
	BBox	bbox;		/* map boundaries */
	ushort	tsize;		/* tile size */
	char	*projection;	/* Mercator or nothing */
	uchar	flags;
	Coord	pos0;		/* map initial position */
	uchar	zoom0;		/* map initial zoom level */
	char	*lang;		/* language preference */
	char	*comment;
	char	*author;
	Tag	*pois;		/* places of interest */
	Tag	*ways;		/* ways */
};

enum {
	Fdebug		= 0x80,
	Fpos0		= 0x40,
	Fzoom0		= 0x20,
	Flang		= 0x10,
	Fcomment	= 0x08,
	Fauthor		= 0x04
};

void
usage(void)
{
	fprint(2, "usage: info [mapfile]\n");
	exits("usage");
}

ushort
get16(uchar *a)
{
	return (a[0]<<8) | a[1];
}

uint
get32(uchar *a)
{
	return (a[0]<<24) | (a[1]<<16) | (a[2]<<8) | a[3];
}

uvlong
get64(uchar *a)
{
	uvlong n;

	n = get32(a);
	return n<<32 | get32(a+4);
}

uint
readuvarint(Biobuf *b)
{
	uint n, shift;
	uchar x;

	n = shift = 0;

	if(Bread(b, &x, 1) != 1)
		sysfatal("readuvarint: %r");
	while(x & 0x80){
		n |= (x & ~0x80) << shift;
		shift += 7;
		Bread(b, &x, 1);
	}
	return n |= x;
}

char *
readstr(Biobuf *b)
{
	uint len;
	char *s;

	len = readuvarint(b);
	s = malloc(len+1);
	if(Bread(b, s, len) != len)
		sysfatal("readstr: %r");
	s[len] = 0;

	return s;
}

double
todeg(long μd)
{
	double d;
	d = μd / 1000000.;
	return d;
}

Coord
getcoord(uchar *a)
{
	Coord c;
	uchar *s;

	s = a;
	c.lat = todeg(get32(s));
	s += 4;
	c.lon = todeg(get32(s));

	return c;
}

BBox
getbbox(uchar *a)
{
	BBox bbox;
	uchar *s;

	s = a;
	bbox.min = getcoord(s);
	s += 8;
	bbox.max = getcoord(s);

	return bbox;
}

Tag *
gettags(Biobuf *b)
{
	Tag *t;
	uchar x[2];
	int len, i;

	if(Bread(b, x, 2) != 2)
		sysfatal("gettags: %r");
	len = get16(x);
	t = malloc(len*sizeof(Tag)+1);
	for(i = 0; i < len; i++)
		t[i].val = readstr(b);
	t[len].val = nil;

	return t;
}

void
main(int argc, char *argv[])
{
	Biobuf bin;
	Tag *tp;
	uchar buf[512];
	int fd;

	MFHeader h;

	if(argc < 2)
		fd = 0;
	else{
		fd = open(argv[1], OREAD);
		if(fd < 0)
			sysfatal("open: %r");
	}

	if(Binit(&bin, fd, OREAD) < 0)
		sysfatal("Binit: %r");

	if(Bread(&bin, buf, 20) != 20)
		sysfatal("wrong magic value");
	strncpy(h.magic, (char *)buf, 20);
	h.magic[20] = 0;

	if(Bread(&bin, buf, 4) != 4)
		sysfatal("couldn't read header size");
	h.size = get32(buf);

	if(Bread(&bin, buf, 4) != 4)
		sysfatal("couldn't read file version");
	h.fversion = get32(buf);

	if(Bread(&bin, buf, 8) != 8)
		sysfatal("couldn't get file size");
	h.fsize = get64(buf);

	if(Bread(&bin, buf, 8) != 8)
		sysfatal("couldn't get date of creation");
	h.dob = get64(buf);

	if(Bread(&bin, buf, 16) != 16)
		sysfatal("couldn't get bbox");
	h.bbox = getbbox(buf);

	if(Bread(&bin, buf, 2) != 2)
		sysfatal("couldn't get tile size");
	h.tsize = get16(buf);

	h.projection = readstr(&bin);

	if(Bread(&bin, &h.flags, 1) != 1)
		sysfatal("couldn't get flags");
	if(h.flags & Fpos0){
		if(Bread(&bin, buf, 8) != 8)
			sysfatal("couldn't get pos0");
		h.pos0 = getcoord(buf);
	}
	if(h.flags & Fzoom0)
		Bread(&bin, &h.zoom0, 1);
	if(h.flags & Flang)
		h.lang = readstr(&bin);
	if(h.flags & Fcomment)
		h.comment = readstr(&bin);
	if(h.flags & Fauthor)
		h.author = readstr(&bin);
	h.pois = gettags(&bin);
	h.ways = gettags(&bin);

	Bterm(&bin);

	print("Mapsforge Header Information\n");
	print("magic: %s\n", h.magic);
	print("header size: %d bytes\n", h.size);
	print("version: %d\n", h.fversion);
	print("file size: %llud bytes\n", h.fsize);
	print("created: %s", ctime(h.dob));
	print("bbox: (%g°N, %g°E)(%g°N, %g°E)\n",
		h.bbox.min.lat, h.bbox.min.lon,
		h.bbox.max.lat, h.bbox.max.lon);
	print("tile size: %d\n", h.tsize);
	print("projection: %s\n", h.projection);
	print("flags:");
	if(h.flags & Fdebug) print(" debug");
	if(h.flags & Fpos0) print(" pos0");
	if(h.flags & Fzoom0) print(" zoom0");
	if(h.flags & Flang) print(" lang");
	if(h.flags & Fcomment) print(" comment");
	if(h.flags & Fauthor) print(" author");
	print("\n");
	if(h.flags & Fpos0) print("pos0: (%g°N, %g°E)\n", h.pos0.lat, h.pos0.lon);
	if(h.flags & Fzoom0) print("zoom0: %d\n", h.zoom0);
	if(h.flags & Flang) print("lang: %s\n", h.lang);
	if(h.flags & Fcomment) print("comment: %s\n", h.comment);
	if(h.flags & Fauthor) print("author: %s\n", h.author);
	print("pois:\n");
	for(tp = h.pois; tp->val != nil; tp++)
		print("\t%s\n", tp->val);
	print("ways:\n");
	for(tp = h.ways; tp->val != nil; tp++)
		print("\t%s\n", tp->val);

	close(fd);
	exits(0);
}
