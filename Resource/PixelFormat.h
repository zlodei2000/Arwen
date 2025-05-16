#ifndef	__PIXEL_FORMAT__
#define	__PIXEL_FORMAT__

#undef	Rgb							// undefine Windows's RGB macro

#pragma pack (push, 1)				// save current pack, set 1-byte packing

struct  Rgb
{
	unsigned char    red;
	unsigned char    green;
	unsigned char    blue;
};

struct  Rgba
{
	unsigned char    red;
	unsigned char    green;
	unsigned char    blue;
	unsigned char	alpha;
};

#pragma	pack (pop)					// restore current byte-alignment

class  PixelFormat
{
public:
	unsigned redMask;                // bit mask for red color bits
	int      redShift;               // position of red bits in pixel data
	int      redBits;                // # of bits for red field
	unsigned greenMask;              // bit mask for green color bits
	int      greenShift;             // position of green bits in pixel data
	int      greenBits;              // # of bits for green field
	unsigned blueMask;               // bit mask for blue color bits
	int      blueShift;              // position of blue bits in pixel data
	int      blueBits;
	unsigned alphaMask;
	int		 alphaShift;
	int		 alphaBits;
	int      bitsPerPixel;           // # of bits per pixel
	int      bytesPerPixel;          // # of bytes per single pixel

	PixelFormat () {}
	PixelFormat ( unsigned theRedMask, unsigned theGreenMask, unsigned theBlueMask, unsigned theAlphaMask = 0 )
	{
		completeFromMasks ( theRedMask, theGreenMask, theBlueMask, theAlphaMask );
	}

	void    completeFromMasks ( unsigned theRedMask, unsigned theGreenMask, unsigned theBlueMask, unsigned theAlphaMask = 0 );

	int	rgbaToInt ( int red, int green, int blue, int alpha = 0 ) const
	{
        return ((blue>>(8-blueBits))<<blueShift) | 
			   ((green>>(8-greenBits))<<greenShift) |
               ((red>>(8-redBits))<<redShift) |
			   ((alpha>>(8-alphaBits))<<alphaShift);;
	}

	int	getRed ( int color ) const
	{
		return ((color >> redShift) << (8 - redBits)) & redMask;
	}

	int	getGreen ( int color ) const
	{
		return ((color >> greenShift) << (8 - greenBits)) & greenMask;
	}

	int	getBlue ( int color ) const
	{
		return ((color >> blueShift) << (8 - blueBits)) & blueMask;
	}

	int	getAlpha ( int color ) const
	{
		return ((color >> alphaShift) << (8 - alphaBits)) & alphaMask;
	}
};

inline int      operator == ( const PixelFormat& f1, const PixelFormat& f2 )
{
        return f1.redMask == f2.redMask && f1.greenMask == f2.greenMask && f1.blueMask == f2.blueMask && f1.alphaMask == f2.alphaMask;
}

inline  int     rgbToInt ( int red, int green, int blue )
{
        return blue | (green << 8) | (red << 16);
}

inline  int     rgbaToInt ( int red, int green, int blue, int alpha )
{
        return blue | (green << 8) | (red << 16) | (alpha << 24);
}

				// these functions only for 24 and 32-bit modes
inline	int	redComponent ( int color )
{
	return (color >> 16) & 0xFF;
}

inline	int	greenComponent ( int color )
{
	return (color >> 8) & 0xFF;
}

inline	int	blueComponent ( int color )
{
	return color & 0xFF;
}

inline	int	alphaComponent ( int color )
{
	return color >> 24;
}

#endif
