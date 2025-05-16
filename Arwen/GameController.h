//
// Basic class to manage game (or walkthrough)
// Processes user input and rendering of the scene
//
// Author: Alex V. Boreskoff
//
// Last modified: 2/04/2003
//

#ifndef __GAME_CONTROLLER__
#define __GAME_CONTROLLER__

#include	"Controller.h"
#include	"Vector3D.h"

class	Console;
class	ResourceManager;
class	ConsoleCommand;

class    GameController : public Controller
{
protected:
    Vector3D	size;						// size of player (used for collision detection)
	float		mouseSensitivity;			// mouse sensitivity

public:
    GameController ( const char * theName, Model * theModel, View * theView, Camera * theCamera, const Vector3D& defSize );
    ~GameController ();

	void	addConsoleCommand ( ConsoleCommand * com );

	void	setMouseSensitivity ( float val )
	{
		mouseSensitivity = val;
	}

	virtual	InputReader * createInputReader ( View * );

	virtual	int		handleKeyboard ( const KeyboardState& keys  );
	virtual	int		handleMouse    ( const MouseState&    mouse );

	static	MetaClass	classInstance;
};

#endif
