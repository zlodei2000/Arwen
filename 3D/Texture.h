//
// Simple textture class. Supports loading 16- and 256-color .BMP textures
// Keeps image in 16(15) bit HiColor format
//
// Author: Alex V. Boreskoff
//
// Last modified: 10/08/2001
//

#ifndef __TEXTURE__
#define __TEXTURE__

#include    "Object.h"
#include	"PixelFormat.h"

class   Texture : public Object
{
protected:
	PixelFormat format;
    void      * data;
    int         width;
    int         height;
	unsigned	id;									// id for use in OpenGL glBindTexture
	int			glFormat;							// OpenGl format of texture (GL_ALPHA, GL_RGB, GL_RGBA & etc
	bool		mipmap;								// whether to create and use mipmaps
	int			components;							// # of color components
public:
	Texture ( const char * theName, int theWidth, int theHeight, const PixelFormat& theFormat );

    ~Texture ()
    {
        delete data;
    }

    virtual bool isOk () const
    {
        return width > 0 && height > 0 && data != NULL;
    }

	int	getWidth () const
	{
		return width;
	}

	int	getHeight () const
	{
		return height;
	}

	const void * getData () const
	{
		return data;
	}

	bool	isTransparent () const
	{
		return format.bitsPerPixel == 8 || format.bitsPerPixel == 32;
	}

	int	getNumComponents () const
	{
		return format.bytesPerPixel;
	}

	bool	isMipmapped () const
	{
		return mipmap;
	}

	void	setMipmap ( bool flag )
	{
		mipmap = flag;
	}

	unsigned&	getId ()
	{
		return id;
	}

	int	getOpenGLFormat () const
	{
		return glFormat;
	}

	void	setOpenGLFormat ( int fmt )
	{
		glFormat = fmt;
	}

	void	upload        ();
	void	unload        ();
	int		getOpenGLType () const;
    void	writeLine     ( int y, long * buf, long * palette = NULL );	// write 32-bit pixel buffer to Texture line
	void	readLine      ( int y, long * buf );

	static	MetaClass	classInstance;
};

#endif

