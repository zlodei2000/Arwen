//
// Generic View object for Model/View/Controller pattern
//
// Author: Alex V. Boreskoff
//
// Last modified: 20/11/2002
//

#ifndef __VIEW__
#define __VIEW__

#include	"Object.h"
#include	"Array.h"
#include	"ObjStr.h"

class   Polygon3D;
class   Texture;
class	KeyboardState;
class	MouseState;
class	Controller;
class	Transform3D;
class	Camera;
class	Vector3D;
class	Vector4D;
class	Texture;
class	Event;

class	VideoMode : public Object
{
private:
	int		bpp;
	int		width;
	int		height;
	int		freq;
	String	descr;
public:
	VideoMode ( int w, int h, int bits, int f ) : Object ( "" )
	{
		bpp       = bits;
		width     = w;
		height    = h;
		freq      = f;
		metaClass = &classInstance;

		descr.printf ( "%d x %d x %d bits %d Hz", w, h, bpp, freq );
	}

	int	getBitsPerPixel () const
	{
		return bpp;
	}

	int	getWidth () const
	{
		return width;
	}

	int	getHeight () const
	{
		return height;
	}

	int	getFreq () const
	{
		return freq;
	}

	const String& getDescr () const
	{
		return descr;
	}

	static	MetaClass	classInstance;
};

class   View : public Object
{
protected:
    int           width;
    int           height;
    int			  bitsPerPixel;
	bool		  visible;
	Controller  * controller;
	Array         modeList;			// array of VideoMode's
public:
    View ( const char * aName ) : Object ( aName ) 
	{
		width        = 0;			// unknown
		height       = 0;			// unknown
		bitsPerPixel = 0;			// unknown
		visible      = false;
		controller   = NULL;
		metaClass    = &classInstance;
	}

    virtual  bool	isOk () const
    {
        return false;
    }

	virtual	void * getWindow () const 
	{
		return NULL;
	}

	virtual	char  * getVendor        ()                 = 0;
	virtual	char  * getRenderer      ()                 = 0;
	virtual	char  * getVersion       ()                 = 0;
	virtual	Event * getEvent         ()                 = 0;
	virtual	bool	getKeyboardState ( KeyboardState& ) = 0;
	virtual	bool	getMouseState    ( MouseState&    ) = 0;

    virtual void    draw          ( const Polygon3D& poly );
	virtual	void	simpleDraw    ( const Polygon3D&, int mask = 0 ) {}
	virtual	void	setFullScreen ( bool toFullScreen ) {}
	virtual	void	setSize       ( int newWidth, int newHeight ) {}
	virtual	void	startOrtho    () {}
	virtual	void	endOrtho      () {}
	virtual	void	drawBillboard ( const Vector3D& pos, float size, const Vector4D& color, Texture * txt = NULL ) {}
	virtual	void	drawBillboard ( const Vector3D& pos, float w, float h, const Vector4D& color, Texture * txt = NULL ) {}
	virtual	void	bindTexture   ( Texture * ) {}
	virtual	void	blendFunc     ( int src, int dst ) {}

	virtual	Texture * getScreenShot () const
	{
		return NULL;
	}

	virtual	void	apply ( const Transform3D& ) {}
	virtual	void	apply ( const Camera& ) {}

	const Array&	getModeList () const
	{
		return modeList;
	}

    int     getWidth () const
    {
        return width;
    }

    int getHeight () const
    {
        return height;
    }

    int getBitsPerPixel () const
    {
        return bitsPerPixel;
    }

	bool	isVisible () const
	{
		return visible;
	}

	Controller * getController () const
	{
		return controller;
	}

	void	setController ( Controller * cntrl )
	{
		controller = cntrl;
	}

	enum	BlendModes
	{
		bmNone             = -1,
		bmZero             = 0,
		bmOne              = 1,
		bmSrcColor         = 2,
		bmOneMinusSrcColor = 3,
		bmDstColor         = 4,
		bmOneMinusDstColor = 5,
		bmSrcAlpha         = 6,
		bmOneMinusSrcAlpha = 7,
		bmDstAlpha         = 8,
		bmOneMinusDstAlpha = 9
	};

	enum DrawMasks
	{
		useColors   = 1,
		useLightmap = 2
	};

	static	MetaClass	classInstance;
};

#endif
