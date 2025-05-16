//
// Basic class to handle writing of MutableData, g.e. building 
// persistent images of objects
//
// Author: Alex V. Boreskoff
//
// Last modified: 20/11/2002
//

#include	<malloc.h>
#include	<io.h>
#include	<fcntl.h>
#include	<sys/stat.h>

#include	"MutableData.h"
#include	"ObjStr.h"

MetaClass	MutableData :: classInstance ( "MutableData", &Object :: classInstance );

MutableData :: MutableData ( const char * theName, int len ) : Object ( theName )
{
	bits      = (unsigned char *) malloc ( len );
	length    = len;
	pos       = 0;
	metaClass = &classInstance;
}

MutableData :: ~MutableData ()
{
	if ( bits != NULL )
		free ( bits );
}

bool	MutableData :: saveToFile ( const String& str ) const
{
	int	file = open ( str, O_WRONLY | O_BINARY | O_CREAT, S_IWRITE );

	if ( file == -1 )
		return false;

	int	bytesWritten = write ( file, bits, pos );

	close ( file );

	return bytesWritten == pos;
}

bool	MutableData :: putBytes  ( const void * ptr, int len )
{
	if ( pos + len >= length )
		if ( !realloc ( pos + len ) )
			return false;

	memcpy ( bits + pos, ptr, len );

	pos += len;

	return true;
}

bool	MutableData :: putString ( const String& str )
{
	return putBytes ( str.c_str (), str.getLength () + 1 );
}

bool	MutableData :: realloc ( int newSize )
{
	unsigned char * ptr = (unsigned char *) :: realloc ( bits, newSize );

	if ( ptr == NULL )
		return false;

	bits   = ptr;
	length = newSize;

	return true;
}

