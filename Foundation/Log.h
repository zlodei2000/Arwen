//
// Basic log class for logging data
//
// Author: Alex V. Boreskoff
//
// Last modified: 15/11/2001
//

#ifndef	__LOG__
#define	__LOG__

#include	"ObjStr.h"

class	Vector3D;

class	LogManipulator
{
public:
	LogManipulator () {}
};

class	Log : public Object
{
private:
	String	buf;
	String	fileName;
public:
	Log ( const String& theFileName = "" );

	Log& operator << ( const String& str )
	{
		buf += str;

		return *this;
	}

	Log& operator << ( const char * str )
	{
		buf += str;

		return *this;
	}

	Log& operator << ( char ch )
	{
		buf += ch;

		return *this;
	}

	Log& operator << ( bool v )
	{
		if ( v )
			buf += "true";
		else
			buf += "false";

		return *this;
	}

	Log& operator << ( int v );
	Log& operator << ( float v );
	Log& operator << ( const Vector3D& v );
	Log& operator << ( const LogManipulator& man );

	static	MetaClass	classInstance;

protected:
	void	flush ();				// flushes buf to log, called on logEndl;
};

extern const LogManipulator	  logEndl;
extern		 Log            * sysLog;

#endif
