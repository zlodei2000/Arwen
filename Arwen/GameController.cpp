//
// Basic class to manage game (or walkthrough)
// Processes user input, rendering of the scene
// and does some collision detection
//
// Author: Alex V. Boreskoff
//
// Last modified: 2/04/2003
//

#include    "GameController.h"
#include	"Model.h"
#include	"ColDet.h"
#include	"Camera.h"
#include	"BoundingBox.h"
#include	"Array.h"
#include	"Console.h"
#include	"Input.h"
#include	"DirectXInputReader.h"
#include	"Timer.h"

MetaClass	GameController :: classInstance ( "GameController", &Controller :: classInstance );

GameController :: GameController ( const char * theName, Model * theModel, View * theView, Camera * theCamera, const Vector3D& defSize) : Controller ( theName, theModel, theView, theCamera ), size ( defSize )
{
	mouseSensitivity = -M_PI / 512.0f;
	metaClass        = &classInstance;
}

GameController :: ~GameController ()
{

}

int	GameController :: handleMouse ( const MouseState& mouse )
{
	if ( console -> getState () == consoleInactive )
	{
		yaw   += mouse.getMouseX () * mouseSensitivity;
		pitch += mouse.getMouseY () * mouseSensitivity;

		camera -> setEulerAngles ( yaw, pitch, roll );
	}

	return controllerContinue;
}

int	GameController :: handleKeyboard ( const KeyboardState& keys )
{
	if ( console -> getState () != consoleInactive )
		return controllerContinue;

	Vector3D	vel ( 0.0f, 0.0f, 0.0f );

	if ( keys.isKeyPressed ( forwardKey ) || keys.isKeyPressed ( forwardKey2 ) )
		vel = forwardSpeed * camera -> getViewDir ();
    else
    if ( keys.isKeyPressed ( backwardKey ) || keys.isKeyPressed ( backwardKey2 ) )
		vel = -backwardSpeed * camera -> getViewDir ();

	if ( keys.isKeyPressed ( leftKey ) || keys.isKeyPressed ( leftKey2 ) )
		if ( keys.isKeyPressed ( strafeKey ) || keys.isKeyPressed ( strafeKey2 ) )
			vel = -strafeSpeed * camera -> getRightDir ();
		else
			yaw += 0.05f;

	if ( keys.isKeyPressed ( rightKey ) || keys.isKeyPressed ( rightKey2 ) )
		if ( keys.isKeyPressed ( strafeKey ) || keys.isKeyPressed ( strafeKey2 ) )
			vel = strafeSpeed * camera -> getRightDir ();
		else
			yaw -= 0.05f;

	if ( keys.isKeyPressed ( strafeLeftKey ) || keys.isKeyPressed ( strafeLeftKey2 ) )
		vel = -strafeSpeed * camera -> getRightDir ();

	if ( keys.isKeyPressed ( strafeRightKey ) || keys.isKeyPressed ( strafeRightKey2 ) )
		vel = strafeSpeed * camera -> getRightDir ();

													// adjust velocity for run/walk mode
	if ( keys.isKeyPressed ( runKey ) || keys.isKeyPressed ( runKey2 ) )
		vel *= 2;

													// compute step distance based on velocity
//	float	curTime  = timer -> getTime ();
//	float	lastTime = lastFrameTimes [numFrames >= 8 ? 7 : numFrames - 1];

	float	delta = timeSinceLastFrame ();

	Vector3D	step ( vel * delta );

	Array		colliders ( "game colliders" );
	BoundingBox	box       ( pos, pos + step );

	box.grow ( size );

	getModel () -> getColliders ( box, colliders );

	pos = getPosition ( colliders, pos, step, size );

	camera -> setEulerAngles ( yaw, pitch, roll );
	camera -> setPos         ( pos );

	return controllerContinue;
}

void	GameController :: addConsoleCommand ( ConsoleCommand * com )
{
	console -> addCommand ( com );
}

InputReader * GameController :: createInputReader ( View * view )
{
	return new DirectXInputReader ( view, "DirectX input reader" );
}
