//
// Timer class interface
// Used to measure time intervals with maximum precision.
// Can be paused/resumed.
//
// Author: Alex V. Boreskoff
//
// Last modified: 08/07/2002
//

#ifndef	__TIMER__
#define	__TIMER__

#include	"Object.h"

class	Timer : public Object
{
public:
	Timer ( const char * theName ) : Object ( theName ) 
	{
		metaClass = &classInstance;
	}

	virtual float	getTime () const = 0;		// get elapsed time since timer's creation
	virtual	void	pause   ()       = 0;		// pause timer, can be nested
	virtual	void	resume  ()       = 0;		// resume timer, can be nested

	static	MetaClass	classInstance;
};

Timer * getTimer ( const char * theName = "" );	// factory method for getting timers

#endif

