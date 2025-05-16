//
// Basic source of named data - file system
//
// Author: Alex V. Boreskoff
// 
// Last modified: 15/11/2002
//

#ifndef	__FILE_SYSTEM__
#define	__FILE_SYSTEM__


#include	"ResourceSource.h"
#include	"ObjStr.h"

class	OsFileSystem : public ResourceSource
{
private:
	String	path;
public:
	OsFileSystem ( const String& thePath = "" );

	virtual	Object * clone () const
	{
		return new OsFileSystem ( *this );
	}

	virtual	Data  * getFile  ( const String& name );
	virtual	void	freeFile ( void * ptr );

	static	MetaClass	classInstance;
};

#endif
