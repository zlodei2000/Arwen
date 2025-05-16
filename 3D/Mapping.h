//
// Affine texture mapping
//
// Author: Alex V. Boreskoff
//
// Last modified: 23/05/2003
//

#ifndef	__MAPPING__
#define	__MAPPING__

#include	"Vector2D.h"
#include	"Vector3D.h"

class	Mapping
{
private:
	Vector3D	uAxis;
	Vector3D	vAxis;
	Vector3D	dir;				// direction of projection
	float		uOffs;
	float		vOffs;
	float		inv [2][2];			// matrix to unmap from 2D to 3D

public:
	Mapping ( const Vector3D& u, float uOffset, const Vector3D& v, float vOffset )
	{
		float	uSq = (u & u);
		float	vSq = (v & v);
		float	uv  = (u & v);
		float	det = uSq * vSq - uv * uv;

		uAxis = u;
		uOffs = uOffset;
		vAxis = v;
		vOffs = vOffset;
		dir   = uAxis ^ vAxis;

		dir.normalize ();

		inv [0][0] = vSq / det;
		inv [0][1] = -uv / det;
		inv [1][0] = inv [0][1];
		inv [1][1] = uSq / det;
	}

	Vector2D	map ( const Vector3D& pos ) const
	{
		return Vector2D ( uOffs + (pos & uAxis), vOffs + (pos & vAxis) );
	}

	Vector3D	unmap ( const Vector2D& tex, const Plane& plane ) const
	{
		Vector3D	pt ( ( (tex.x-uOffs)*inv [0][0] + (tex.y-vOffs)*inv [0][1]) * uAxis +
			             ( (tex.x-uOffs)*inv [1][0] + (tex.y-vOffs)*inv [1][1]) * vAxis );

		float	dot = dir & plane.n;

		if ( fabs ( dot ) > EPS )
			pt -= dir * (plane.dist + (pt & plane.n)) / dot;

		return pt;
	}
};

#endif

