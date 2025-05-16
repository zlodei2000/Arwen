//
// Simple file utils
//

#include	<sys/types.h>
#include	<sys/stat.h>
#include	<direct.h>

#include	"FileUtils.h"

bool	fileExist ( const String& name )
{
	struct	_stat	info;

	return _stat ( name, &info ) != -1;
}

bool	isDir     ( const String& name )
{
	struct	_stat	info;

	if ( _stat ( name, &info ) == -1 )		// no object
		return false;

	return (info.st_mode & _S_IFDIR) != 0;
}

bool	createDir ( const String& name )
{
	if ( isDir ( name ) )
		return true;

	return _mkdir ( name ) != -1;
}

