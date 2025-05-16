//
// Lightmap class
//
// Author: Alex V. Boreskoff
//

#include	"Lightmap.h"
#include	"Texture.h"

MetaClass	Lightmap :: classInstance ( "Lightmap", &Object :: classInstance );

Lightmap :: Lightmap ( const char * theName, const Vector2D& texMin, const Vector2D& texMax, Texture * txt ) : Object ( theName )
{
	offs      = texMin;
	scale     = (1 - 2*LM_DELTA) / (texMax - texMin);
	map       = txt;
	metaClass = &classInstance;

	if ( map != NULL )
		map -> retain ();

	matrix [0][0] = scale.x;
	matrix [0][1] = 0;
	matrix [0][2] = 0;
	matrix [0][3] = -scale.x * offs.x;
	matrix [1][0] = 0;
	matrix [1][1] = scale.y;
	matrix [1][2] = 0;
	matrix [1][3] = -scale.y * offs.y;
	matrix [2][0] = 0;
	matrix [2][1] = 0;
	matrix [2][2] = 1;
	matrix [2][3] = 0;
	matrix [3][0] = 0;
	matrix [3][1] = 0;
	matrix [3][2] = 0;
	matrix [3][3] = 1;
}

Lightmap :: ~Lightmap ()
{
	if ( map != NULL )
		map -> release ();
}

