//
// Ganeric class to decode resources (textures, models, scenes & etc)
//
// Author: Alex V. Boreskoff
//
// Last modified: 6/11/2002
//

#ifndef	__RESOURCE_DECOER__
#define	__RESOURCE_DECOER__

#include	"Object.h"

class	Data;
class	String;
class	ResourceManager;

class	ResourceDecoder : public Object
{
private:
	ResourceManager * resManager;

public:
	ResourceDecoder ( const char * theName, ResourceManager * rm ) : Object ( theName ) 
	{
		resManager = rm;
		metaClass  = &classInstance;
	}

	virtual	Object * clone () const
	{
		return NULL;
	}

	ResourceManager	* getResourceManager () const
	{
		return resManager;
	}

										// try to build object from data, 
										// returns NULL on failure
	virtual	Object * decode         ( Data * ) = 0;

										// kind of a hunt, whether name corresponds
										// to decoded type of data
	virtual	bool	 checkExtension ( const String& theName ) = 0;

	static	MetaClass	classInstance;
};

#endif