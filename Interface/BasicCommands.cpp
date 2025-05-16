//
// Some basic console commands
//
// Author: Alex V. Boreskoff
//
// Last modified: 2/04/2003
//

#include	"BasicCommands.h"
#include	"OpenGlView.h"
#include	"ObjStr.h"
#include	"Console.h"
#include	"ResourceManager.h"
#include	"Controller.h"
#include	"Camera.h"
#include	"Model.h"
#include	"Application.h"

MetaClass	FullScreenCommand :: classInstance ( "FullScreenCommand", &ConsoleCommand :: classInstance );
MetaClass	SetVidModeCommand :: classInstance ( "SetVidModeCommand", &ConsoleCommand :: classInstance );
MetaClass	SetVarCommand     :: classInstance ( "SetVarCommand",     &ConsoleCommand :: classInstance );
MetaClass	SetFovCommand     :: classInstance ( "SetFovCommand",     &ConsoleCommand :: classInstance );
MetaClass	MapCommand        :: classInstance ( "MapCommand",        &ConsoleCommand :: classInstance );
MetaClass	ReloadCommand     :: classInstance ( "ReloadCommand",     &ConsoleCommand :: classInstance );

void	FullScreenCommand :: execute ( const Array& argv, Console * console )
{
	if ( argv.getCount () < 1 )
		return;

	String * verb = (String *) argv.at ( 0 );
	String	cmd ( *verb );

	if ( cmd.toLower () != "fullscreen" )
		return;

	if ( argv.getCount () != 2 )
		console -> addString ( "Usage: fullscreen {on|off}" );
	else
	{
		String	* flag = (String *) argv.at ( 1 );
		String	on ( *flag );

		console -> getController () -> getView ()            -> setFullScreen ( on == "on" ? true : false );

		Application :: instance -> getResourceManager () -> upload ();
	}
}

void	FullScreenCommand :: printHelp ( Console * console ) const 
{
	console -> addString ( "Syntax:" );
	console -> addString ( "     fullscreen on|off" );
	console -> addString ( "Sets fullsceen mode on or offs" );
}

void	SetVidModeCommand :: execute ( const Array& argv, Console * console )
{
	if ( argv.getCount () < 1 )
		return;

	String * verb = (String *) argv.at ( 0 );
	String	cmd ( *verb );

	if ( cmd.toLower () != "setvidmode" )
		return;

	if ( argv.getCount () != 3 && argv.getCount () != 4 )
		console -> addString ( "Usage: setvidmode xRes yRes [bitsPerPixel]" );
	else
	{
		String * s1 = (String *) argv.at ( 1 );
		String * s2 = (String *) argv.at ( 2 );

		int	width  = s1 -> toInt ();
		int	height = s2 -> toInt ();
		int	bpp;

		if ( argv.getCount () == 4 )
			bpp = ((String *) argv.at ( 3 )) -> toInt ();
		else
			bpp = console -> getController () -> getView () -> getBitsPerPixel ();

		console -> getController () -> getView ()            -> setSize ( width, height );

		Application :: instance -> getResourceManager () -> upload ();
	}
}

void	SetVidModeCommand :: printHelp ( Console * console ) const 
{
	console -> addString ( "Syntax:" );
	console -> addString ( "     setvidmode width height [bpp]" );
	console -> addString ( "Sets sceen to width*height pixels" );
	console -> addString ( "with bpp bits per pixel" );
}

void	SetVarCommand :: execute ( const Array& argv, Console * console )
{
	if ( argv.getCount () < 1 )
		return;

	String * verb = (String *) argv.at ( 0 );
	String	cmd ( *verb );

	if ( cmd.toLower () != "set" )
		return;

	if ( argv.getCount () != 3 )
		console -> addString ( "Usage: set var value" );
	else
	{
		String	* var = (String *) argv.at ( 1 );
		String  * val = (String *) argv.at ( 2 );


		switch ( console -> getController () -> setValue ( *var, *val ) )
		{
			case CONTROLLER_SET_UNKNOWN_VAR:
				console -> addString ( String ().printf ( "Unknown variable: %s", var -> c_str () ) );
				break;

			case CONTROLLER_SET_ILLEGAL_VAL:
				console -> addString ( String ().printf ( "Illegal value:%s", val -> c_str () ) );
				break;
		}
	}
}

void	SetFovCommand :: execute ( const Array& argv, Console * console )
{
	if ( argv.getCount () < 1 )
		return;

	String * verb = (String *) argv.at ( 0 );
	String	cmd ( *verb );

	if ( cmd.toLower () != "fov" )
		return;

	if ( argv.getCount () != 2 )
		console -> addString ( "Usage: fov fov-value-in-degrees" );
	else
	{
		String	* sVal = (String *) argv.at ( 1 );
		float	  fov  = sVal -> toFloat ();

		if ( fov > 0 && fov < 180 )
			console -> getController () -> getCamera () -> setFov ( fov );
		else
			console -> addString ( "Illegal fov value, must be in [1,179] range" );
	}
}

void	SetFovCommand :: printHelp ( Console * console ) const 
{
	console -> addString ( "Syntax:" );
	console -> addString ( "     fov fov-value" );
	console -> addString ( "where fov-value is a new fov for camera" );
}

void	MapCommand :: execute ( const Array& argv, Console * console )
{
	String * verb = (String *) argv.at ( 0 );
	String	cmd ( *verb );

	if ( cmd.toLower () != "map" )
		return;

	if ( argv.getNumItems () != 2 )
	{
		console -> addString ( "Usage: map map-file" );

		return;
	}

	String	mapName ( * (String *) argv.at ( 1 ) );
	Object * object = Application :: instance -> getResourceManager () -> getObject ( mapName );

	if ( object == NULL )
	{
		console -> addString ( String ( "Cannot load map " ) + mapName );

		return;
	}

	Model * map = dynamic_cast <Model *> ( object );

	if ( map != NULL && map -> isOk  () )
	{
		console -> getController () -> setModel ( map );

		Application :: instance -> getResourceManager () -> upload ();
	}
	else
		console -> addString ( String ( "Error loading map " ) + mapName );
}

void	MapCommand :: printHelp ( Console * console ) const 
{
	console -> addString ( "Syntax:" );
	console -> addString ( "     map map-name" );
	console -> addString ( "where map-name is a file name of the map to be loaded" );
}

void	ReloadCommand :: execute ( const Array& argv, Console * console )
{
	String * verb = (String *) argv.at ( 0 );
	String	cmd ( *verb );

	if ( cmd.toLower () != "reload" )
		return;

	if ( argv.getNumItems () != 1 )
	{
		console -> addString ( "Usage: reload" );

		return;
	}

	Model  * model = Application :: instance -> getModel ();

	if ( model == NULL )
		return;

	String	mapName ( model -> getName () );

											// have to remove it from RM's cache
	Application :: instance -> getResourceManager () -> freeObject ( mapName );

											// load again
	Object * object = Application :: instance -> getResourceManager () -> getObject ( mapName );

	if ( object == NULL )
	{
		console -> addString ( String ( "Cannot load map " ) + mapName );

		return;
	}

	Model * map = dynamic_cast <Model *> ( object );

	if ( map != NULL && map -> isOk  () )
	{
		console -> getController () -> setModel ( map );

		Application :: instance -> getResourceManager () -> upload ();
	}
	else
		console -> addString ( String ( "Error loading map " ) + mapName );
}

void	ReloadCommand :: printHelp ( Console * console ) const 
{
	console -> addString ( "Syntax:" );
	console -> addString ( "     reload" );
	console -> addString ( "Reloads current model from disk" );
}

