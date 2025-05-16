//
// Simple stack class 
//
// Author: Alex V. Boreskoff
//
// Last modified: 2/12/2002
//

#ifndef	__STACK__
#define	__STACK__

#include	"Object.h"
#include	"Array.h"

class	Stack : public Object
{
protected:
	Array	items;

public:
	Stack ( const char * theName ) : Object ( theName ) 
	{
		metaClass = &classInstance;
	}

	bool	isEmpty () const
	{
		return items.isEmpty ();
	}

	void	push ( Object * object )
	{
		items.insert ( object );
	}

	Object * pop ()
	{
		int	index = items.getNumItems () - 1;

		if ( index < 0 )
			return NULL;

		Object * object = items.at ( index );

		if ( object == NULL )
			return NULL;

		object -> retain ();					// since it'll be release in removeAtIndex

		items.removeAtIndex ( index );

		return object;
	}

	static	MetaClass	classInstance;
};

#endif

