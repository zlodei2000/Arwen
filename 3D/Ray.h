//
// Ray class.
//
// Author: Alex V. Boreskoff
//
// Last modified: 16/07/2002
//

#ifndef	__RAY__
#define	__RAY__

#include	"Vector3D.h"

class	Plane;
class	Transform3D;

class	Ray
{
private:
	Vector3D	org;							// origin of ray
	Vector3D	dir;							// it's direction, always normalized
public:
	Ray ( const Vector3D& theOrg, const Vector3D& theDir ) : org ( theOrg ), dir ( theDir )
	{
		dir.normalize ();
	}

	const Vector3D&	getOrigin () const
	{
		return org;
	}

	const Vector3D&	getDir () const
	{
		return dir;
	}

	Vector3D	point ( float t ) const			// point on ray
	{
		return org + t * dir;
	}

	float	intersect ( const Plane& plane ) const;
	void	transform ( const Transform3D& transf );
};

#endif
