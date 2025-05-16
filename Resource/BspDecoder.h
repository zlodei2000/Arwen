//
// Decoder class for QuakeII bps files
//
// Author: Alex V. Boreskoff
//

#ifndef	__BSP_DECODER__
#define	__BSP_DECODER__

#include	"ResourceDecoder.h"

class	BspDecoder : public ResourceDecoder
{
public:
	BspDecoder ( ResourceManager * rm ) : ResourceDecoder ( "QuakeIIBspResourceDecoder", rm )
	{
		metaClass = &classInstance;
	}

	virtual	Object * clone () const
	{
		return new BspDecoder ( getResourceManager () );
	}

	virtual	Object * decode         ( Data * );
	virtual	bool	 checkExtension ( const String& theName );

	static	MetaClass	classInstance;
};

#endif

