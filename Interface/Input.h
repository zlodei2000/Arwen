//
// Class to hold data about user input (mouse and keyboard)
//
// Author: Alex V. Boreskoff
//
// Last modified: 12/07/2002
//

#ifndef	__INPUT__
#define	__INPUT__

#include	<memory.h>						// for memset

class	KeyboardState
{
private:
	char	keys [256];
public:
	KeyboardState ()
	{
		memset ( keys, '\0', sizeof ( keys ) );
	}

	char * getKeys ()
	{
		return keys;
	}

	bool	isKeyPressed ( int keyCode ) const
	{
		return (keyCode >= 0 && keyCode < sizeof ( keys )) ? (keys [keyCode] != 0) : false;
	}

};

class	MouseState
{
public:
	int		mouseX;
	int		mouseY;
	int		wheel;
	int		mouseFlags;
public:
	MouseState ()
	{
		mouseX     = 0;
		mouseY     = 0;
		wheel      = 0;
		mouseFlags = 0;
	}

	bool	isMouseButtonPressed ( int buttonNo ) const
	{
		return (mouseFlags & (1 << buttonNo)) != 0;
	}

	int		getMouseX () const
	{
		return mouseX;
	}

	int		getMouseY () const
	{
		return mouseY;
	}

	int		getWheel () const
	{
		return wheel;
	}

	void	setState ( int x, int y, int wh, bool b1, bool b2, bool b3, bool b4 )
	{
		mouseX     = x;
		mouseY     = y;
		wheel      = wh;
		mouseFlags = (b1 ? 1 : 0 ) | (b2 ? 2 : 0 ) | (b3 ? 4 : 0) | (b4 ? 8 : 0 );
	}
};

#endif
