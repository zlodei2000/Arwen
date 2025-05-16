//
// Bounding sphere class
//
// Author: Alex V. Boreskoff
//

#ifndef	__BOUNDING_SPHERE__
#define	__BOUNDING_SPHERE__

#include	"Vector3D.h"
#include	"Plane.h"

class	BoundingSphere
{
private:
	Vector3D	center;
	float		radius;
	float		radiusSq;
public:
	BoundingSphere ( const Vector3D& c, float r ) : center ( c ), radius ( r ) 
	{
		radiusSq = r * r;
	}

	BoundingSphere ( const Vector3D * v, int n );

	const Vector3D&	getCenter () const
	{
		return center;
	}

	float	getRadius () const
	{
		return radius;
	}

	void	addPoint ( const Vector3D& p )
	{
		float	rSq = (p-center).lengthSq ();

		if ( rSq > radiusSq )
		{
			radiusSq = rSq;
			radius   = (float) sqrt ( radiusSq );
		}
	}

	void	merge ( const BoundingSphere& s );

	bool	contains ( const Vector3D& p ) const
	{
		return (p-center).lengthSq () <= radiusSq;
	}

	bool	contains ( const BoundingSphere& s ) const
	{
		return (s.center - center).length () + s.radius <= radius;
	}

	bool	intersects ( const BoundingSphere& s ) const
	{
		return (center - s.center).lengthSq () <= (radius + s.radius) * (radius + s.radius);
	}

	int	classify ( const Plane& plane ) const
	{
		float	v = plane.signedDistanceTo ( center );

		if ( v > radius || v < -radius )
			return plane.classify ( center );

		return IN_BOTH;
	}
};

#endif
