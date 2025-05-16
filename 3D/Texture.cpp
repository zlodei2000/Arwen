//
// Simple texture class. Supports 8-bit alpha, 24-bit RGB and 32-bit RGBA textures
//
// Author: Alex V. Boreskoff
//
// Last modified: 31/07/2002
//

#include	<windows.h>						// for some reason gl/gl cannot compile without it
#include	<gl/gl.h>						// for GL_*
#include	<gl/glu.h>

#include	"Texture.h"

MetaClass	Texture :: classInstance ( "Texture", &Object :: classInstance );

Texture :: Texture ( const char * theName, int theWidth, int theHeight, const PixelFormat& theFormat ) : Object ( theName )
{
	metaClass = &classInstance;
	width     = theWidth;
	height    = theHeight;
	id        = 0;
	format    = theFormat;
	mipmap    = true;
	data      = new char [width*height*format.bytesPerPixel];

	if ( format.bitsPerPixel <= 8 )										// single channel data
	{
		glFormat   = GL_ALPHA;
		components = 1;
	}
	else
	if ( format.alphaBits == 0 )
	{
		glFormat   = GL_RGB;
		components = 3;
	}
	else
	{
		glFormat   = GL_RGBA;			// assume 24-bit or 32-bit formats
		components = 4;
	}

	memset ( data, '\0', width*height*format.bytesPerPixel );
}

void	Texture :: writeLine ( int y, long * buf, long * palette )
{
	if ( y < 0 || y >= height )
		return;

	if ( palette != NULL )
		for ( register int i = 0; i < width; i++ )
			buf [i] = palette [buf [i] & 0xFF];

	if ( format.bytesPerPixel == 1 )
	{
		char * ptr = y * format.bytesPerPixel * width + (char *) data;
	
		for ( register int i = 0; i < width; i++, buf++ )
			*ptr++ = (char) rgbaToInt ( redComponent ( buf [0] ), greenComponent ( buf [0] ), blueComponent ( buf [0] ), alphaComponent ( buf [0] ) );
	}
	else
	if ( format.bytesPerPixel == 2 )
	{
		short * ptr = (short *)(y * format.bytesPerPixel * width + (char *) data);

		for ( register int i = 0; i < width; i++, buf++ )
			*ptr++ = (short) rgbaToInt ( redComponent ( buf [0] ), greenComponent ( buf [0] ), blueComponent ( buf [0] ), alphaComponent ( buf [0] ) );;
	}
	else
	if ( format.bytesPerPixel == 3 )
	{
		Rgb   * ptr = (Rgb *)(y * format.bytesPerPixel * width + (char *) data);
		long	val;

		for ( register int i = 0; i < width; i++, buf++ )
		{
			val    = rgbaToInt ( redComponent ( buf [0] ), greenComponent ( buf [0] ), blueComponent ( buf [0] ), alphaComponent ( buf [0] ) );
			*ptr++ = *(Rgb *)&val;
		}
	}
	else
	if ( format.bytesPerPixel == 4 )
	{
		long * ptr = (long *)(y * format.bytesPerPixel * width + (char *) data);

		for ( register int i = 0; i < width; i++, buf++ )
			*ptr++ = rgbaToInt ( redComponent ( buf [0] ), greenComponent ( buf [0] ), blueComponent ( buf [0] ), alphaComponent ( buf [0] ) );
	}
}

void	Texture :: readLine ( int y, long * buf )
{
	if ( y < 0 || y >= height )
		return;

	if ( format.bytesPerPixel == 1 )
	{
		char * ptr = y * format.bytesPerPixel * width + (char *) data;
	
		for ( register int i = 0; i < width; i++ )
			*buf++ = *ptr++;
	}
	else
	if ( format.bytesPerPixel == 2 )
	{
		short * ptr = (short *)(y * format.bytesPerPixel * width + (char *) data);

		for ( register int i = 0; i < width; i++ )
		{
			int	color = *ptr++;

			*buf++ = rgbaToInt ( format.getRed ( color ), format.getGreen ( color ), format.getBlue ( color ), format.getAlpha ( color ) );
		}
	}
	else
	if ( format.bytesPerPixel == 3 )
	{
		Rgb   * ptr = (Rgb *)(y * format.bytesPerPixel * width + (char *) data);

		for ( register int i = 0; i < width; i++, ptr++ )
			*buf++ = rgbToInt ( ptr -> red, ptr -> green, ptr -> blue );
	}
	else
	if ( format.bytesPerPixel == 4 )
	{
		long * ptr = (long *)(y * format.bytesPerPixel * width + (char *) data);

		for ( register int i = 0; i < width; i++ )
			*buf++ = *ptr++;
	}
}

int	Texture :: getOpenGLType () const
{
	return GL_UNSIGNED_BYTE;
}

void	Texture :: upload ()
{
	glGenTextures ( 1, &id );											// create the texture

																		// typical texture generation from the bitmap
	glBindTexture   ( GL_TEXTURE_2D, id );
	glPixelStorei   ( GL_UNPACK_ALIGNMENT, 1 );							// set 1-byte alignment
	glPixelStorei   ( GL_PACK_ALIGNMENT,   1 );
	glTexParameteri ( GL_TEXTURE_2D,  GL_TEXTURE_WRAP_S, GL_REPEAT );	// set texture to repeat mode
	glTexParameteri ( GL_TEXTURE_2D,  GL_TEXTURE_WRAP_T, GL_REPEAT );

	if ( mipmap )
	{
		gluBuild2DMipmaps ( GL_TEXTURE_2D, getOpenGLFormat (), getWidth (), getHeight (), getOpenGLFormat (), getOpenGLType (), getData () );
		glTexParameteri   ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );	
		glTexParameteri   ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	}
	else
	{
		glTexImage2D    ( GL_TEXTURE_2D,  0, components, getWidth (), getHeight (), 0, getOpenGLFormat (), getOpenGLType (), getData () );
		glTexParameteri ( GL_TEXTURE_2D,  GL_TEXTURE_MAG_FILTER, GL_LINEAR );	
		glTexParameteri ( GL_TEXTURE_2D,  GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	}
}

void	Texture :: unload ()
{
	glDeleteTextures ( 1, &id );
}
