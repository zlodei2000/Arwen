//
// Basic class to handle keys (WM_CHAR messages), key pressing/releasing
// and mouse/joystick movements
//
// Author: Alex V. Boreskoff
//
// Last modified: 6/12/2002
//

#ifndef	__CONTROLLER__
#define	__CONTROLLER__

#include	"Object.h"
#include	"Vector3D.h"
#include	"Keys.h"

#define	controllerQuit				-1
#define	controllerContinue			0

#define	CONTROLLER_SET_UNKNOWN_VAR	1
#define	CONTROLLER_SET_ILLEGAL_VAL	2

class	KeyboardState;
class	MouseState;
class	InputReader;
class	View;
class	Timer;
class	Model;
class	String;
class	Font;
class	Camera;
class	Console;

class	Controller : public  Object
{
protected:
	Model           * model;					// model we're rendering
	View            * view;						// view, we're using for rendering
	Camera          * camera;					// camera used for rendering
	Timer           * timer;
	Console         * console;
	Font            * font;
	int		          numFrames;				// # of rendered frames
	float	          lastFrameTimes [8];		// time of processing of the last 8 frames
	float	          fps;
	Vector3D		  pos;						// position of player
	float			  yaw, pitch, roll;			// euler angles of player
	bool			  resourcesUploaded;
	Vector3D		  gravity;					// gravity vector
	Vector3D		  velocity;
	float			  forwardSpeed;				// speed for moving forward
	float		      backwardSpeed;
	float		      strafeSpeed;				// speed for strafe
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
	Controller  ( const char * theName, Model * theModel, View * theView, Camera * theCamera );
	~Controller ();

	Timer * getTimer () const
	{
		return timer;
	}

	Model * getModel () const
	{
		return model;
	}

	View * getView () const
	{
		return view;
	}

	Camera * getCamera () const
	{
		return camera;
	}

	Console * getConsole () const
	{
		return console;
	}

	float	timeSinceLastFrame () const;

	float	getFps () const
	{
		return fps;
	}

	int	getNumFrames () const
	{
		return numFrames;
	}

	float	getYaw () const
	{
		return yaw;
	}

	float	getPitch () const
	{
		return pitch;
	}

	float	getRoll () const
	{
		return roll;
	}

	const Vector3D&	getPos () const
	{
		return pos;
	}

	const Vector3D& getGravity () const
	{
		return gravity;
	}

	void	setGravity ( float g )
	{
		gravity.y = -g;
	}

	void	setForwardSpeed ( float val )
	{
		forwardSpeed = val;
	}

	void	setBackwardSpeed ( float val )
	{
		backwardSpeed = val;
	}

	void	setStrafeSpeed ( float val )
	{
		strafeSpeed = val;
	}

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

	void	setView   ( View   * theView   );
	void	setModel  ( Model  * theModel  );
	void	setCamera ( Camera * theCamera );

	virtual	bool	isOk () const;

	virtual	InputReader * createInputReader ( View * )
	{
		return NULL;
	}

	virtual	void	draw           ();
	virtual	int		update         ();
	virtual	int		handleChar     ( int ch );
	virtual	int		handleKey      ( int key, bool pressed );
	virtual	int		handleKeyboard ( const KeyboardState& keys  ) = 0;
	virtual	int		handleMouse    ( const MouseState&    mouse ) = 0;

	virtual	int		setValue       ( const String& var, const String& val ) 
	{
		return CONTROLLER_SET_UNKNOWN_VAR;
	}

	static	MetaClass	classInstance;
};

#endif
