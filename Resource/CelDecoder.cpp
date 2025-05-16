//
// Class to load .cel images
// Based on BMGLib
//
// Author: Alex V. Boreskoff
//
// Last modified: 10/11/2002
//

#include	"CelDecoder.h"
#include	"ObjStr.h"
#include	"ResourceManager.h"
#include	"Data.h"
#include	"Texture.h"

MetaClass	CelDecoder :: classInstance ( "CelDecoder", &ResourceDecoder :: classInstance );

bool	CelDecoder :: checkExtension ( const String& theName )
{
	String	ext = String :: getExtension ( theName ).toLower ();

	return ext == "cel";
}

Object * CelDecoder :: decode ( Data * data )
{
	PixelFormat	rgbFormat  ( getResourceManager () -> getRgbFormat  () );

	hdr = (CelHeader *) data -> getPtr ();

    if ( hdr -> id != CEL_ID || hdr -> bitsPerPixel != 8 || hdr -> compressionFlag != 0 )
		return NULL;

    width  = hdr -> width;
    height = hdr -> height;

							// read the palette
	Rgb * pal = (Rgb *) data -> getPtr ( sizeof ( CelHeader ) );
	int i;
	for ( i = 0; i < 256; i++ )
		palette [i] = rgbToInt ( pal [i].blue << 2, pal [i].green << 2, pal [i].blue  << 2 );
	
	Texture	* texture = new Texture ( "", width, height, rgbFormat );

	if ( texture == NULL || !texture -> isOk () )
	{
		delete texture;

		return NULL;
	}

	long          * pixBuf = new long [width];
	unsigned char * ptr    = (unsigned char *) data -> getPtr ( sizeof ( CelHeader ) + 256*3 );

    for ( int y = 0; y < height; y++ )
    {
		for ( int x = 0; x < width; x++ )
			pixBuf [i] = palette [*ptr++];

        texture -> writeLine ( y, pixBuf );
    }

	delete [] pixBuf;

	return texture;
}
