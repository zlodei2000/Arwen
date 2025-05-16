//
// Basic class to handle reading of static data
//
// Author: Alex V. Boreskoff
//
// Last modified: 4/11/2002
//

#ifndef	__DATA__
#define	__DATA__

#include	"Object.h"

class	String;

class	Data : public Object
{
private:
	unsigned char * bits;
	int				length;
	int				pos;
public:
	Data () : Object ( "" )
	{
		bits      = NULL;
		length    = 0;
		pos       = 0;
		metaClass = &classInstance;
	}

	Data ( const char * theName, void * ptr, int len ) : Object ( theName )
	{
		bits      = (unsigned char *) ptr;
		length    = len;
		pos       = 0;
		metaClass = &classInstance;
	}

	virtual	bool	isOk () const
	{
		return bits != NULL;
	}

	bool	isEmpty () const
	{
		return pos >= length;
	}

	bool	isAscii () const;		// whether this object contains text data

	int	getLength () const
	{
		return length;
	}

	int	getByte ()
	{
		if ( pos < length )
			return bits [pos++];
		else
			return -1;
	}

	short	getShort ()
	{
		if ( pos + 1 >= length )
			return -1;

		short 	v = *(short *) (bits + pos);

		pos += 2;

		return v;
	}

	unsigned short	getUnsignedShort ()
	{
		if ( pos + 1 >= length )
			return -1;

		unsigned short 	v = *(unsigned short *) (bits + pos);

		pos += 2;

		return v;
	}

	long getLong ()
	{
		if ( pos + 3 >= length )
			return -1;

		long 	v = *(long *) (bits + pos);

		pos += 4;

		return v;
	}

	unsigned long getUnsignedLong ()
	{
		if ( pos + 3 >= length )
			return -1;

		unsigned long 	v = *(unsigned long *) (bits + pos);

		pos += 4;

		return v;
	}

	void * getPtr () const
	{
		return bits + pos;
	}

	void * getPtr ( int offs ) const
	{
		if ( offs < 0 || offs >= length )
			return NULL;

		return bits + offs;
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
 
	int		getBytes  ( void * ptr, int len );
	bool	getString ( String& str );

	static	Data * getDataFromFile ( const char * fileName );

	static	MetaClass	classInstance;
};

#endif
