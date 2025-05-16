//
// Class to load images from PNG files
//
// Author: Alex V. Boreskoff
//
// Last modified: 6/11/2002
//

#ifndef	__PNG_LOADER__
#define	__PNG_LOADER__

#include	"ResourceDecoder.h"

class	PngDecoder : public ResourceDecoder
{
public:
	PngDecoder ( ResourceManager * rm ) : ResourceDecoder ( "PngResourceDecoder", rm ) 
	{
		metaClass = &classInstance;
	}

	virtual	Object * clone () const
	{
		return new PngDecoder ( getResourceManager () );
	}

	virtual	Object * decode         ( Data * );
	virtual	bool	 checkExtension ( const String& theName );

	static	MetaClass	classInstance;
};

#endif
