//
// Author: Alex V. Boreskoff
//

#include	"Entities.h"

int	numEntities = 0;
int	numGroups   = 0;

EntityGroup :: EntityGroup ( char * text ) : Object ( "" ), entities ( "" ) 
{
	String	tag;
	String	value;
	int		i = 0;

	numGroups++;

	for ( ; ; i++)
	{
		tag   = "";
		value = "";
								// find starting '"' (of tag)
		while ( text [i] != '\"' && text [i] != '\0' )
			i++;

		if ( text [i] == '\0' )	// end of group
			return;

								// build tag
		for ( i++; text [i] != '\"' && text [i] != '\0'; i++ )
			tag += text [i];

								// find '"' of value
		for ( i++; text [i] != '\"' && text [i] != '\0'; i++ )
			;

		if ( text [i] == '\0' )	// end of group, must be some error in lump
			return;

								// build value
		for ( i++; text [i] != '\"' && text [i] != '\0'; i++ )
			value += text [i];

		addEntity ( tag, value );

		numEntities ++;

		if ( text [i] == '\0' )
			return;
	}
}

void	EntityGroup :: addEntity ( const String& tag, const String& value )
{
	entities.insert ( new Entity ( tag, value ) );
}

const String	EntityGroup :: getValue  ( const String& tag ) const
{
	for ( int i = 0; i < getNumEntites (); i++ )
	{
		Entity * entity = getEntity ( i );

		if ( entity == NULL )
			continue;

		if ( entity -> getTag () == tag )
			return entity -> getValue ();
	}

	return String ( "" );
}

Entities :: Entities ( char * text ) : Object ( "Quake II entites" ), groups ( "" ) 
{
	int	start = 0;

	for ( int i = 0; text [i] != '\0'; i++ )
	{
		if ( text [i] == '}' )
		{
			text [i] = '\0';

			groups.insert ( new EntityGroup ( text + start ) );

			text [i] = '}';
		}
		else
		if ( text [i] == '{' )
			start = i + 1;
	}
}