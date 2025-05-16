//
// Class to load images from PNG files
//
// Author: Alex V. Boreskoff
// Based on the code from Crystal Space engine
//
// Last modified: 6/11/2002
//

#include	<malloc.h>

extern "C"
{
	#define		Byte	z_Byte					// to avoid conflcits in zconf.h
	#include	"zlib.h"
	#undef		Byte
	#include	"png.h"
}

#include	"PngDecoder.h"
#include	"ObjStr.h"
#include	"Data.h"
#include	"ResourceManager.h"
#include	"Texture.h"

struct	PngImageRawData
{
	char  * data;
	size_t	size;
};

static	void	pngImageRead ( png_structp pngPtr, png_bytep data, png_size_t size )
{
	PngImageRawData * self = (PngImageRawData *) pngPtr -> io_ptr;

	if ( self -> size < size )
		png_error ( pngPtr, "Read error" );
	else
	{
		memcpy ( data, self -> data, size );

		self -> size -= size;
		self -> data += size;
	}
}

MetaClass	PngDecoder :: classInstance ( "PngDecoder", &ResourceDecoder :: classInstance );

bool	PngDecoder :: checkExtension ( const String& theName )
{
	return String :: getExtension ( theName ).toLower () == "png";
}

Object * PngDecoder :: decode ( Data * data )
{
								// check signature
	if ( !png_check_sig ( (unsigned char *)data -> getPtr (), data -> getLength () ) )
        return NULL;

	png_structp	pngPtr = png_create_read_struct ( PNG_LIBPNG_VER_STRING, NULL, NULL, NULL );

	if ( pngPtr == NULL )
		return NULL;

	png_infop	infoPtr = png_create_info_struct ( pngPtr );

	if ( infoPtr == NULL )
	{
		png_destroy_read_struct ( &pngPtr, (png_infopp) NULL, (png_infopp) NULL );

		return NULL;
	}

	png_infop	endInfo = png_create_info_struct ( pngPtr );

	if ( endInfo == NULL )
	{
		png_destroy_read_struct ( &pngPtr, &infoPtr, (png_infopp) NULL );

		return NULL;
	}

	PngImageRawData	raw;

	raw.data = (char *) data -> getPtr ();
	raw.size = data -> getLength ();

	png_set_read_fn ( pngPtr, &raw, pngImageRead );
	png_read_info   ( pngPtr, infoPtr );

	int	width     = png_get_image_width  ( pngPtr, infoPtr );
	int	height    = png_get_image_height ( pngPtr, infoPtr );
	int	bpp       = png_get_bit_depth    ( pngPtr, infoPtr );
	int	colorType = png_get_color_type   ( pngPtr, infoPtr );
	int	rowBytes  = png_get_rowbytes     ( pngPtr, infoPtr );

	Texture * texture = NULL;
/*
	if ( setjmp ( pngPtr -> jmpbuf ) )             // Set a new exception handler
	{
		errorCode = LE_INTERNALERROR;

		png_destroy_read_struct ( &pngPtr, &infoPtr, &endInfo );

		return NULL;
	}
*/
	PixelFormat	rgbFormat  ( getResourceManager () -> getRgbFormat  () );
	PixelFormat	rgbaFormat ( getResourceManager () -> getRgbaFormat () );
	PixelFormat monoFormat ( getResourceManager () -> getMonoFormat () );

	switch ( colorType )
	{
		case PNG_COLOR_TYPE_RGB:
			texture = new Texture ( "", width, height, rgbFormat );
			break;

		case PNG_COLOR_TYPE_RGB_ALPHA:
			texture = new Texture ( "", width, height, rgbaFormat );
			break;

		case PNG_COLOR_TYPE_GRAY:
		case PNG_COLOR_TYPE_GRAY_ALPHA:
			texture = new Texture ( "", width, height, monoFormat );
			break;

		case PNG_COLOR_TYPE_PALETTE:
			texture = new Texture ( "", width, height, rgbFormat );
			break;

		default:
			break;
	}

	if ( texture != NULL )
	{
		png_bytep * rowPtr  = new png_bytep [height];
		long      * lineBuf = new long [width];

		for ( int i = 0; i < height; i++ )
			rowPtr [i] = (png_bytep) malloc ( rowBytes );

		png_read_image ( pngPtr, rowPtr );

										// now rebuild the image
		for ( int i = 0; i < height; i++ )
		{
			png_bytep	ptr = rowPtr [i];

			if ( colorType == PNG_COLOR_TYPE_RGB )
			{
				for ( int x = 0; x < width; x++, ptr += 3 )
					lineBuf [x] = rgbToInt ( ptr [0], ptr [1], ptr [2] );
			}
			else
			if ( colorType == PNG_COLOR_TYPE_RGB_ALPHA )
			{
				for ( int x = 0; x < width; x++, ptr += 4 )
					lineBuf [x] = rgbaToInt ( ptr [0], ptr [1], ptr [2], ptr [3] );

			}
			else
			if ( colorType == PNG_COLOR_TYPE_PALETTE )
			{
				png_color * pngPal = NULL;
				png_color   pal [256];
				int			numColors;
				int			numEntries = (1 << bpp) - 1;
     

				if (!png_get_PLTE ( pngPtr, infoPtr, (png_colorp *)&pngPal, &numColors ) )
				{
						// This is a grayscale image, build a grayscale palette
					numColors = numEntries + 1;
					pngPal    = pal;
					
					for ( int i = 0; i < numColors; i++)
						pngPal [i].red   = 
						pngPal [i].green = 
						pngPal [i].blue  = (i * 255) / numEntries;
				}
				else
				{
					for ( int i = 0; i < numColors; i++ )
					{
						pngPal [i].red   = (pngPal [i].red   * 255) / numEntries;
						pngPal [i].green = (pngPal [i].green * 255) / numEntries;
						pngPal [i].blue  = (pngPal [i].blue  * 255) / numEntries;
					}
				}

				for ( int i = 0; i < width; i++ )
				{
					int	ind = ptr [i];

					lineBuf [i] = rgbToInt ( pngPal [ind].blue, pngPal [ind].green, pngPal [ind].red );
				}

			}
			else
			{
				for ( int x = 0; x < width; x++, ptr++ )
					lineBuf [x] = ptr [0];
			}

			texture -> writeLine ( i, lineBuf );

			free ( rowPtr [i] );
		}

		delete [] lineBuf;
		delete [] rowPtr;

		png_read_end ( pngPtr, endInfo );
	}

	png_destroy_read_struct ( &pngPtr, &infoPtr, &endInfo );

	return texture;
}
