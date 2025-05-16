//
// Class to load Quake II .md2 models
//
// Author: Alex V. Boreskoff
//
// Last modified: 6/11/2002
//

#ifndef	__MD2_DECODER__
#define	__MD2_DECODER__

#include	"ResourceDecoder.h"

class	Md2Decoder : public ResourceDecoder
{
public:
	Md2Decoder ( ResourceManager * rm ) : ResourceDecoder ( "Md2Loader", rm ) 
	{
		metaClass = &classInstance;
	}

	virtual	Object * decode         ( Data * data );
	virtual	bool     checkExtension ( const String& theName );

	static	MetaClass	classInstance;
};

#endif
