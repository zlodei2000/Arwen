//
// .pcx image loader
//
// Author: Alex V. Boreskoff
//
// Last modified: 6/11/2002
//

#ifndef	__PCX_LOADER__
#define	__PCX_LOADER__

#include	"ResourceDecoder.h"
#include	"PixelFormat.h"

#pragma pack (push, 1)					// save current pack, set 1-byte packing

struct	PcxHeader						// header of .pcx file
{
	char	        id;					// must be 0x0A
	char	        version;
	char	        encoding;			// 0 - no encoding, 1 - run-length encoding
	char	        bitsPerPixel;
	unsigned short  leftX, leftY;
	unsigned short	rightX, rightY;
	unsigned short	displayXRes, displayYRes;
	Rgb				palette [16];		// 16-color palette
	char	        reserved;
	char	        numPlanes;
	unsigned short  bytesPerRow;		// # of bytes in unpacked line
	unsigned short  paletteInfo;		// 1 - color palette, 2 - grayscale
	char	        reserved2 [58];
};

#pragma	pack (pop)

class	PcxDecoder : public ResourceDecoder
{
public:
	PcxDecoder ( ResourceManager * rm ) : ResourceDecoder ( "PcxResourceDecoder", rm ) 
	{
		metaClass = &classInstance;
	}

	virtual	Object * clone () const
	{
		return new PcxDecoder ( getResourceManager () );
	}

	virtual	Object * decode         ( Data * );
	virtual	bool	 checkExtension ( const String& theName );

	static	MetaClass	classInstance;
};

#endif
