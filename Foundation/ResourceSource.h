//
// Basic model of source of named data
//
// Author: Alex V. Boreskoff
// 
// Last modified: 15/11/2002
//

#ifndef	__RESOURCE_SOURCE__
#define	__RESOURCE_SOURCE__

#include	"Object.h"

class	String;
class	Data;

class	ResourceSource : public Object
{
public:
	ResourceSource ( const char * theName ) : Object ( theName ) {}

	virtual	Data * getFile ( const String& name )
	{
		return NULL;
	}

	static	MetaClass	classInstance;
};

#endif
