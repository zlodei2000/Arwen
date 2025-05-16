//
// Class to parser hierarchical config file
//
// Author: Alex V. Boreskoff
//
// Last modified: 3/04/2003
//

#ifndef	__CONFIFG_PARSER__
#define	__CONFIFG_PARSER__

#include	"Object.h"
#include	"ObjStr.h"
#include	"Array.h"

class	Data;

class	ConfigItem : public Object				// class to parse one config item
{
public:
	ConfigItem ( const char * theName ) : Object ( theName ) 
	{
		metaClass = &classInstance;
	}

	virtual	bool	parseString ( const String& str ) = 0;

	static	MetaClass	classInstance;

protected:
	bool	parseValue ( const String& str, bool   * );
	bool	parseValue ( const String& str, int    * );
	bool	parseValue ( const String& str, float  * );
	bool	parseValue ( const String& str, String * );
};

template <class T>
class BasicConfigItem : public ConfigItem
{
private:
	T * ptr;

public:
	BasicConfigItem ( const char * theName, T * thePtr, T defValue ) : ConfigItem ( theName )
	{
		ptr  = thePtr;
		*ptr = defValue;
	}

	virtual	bool	parseString ( const String& str )
	{
		return parseValue ( str, ptr );
	}
};

class	ConfigParser : public Object
{
private:
	Array	items;				// array of ConfigItem's

public:
	ConfigParser () : Object ( "Config Parser" ) 
	{
		metaClass = &classInstance;
	}

							// parse data from source
	bool	parse ( Data * data ) const;
	bool	parse ( const char * fileName ) const;

	void	addItem ( ConfigItem * theItem )
	{
		items.insert ( theItem );
	}

	template <class T>
	void	addItem ( const char * itemName, T * ptr, T defValue )
	{
		items.insertNoRetain ( new BasicConfigItem <T> ( itemName, ptr, defValue ) );
	}

protected:
	void	strip         ( String& str ) const;
	String	buildFullName ( const String& path, const String& name ) const;

	static	MetaClass	classInstance;
};

#endif

