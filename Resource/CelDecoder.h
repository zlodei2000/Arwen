//
// Class to load .cel images
// Based on BMGLib
//
// Author: Alex V. Boreskoff
//
// Last modified: 29/12/2001
//

#ifndef	__CEL_LOADER__
#define	__CEL_LOADER__

#include	"ResourceDecoder.h"
										// CEL specific constants and structs
#define	CEL_ID			0x9119
#define	PALETTE_SIZE	(3*256)

#pragma pack (push, 1)					// save current pack, set 1-byte packing

struct CelHeader
{
    unsigned short id;					// 0x9119
    unsigned short width;
    unsigned short height;
    unsigned short x_offset;
    unsigned short y_offset;
    unsigned char  bitsPerPixel;
    unsigned char  compressionFlag;
    unsigned int   imageSize;			// in bytes
    unsigned char  reserved [16];
};

#pragma	pack (pop)

class	CelDecoder : public ResourceDecoder
{
private:
	CelHeader     * hdr;
	unsigned char * bits;
	int             palette [256];
	int             bitsSize;
	int             numColors;
	int             width;
	int             height;
	int             bitsPos;
public:
	CelDecoder ( ResourceManager * rm ) : ResourceDecoder ( "CelResourceDecoder", rm )
	{
		hdr       = NULL;
		bits      = NULL;
		metaClass = &classInstance;
	}

	virtual	Object * clone () const
	{
		return new CelDecoder ( getResourceManager () );
	}

	virtual	Object * decode         ( Data * );
	virtual	bool	 checkExtension ( const String& theName );

	static	MetaClass	classInstance;

private:
	int	getByte ()
	{
        return bitsPos < bitsSize ? bits [bitsPos++] : -1;
	}
/*
	int	loadRGB4  ( Texture *, long * );
	int loadRGB8  ( Texture *, long * );
	int loadRGB24 ( Texture *, long * );
	int	loadRGB32 ( Texture *, long * );
	int loadRLE4  ( Texture *, long * );
	int loadRLE8  ( Texture *, long * );
*/
};


#endif
