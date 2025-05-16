//
// Basic class for lights
//
// Author: Alex V. Boreskoff
//
// Last modified: 2/12/2002
//

#include	"Light.h"

MetaClass	Light :: classInstance ( "Light", &VisualObject :: classInstance );

Light :: Light ( const char * theName, const Vector3D& thePos, const Vector4D& theColor, float br, float rad, float constant, float linear, float square ) : 
	VisualObject ( theName, thePos, theColor )
{
	brightness = br;
	radius     = rad;
	a          = constant;
	b          = linear;
	c          = square;
	metaClass  = &classInstance;

}

