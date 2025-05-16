//
// Loader for Quake II .wal textures
//
// Author: Alex V. Boreskoff
// 
// Last modified: 6/11/2002
//

#ifndef	__WAL_LOADER__
#define	__WAL_LOADER__

#include	"ResourceDecoder.h"

class	WalDecoder : public ResourceDecoder
{
public:
	WalDecoder ( ResourceManager * rm ) : ResourceDecoder ( "WalResourceDecoder", rm ) 
	{
		metaClass = &classInstance;
	}

	virtual	Object * clone () const
	{
		return new WalDecoder ( getResourceManager () );
	}

	virtual	Object * decode         ( Data * );
	virtual	bool	 checkExtension ( const String& theName );

	static	MetaClass	classInstance;
};

#endif

