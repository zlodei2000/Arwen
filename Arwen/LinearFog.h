//
// Class for fog with linear density
// 
//   density (p) = (grad, p) + offs
//
// Author: Alex V. Boreskoff
//
// Last modified: 14/11/2002
//

#ifndef	__LINEAR_FOG__
#define	__LINEAR_FOG__

#include	"Fog.h"

class	LinearFog : public Fog
{
private:
	Vector3D	grad;
	float		offs;
public:
	LinearFog ( const char * theName, Plane * thePlane, const Vector3D& theColor, 
		        const Vector3D& theGrad, float theOffs ) : Fog ( theName, thePlane, theColor, 1.0f )
	{
		grad      = theGrad;
		offs      = theOffs;
		metaClass = &classInstance;
	}

	virtual	float	getOpacity ( const Vector3D& from, const Vector3D& to, Plane * clipPlane ) const;

	static	MetaClass	classInstance;
};

#endif
