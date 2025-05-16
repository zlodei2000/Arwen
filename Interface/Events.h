//
// Basic events for windows messages
//
// Author: Alex V. Boreskoff
//
// Last modified: 28/11/2002
//

#ifndef	__EVENTS__
#define	__EVENTS__

#include	"Object.h"

class	Controller;

class	Event : public Object
{
public:
	Event () : Object ( "" )
	{
		metaClass = &classInstance;
	}

	virtual	int	handle ( Controller * ) = 0;

	static	MetaClass	classInstance;
};

class	KeyEvent : public Event
{
protected:
	int		keyCode;				// which key was pressed or released
	bool	pressed;				// whether the key was pressed
public:
	KeyEvent ( int code, bool flag ) : Event ()
	{
		keyCode   = code;
		pressed   = flag;
		metaClass = &classInstance;
	}

	virtual	int	handle ( Controller * );

	static	MetaClass	classInstance;
};

class	CharEvent : public Event
{
protected:
	char	ch;						// which character
public:
	CharEvent ( char theChar ) : Event ()
	{
		ch        = theChar;
		metaClass = &classInstance;
	}

	virtual	int	handle ( Controller * );

	static	MetaClass	classInstance;
};

#endif
