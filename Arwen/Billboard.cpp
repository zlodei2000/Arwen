//
// Billboard class
//
// Author: Alex V. Boreskoff
//
// Last modified: 26/11/2002
//

#include	"Billboard.h"
#include	"View.h"
#include	"Texture.h"

MetaClass	Billboard :: classInstance ( "Billboard", &VisualObject :: classInstance );

Billboard :: Billboard ( const char * theName, const Vector3D& thePos, const Vector4D& theColor, 
						 Texture * theTexture, float theWidth, float theHeight ) : VisualObject ( theName, thePos, theColor )
{
	width     = theWidth;
	height    = theHeight;
	texture   = theTexture;
	metaClass = &classInstance;

	if ( texture != NULL )
		texture -> retain ();

	buildBoundingBox ();
}

Billboard :: ~Billboard ()
{
	if ( texture != NULL )
		texture -> release ();
}

void	Billboard  :: draw ( View& view, const Camera& camera, const Frustrum& frustrum, Fog * fog )
{
	view.drawBillboard ( pos, width, height, color, texture );
}

void	Billboard :: buildBoundingBox ()
{
	float	size = (width > height ? width : height) * 0.5f;

	boundingBox = BoundingBox ( Vector3D ( pos.x - size, pos.y - size, pos.z - size ),
								Vector3D ( pos.x + size, pos.y + size, pos.z + size ) );
}

