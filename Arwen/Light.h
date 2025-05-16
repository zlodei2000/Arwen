//
// Basic class for lights
//
// Author: Alex V. Boreskoff
//
// Last modified: 2/12/2002
//

#ifndef	__LIGHT__
#define	__LIGHT__

#include	"VisualObject.h"

#define	LF_HAS_HALO			0x0004
#define	LF_HAS_LENS_FLARE	0x0008

class Light : public VisualObject
{
protected:
	float	brightness;				// how bright is light source
	float	radius;					// it's effective radius
	float	a;						// constant part of inverse attenuation
	float	b;						// linear part of inverse attenuation
	float	c;						// square part of inverse attenuation

public:
	Light ( const char * theName, const Vector3D& thePos, const Vector4D& theColor, 
		    float br, float rad, float constant, float linear, float square );

	float	getBrightness () const
	{
		return brightness;
	}

	float	getRadius () const
	{
		return radius;
	}

	Vector3D	getLightAt ( const Vector3D& point, const Vector3D& normal ) const
	{
		Vector3D	l    = pos - point;
		float		dist = l.length ();

		if ( dist >= radius )
			return Vector3D ( 0, 0, 0 );

		float	cosAngle = (float) fabs ( (l & normal) / dist );

		Vector3D	cl ( color.x, color.y, color.z );

		return cl * ( brightness * cosAngle / ( a + dist * ( b + c * dist ) ) );
	}

	static	MetaClass	classInstance;
};

#endif
