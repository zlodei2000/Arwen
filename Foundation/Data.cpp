//
// Basic class to handle reading of static data
//
// Author: Alex V. Boreskoff
//
// Last modified: 1/04/2003
//

#include	<fcntl.h>
#include	<io.h>
#include	<malloc.h>
#include	<ctype.h>

#include	"Data.h"
#include	"ObjStr.h"

MetaClass	Data :: classInstance ( "Data", &Object :: classInstance );

bool	Data :: isAscii () const
{
	for ( int i = 0; i < length; i++ )
	{
		unsigned	ch = (unsigned) bits [i];

											// all chars ASCII are ok
		if ( ch >= 32 && ch < 128 )
			continue;

											// these are ok also
		if ( ch == '\n' || ch == '\r' || ch == '\t' )
			continue;

		if ( isalpha ( ch ) )					// check for locale characters
			continue;

		return false;
	}

	return true;
}

int	Data :: getBytes ( void * ptr, int len )
{
	if ( pos >= length )
		return -1;

	if ( pos + len > length )
		len = length - pos;

	memcpy ( ptr, bits + pos, len );

	pos += len;

	return len;
}

bool	Data :: getString ( String& str )
{
	char	buf [256];
	int		i = 0;

	str = "";

	if ( isEmpty () )
		return false;

	for ( ; pos < length && bits [pos] != '\n' && bits [pos] != '\0'; pos++ )
	{
		buf [i++] = bits [pos];

		if ( i >= sizeof ( buf ) - 1 )			// overflow
		{
			buf [i] = '\0';

			str += buf;

			i = 0;
		}
	}

	if ( pos < length && bits [pos] == '\n' )	// skip '\n'
		pos++;

	buf [i] = '\0';

	str += buf;

	return true;
}

Data * Data :: getDataFromFile ( const char * fileName )
{
	int	file = _open ( fileName, O_RDONLY | O_BINARY );

	if ( file == -1 )
		return NULL;

	long	len = _filelength ( file );

	if ( len == -1 )
	{
		_close ( file );

		return NULL;
	}

	char * buf = (char *) malloc ( len );

	if ( buf == NULL )
	{
		_close ( file );

		return NULL;
	}

	Data * data = new Data ( fileName, buf, _read ( file, buf, len ) );

	_close ( file );

	return data;
}
