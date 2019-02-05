#include <u.h>
#include <libc.h>
#include <json.h>

typedef struct TargetInfo TargetInfo;

struct TargetInfo {
	char	*name;
	int	type;
};

void
usage(void)
{
	fprint(2, "usage: geoparse [jsonfile]\n");
	exits("usage");
}

char *
jsontypestr(int type)
{
	char *tab[] = {
	[JSONNull]	"null",
	[JSONBool]	"bool",
	[JSONNumber]	"number",
	[JSONString]	"string",
	[JSONArray]	"array",
	[JSONObject]	"object"
	};
	return tab[type];
}

TargetInfo ti[] = {
	{ "as", JSONString },
	{ "city", JSONString },
	{ "country", JSONString },
	{ "countryCode", JSONString },
	{ "isp", JSONString },
	{ "lat", JSONNumber },
	{ "lon", JSONNumber },
	{ "org", JSONString },
	{ "query", JSONString },
	{ "region", JSONString },
	{ "regionName", JSONString },
	{ "status", JSONString },
	{ "timezone", JSONString },
	{ "zip", JSONString }
};

void
main(int argc, char *argv[])
{
	char buf[1024];
	JSON *obj, *record;
	int fd, n;
	int i;

	if(argc < 2)
		fd = 0;
	else{
		fd = open(argv[1], OREAD);
		if(fd < 0)
			sysfatal("open: %r");
	}

	n = read(fd, buf, sizeof buf);
	if(n <= 0)
		sysfatal("read: %r");
	buf[n] = 0;

	obj = jsonparse(buf);
	if(!obj || obj->t != JSONObject)
		sysfatal("wrong input");

	for(i = 0; i < nelem(ti); i++){
		record = jsonbyname(obj, ti[i].name);
		if(record->t != ti[i].type)
			sysfatal("%s field not a %s\n",
				ti[i].name, jsontypestr(ti[i].type));
		if(ti[i].type == JSONString)
			print("%s=%s\n", ti[i].name, record->s);
		else
			print("%s=%g\n", ti[i].name, record->n);
	}

	jsonfree(obj);
	exits(0);
}
