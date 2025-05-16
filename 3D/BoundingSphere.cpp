//
// Bounding sphere class
//
// Author: Alex V. Boreskoff
//

#include	"BoundingSphere.h"

BoundingSphere :: BoundingSphere ( const Vector3D * v, int n )
{
	center = Vector3D ( 0, 0, 0 );

	for ( int i = 0; i < n; i++ )
		center += v [i];

	center  /= (float)n;
	radiusSq = (center - v [0]).lengthSq ();

	for ( int i = 1; i < n; i++ )
	{
		float	rSq = (center - v [i]).lengthSq ();

		if ( rSq > radiusSq )
			radiusSq = rSq;
	}

	radius = (float) sqrt ( radiusSq );
}

void	BoundingSphere :: merge ( const BoundingSphere& s )
{
	Vector3D	dc ( s.center - center );
	float		lc ( dc.length () );
	float		dr ( s.radius - radius );

	float		r = 0.5f * (radius + s.radius + lc);
	Vector3D	c ( center + (0.5f * ( lc + dr ) / lc) * dc );

	center   = c;
	radius   = r;
	radiusSq = r * r;
}

