//
// Basic class to manage game (or walkthrough)
// Processed user input and redndreing of the scene
//
// Author: Alex V. Boreskoff
//
// Last modified: 27/12/2001
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
    Vector3D   size;					// size of player (used for collision detection)
	Vector3D   velocity;				// player's velocity
	float	   mouseSensitivity;		// mouse sensitivity
	float	   forwardSpeed;			// speed for moving forward
	float	   backwardSpeed;
	float	   sideSpeed;
											// here goes the keys: 2 keys for each function
    int        forwardKey, forwardKey2;
    int        backwardKey, backwardKey2;
    int        leftKey, leftKey2;
    int        rightKey, rightKey2;
    int        jumpKey, jumpKey2;
    int        crouchKey, crouchKey2;
	int		   strafeKey, strafeKey2;
	int		   strafeLeftKey, strafeLeftKey2;
	int		   strafeRightKey, strafeRightKey2;
	int		   runKey, runKey2;
	int		   fireKey, fireKey2;
	int		   useKey, useKey2;
public:
    GameController ( const char * theName, Model * theModel, View * theView, Camera * theCamera, const Vector3D& defSize );
    ~GameController ();

    virtual const char * getClassName () const
    {
        return "GameController";
    }

	void	addConsoleCommand ( ConsoleCommand * com );

	void	setForwardKey  ( int key );
	void	setBackwardKey ( int key );
	void	setLeftKey     ( int key );
	void	setRightKey    ( int key );
	void	setStrafeKey   ( int key );
	void	setRunKey      ( int key );
	void	setStrafeLeft  ( int key );
	void	setStrafeRight ( int key );
	void	setFireKey     ( int key );
	void	setUseKey      ( int key );

	virtual	InputReader * createInputReader ( View * );

	virtual	int		handleKeyboard ( const KeyboardState& keys  );
	virtual	int		handleMouse    ( const MouseState&    mouse );

	static MetaClass classInstance;
};

#endif
