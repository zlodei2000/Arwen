//
// .pcx image loader
//
// Author: Alex V. Boreskoff
//
// Last modified: 10/11/2002
//

#include	"PcxDecoder.h"
#include	"ObjStr.h"
#include	"Data.h"
#include	"ResourceManager.h"
#include	"Texture.h"

MetaClass	PcxDecoder :: classInstance ( "PcxDecoder", &ResourceDecoder :: classInstance );

bool	PcxDecoder :: checkExtension ( const String& theName )
{
	return String :: getExtension ( theName ).toLower () == "pcx";
}

Object * PcxDecoder :: decode ( Data * data )
{
	PixelFormat	rgbFormat  ( getResourceManager () -> getRgbFormat  () );

	PcxHeader     * hdr = (PcxHeader *) data -> getPtr ();
	unsigned char * bits;
	int             width;
	int             height;
	int             bitsPos;
	int				bitsSize;
	long			palette [256];

	if ( hdr -> id != 0x0A || hdr -> version != 5 || hdr -> encoding != 1 )
		return NULL;

	if ( hdr -> numPlanes == 1 )
	{
		if ( hdr -> bitsPerPixel != 1 && hdr -> bitsPerPixel != 2 && hdr -> bitsPerPixel != 8 ) 
			return NULL;
	}
	else
	if ( hdr -> numPlanes == 3 )
	{
		if ( hdr -> bitsPerPixel != 1 && hdr -> bitsPerPixel != 8 )
			return NULL;
	}
	else
	if ( hdr -> numPlanes == 4 )
	{
		if ( hdr -> bitsPerPixel != 1 )
			return NULL;
	}
	else
		return NULL;

	width    = hdr -> rightX - hdr -> leftX + 1;			// get size
	height   = hdr -> rightY - hdr -> leftY + 1;
    bits     = (unsigned char *) data -> getPtr ( sizeof ( PcxHeader ) );
    bitsSize = data -> getLength () - sizeof ( PcxHeader );
    bitsPos  = 0;

											// setup palette
	if ( hdr -> numPlanes == 1 && hdr -> bitsPerPixel == 8 )
	{
		const char * palPtr = data -> getLength () - 1 - 256*3 + (const char *) data -> getPtr ();

		if ( *palPtr == 0x0C )
		{
			Rgb * pal = (Rgb *)(palPtr + 1);

			for ( int i = 0; i < 256; i++ )
				palette [i] = rgbToInt ( pal [i].blue, pal [i].green, pal [i].red );
		}
		else
			return NULL;
	}

	int				bytesPerPixel  = hdr -> numPlanes == 3 && hdr -> bitsPerPixel == 8 ? 3 : 1;
	int				scanLineLength = hdr -> numPlanes * hdr -> bytesPerRow;
	long          * pixelBuf       = new long [width];
	unsigned char *	planeBuf       = new unsigned char [hdr -> numPlanes * hdr -> bytesPerRow];

	if ( pixelBuf == NULL || planeBuf == NULL )
	{
		delete pixelBuf;
		delete planeBuf;

		return NULL;
	}

    Texture * texture = new Texture ( name, width, height, rgbFormat );

    if ( texture == NULL )
	{
		delete pixelBuf;
		delete planeBuf;

		return NULL;
	}

    if ( !texture -> isOk () )
	{
		delete pixelBuf;
		delete planeBuf;

		return NULL;
	}

	int		x;
	int		byte, count;
	long	offs = 0;

	for ( int y = 0; y < height; y++ )						// for every row
	{
		int	index = 0;
		int scanLineLength = hdr -> numPlanes * hdr -> bytesPerRow; 

		for ( x = 0; x < scanLineLength; )				// real unpacking
		{
			if ( ( byte = data -> getByte () ) == -1 )
			{
				delete pixelBuf;
				delete planeBuf;

				return NULL;
			}

			if ( byte > 0xC0 )							// XXX: & 0xC0 )== 0xC0
			{
				count = byte & 0x3F;

				if ( ( byte = data -> getByte () ) == -1 )
				{
					delete pixelBuf;
					delete planeBuf;

					return NULL;
				}
			}
			else
				count = 1;

			for ( ; count > 0 && x < scanLineLength; count--, x++ )
				planeBuf [index++] = byte;
		}

		// line has been read & decoded for all planes, now store it

		if ( hdr -> numPlanes == 3 && hdr -> bitsPerPixel == 8 )	// decode true-color image
		{
			for ( x = 0; x < hdr -> bytesPerRow; x++ )
				pixelBuf [x] = rgbToInt ( planeBuf [x], planeBuf [x+hdr -> bytesPerRow], planeBuf [x+2*hdr -> bytesPerRow] );
		}
		else
		if ( hdr -> numPlanes == 1 && hdr -> bitsPerPixel == 8 )	// decode 256-color image
		{
			for ( x = 0; x < width; x++ )
				pixelBuf [x] = palette [planeBuf [x]];
		}
		else														// 8 or 16 color image
		if ( ( hdr -> numPlanes == 3 || hdr -> numPlanes == 4 ) && hdr -> bitsPerPixel == 1 )
		{
			for ( x = 0; x < width; x++ )
			{
				pixelBuf [x] = 0;

				for ( int p = 0; p < hdr -> numPlanes; p++ )
					if ( planeBuf [p*hdr->bytesPerRow+(x>>3)] & (1 << (x&7)) )
						pixelBuf [x] |= 1 << p;
			}
		}
		else
		if ( hdr -> numPlanes == 1 && hdr -> bitsPerPixel == 2 )	// 4-color image
		{
			for ( x = 0; x < width; x++ )
				pixelBuf [x] = ( planeBuf [x >> 2] >> ( (x&3) << 1 ) ) & 3;
		}
		else														// mono (b/w) image
		{
			for ( x = 0 ; x < width; x++ )
				pixelBuf [x] = (planeBuf [x >> 3] >> (x & 7) ) & 1;
		}

		texture -> writeLine ( y, pixelBuf );
	}

	delete planeBuf;
	delete pixelBuf;

	return texture;
}
