//
// Basic class to handle writing of MutableData, g.e. building 
// persistent images of objects
//
// Author: Alex V. Boreskoff
//
// Last modified: 20/11/2002
//

#ifndef	__MUTABLE_DATA__
#define	__MUTABLE_DATA__

#include	"Object.h"

class	String;

class	MutableData : public Object
{
private:
	unsigned char * bits;
	int				length;
	int				pos;
public:
	MutableData () : Object ( "" )
	{
		bits   = NULL;
		length = 0;
		pos    = 0;
	}

	MutableData ( const char * theName, int len = chunkSize );

	~MutableData ();

	bool	isEmpty () const
	{
		return length < 1;
	}

	int	getLength () const
	{
		return length;
	}

	bool	putByte ( int byte )
	{
		if ( pos >= length )
			if ( !realloc ( length + chunkSize ) )
				return false;

		bits [pos++] = (unsigned char) byte;

		return true;
	}

	bool	putShort ( short val )
	{
		if ( pos + 1 >= length )
			if ( !realloc ( length + chunkSize ) )
				return false;

		*(short *) &bits [pos] = val;
		pos                   += 2;

		return true;
	}

	bool	putLong ( long val )
	{
		if ( pos + 3 >= length )
			if ( !realloc ( length + chunkSize ) )
				return false;

		*(long *) &bits [pos] = val;
		pos                   += 4;

		return true;
	}

	int	seekCur ( int delta )
	{
		pos += delta;

		if ( pos > length )
			pos = length;

		if ( pos < 0 )
			pos = 0;

		return pos;
	}

	int	seekAbs ( int offs )
	{
		pos = offs;

		if ( pos > length )
			pos = length;

		if ( pos < 0 )
			pos = 0;

		return pos;
	}
 
	bool	putBytes   ( const void * ptr, int len );
	bool	putString  ( const String& str );
	bool	saveToFile ( const String& str ) const;

	static	MetaClass	classInstance;

protected:
	bool	realloc ( int newSize );

	enum
	{
		chunkSize = 50*1024				// use 50K increments
	};
};

#endif
