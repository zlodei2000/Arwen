//
// Basic class to read input from devices (mouse, keyboard & etc)
//
// Author: Alex V. Boreskoff
//
// Last modified: 12/07/2002
//

#ifndef	__INPUT_READER__
#define	__INPUT_READER__

#include	"Object.h"

class	KeyboardState;
class	MouseState;
class	View;

class	InputReader : public Object
{
public:
	InputReader ( View *, const char * theName ) : Object ( theName ) {}

	virtual	bool	getKeyboardState ( KeyboardState& ) = 0;
	virtual	bool	getMouseState    ( MouseState&    ) = 0;
};

#endif

