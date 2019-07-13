#include <u.h>
#include <libc.h>
#include "../geometry.h"

Point2
rframexform(Point2 p, ReferenceFrame rf)
{
	Matrix m = {
		rf.xb.x, rf.xb.y, -dotvec2(rf.xb, rf.p),
		rf.yb.x, rf.yb.y, -dotvec2(rf.yb, rf.p),
		0, 0, 1,
	};
	return xform(p, m);
}

Point3
rframexform3(Point3 p, ReferenceFrame3 rf)
{
	Matrix3 m = {
		rf.xb.x, rf.xb.y, rf.xb.z, -dotvec3(rf.xb, rf.p),
		rf.yb.x, rf.yb.y, rf.yb.z, -dotvec3(rf.yb, rf.p),
		rf.zb.x, rf.zb.y, rf.zb.z, -dotvec3(rf.zb, rf.p),
		0, 0, 0, 1,
	};
	return xform3(p, m);
}

Point3
hsrframexform3(Point3 p, ReferenceFrame3 rf)
{
	Matrix3 m;

	identity3(m);
	m[2][2] = -1;
	return xform3(xform3(rframexform3(p, rf), m), m);
}
