//
// Basic class for implementing console commands
//
// Author: Alex V. Boreskoff
//
// Last modified: 27/12/2001
//

#ifndef	__CONSOLE_COMMAND__
#define	__CONSOLE_COMMAND__

#include	"Object.h"

class	Array;
class	Console;

class	ConsoleCommand : public Object
{
public:
	ConsoleCommand ( const char * theName ) : Object ( theName ) {}

	virtual	void	execute   ( const Array& argv, Console * console ) {}
	virtual	void	printHelp ( Console * console ) const {}

	static	MetaClass	classInstance;
};

#endif
