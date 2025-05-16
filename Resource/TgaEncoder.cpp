//
// Class to encode textures as tga files
//
// Author: Alex V. Boreskoff
//
// Last modified: 5/12/2002
//

#include	"TgaEncoder.h"
#include	"ObjStr.h"
#include	"MutableData.h"
#include	"Texture.h"

MetaClass	TgaEncoder :: classInstance ( "TgaEncoder", &Object :: classInstance );

bool	TgaEncoder :: encode ( const Object * object, MutableData * data, Dictionary * hints )
{
	Texture * txt = dynamic_cast <Texture *> ( (Object *) object );

	if ( txt == NULL )				// can encode only textures
		return false;

	width  = txt -> getWidth  ();
	height = txt -> getHeight ();

	memset ( &hdr, '\0', sizeof ( hdr ) );

	hdr.imageType         = 2; //1;		// unecndoded image
	hdr.width             = width;
	hdr.height            = height;
	hdr.pixelSize         = txt -> isTransparent () ? 32 : 24;

	data -> putBytes ( &hdr, sizeof ( hdr ) );

	if ( hdr.pixelSize == 24 )
		return encodeRgb24 ( data, (byte *)txt -> getData () );
	
	return encodeRgb32 ( data, (byte *)txt -> getData () );
}

bool	TgaEncoder :: encodeRgb24 ( MutableData * data, byte * bytes )
{
	Rgb * ptr = (Rgb *) bytes;

	for ( int y = 0; y < height; y++ )
	{
		for ( int x = 0; x < width; x++, ptr++ )
		{
			data -> putByte ( ptr -> blue  );
			data -> putByte ( ptr -> green );
			data -> putByte ( ptr -> red   );
		}
	}

	return true;
}
	
bool	TgaEncoder :: encodeRgb32 ( MutableData * data, byte * bytes )
{
	Rgba * ptr = (Rgba *) bytes;

	for ( int y = 0; y < height; y++ )
	{
		for ( int x = 0; x < width; x++, ptr++ )
		{
			data -> putByte ( ptr -> blue  );
			data -> putByte ( ptr -> green );
			data -> putByte ( ptr -> red   );
			data -> putByte ( ptr -> alpha );
		}
	}

	return true;
}

