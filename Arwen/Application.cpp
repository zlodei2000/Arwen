//
// Application class of Arwen project
//
// Author: Alex V. Boreskoff
//

#include	<stdio.h>
#include	<stdarg.h>
#include	<locale.h>

#include	"Application.h"
#include	"AutoreleasePool.h"
#include	"View.h"
#include	"OpenGlView.h"
#include	"Controller.h"
#include	"GameController.h"
#include	"Model.h"
#include	"World.h"
#include	"ConfigParser.h"
#include	"Camera.h"
#include	"Log.h"
#include	"ResourceManager.h"
#include	"FileSystem.h"

#include	"SceneDecoder.h"
									// here are image decoders
#include	"BmpDecoder.h"
#include	"PcxDecoder.h"
#include	"TgaDecoder.h"
#include	"WalDecoder.h"
#include	"RgbDecoder.h"
#include	"PngDecoder.h"
#include	"GifDecoder.h"
#include	"JpegDecoder.h"
									// here are models decoder
#include	"AseDecoder.h"
#include	"Md2Decoder.h"
#include	"Md3Decoder.h"


class	Vector3DConfigItem : public ConfigItem
{
private:
	Vector3D * ptr;

public:
	Vector3DConfigItem ( const char * theName, Vector3D * thePtr, const Vector3D& defValue ) : ConfigItem ( theName )
	{
		ptr  = thePtr;
		*ptr = defValue;
	}

	virtual	bool	parseString ( const String& str )
	{
		return sscanf ( str.c_str (), "(%f, %f, %f)", &ptr -> x, &ptr -> y, &ptr -> z ) == 3;
	}
};

Log         * sysLog = new Log ( "Arwen.log" );
Application * Application :: instance = NULL;
MetaClass	  Application :: classInstance ( "Application", &Object :: classInstance );

Application :: Application ( const char * theName, const char * argsStr ) : Object ( theName )
{
	hInstance       = :: GetModuleHandle ( NULL );
	instance        = this;
	model           = NULL;
	view            = NULL;
	controller      = NULL;
	resourceManager = NULL;
	buildLightmaps  = false;
	metaClass       = &classInstance;

	setlocale ( LC_CTYPE, "" );			// set current locale as default ANSI 
										// code page obtained from operating system

										// create and prepare resource manager
	resourceManager = new ResourceManager ( "ResourceManager" );

	resourceManager -> registerSource  ( new OsFileSystem () );
	resourceManager -> registerDecoder ( new SceneDecoder ( resourceManager ) );

										// register image decoders
	resourceManager -> registerDecoder ( new TgaDecoder  ( resourceManager ) );
	resourceManager -> registerDecoder ( new WalDecoder  ( resourceManager ) );
	resourceManager -> registerDecoder ( new PcxDecoder  ( resourceManager ) );
	resourceManager -> registerDecoder ( new BmpDecoder  ( resourceManager ) );
	resourceManager -> registerDecoder ( new RgbDecoder  ( resourceManager ) );
	resourceManager -> registerDecoder ( new PngDecoder  ( resourceManager ) );
	resourceManager -> registerDecoder ( new GifDecoder  ( resourceManager ) );
	resourceManager -> registerDecoder ( new JpegDecoder ( resourceManager ) );

										// register models decoders
	resourceManager -> registerDecoder ( new AseDecoder ( resourceManager ) );
	resourceManager -> registerDecoder ( new Md2Decoder ( resourceManager ) );
	resourceManager -> registerDecoder ( new Md3Decoder ( resourceManager ) );

										// parse args
										// possible args:
										// -lightmaps
										// map name
	Array * args  = String ( argsStr ).split ();
	String	mapName;

	if ( args != NULL )
	{
		for ( Array :: Iterator it = args -> getIterator (); !it.end (); ++it )
		{
			String	* str = (String *) it.value ();

			if ( *str == "-lightmaps" )	// request to build lightmaps
				buildLightmaps = true;
			else
				mapName = *str;
		}
	}
										// read .cfg file
	ConfigParser	parser;

	int		 width;
	int		 height;
	int		 bpp;
	bool	 fullScreen;
	float	 fov;
	String	 map;
	float	 fwVel;
	float	 bkVel;
	float	 stVel;
	Vector3D size;
	String	 detailTexture;
	float	 detailDist;
	float	 detailScale;

	parser.addItem ( "video/width",         &width,      800 );
	parser.addItem ( "video/height",        &height,     600 );
	parser.addItem ( "video/fullscreen",    &fullScreen, false );
	parser.addItem ( "video/bpp",           &bpp,        32 );
	parser.addItem ( "player/fov",          &fov,        60.0f );
	parser.addItem ( "player/forward_vel",  &fwVel,      3.0f );
	parser.addItem ( "player/backward_vel", &bkVel,      2.0f );
	parser.addItem ( "player/strafe_vel",   &stVel,      2.0f );
	parser.addItem ( "map",                 &map,           String ( "Data/Test1.sc" ) );
	parser.addItem ( "detail/texture",      &detailTexture, String ( "" ) );
	parser.addItem ( "detail/distance",     &detailDist,    2.0f );
	parser.addItem ( "detail/scale",        &detailScale,   50.0f );

	parser.addItem ( new Vector3DConfigItem ( "player/size", &size, Vector3D ( 0.2f, 0.7f, 0.2f ) ) );

	if ( !parser.parse ( "Arwen.cfg" ) )
		(*sysLog) << "Error parsing Arwen.cfg" << logEndl;


	if ( !mapName.isEmpty () )				// command line overrides name in .cfg file
		map = mapName;

	if ( buildLightmaps )
	{
		if ( map.isEmpty () )
		{
			okBox ( "Error", "No map specified" );

			return;
		}

		World * world = dynamic_cast <World *> ( resourceManager -> getObject ( map ) );

		if ( world == NULL )
		{
			okBox ( "Error", "Cannot load scene %s", map.c_str () );

			return;
		}

		if ( !world -> buildAllLightmaps () )
			okBox ( "Error", "Error building lightmaps.\nSee log file" );

		world -> release ();

		world = NULL;

		return;
	}

	Camera * camera;

	camera    = new Camera     ( "camera", Vector3D ( 0, 0, 0 ), 0, 0, 0, fov, EPS, 200 );
    view      = new OpenGlView ( "Main view", hInstance, camera, width, height, bpp, fullScreen );

	if ( !view -> isOk () )
	{
		okBox ( "Error", "Cannot create main view" );

//		camera -> release ();
//		view   -> release ();

		return;
	}

	controller = new GameController ( "controller", NULL, view, camera, size );

	controller -> setCamera        ( camera );					// setup camera
	controller -> setForwardSpeed  ( fwVel );
	controller -> setBackwardSpeed ( bkVel );
	controller -> setStrafeSpeed   ( stVel );

	model = dynamic_cast <Model *> ( resourceManager -> getObject ( map ) );

	if ( model == NULL || !model -> isOk () )
	{
		if ( model == NULL )
			(*sysLog) << "Model NULL" << logEndl;
		else
			(*sysLog) << "Model invalid" << logEndl;

		okBox ( "Error", "Cannot load scene %s", map.c_str () );

//		controller -> release ();

		return;
	}

	controller -> setModel ( model );

	World :: detailDistance     = detailDist;
	World :: detailTextureScale = detailScale;
	World :: detailTexture      = resourceManager -> getTexture ( detailTexture );

	(*sysLog) << "Application successfully initialized" << logEndl;
}

Application ::~Application ()
{
	if ( controller != NULL )
		controller -> release ();

	if ( resourceManager != NULL )
		resourceManager -> release ();
}

bool	Application :: isOk () const
{
	if ( controller == NULL || model == NULL || view == NULL )
		return false;

	return controller -> isOk () && model -> isOk () && view -> isOk ();
}

int	Application :: run ()
{
	if ( !isOk () )
		return 0;

	if ( buildLightmaps )				// do not go into message loop
		return 0;

    MSG		msg;
    
    for ( ; ; )
    {
		autoreleasePool -> releaseAll ();

        if ( PeekMessage ( &msg, NULL, 0, 0, PM_NOREMOVE ) )
        {
            if ( !GetMessage ( &msg, NULL, 0, 0 ) )
                return msg.wParam;

            TranslateMessage ( &msg );
            DispatchMessage  ( &msg );
        }
        else
        if ( view -> isVisible () )
		{
			view -> lock ();

			if ( controller -> update () == controllerQuit )
				return 0;

			view -> unlock ();
		}
    }
}

void	Application :: okBox ( const char * caption, const char * format, ... ) const
{
	va_list	argptr;
	char	buf [1024];

	va_start ( argptr, format );
	vsprintf ( buf, format, argptr );
	va_end   ( argptr );

	MessageBox ( NULL, buf, caption, MB_ICONEXCLAMATION | MB_OK );
}

