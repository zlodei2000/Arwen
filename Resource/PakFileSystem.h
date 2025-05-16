//
// Class PakFileSystem to manage resources within Quake II pak files
//
// Author: Alex V. Boreskoff
//
// Last modified: 18/12/2001
//

#ifndef	__PAK__
#define	__PAK__

#include	"ResourceSource.h"
#include	"QuakeII.h"

class	PakFileSystem : public ResourceSource
{
private:
	char		fileName [128];			// name of file
	int			file;					// file handle
	PakHeader	hdr;					// header of pak file
	DirEntry  * dir;					// pointer to the directory
	int			numDirEntries;			// number of directory entries
public:
	PakFileSystem ( const char * name );
	~PakFileSystem ();
									
	virtual	Object * clone () const
	{
		return new PakFileSystem ( *this );
	}

	virtual	bool   isOk () const
	{
		return file != -1 && hdr.isOk () && dir != NULL && numDirEntries > 0;
	}

	virtual	Data  * getFile  ( const String& name );

	static	MetaClass	classInstance;

private:
	DirEntry * locateFile ( const char * name );
};

#endif
