//
// Class to load images from GIF files
//
// Author: Alex V. Boreskoff
//
// Last modified: 17/07/2002
//

#ifndef	__GIF_LOADER__
#define	__GIF_LOADER__

#include	"ResourceDecoder.h"

class	GifDecoder : public ResourceDecoder
{
public:
	GifDecoder ( ResourceManager * rm ) : ResourceDecoder ( "GifResourceDecoder", rm ) 
	{
		metaClass = &classInstance;
	}

	virtual	Object * clone () const
	{
		return new GifDecoder ( getResourceManager () );
	}

	virtual	Object * decode         ( Data * );
	virtual	bool	 checkExtension ( const String& theName );

	static	MetaClass	classInstance;

private:
	int	loadBlock ( Data * data );
	int	getCode   ( Data * data );
};

#endif
