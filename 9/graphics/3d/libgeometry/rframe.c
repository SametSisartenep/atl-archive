#include <u.h>
#include <libc.h>
#include "../geometry.h"

Point3
rframexform(Point3 p, ReferenceFrame rf)
{
	Matrix3 m = {
		rf.xb.x, rf.xb.y, rf.xb.z, -dotvec3(rf.xb, rf.p),
		rf.yb.x, rf.yb.y, rf.yb.z, -dotvec3(rf.yb, rf.p),
		rf.zb.x, rf.zb.y, rf.zb.z, -dotvec3(rf.zb, rf.p),
		0, 0, 0, 1,
	};
	return xform3(p, m);
}
