//
// Image loader for .rgb, .rgba, .a SGI image files
//
// Author: Alex V. Boreskoff
//
// Last modified: 28/12/2001
//

#ifndef	__RGB_LOADER__
#define	__RGB_LOADER__

#include	"ResourceDecoder.h"

#pragma pack (push, 1)					// save current pack, set 1-byte packing

struct RgbImageHeader					// header of an rgb file
{
    unsigned short	magic;				// signature
    unsigned short	packed;				// rle or straight format
    unsigned short	dimension;			// 1 = one scanline per channel, 
										// 2 = many scanlines per channel
										// entire picture for each channel
    unsigned short	width;
	unsigned short	height;
	unsigned short	bytesPerPixel;
    unsigned long	min, max;			// unused
    unsigned long	dummy;
    char			name [80];
    unsigned long	colorMap;
};

#pragma	pack (pop)

class	RgbDecoder : public ResourceDecoder
{
private:
	RgbImageHeader   hdr;
	unsigned char  * bits;
	int              palette [256];
	int              bitsSize;
	int              numColors;
	int              width;
	int              height;
	int              bitsPos;
	unsigned long  * rowStart;
	unsigned long  * rowSize;
	unsigned char  * pix [4];
public:
	RgbDecoder ( ResourceManager * rm ) : ResourceDecoder ( "RgbResourceDecoder", rm )
	{
		bits      = NULL;
		metaClass = &classInstance;
	}

	virtual	Object * clone () const
	{
		return new RgbDecoder ( getResourceManager () );
	}

	virtual	Object * decode         ( Data * );
	virtual	bool	 checkExtension ( const String& theName );

	static	MetaClass	classInstance;

private:
	unsigned short	getShort ()
	{
		unsigned b1 = *bits++;
		unsigned b2 = *bits++;

		return (b1 << 8) | b2;
	}

	unsigned long	getLong ()
	{
		unsigned b1 = *bits++;
		unsigned b2 = *bits++;
		unsigned b3 = *bits++;
		unsigned b4 = *bits++;

		return (b1 << 24) | (b2 << 16) | (b3 << 8) | b4;
	}

	void	getRow      ( unsigned char * pixBuf, const void * buf, int y, int depth );
	void	convertLong ( unsigned long * buf, int size );
};

#endif