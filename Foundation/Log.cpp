//
// Basic log class for logging data
//
// Author: Alex V. Boreskoff
//
// Last modified: 6/12/2001
//

#include	<stdio.h>
#include	<windows.h>
#include	<time.h>
#include	"Log.h"
#include	"Vector3D.h"

const LogManipulator	logEndl;

MetaClass	Log :: classInstance ( "Log", &Object :: classInstance );

Log :: Log ( const String& theFileName ) : Object ( theFileName ), fileName ( theFileName )
{
	time_t now;   
   
	time ( &now );

	struct tm when = *localtime( &now );

	(*this) << logEndl;
	(*this) << "******************************* App start *******************************" << logEndl << logEndl;
	(*this) << "Started " << asctime ( &when ) << logEndl << logEndl;
}

Log& Log :: operator << ( int v )
{
	char	str [20];

	sprintf_s ( str, "%d", v );

	buf += str;

	return *this;
}

Log& Log :: operator << ( float v )
{
	char	str [128];

	sprintf_s ( str, "%f", v );

	buf += str;

	return *this;
}

Log& Log :: operator << ( const Vector3D& v )
{
	char	str [128];

	sprintf_s ( str, "(%f %f %f)", v.x, v.y, v.z );

	buf += str;

	return *this;
}

Log& Log :: operator << ( const LogManipulator& man )
{
	flush ();

	buf = "";

	return *this;
}

void	Log :: flush ()
{
	String	str = buf + "\n";

	OutputDebugString ( str );

	if ( fileName != "" )
	{
		FILE * file = fopen ( fileName, "at" );

		fputs  ( str, file );
		fclose ( file );
	}
}

