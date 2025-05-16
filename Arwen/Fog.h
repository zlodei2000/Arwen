//
// Basic layered fog class
//
// Author: Alex V. Boreskoff
//
// Last modified: 14/11/2002
//

#ifndef	__FOG__
#define	__FOG__

#include	"Object.h"
#include	"Vector3D.h"
#include	"Vector4D.h"

#define	MAX_FOG_PLANES	10

class	Plane;
class	View;
class	Frustrum;
class	Polygon3D;
class	Ray;

class	Fog : public Object
{
private:
	int			srcBlend;
	int			dstBlend;
	Vector3D	color;
	float		density;
	int			numPlanes;
	Plane     * plane [MAX_FOG_PLANES];

public:
	Fog ( const char * theName, Plane * thePlane, const Vector3D& theColor, float theDensity );
	~Fog ();

	virtual	void	drawFogPoly   ( View& view, const Vector3D& pos, const Frustrum& viewFrustrum, const Polygon3D& poly ) const;
	bool	        addFogPlane   ( Plane * thePlane );
	virtual	float	getOpacity    ( const Vector3D& from, const Vector3D& to, Plane * clipPlane ) const;
	bool	        traceRay      ( const Ray& ray, float& t1, float& t2 ) const;

	Vector4D		getBlendColor ( const Vector3D& from, const Vector3D& to, Plane * clipPlane ) const
	{
		return Vector4D ( color.x, color.y, color.z, getOpacity ( from, to, clipPlane ) );
	}

	void	setSrcBlend ( int mode )
	{
		srcBlend = mode;
	}

	void	setDstBlend ( int mode )
	{
		dstBlend = mode;
	}

	static	MetaClass	classInstance;
};

#endif
