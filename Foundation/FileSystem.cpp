//
// Basic model of file system - source of named data
//
// Author: Alex V. Boreskoff
// 
// Last modified: 15/11/2002
//

#include	<malloc.h>
#include	<fcntl.h>
#include	<io.h>

#include	"FileSystem.h"
#include	"Log.h"
#include	"Data.h"

MetaClass	OsFileSystem :: classInstance ( "OsFileSystem", &ResourceSource :: classInstance );

OsFileSystem :: OsFileSystem ( const String& thePath ) : ResourceSource ( "OsFileSystem" ), path ( thePath )
{
	(*sysLog) << "Created resource source " << thePath << logEndl;
}

void	OsFileSystem :: freeFile ( void * ptr )
{
	free ( ptr );
}

Data  * OsFileSystem :: getFile  ( const String& name )
{
	String	fileName = String :: buildFileName ( path, name );

	int		file = _open ( fileName, O_RDONLY | O_BINARY );
	void  * ptr  = NULL;
	int		size = 0;

	if ( file == -1 )
		return NULL;

	size = _filelength ( file );
	ptr  = malloc     ( size );

	if ( ptr == NULL )
	{
		_close ( file );

		size = 0;

		return NULL;
	}

	if ( _read ( file, ptr, size ) != size )
	{
		free ( ptr );

		size = 0;
		ptr  = NULL;
	}

	_close ( file );

	return new Data ( name, ptr, size );
}

