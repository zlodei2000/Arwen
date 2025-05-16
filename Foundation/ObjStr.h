//
// Basic string class based on Object class
//
// Author: Alex V. Boreskoff
//
// Last modified: 25/11/2002
//

#ifndef	__OBJECT_STRING__
#define	__OBJECT_STRING__

#include	<string.h>
#include	"Object.h"

#define	STR_SEARCH_NO_CASE	1		// do case insensitive search in find and replace

class	Array;

class	String : public Object
{
private:
	char  * contents;
	int		length;					// actual length of string
	int		maxLength;				// size of preallocated buffer
public:
	String ();
	String ( const char *  value );
	String ( const String& str   );
	String ( int   value );
	String ( float value );

	~String ()
	{
		delete contents;
	}

    virtual bool isOk () const			// returns non-zero if objects is ok
    {
        return contents != NULL && maxLength > 0 && length < maxLength;
    }

	virtual	bool	isNull () const
	{
		return contents == NULL;
	}

    virtual long	hash () const		// return hash for object
    {
        return crc32 ( contents, length );
    }
                                        // returns  non-zero if this object is
                                        // equal to the object obj
    virtual int compare ( const Object * obj ) const;

	virtual	Object * clone () const
	{
		return new String ( contents );
	}

	int	caseInsensitiveCompare ( const String& str ) const
	{
		return _stricmp ( contents, str.contents );
	}

	operator const char * () const
	{
		return contents;
	}

	const char * c_str () const
	{
		return contents;
	}

	String& operator = ( const String& str );
	String& operator = ( char ch );

	String& operator += ( const String& str )
	{
		append ( str.contents );

		return *this;
	}

	String& operator += ( const char * str )
	{
		append ( str );

		return *this;
	}

	String& operator += ( char ch );

	String  operator +  ( const String& str ) const
	{
		String	s ( *this );

		s.append ( str.contents );

		return s;
	}

	String  operator +  (const char * str ) const
	{
		String	s ( *this );

		s.append ( str );

		return s;
	}

	String  operator +  (char ch ) const
	{
		String	s ( *this );

		return s += ch;
	}

	bool	operator == ( const String& str ) const
	{
		return strcmp ( contents, str.contents ) == 0;
	}

	bool	operator != ( const String& str ) const
	{
		return strcmp ( contents, str.contents ) != 0;
	}

	bool	operator < ( const String& str ) const
	{
		return strcmp ( contents, str.contents ) < 0;
	}

	bool	operator <= ( const String& str ) const
	{
		return strcmp ( contents, str.contents ) <= 0;
	}

	bool	operator > ( const String& str ) const
	{
		return strcmp ( contents, str.contents ) > 0;
	}

	bool	operator >= ( const String& str ) const
	{
		return strcmp ( contents, str.contents ) >= 0;
	}

	bool	operator == ( const char * str ) const
	{
		return strcmp ( contents, str ) == 0;
	}

	bool	operator != ( const char * str ) const
	{
		return strcmp ( contents, str ) != 0;
	}

	bool	operator < ( const char * str ) const
	{
		return strcmp ( contents, str ) < 0;
	}

	bool	operator <= ( const char * str ) const
	{
		return strcmp ( contents, str ) <= 0;
	}

	bool	operator > ( const char * str ) const
	{
		return strcmp ( contents, str ) > 0;
	}

	bool	operator >= ( const char * str ) const
	{
		return strcmp ( contents, str ) >= 0;
	}

	char&	operator [] ( int index ) const
	{
		return contents [index];
	}

	int	getLength () const
	{
		return length;
	}

    bool isEmpty () const
    {
        return length < 1;
    }

	String&	toUpper ()
	{
		_strupr ( contents );

		return *this;
	}

	String& toLower ()
	{
		_strlwr ( contents );

		return *this;
	}

	int		toInt     () const;
	float	toFloat   () const;
    double  toDouble  () const;

	bool	isAlpha     () const;
	bool	isAlNum     () const;
	bool	isDigits    () const;
	bool	isHexDigits () const;
	bool	isInt       () const;
	bool	isFloat     () const;

	String  substr    ( int from, int count = toTheEnd ) const;
	String&	setLength ( int );
	String&	cut       ( int from, int count = toTheEnd );
	String&	trim      ();
	String&	dequote   ();

	String& removeLeadingSpaces  ();
	String& removeTrailingSpaces ();

	Array * split    ( const String& sep = " \t" ) const;
	int		find     ( const String& pattern, int start = 0, int options = 0 ) const;
	int		find     ( char ch, int start = 0, int options = 0 ) const;
	int		findLast ( char ch ) const;

	String&	replace ( int start, int len, const String& str );
	String&	insert  ( int pos, char ch );
	String&	insert	( int pos, const String& str );

	static	String	getExtension  ( const String& fileName );
	static	void	setExtension  ( String& fileName, const String& ext );
	static	String	getPath       ( const String& fullName );
	static	String	getFileName   ( const String& fullName );
	static	String	buildFileName ( const String& path, const String& name );
    static	String  printf        ( const char * format, ... );
	static	void	parseString   ( const String& str, String& cmd, String& args );

	enum
	{
		toTheEnd = 0x80000000		// operation up to the end of string
	};

	static	MetaClass	classInstance;

private:
	void	realloc ( int newLen );
	void	append  ( const char * );
};

#endif
