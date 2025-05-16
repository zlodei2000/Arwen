//
// Class to load images from TGA files
//
// Author: Alex V. Boreskoff
//
// Last modified: 6/11/2002
//

#ifndef	__TGA_LOADER__
#define	__TGA_LOADER__

#include	"ResourceDecoder.h"

class	Texture;
struct	TgaHeader;

class	TgaDecoder : public ResourceDecoder
{
private:
	TgaHeader * hdr;
	int			width;
	int			height;
	long      * palette;
public:
	TgaDecoder ( ResourceManager * rm ) : ResourceDecoder ( "TgaResourceDecoder", rm ) 
	{
		metaClass = &classInstance;
	}

	virtual	Object * clone () const
	{
		return new TgaDecoder ( getResourceManager () );
	}

	virtual	Object * decode         ( Data * );
	virtual	bool	 checkExtension ( const String& theName );

	static	MetaClass	classInstance;

private:

	int	loadRGB8  ( Data *, Texture *, long * );
	int loadRGB15 ( Data *, Texture *, long * );
	int loadRGB24 ( Data *, Texture *, long * );
	int loadRGB32 ( Data *, Texture *, long * );
	int	loadRLE8  ( Data *, Texture *, long * );
	int loadRLE15 ( Data *, Texture *, long * );
	int loadRLE24 ( Data *, Texture *, long * );
	int loadRLE32 ( Data *, Texture *, long * );
};

#endif
