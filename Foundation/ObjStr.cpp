//
// Basic string class based on Object class
//
// Author: Alex V. Boreskoff
//
// Last modified: 25/11/2002
//

#include	<ctype.h>
#include	<stdarg.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	"ObjStr.h"
#include	"Array.h"

const	int		STR_BLOCK_SIZE = 32;			// string data is alocated by STR_BLOCK_SIZE chunks
const	char	pathSeparator  = '\\';

MetaClass	String :: classInstance ( "String", &Object :: classInstance );

String :: String () : Object ( "" )
{
	contents     = new char [STR_BLOCK_SIZE];
	length       = 0;
	maxLength    = STR_BLOCK_SIZE;
	contents [0] = '\0';
	metaClass    = &classInstance;
}

String :: String ( const char * val ) : Object ( "" )
{
	length    = strlen ( val );
	maxLength = STR_BLOCK_SIZE * ((length + 1 + STR_BLOCK_SIZE - 1) / STR_BLOCK_SIZE);
	contents  = new char [maxLength];
	metaClass = &classInstance;

	strcpy ( contents, val );
}

String :: String ( const String& str ) : Object ( str.getName () )
{
	length    = str.length;
	maxLength = str.maxLength;
	contents  = new char [maxLength];
	metaClass = &classInstance;

	strcpy ( contents, str.contents );
}

String :: String ( int value ) : Object ( "" )
{
	char	buf [256];

	_itoa ( value, buf, 10 );

	maxLength = STR_BLOCK_SIZE*((strlen ( buf ) + STR_BLOCK_SIZE)/STR_BLOCK_SIZE);
	contents  = new char [maxLength];
	length    = strlen ( strcpy ( contents, buf ) );
	metaClass = &classInstance;
}

String :: String ( float value ) : Object ( "" )
{
	int	   dec;
	int	   sign;
	char * ptr = _ecvt( (double)value, 10, &dec, &sign );

	maxLength = STR_BLOCK_SIZE*((strlen ( ptr ) + STR_BLOCK_SIZE)/STR_BLOCK_SIZE);
	contents  = new char [maxLength];
	length    = strlen ( strcpy ( contents, ptr ) );
	metaClass = &classInstance;
}

int	String :: compare ( const Object * obj ) const
{
	int	res = Object :: compare ( obj );

	if ( res != 0 )
		return res;

	String * str = (String *) obj;

	return strcmp ( contents, str -> contents );
}

String& String :: operator = ( const String& str )
{
	if ( &str == this )
		return *this;

	delete contents;

	maxLength = str.maxLength;
	length    = str.length;
	contents  = new char [maxLength];

	strcpy ( contents, str.contents );

	return *this;
}

String& String :: operator = ( char ch )
{
	contents [0] = ch;
	contents [1] = '\0';
	length       = 1;

	return *this;
}

String& String :: operator += ( char ch )
{
	realloc ( length + 1 );

	contents [length++] = ch;
	contents [length]   = '\0';

	return *this;
}

int	String :: toInt () const
{
	return atoi ( contents );
}

float	String :: toFloat () const
{
	return (float)atof ( contents );
}

double	String::toDouble() const
{
	return (double)atof(contents);
}


void	String :: realloc ( int newLength )
{
	int	newMaxLength = STR_BLOCK_SIZE * ((newLength + 1 + STR_BLOCK_SIZE - 1) / STR_BLOCK_SIZE);

	if ( newMaxLength <= maxLength )
		return;

	char * newBuf = new char [maxLength = newMaxLength];

	strcpy ( newBuf, contents );

	delete contents;

	contents = newBuf;
}

void	String :: append ( const char * str )
{
	realloc ( length + strlen ( str ) );
	strcat  ( contents, str );

	length = strlen ( contents );
}

String& String :: setLength ( int newLength )
{
	if ( newLength < 0 )
		newLength = 0;

	if ( newLength > length )
		newLength = length;

	contents [length = newLength] = '\0';

	return *this;
}

String String :: substr ( int pos, int len ) const
{
	if ( pos < 0 )							// go backwards from the end of the string
	{										// a'la Python (g.e substr ( -1 ) == last char
		pos += length;

		if ( pos < 0 )
			pos = 0;
	}

	if ( pos > length )
		return String ( "" );

	if ( len == toTheEnd )					// return substring from the pos to the end
		return String ( contents + pos );

	if ( len >= 0 )							// normal substring
	{
		String	res ( contents + pos );

		return res.setLength ( len );
	}

											// len < 0 -> it's Python-like string, 
											// we're taking chars before the pos
	int	upTo = length + pos;

	if ( upTo < 0 )
		return String ();

	len = upTo - pos;						// adjust len

	String	res ( contents + pos );

	res.setLength ( len );

	return res;
}

String& String :: removeLeadingSpaces  ()
{
	int i;
	for ( i = 0; i < length && isspace ( contents [i] ); i++ )
		;

	length = strlen ( strcpy ( contents, contents + i ) );

	return *this;
}

String& String :: removeTrailingSpaces ()
{
	int i;
	for ( i = length - 1; i >= 0 && isspace ( contents [i] ); i-- )
		;

	contents [i+1] = '\0';

	return *this;
}


String& String :: trim ()
{
	removeTrailingSpaces ();
	removeLeadingSpaces  ();

	return *this;
}

String&	String :: dequote ()
{
	if ( contents [0] == '\"' || contents [0] == '\'' )			// remove quotes
		*this = substr ( 1, getLength () - 2 );

	return *this;
}

String&	String :: cut ( int from, int count )
{
	if ( from > length )
		from = length;

	if ( from < 0 )
		from = 0;

	if ( from + count >= length )
		count = toTheEnd;

	if ( count == toTheEnd )		// cut to the end of string
		contents [from] = '\0';
	else
		strcpy ( contents + from, contents + from + count );

	length = strlen ( contents );

	return *this;
}

Array * String :: split ( const String& sep ) const
{
	String	temp ( *this );
	Array * arr = new Array;
	char  * ptr = strtok ( temp.contents, sep.contents );

	while ( ptr != NULL )
	{
		arr -> insertNoRetain ( new String ( ptr ) );

		ptr = strtok ( NULL, sep.contents );
	}

	return arr;
}

int		String :: find  ( const String& pattern, int start, int options ) const
{
	if ( start >= length )
		return -1;

	if ( options == 0 )
	{
		char * ptr = strstr ( contents + start, pattern.contents );

		if ( ptr == NULL )
			return -1;

		return ptr - contents;
	}
	else
	if ( options == STR_SEARCH_NO_CASE )
	{
		for ( int i = start, j = 0; contents [i] != '\0'; i++ )
			if ( pattern.contents [j] == '\0' )
				return start;
			else
			if ( toupper ( (unsigned char) contents [i] ) == toupper ( (unsigned char) pattern.contents [j] ) )
				j++;
			else
			{
				i = start;
				j = 0;
			}

			return -1;
	}

	return -1;					// ilegal options
}

int		String :: find  ( char ch, int start, int options ) const
{
	if ( start >= length )
		return -1;

	if ( options == 0 )
	{
		char * ptr = strchr ( contents + start, ch );

		if ( ptr == NULL )
			return -1;

		return ptr - contents;
	}
	else
	if ( options == STR_SEARCH_NO_CASE )
	{
		ch = toupper ( ch );

		for ( int i = start; contents [i] != '\0'; i++ )
			if ( toupper ( (unsigned char) contents [i] ) == ch )
				return i;
	}

	return -1;					// illegal options
}

int		String :: findLast ( char ch ) const
{
	char * ptr = strrchr ( contents, ch );

	if ( ptr == NULL )
		return -1;

	return ptr - contents;
}

String&	String :: replace ( int start, int len, const String& str )
{
	if ( start > length )
		start = length;

	String	temp ( substr ( 0, start ) );

	temp += str;
	temp += substr ( start + len );

	return *this = temp;
}

String&	String :: insert ( int pos, char ch )
{
	if ( pos >= length )
		return *this += ch;

	String	temp ( substr ( 0, pos ) );

	temp += ch;
	temp += substr ( pos );

	return *this = temp;
}

String&	String :: insert ( int pos, const String& str )
{
	if ( pos >= length )
		return *this += str;

	String	temp ( substr ( 0, pos ) );

	temp += str;
	temp += substr ( pos );

	return *this = temp;
}

bool	String ::isAlpha () const
{
	for ( char * ptr = contents; *ptr != '\0'; ptr++ )
		if ( !isalpha ( (unsigned char) *ptr ) )
			return false;

	return true;
}

bool	String :: isAlNum () const
{
	for ( char * ptr = contents; *ptr != '\0'; ptr++ )
		if ( !isalnum ( (unsigned char) *ptr ) )
			return false;

	return true;
}

bool	String :: isDigits () const
{
	for ( char * ptr = contents; *ptr != '\0'; ptr++ )
		if ( !isdigit ( (unsigned char) *ptr ) )
			return false;

	return true;
}

bool	String :: isHexDigits () const
{
	for ( char * ptr = contents; *ptr != '\0'; ptr++ )
		if ( !isxdigit ( (unsigned char) *ptr ) )
			return false;

	return true;
}

bool	String :: isInt () const
{
	char * ptr = contents;

	if ( *ptr == '+' )
		ptr++;
	else
	if ( *ptr == '-' )
		ptr++;

	for ( ; *ptr != '\0'; ptr++ )
		if ( !isdigit ( *ptr ) )
			return false;

	return true;
}

bool	String :: isFloat () const
{
	char  * ptr = contents;
	char  * end;
	double	res = strtod ( ptr, &end );			// use standard routine for conversion

	return *end == '\0';						// ok if we stopped at end of string
}

String	String :: getExtension ( const String& fileName )
{
	if ( fileName.getLength () < 1 )
		return String ( "" );

	for ( int i = fileName.getLength () - 1; i >= 0; i-- )
	{
		char	ch = fileName [i];

		if ( ch == '.' )
			return String ( fileName.c_str () + i + 1 );

		if ( ch == '\\' || ch == '/' || ch == ':' )
			return String ( "" );
	}

	return String ( "" );
}

void	String :: setExtension ( String& fileName, const String& ext )
{
	for ( int i = fileName.getLength () - 1; i >= 0; i-- )
	{
		char	ch = fileName [i];

		if ( ch == '.' )
		{
			fileName = fileName.substr ( 0, i + 1 ) += ext;

			return;
		}

		if ( ch == '\\' || ch == '/' || ch == ':' )
		{
			fileName += '.';
			fileName += ext;

			return;
		}
	}

	fileName += '.';
	fileName += ext;
}

String	String :: getPath ( const String& fullName )
{
	for ( int i = fullName.getLength () - 1; i >= 0; i-- )
	{
		char	ch = fullName [i];

		if ( ch == '\\' || ch == '/' || ch == ':' )
			return fullName.substr ( 0, i + 1 );
	}

	return String ( "" );
}

String	String :: getFileName ( const String& fullName )
{
	for ( int i = fullName.getLength () - 1; i >= 0; i-- )
	{
		char	ch = fullName [i];

		if ( ch == '\\' || ch == '/' || ch == ':' )
			return fullName.substr ( i + 1 );
	}

	return fullName;
}

String	String :: buildFileName ( const String& path, const String& name )
{
	if ( path == "" )
		return name;

	int	pos = path.getLength () - 1;

	String	res ( path );

	if ( path [pos] == '\\' || path [pos] == '/' || path [pos] == ':' )
		res += name;
	else
	{
		res += pathSeparator;
		res += name;
	}

	for ( int i = 0; i < res.getLength (); i++ )
		if ( res [i] == '/' )
			res [i] = pathSeparator;

	return res;
}

String String :: printf ( const char * str, ... )
{
	char	buf [1024];
	va_list	args;
	bool	ok;

	va_start ( args, str );

	ok = vsprintf ( buf, str, args ) < sizeof ( buf );

	va_end ( args );

	if ( ok )
		return String ( buf );

						// try use dynamic memory allocation
	int	   size = 1024*4;
	char * ptr;

	while ( size < (1 << 20) )
	{
		ptr = (char *) malloc ( size );

		if ( ptr == NULL )
			break;

		va_start ( args, str );

		ok = vsprintf ( ptr, str, args ) < size;

		va_end ( args );

		if ( ok )
		{
			String	res ( buf );

			free ( ptr );

			return res;
		}

		free ( ptr );

		size *= 2;
	}
	
	return String ();
}

void	String :: parseString ( const String& str, String& cmd, String& args )
{
	int pos;
	for ( pos = 0; pos < str.getLength () && str [pos] != ' ' && str [pos] != '\t'; pos++ )
		;

	cmd  = str.substr ( 0, pos );
	args = str.substr ( pos ).trim ();
}

