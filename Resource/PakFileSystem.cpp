//
// Class PakFileSystem to manage resources within Quake II pak files
//
// Author: Alex V. Boreskoff
//
// Last modified: 18/12/2001
//

#include	<fcntl.h>
#include	<io.h>
#include	<malloc.h>
#include	<stdio.h>

#include	"PakFileSystem.h"
#include	"Log.h"
#include	"Data.h"

MetaClass	PakFileSystem :: classInstance ( "PakFileSystem", &ResourceSource :: classInstance );

PakFileSystem :: PakFileSystem ( const char * name ) : ResourceSource ( name )
{
	metaClass = &classInstance;

	memset ( &hdr, '\0', sizeof ( hdr ) );
	memset ( fileName, '\0', sizeof ( fileName ) );
	strcpy ( fileName, name );

	dir           = NULL;
	numDirEntries = 0;
	file          = open ( fileName, O_RDONLY | O_BINARY );

	if ( file == -1 )
		return;

	read ( file, &hdr, sizeof ( hdr ) );

	if ( !hdr.isOk () )
		return;

	numDirEntries = hdr.getNumEntries ();
	dir           = new DirEntry [numDirEntries];

	lseek ( file, hdr.dirOffset, SEEK_SET );
	read  ( file, dir, hdr.dirSize );

	(*sysLog) << "Added pak source: " << name << logEndl;
}

PakFileSystem :: ~PakFileSystem ()
{
	close ( file );

	delete dir;
}

Data * PakFileSystem :: getFile ( const String& name )
{
	int	size = 0;

	DirEntry * res = locateFile ( name.c_str () );

	if ( res == NULL )
		return NULL;

	void  * ptr = malloc ( res -> size );			// allocate memory from resource

	lseek ( file, res -> offset, SEEK_SET );		// load resource from pak file

	size = read ( file, ptr, res -> size );

	return new Data ( name, ptr, size );
}

DirEntry * PakFileSystem :: locateFile ( const char * name )
{
	for ( int i = 0; i < numDirEntries; i++ )
		if ( !stricmp ( dir [i].fileName, name ) )
			return &dir [i];

	return NULL;
}

