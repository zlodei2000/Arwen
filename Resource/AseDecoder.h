//
// Class to decode 3DS Max ASE files
//
// Author: Alex V. Boreskoff
//
// Last modified: 6/11/2002
//

#ifndef	__ASE_DECODER__
#define	__ASE_DECODER__

#include	"ResourceDecoder.h"

class	Data;
class	String;

class	AseDecoder : public ResourceDecoder
{
public:
	AseDecoder ( ResourceManager * rm ) : ResourceDecoder ( "AseDecoder", rm ) 
	{
		metaClass = &classInstance;
	}

	virtual Object * decode ( Data * );
	virtual	bool	 checkExtension ( const String& theName );

	static	MetaClass	classInstance;
};

#endif

