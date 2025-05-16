//
// Class to parser hierarchical config file
//
// Author: Alex V. Boreskoff
//
// Last modified: 2/04/2003
//

#include	"ConfigParser.h"
#include	"Data.h"
#include	"Log.h"

MetaClass	ConfigItem   :: classInstance ( "ConfigItem",   &Object :: classInstance );
MetaClass	ConfigParser :: classInstance ( "ConfigParser", &Object :: classInstance );

bool	ConfigItem :: parseValue ( const String& str, bool * ptr )
{
	if ( str == "yes" || str == "on" || str == "true" )
	{
		* ptr = true;

		return true;
	}

	if ( str == "no" || str == "off" || str == "false" )
	{
		* ptr = false;

		return true;
	}

	return false;
}

bool	ConfigItem :: parseValue ( const String& str, int * ptr )
{
	if ( !str.isInt () )
		return false;

	*ptr = str.toInt ();

	return true;
}

bool	ConfigItem :: parseValue ( const String& str, float * ptr )
{
	if ( !str.isFloat () )
		return false;

	*ptr = str.toFloat ();

	return true;
}

bool	ConfigItem :: parseValue ( const String& str, String * ptr )
{
	* ptr = str;

	return true;
}

bool	ConfigParser :: parse ( Data * data ) const
{
	String	str;
	String	prev;
	String	curPath;
	String	cmd;
	String	args;
	int		level = 0;
	bool	res   = true;

	(*sysLog) << "ConfigParser: parsing " << data -> getName () << logEndl << logEndl;

	for ( int line = 1; data -> getString ( str ); line++ )
	{
		strip ( str );

		if ( str.isEmpty () )
			continue;

		if ( str == "{" )						// open next level
		{
			if ( prev.isEmpty () )
			{
				res = false;

				(*sysLog) << "ConfigParser: line " << line << " - empty level name" << logEndl;
			}

			level++;

			curPath = buildFullName ( curPath, prev );
			prev    = "";
		}
		else
		if ( str == "}" )						// close current level
		{
			if ( level < 1 )
			{
				res = false;

				(*sysLog) << "ConfigParser: line " << line << " - extra } command" << logEndl;
			}
			else
			{
				level--;

												// cut off last component
				int	pos = curPath.findLast ( '/' );

				if ( pos != -1 )
					curPath.setLength ( pos );
				else
					curPath.setLength ( 0 );	// reached top
			}

			prev = "";
		}
		else
		{
			String :: parseString ( str, cmd, args );

			ConfigItem * item = (ConfigItem *) items.getObjectWithName ( buildFullName ( curPath, cmd ) );

			if ( item != NULL )
				if ( ! item -> parseString ( args.dequote () ) )
				{
					res = false;

					(*sysLog) << "ConfigParser: line " << line << " - error parsing string \'" << str << "\'" << logEndl;
				}

			prev = str;
		}
	}

	(*sysLog) << "ConfigParser: done parsing " << data -> getName () << logEndl;

	return res;
}

bool	ConfigParser ::parse ( const char * fileName ) const
{
	Data * data = Data :: getDataFromFile ( fileName );

	if ( data == NULL )
	{
		(*sysLog) << "ConfigParser: Cannot open " << fileName << logEndl;

		return false;
	}

	bool	res = parse ( data );

	data -> release ();

	return res;
}

void	ConfigParser :: strip ( String& str ) const
{
	int	pos = str.find ( "#" );

	if ( pos != -1 )
		str.setLength ( pos );

	str.trim ();

	if ( ( pos = str.find ( ";" ) ) != -1 )
		str.setLength ( pos );

	str.trim ();
}

String	ConfigParser :: buildFullName ( const String& path, const String& name ) const
{
	if ( path.isEmpty () )
		return name;

	return path + '/' + name;
}
