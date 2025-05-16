//
// Basic class to manage game (or walkthrough)
// Processes user input, rendreing of the scene
// and does some collision detection
//
// Author: Alex V. Boreskoff
//
// Last modified: 18/07/2002
//

#include	"Model.h"
#include    "GameController.h"
#include	"ColDet.h"
#include	"Camera.h"
#include	"BoundingBox.h"
#include	"Array.h"
#include	"Console.h"
#include	"Input.h"
#include	"ResourceManager.h"
#include	"Texture.h"
#include	"DirectXInputReader.h"
#include	"View.h"

MetaClass	GameController :: classInstance ( "GameController", &Controller :: classInstance );
	
GameController :: GameController ( const char * theName, Model * theModel, View * theView, Camera * theCamera, const Vector3D& defSize ) : 
     Controller ( theName, theModel, theView, theCamera ), size ( defSize ), velocity ( 0.0f, 0.0f, 0.0f )
{
	metaClass        = &classInstance;

	mouseSensitivity = -M_PI / 512.0f;

								// set all keys to default values
    forwardKey      = keyUp;
	forwardKey2     = keyW;

    backwardKey     = keyDown;
	backwardKey2    = keyX;

    leftKey         = keyLeft;
	leftKey2        = 0;

    rightKey        = keyRight;
	rightKey2       = 0;

    jumpKey         = keySpace;
	jumpKey2        = 0;

    crouchKey       = keyC;
	crouchKey2      = 0;

	strafeKey       = keyLeftAlt;
	strafeKey2      = keyRightAlt;

	strafeLeftKey   = keyA;
	strafeLeftKey2  = 0;

	strafeRightKey  = keyD;
	strafeRightKey2 = 0;

	runKey          = keyLeftShift;
	runKey2         = keyRightShift;

	fireKey         = keyLeftControl;
	fireKey2        = keyRightControl;

	useKey          = keyEnter;
	useKey2         = 0;
}

GameController :: ~GameController ()
{

}

int	GameController :: handleMouse ( const MouseState& mouse )
{
	if ( console -> getState () == consoleInactive )
	{
		roll  += mouse.getMouseX () * mouseSensitivity;
		pitch += mouse.getMouseY () * mouseSensitivity;
//		yaw   += mouse.getMouseX () * mouseSensitivity;
//		pitch += mouse.getMouseY () * mouseSensitivity;

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
		if ( keys.isKeyPressed ( runKey ) || keys.isKeyPressed ( runKey2 ) )
			vel = camera -> getViewDir () * 6.0f;
		else
			vel = camera -> getViewDir () * 4.0f;
    else
    if ( keys.isKeyPressed ( backwardKey ) || keys.isKeyPressed ( backwardKey2 ) )
		if ( keys.isKeyPressed ( runKey ) || keys.isKeyPressed ( runKey2 ) )
			vel = -camera -> getViewDir () * 6.0f;
		else
			vel = -camera -> getViewDir () * 4.0f;

	if ( keys.isKeyPressed ( leftKey ) || keys.isKeyPressed ( leftKey2 ) )
		if ( keys.isKeyPressed ( strafeKey ) || keys.isKeyPressed ( strafeKey2 ) )
			vel = -camera -> getRightDir () * 0.05f;
		else
			yaw += 0.05f;

	if ( keys.isKeyPressed ( rightKey ) || keys.isKeyPressed ( rightKey2 ) )
		if ( keys.isKeyPressed ( strafeKey ) || keys.isKeyPressed ( strafeKey2 ) )
			vel = camera -> getRightDir () * 0.05f;
		else
			yaw -= 0.05f;

	if ( keys.isKeyPressed ( strafeLeftKey ) || keys.isKeyPressed ( strafeLeftKey2 ) )
		vel = -camera -> getRightDir () * 0.05f;

	if ( keys.isKeyPressed ( strafeRightKey ) || keys.isKeyPressed ( strafeRightKey2 ) )
		vel = camera -> getRightDir () * 0.05f;

	Array		colliders ( "game colliders" );
	BoundingBox	box       ( pos, pos + vel );

	box.grow ( size );

	getModel () -> getColliders ( box, colliders );

	pos = getPosition ( colliders, pos, vel, size );

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
