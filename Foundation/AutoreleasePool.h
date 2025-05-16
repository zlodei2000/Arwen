//
// Autorelease pool class, based on array class
//
// Author: Alex V. Boreskoff
//
// Last modified: 19/08/2002
//

#ifndef	__AUTORELEASE_POOL__
#define	__AUTORELEASE_POOL__

#include	"Object.h"

class	Array;

class	AutoreleasePool : public Object
{
	Array * pool;
public:
	AutoreleasePool ( const char * theName = "" );
	~AutoreleasePool ();

	virtual	bool	isOk () const;

	static	Object * createObject ()
	{
		return new AutoreleasePool;
	}

	void	add        ( Object * );
	void	releaseAll ();

	static	MetaClass	classInstance;
};

extern	AutoreleasePool * autoreleasePool;

#endif
