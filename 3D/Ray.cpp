//
// Ray class.
//
// Author: Alex V. Boreskoff
//
// Last modified: 16/07/2002
//

#include	"Ray.h"
#include	"Plane.h"
#include	"Transform3D.h"

float Ray :: intersect ( const Plane& plane ) const
{
	float	denom = dir & plane.n;

	if ( fabs ( denom ) < EPS )
		return -1.0f;

	return - ( plane.dist + (org & plane.n) ) / denom;
}

void	Ray :: transform ( const Transform3D& transf )
{
	org = transf.transformPoint( org );
	dir = transf.getLinearPart () * dir;		// XXX: may be not normalized
}
