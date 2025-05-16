//
// Simple queue class
//
// Author: Alex V. Boreskoff
//
// Last modified: 12/11/2002
//

#ifndef	__QUEUE__
#define	__QUEUE__

#include	"Object.h"
#include	"Array.h"

class	Queue : public Object
{
protected:
	Array	items;

public:
	Queue ( const char * theName ) : Object ( theName ) 
	{
		metaClass = &classInstance;
	}

	bool	isEmpty () const
	{
		return items.isEmpty ();
	}

	void	put ( Object * object )
	{
		items.insert ( object );
	}

	void	putNoRetain ( Object * object )
	{
		items.insertNoRetain ( object );
	}

	Object * get ()
	{
		Object * object = items.at ( 0 );

		if ( object == NULL )
			return NULL;

		object -> retain ();					// since it'll be release in removeAtIndex

		items.removeAtIndex ( 0 );				// remove from array

		return object;
	}

	void	clear ()
	{
		items.removeAll ();
	}

	static	MetaClass	classInstance;
};

#endif
