//
// Image loader for .rgb, .rgba, .a SGI image files
//
// Author: Alex V. Boreskoff
//
// Last modified: 10/11/2002
//

#include	<malloc.h>

#include	"RgbDecoder.h"
#include	"ObjStr.h"
#include	"Data.h"
#include	"ResourceManager.h"
#include	"Texture.h"

MetaClass	RgbDecoder :: classInstance ( "RgbDecoder", &ResourceDecoder :: classInstance );

bool	RgbDecoder :: checkExtension ( const String& theName )
{
	String	ext = String :: getExtension ( theName ).toLower ();

	if ( ext == "rgb" || ext == "rgba" || ext == "a" )
		return 1;

	return 0;
}

Object * RgbDecoder :: decode ( Data * data )
{
	PixelFormat	rgbFormat  ( getResourceManager () -> getRgbFormat  () );
	PixelFormat	rgbaFormat ( getResourceManager () -> getRgbaFormat () );
	PixelFormat monoFormat ( getResourceManager () -> getMonoFormat () );

	bits = (unsigned char *) data -> getPtr ();

	hdr.magic         = getShort ();
	hdr.packed        = getShort ();
	hdr.dimension     = getShort ();
	hdr.width         = getShort ();
	hdr.height        = getShort ();
	hdr.bytesPerPixel = getShort ();

	if ( hdr.magic != 474 )			// check magic
		return NULL;

	width    = hdr.width;
	height   = hdr.height;

	Texture * texture;

	if ( hdr.bytesPerPixel == 1 )
		texture = new Texture ( "", width, height, monoFormat );
	else
	if ( hdr.bytesPerPixel == 3 )
		texture = new Texture ( "", width, height, rgbFormat );
	else
	if ( hdr.bytesPerPixel == 4 )
		texture = new Texture ( "", width, height, rgbaFormat );
	else
		return NULL;

	for ( int i = 0; i < 4; i++ )
		pix [i] = (unsigned char *) malloc ( width );

	long * tmpBuf = new long [width];

	if ( (hdr.packed & 0xFF00) == 0x100 )
	{
		int	x      = width * hdr.bytesPerPixel * sizeof ( unsigned );
		int	rleEnd = 512 + 2*x;

		rowStart = (unsigned long *) data -> getPtr ( 512 );
		rowSize  = (unsigned long *) data -> getPtr ( 512 + x );

		convertLong ( rowStart, width  * hdr.bytesPerPixel );
		convertLong ( rowSize,  width  * hdr.bytesPerPixel );
	}
	else
	{
		rowStart = NULL;
		rowSize  = NULL;
	}

	void * buf = data -> getPtr ();

	for ( int y = 0; y < height; y++ )
	{
		for ( int i = 0; i < 4; i++ )
			memset ( pix [i], '\0', width );

		switch ( hdr.bytesPerPixel )
		{
			case 1:
				getRow ( pix [0], buf, y, 0 );
				break;

			case 3:
				getRow ( pix [0], buf, y, 0 );
				getRow ( pix [1], buf, y, 1 );
				getRow ( pix [2], buf, y, 2 );
				break;

			case 4:
				getRow ( pix [0], buf, y, 0 );
				getRow ( pix [1], buf, y, 1 );
				getRow ( pix [2], buf, y, 2 );
				getRow ( pix [3], buf, y, 3 );
				break;
		}

		for ( int i = 0; i < width; i++ )
			tmpBuf [i] = rgbaToInt ( pix [2][i], pix [1][i], pix [0][i], pix [3][i] );

		texture -> writeLine ( y, tmpBuf );
	}

	delete [] tmpBuf;

	for ( int i = 0; i < 4; i++ )
		free ( pix [i] );

	return texture;
}

void	RgbDecoder :: getRow ( unsigned char * pixBuf, const void * buf, int y, int depth )
{
    if ( (hdr.packed & 0xFF00) == 0x0100 )
	{
		long		    offs = rowStart [y + depth * height];
		unsigned char * in   = offs  + (unsigned char *) buf;

		for ( ;; )
		{
			int	pixel = *in++;
			int	count = pixel & 0x7F;
	    
			if ( count == 0)
				return;
	    
			if ( pixel & 0x80 )
			{
				while ( count-- > 0)
					*pixBuf++ = *in++;
			} 
			else 
			{
				pixel = *in++;
				
				while ( count-- > 0 )
					*pixBuf++ = pixel;
			}
		}
    } 
	else 
	{
		int				offs = 512 + y * width + depth * width * height;
		unsigned char * in   = offs + (unsigned char *) buf;

		memcpy ( pixBuf, in, width );
    }
}

void	RgbDecoder :: convertLong ( unsigned long * buf, int size )
{
	unsigned char * in = (unsigned char *) buf;

	for ( int i = 0; i < size; i++ )
	{
		unsigned b1 = *in++;
		unsigned b2 = *in++;
		unsigned b3 = *in++;
		unsigned b4 = *in++;

		*buf++ = (b1 << 24) | (b2 << 16) | (b3 << 8) | b4;
	}
}
