//
// Class for fog with linear density
// 
//   density (p) = (grad, p) + offs
//
// Author: Alex V. Boreskoff
//
// Last modified: 14/11/2002
//

#include	"LinearFog.h"
#include	"Ray.h"
#include	"Plane.h"

MetaClass	LinearFog :: classInstance ( "LinearFog", &Fog :: classInstance );

float	LinearFog :: getOpacity ( const Vector3D& from, const Vector3D& to, Plane * clipPlane ) const
{
	Ray		ray ( from, to - from );			// ray to sample fog
	float	t1, t2;								// points of entering and leaving fog
	float	t;

	t1 = 0.0f;
	t2 = from.distanceTo ( to );

	if ( !traceRay ( ray, t1, t2 ) )
		return 0.0f;

												// now clip segment [t1, t2] to clipPlane
	if ( clipPlane != NULL && clipPlane -> intersectByRay ( ray.getOrigin (), ray.getDir (), t ) )
	{
		bool	clipFlag = clipPlane -> classify ( from ) == IN_FRONT;

		if ( clipFlag )							// ray is entering clipPlane
		{
			if ( t > t1 )
				t1 = t;
		}
		else
		{
			if ( t < t2 )
				t2 = t;
		}
	}

	float	dens1 = (ray.point ( t1 ) & grad) + offs;	// density at point t1
	float	dens2 = (ray.point ( t2 ) & grad) + offs;	// density at point t2
														// now integrate density using trapzoid rule
	float	opacity = 0.5f * (dens1 + dens2) * (t2 - t1);

	return (opacity > 1.0f ? 1.0f : opacity);
}
