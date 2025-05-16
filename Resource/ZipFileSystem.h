//
// Class to read data from zip file
// Uses zlib to unzip data from file
// 
// Author: Alex V. Boreskoff
//
// Last modified: 29/12/2001
//

#ifndef	__ZIP_FILE_SYSTEM__
#define	__ZIP_FILE_SYSTEM__

#include	<fcntl.h>
#include	<io.h>
#include	<malloc.h>
#include	<stdio.h>

#include	"Array.h"
#include	"ResourceSource.h"
#include	"ObjStr.h"

struct	ZipCentralHeader;

class	ZipFileSystem : public ResourceSource
{
private:
	String	fileName;								// name of zip file
	Array   dir;									// contains directory of zip file
	bool	broken;
public:
	ZipFileSystem ( const String& zipName );

	virtual	bool	isOk () const
	{
		return !broken;
	}

	virtual	Object * clone () const
	{
		return new ZipFileSystem ( fileName );
	}

	virtual	Data  * getFile  ( const String& name );

	static	MetaClass	classInstance;

private:
	void	readDirectory ( int file );
	Data  * readEntry     ( const ZipCentralHeader& hdr );
};

#endif
