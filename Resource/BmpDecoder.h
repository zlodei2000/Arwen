//
// Class to load images from BMP files
//
// Author: Alex V. Boreskoff
//
// Last modified: 28/09/2001
//

#ifndef	__BMP_LOADER__
#define	__BMP_LOADER__

#include	"ResourceDecoder.h"

class	Texture;

class	BmpDecoder : public ResourceDecoder
{
private:
	int	width, height;
	int palette [256];
public:
	BmpDecoder ( ResourceManager * rm ) : ResourceDecoder ( "BmpImageLoader", rm ) 
	{
		metaClass = &classInstance;
	}

	virtual	Object * clone () const
	{
		return new BmpDecoder ( getResourceManager () );
	}

	virtual	Object * decode         ( Data * );
	virtual	bool	 checkExtension ( const String& theName );

	static	MetaClass	classInstance;

private:
	int	loadRGB4  ( Data *, Texture *, long * );
	int loadRGB8  ( Data *, Texture *, long * );
	int loadRGB24 ( Data *, Texture *, long * );
	int	loadRGB32 ( Data *, Texture *, long * );
	int loadRLE4  ( Data *, Texture *, long * );
	int loadRLE8  ( Data *, Texture *, long * );
};

#endif
