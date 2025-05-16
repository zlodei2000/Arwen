//
// View for rendering via OpenGL
// Supports windowed and full screen mode, various screen resolutions and pixel depths
//
// Author: Alex V. Boreskoff
//
// Last modified: 28/11/2002
//

#include	<stdio.h>
#include	<windows.h>
#include	<winuser.h>				// for WM_MOUSEWHEEL
#include	<gl/glu.h>


#include	"OpenGlView.h"
#include	"Poly3D.h"
#include	"Camera.h"
#include	"Texture.h"
#include	"Lightmap.h"
#include	"Vector2D.h"
#include	"Vector3D.h"
#include	"Vector4D.h"
#include	"Controller.h"
#include	"InputReader.h"
#include	"Transform3D.h"
#include	"Log.h"
#include	"Events.h"

static	const char * className = "OpenGlViewClass";		// window class name for view window

static	int	blendCodes [] =								// table for OpenGL blend modes
{ 
	GL_ZERO,      GL_ONE, 
	GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR, 
	GL_DST_COLOR, GL_ONE_MINUS_DST_COLOR, 
	GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,
	GL_DST_ALPHA, GL_ONE_MINUS_DST_ALPHA
};

struct	KeyTable
{
	int	vkCode;
	int	ourCode;
};

static	KeyTable	keyTable [] =
{
	{ VK_BACK,    keyBs          },
	{ VK_TAB,     keyTab         },
	{ VK_ESCAPE,  keyEsc         },
	{ VK_SPACE,   keySpace       },
	{ VK_END,     keyEnd         },
	{ VK_HOME,    keyHome        },
	{ VK_LEFT,    keyLeft        },
	{ VK_RIGHT,   keyRight       },
	{ VK_UP,      keyUp          },
	{ VK_DOWN,    keyDown        },
	{ VK_PRIOR,   keyPgUp        },
	{ VK_NEXT,    keyPgDown      },
	{ VK_INSERT,  keyIns         },
	{ VK_DELETE,  keyDel         },
	{ VK_NUMLOCK, keyNumLock     },
	{ VK_SCROLL,  keyScrollLock  },
	{ VK_F1,      keyF1          },
	{ VK_F2,      keyF2          },
	{ VK_F3,      keyF3          },
	{ VK_F4,      keyF4          },
	{ VK_F5,      keyF5          },
	{ VK_F6,      keyF6          },
	{ VK_F7,      keyF7          },
	{ VK_F8,      keyF8          },
	{ VK_F9,      keyF9          },
	{ VK_F10,     keyF10         },
	{ VK_F11,     keyF11         },
	{ VK_F12,     keyF12         },
	{ 19,         keyPrintScreen },
	{ 192,        keyTilda       },
	{ 222,        keySingleQuote },
	{ '0',        key0           },
	{ '1',        key1           },
	{ '2',        key2           },
	{ '3',        key3           },
	{ '4',        key4           },
	{ '5',        key5           },
	{ '6',        key6           },
	{ '7',        key7           },
	{ '8',        key8           },
	{ '9',        key9           },
	{ 'A',        keyA           },
	{ 'B',        keyB           },
	{ 'C',        keyC           },
	{ 'D',        keyD           },
	{ 'E',        keyE           },
	{ 'F',        keyF           },
	{ 'G',        keyG           },
	{ 'H',        keyH           },
	{ 'I',        keyI           },
	{ 'J',        keyJ           },
	{ 'K',        keyK           },
	{ 'L',        keyL           },
	{ 'M',        keyM           },
	{ 'N',        keyN           },
	{ 'O',        keyO           },
	{ 'P',        keyP           },
	{ 'Q',        keyQ           },
	{ 'R',        keyR           },
	{ 'S',        keyS           },
	{ 'T',        keyT           },
	{ 'U',        keyU           },
	{ 'V',        keyV           },
	{ 'W',        keyW           },
	{ 'X',        keyX           },
	{ 'Y',        keyY           },
	{ 'Z',        keyZ           },
	{ 191,        keySlash       },
	{ 190,        keyPeriod      },
	{ 188,        keyComma       },
	{ 219,        keyOpenBracket },
	{ 221,        keyCloseBracket },
	{ VK_PAUSE,   keyPause        },
	{ VK_RETURN,  keyEnter        },
	{ 186,        keySemiColon    },
	{ 220,        keyBackSlash    },
	{ 187,        keyEquals       },
	{ 189,        keyMinus        },
	{ VK_NUMPAD0, keyNumPad0      },
	{ VK_NUMPAD1, keyNumPad0      },
	{ VK_NUMPAD2, keyNumPad0      },
	{ VK_NUMPAD3, keyNumPad0      },
	{ VK_NUMPAD4, keyNumPad0      },
	{ VK_NUMPAD5, keyNumPad0      },
	{ VK_NUMPAD6, keyNumPad0      },
	{ VK_NUMPAD7, keyNumPad0      },
	{ VK_NUMPAD8, keyNumPad0      },
	{ VK_NUMPAD9, keyNumPad0      },
	{ 111,        keyNumPadSlash  },
	{ 106,        keyNumPadMult   },
	{ 107,        keyNumPadPlus   },
	{ 109,        keyNumPadMinus  }
};

MetaClass	OpenGlView :: classInstance ( "OpenGlView", &View :: classInstance );

OpenGlView :: OpenGlView ( const char * theName, HINSTANCE hInst, Camera * theCamera, int theWidth, int theHeight, int bpp, bool toFullScreen ) : View ( theName ), events ( "events queue" )
{
	(*sysLog) << "Creating OpenGlView" << logEndl;
	(*sysLog) << "Width: " << theWidth << ", Height: " << theHeight << " Bits per pixel: " << bpp << " Fullscreen " << toFullScreen << logEndl;
	(*sysLog) << logEndl;

	hInstance     = hInst;
	camera        = theCamera;
	hWnd          = NULL;
	hDC           = NULL;
	hRC           = NULL;
	visible       = false;
	lastTextureId = -1;
	reader        = NULL;
	mouseX        = 0;
	mouseY        = 0;
	mouseLeft     = 0;
	mouseRight    = 0;
	mouseMiddle   = 0;
	metaClass     = &classInstance;

	camera -> retain ();

	WNDCLASSEX windowClass;												

	ZeroMemory ( &windowClass, sizeof (WNDCLASSEX) );					// clear the memory
	
	windowClass.cbSize			= sizeof (WNDCLASSEX);					// size of the windowClass structure
	windowClass.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// redraws the window on any movement / resizing
	windowClass.lpfnWndProc		= (WNDPROC) windowProc;					// windowProc Handles Messages
	windowClass.hInstance		= hInstance;							// set the instance
	windowClass.hbrBackground	= (HBRUSH)COLOR_APPWORKSPACE;			// class background brush color
	windowClass.hCursor			= LoadCursor ( NULL, IDC_ARROW );		// load the arrow pointer
	windowClass.lpszClassName	= className;							// sets the classname
	windowClass.hIcon           = LoadIcon ( hInstance, "ARWEN" );
	
	if ( RegisterClassEx (&windowClass) == 0 )							
		return;

	getDisplayModes ();
	createWindow    ( theWidth, theHeight, bpp, toFullScreen, 32 );
}

OpenGlView :: ~OpenGlView ()
{
	(*sysLog) << "Destroying OpenGlView" << logEndl;

	done ();

	camera -> release ();
}

void	OpenGlView :: getDisplayModes ()
{
	DEVMODE	devMode;

	for ( int i = 0; EnumDisplaySettings ( NULL, i, &devMode ); i++ )
		modeList.insertNoRetain ( new VideoMode ( devMode.dmPelsWidth, devMode.dmPelsHeight,
			                                      devMode.dmBitsPerPel, devMode.dmDisplayFrequency ) );
}

char  * OpenGlView :: getVendor ()
{
	return (char *) glGetString ( GL_VENDOR );
}

char  * OpenGlView :: getRenderer ()
{
	return (char *) glGetString ( GL_RENDERER );
}

char  * OpenGlView :: getVersion ()
{
	return (char *) glGetString ( GL_VERSION );
}

Event * OpenGlView :: getEvent ()
{
	Object * ev = events.get ();

	if ( ev == NULL )
		return NULL;

	if ( dynamic_cast <Event *> ( ev ) != NULL )
		return dynamic_cast <Event *> ( ev );

	return NULL;
}

bool	OpenGlView :: getKeyboardState ( KeyboardState& state )
{
	if ( reader == NULL && controller != NULL )
		reader = controller -> createInputReader ( this );		// ask controller to create input reader

	if ( reader != NULL )
		return reader -> getKeyboardState ( state );

	memcpy ( state.getKeys (), keys, 256 );

	return true;
}

bool	OpenGlView :: getMouseState ( MouseState& state )
{
	if ( reader == NULL && controller != NULL )
		reader = controller -> createInputReader ( this );		// ask controller to create input reader

	if ( reader != NULL )
		return reader -> getMouseState ( state );

	state.setState ( mouseX, mouseY, 0, mouseLeft != 0, mouseRight != 0, mouseMiddle != 0, 0 );

	return true;
}

void    OpenGlView :: doLock ()
{
	initGL  ();
	glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );

	lastTextureId = -1;
}

void    OpenGlView :: doUnlock ()
{
	SwapBuffers ( hDC );
}

bool	OpenGlView :: registerTexture ( Texture * texture )
{
	return true;
}

void	OpenGlView :: bindTexture ( Texture * texture )
{
	if ( texture == NULL )
		return;

	if ( texture -> getId () != lastTextureId )
		glBindTexture ( GL_TEXTURE_2D, lastTextureId = texture -> getId () );
}

void	OpenGlView :: blendFunc ( int src, int dst )
{
	if ( src < 0 || src > bmOneMinusDstAlpha || dst < 0 || dst > bmOneMinusDstAlpha )
		return;

	glBlendFunc ( blendCodes [src], blendCodes [dst] );
}

void	OpenGlView :: simpleDraw ( const Polygon3D& poly, int mask )
{
	// assume texture, texture transform already set
	// skip lightmap and detail textures
	// do colors only if bit is set

	const Vector3D * vertices    = poly.getVertices    ();
	const Vector2D * uvMap       = poly.getUvMap       ();
	const Vector4D * colors      = poly.getColors      ();
	Vector4D	     polyColor   = poly.getColor       ();
	int				 numVertices = poly.getNumVertices ();

	if ( colors == NULL )
		mask &= ~useColors;

	glBegin ( GL_POLYGON );

	if ( mask & useColors )
	{
		for ( int i = 0; i < numVertices; i++ )
		{
			glColor4fv    ( colors   [i] * polyColor );
			glTexCoord2fv ( uvMap    [i] );
			glVertex3fv   ( vertices [i] );
		}
	}
	else
	if ( mask & useLightmap )
	{
		Lightmap * map = poly.getLightmap ();

		glColor4f ( 1, 1, 1, 1 );

		if ( map != NULL )
			for ( int i = 0; i < numVertices; i++ )
			{
				glTexCoord2fv ( map -> remap ( uvMap [i] ) );
				glVertex3fv   ( vertices [i] );
			}
	}
	else
	{
		glColor4fv ( polyColor );

		for ( int i = 0; i < numVertices; i++ )
		{
			glTexCoord2fv ( uvMap    [i] );
			glVertex3fv   ( vertices [i] );
		}
	}

	glEnd ();
}
/*
void    OpenGlView :: draw ( const Polygon3D& poly )
{
	if ( poly.getTexture () == NULL )
		return;

	if ( poly.getTexture () -> getId () != lastTextureId )
		glBindTexture ( GL_TEXTURE_2D, lastTextureId = poly.getTexture () -> getId () );

	Vector4D	     polyColor   = poly.getColor       ();
	const Vector4D * colors      = poly.getColors      ();
	const Vector3D * vertices    = poly.getVertices    ();
	const Vector2D * uvMap       = poly.getUvMap       ();
	int				 numVertices = poly.getNumVertices ();

	glBegin ( GL_POLYGON );

	for ( int i = 0; i < numVertices; i++ )
	{
		if ( colors != NULL )
			color = colors [i] * polyColor;

		glColor4fv    ( color        );
		glTexCoord2fv ( uvMap    [i] );
		glVertex3fv   ( vertices [i] );
	}

	glEnd();
}
*/
void	OpenGlView :: drawBillboard ( const Vector3D& pos, float size, const Vector4D& color, Texture * txt )
{
	size *= 0.5f;

	Vector3D	r  ( camera -> getRightDir () * size );
	Vector3D	u  ( camera -> getUpDir    () * size );
	Vector3D	v1 ( pos - r - u );
	Vector3D	v2 ( pos + r - u );
	Vector3D	v3 ( pos + r + u );
	Vector3D	v4 ( pos - r + u );

	if ( txt != NULL )
		if ( txt -> getId () != lastTextureId )
			glBindTexture ( GL_TEXTURE_2D, lastTextureId = txt -> getId () );

	glBegin ( GL_POLYGON );

	glColor4fv ( color );

	glTexCoord2f ( 0.0f, 0.0f );
	glVertex3fv  ( v1 );

	glTexCoord2f ( 1.0f, 0.0f );
	glVertex3fv  ( v2 );

	glTexCoord2f ( 1.0f, 1.0f );
	glVertex3fv  ( v3 );

	glTexCoord2f ( 0.0f, 1.0f );
	glVertex3fv  ( v4 );

	glEnd();
}

void	OpenGlView :: drawBillboard ( const Vector3D& pos, float width, float height, const Vector4D& color, Texture * txt )
{
	width  *= 0.5f;
	height *= 0.5f;

	Vector3D	r  ( camera -> getRightDir () * width  );
	Vector3D	u  ( camera -> getUpDir    () * height );
	Vector3D	v1 ( pos - r - u );
	Vector3D	v2 ( pos + r - u );
	Vector3D	v3 ( pos + r + u );
	Vector3D	v4 ( pos - r + u );

	if ( txt != NULL )
		if ( txt -> getId () != lastTextureId )
			glBindTexture ( GL_TEXTURE_2D, lastTextureId = txt -> getId () );

	glBegin ( GL_POLYGON );

	glColor4fv ( color );

	glTexCoord2f ( 0.0f, 0.0f );
	glVertex3fv  ( v1 );

	glTexCoord2f ( 1.0f, 0.0f );
	glVertex3fv  ( v2 );

	glTexCoord2f ( 1.0f, 1.0f );
	glVertex3fv  ( v3 );

	glTexCoord2f ( 0.0f, 1.0f );
	glVertex3fv  ( v4 );

	glEnd();
}

void	OpenGlView :: initGL ()
{
	glClearColor    ( 0, 0, 0, 0 );
	glClearDepth    ( 1 );	
	glClearStencil  ( 0 );
	glHint          ( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
	glEnable        ( GL_TEXTURE_2D );							// enable texture mapping
	glEnable        ( GL_DEPTH_TEST );							// enable z-buffering
	glEnable        ( GL_BLEND );
	glDepthFunc     ( GL_LEQUAL );								// set depth comparison (<=)
	glBlendFunc     ( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glPixelStorei   ( GL_PACK_ALIGNMENT,   1 );
	glPixelStorei   ( GL_UNPACK_ALIGNMENT, 1 );
}

bool	OpenGlView :: changeScreenResolution ( int theWidth, int theHeight, int bpp )
{
	DEVMODE dmScreenSettings;

	ZeroMemory ( &dmScreenSettings, sizeof ( DEVMODE ) );		// clear the memory

	dmScreenSettings.dmSize				= sizeof (DEVMODE);		// size of the DEVMODE structure
	dmScreenSettings.dmPelsWidth		= theWidth;				// screen width
	dmScreenSettings.dmPelsHeight		= theHeight;			// screen height
	dmScreenSettings.dmBitsPerPel		= bpp;					// select bits per pixel
	dmScreenSettings.dmFields			= DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

	if ( ChangeDisplaySettings ( &dmScreenSettings, CDS_FULLSCREEN ) != DISP_CHANGE_SUCCESSFUL )
		return false;											// Display Change Failed, Return False
	
	width        = theWidth;
	height       = theHeight;
	bitsPerPixel = bpp;

	return true;												// display change was successful
}

bool	OpenGlView :: createWindow ( int theWidth, int theHeight, int bpp, bool isFullScreen, int depthBits )
{
	DWORD windowStyle         = WS_OVERLAPPEDWINDOW;			// set window style
	DWORD windowExtendedStyle = WS_EX_APPWINDOW;				// set window extended style
	RECT  windowRect          = { 0, 0, theWidth, theHeight };	// define window coordinates

	if ( isFullScreen)											// are we in fullscreen mode?
	{
		windowExtendedStyle = WS_EX_APPWINDOW;					// window extended style
		windowStyle         = WS_POPUP;							// window style

		ShowCursor(FALSE);										// hide mouse pointer
	}
	else
	{
		windowExtendedStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;	// window xxtended style
		windowStyle         = WS_OVERLAPPEDWINDOW;					// window style
	}

	if ( isFullScreen )											// fullscreen requested, Try changing video mode
	{
		if ( !changeScreenResolution ( theWidth, theHeight, bpp ) )
		{
																// fullscreen mode failed. Run in windowed mode Instead
			MessageBox (HWND_DESKTOP, "Mode Switch Failed.\nRunning In Windowed Mode.", "Error", MB_OK | MB_ICONEXCLAMATION);

			isFullScreen = false;								// set isFullscreen to false (windowed mode)
		}
		else													// otherwise (if fullscreen mode was successful)
		{
			ShowCursor ( FALSE );								// turn off the cursor

			windowExtendedStyle |= WS_EX_TOPMOST;				// set the extended window style to WS_EX_TOPMOST
		}														// (top window covering everything else)
	}
	else														// if fullscreen was not selected
																// adjust window, Account for window worders
		AdjustWindowRectEx ( &windowRect, windowStyle, 0, windowExtendedStyle );

	if ( !isFullScreen )										// if not fullscreen then center window on the screen
	{
		int	screenWidth  = GetSystemMetrics ( SM_CXSCREEN );
		int	screenHeight = GetSystemMetrics ( SM_CYSCREEN );
		int	xPos         = ( screenWidth  - (windowRect.right  - windowRect.left ) ) >> 1;
		int	yPos         = ( screenHeight - (windowRect.bottom - windowRect.top  ) ) >> 1;

		windowRect.left   += xPos;
		windowRect.right  += xPos;
		windowRect.top    += yPos;
		windowRect.bottom += yPos;
	}
																// create the OpenGL window
	hWnd = CreateWindowEx ( windowExtendedStyle,				// extended style
					        className,							// class name
						    name,								// window title
						    windowStyle,						// window style
						    windowRect.left,					// window (X,Y) position
							windowRect.top,								
						    windowRect.right - windowRect.left,	// window width
						    windowRect.bottom - windowRect.top,	// window height
						    HWND_DESKTOP,						// desktop is window's parent
						    0,									// no nenu
						    hInstance,							// pass the window instance
						    this );

	if ( hWnd == NULL )											// was window creation a success?
		return false;											// if not then return false

	hDC = GetDC ( hWnd );										// get device context for this window

	if ( hDC == NULL )											// did we get a device context?
		return done ();

	if ( !isFullScreen )										// for windowed mode use current
		bpp = getBitDepth ();									// bit depth

	PIXELFORMATDESCRIPTOR pfd =									
	{
		sizeof (PIXELFORMATDESCRIPTOR),							// size of this Pixel Format Descriptor
		1,														// version number
		PFD_DRAW_TO_WINDOW |									// format must support Window
		PFD_SUPPORT_OPENGL |									// format must support OpenGL
		PFD_DOUBLEBUFFER,										// must support double buffering
		PFD_TYPE_RGBA,											// request an RGBA format
		bpp,													// select color depth
		0, 0, 0, 0, 0, 0,										// color bits
		0,														// no alpha buffer
		0,														// shift bit ignored
		0,														// no accumulation cuffer
		0, 0, 0, 0,												// accumulation bits ignored
		depthBits,												// Z-Buffer depth
		8,														// 8-bit stencil buffer
		0,														// no auxiliary buffer
		PFD_MAIN_PLANE,											// main drawing layer
		0,														// reserved
		0, 0, 0													// layer masks ignored
	};

	GLuint pixelFormat;											// will hold the selected pixel format

	pixelFormat = ChoosePixelFormat ( hDC, &pfd );				// find a compatible pixel format

	if ( pixelFormat == 0 )										// did we find a compatible format?
		return done ();											// failed

	if ( SetPixelFormat ( hDC, pixelFormat, &pfd ) == FALSE )	// try to set pixel format
		return done ();											// failed

	hRC = wglCreateContext ( hDC );								// try to get a rendering context

	if ( hRC == NULL )											// did we get a rendering context?
		return done ();											// failed

																// make the rendering context our current rendering context
	if ( wglMakeCurrent ( hDC, hRC ) == FALSE )
		return done ();											// failed

	reshape ( theWidth, theHeight );							// reshape our GL window
	initGL  ();

	memset ( keys, '\0', sizeof ( keys ) );

	events.clear ();

	reader       = NULL;
	visible      = true;										// set visible to true
	bitsPerPixel = bpp;
	fullScreen   = isFullScreen;

	ShowWindow ( hWnd, SW_SHOW );

	return true;												// window creating was successful
																// further initialization will be done in WM_CREATE
}

bool	OpenGlView :: destroyWindow ()
{
	if ( reader != NULL )										// destroy reader
		reader -> release ();

	reader = NULL;

	if ( fullScreen )											// is window in fullscreen mode
	{
		ChangeDisplaySettings ( NULL, 0 );						// switch back to desktop resolution
		ShowCursor            ( TRUE );							// show the cursor
	}	

	done ();

	return true;
}

void	OpenGlView :: reshape ( int theWidth, int theHeight )
{
	camera -> setViewSize ( width = theWidth, height = theHeight, camera -> getFov () );
}

bool	OpenGlView :: done ()
{
	wglMakeCurrent ( NULL, NULL );

	if ( hRC != NULL )
		wglDeleteContext ( hRC );								// delete The Rendering Context

	if ( hDC != NULL )
		ReleaseDC ( hWnd, hDC );								// release Our Device Context

	if ( hWnd != NULL )
		DestroyWindow ( hWnd );									// destroy The Window

	hWnd = NULL;												// zero The Window Handle
	hDC  = NULL;												// zero The Device Context
	hRC  = NULL;												// zero The Rendering Context

	ChangeDisplaySettings ( NULL, 0 );							// restore display mode

	return false;												// return False (will be used later)
}

void	OpenGlView :: setSize ( int newWidth, int newHeight )
{
	destroyWindow ();
	createWindow  ( newWidth, newHeight, bitsPerPixel, fullScreen );
}

void	OpenGlView :: setFullScreen ( bool toFullScreen )
{
	if ( toFullScreen == fullScreen )
		return;

	destroyWindow ();
	createWindow  ( width, height, bitsPerPixel, fullScreen = toFullScreen );
}

Texture * OpenGlView :: getScreenShot () const
{
	PixelFormat	rgbFormat;

	rgbFormat.completeFromMasks ( 0xFF, 0xFF00, 0xFF0000 );

	Texture * txt = new Texture ( "screenshot", width, height, rgbFormat );
	void	* ptr = (void *)txt -> getData ();

	glReadBuffer ( GL_FRONT );
	glReadPixels ( 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, ptr );

	return txt;
}

void	OpenGlView :: startOrtho ()
{
	glMatrixMode   ( GL_PROJECTION );					// select the projection matrix
	glPushMatrix   ();									// store the projection matrix
	glLoadIdentity ();									// reset the projection matrix
														// set up an ortho screen
	glOrtho        ( 0,getWidth (), 0, getHeight (), -1, 1 );							
	glMatrixMode   ( GL_MODELVIEW );					// select the modelview matrix
	glPushMatrix   ();									// store the modelview matrix
	glLoadIdentity ();									// reset the modelview matrix
}

void	OpenGlView :: endOrtho ()
{
	glMatrixMode ( GL_PROJECTION );						// select the projection matrix
	glPopMatrix  ();									// restore the old projection matrix
	glMatrixMode ( GL_MODELVIEW );						// select the modelview matrix
	glPopMatrix  ();									// restore the old projection matrix
}

void	OpenGlView :: apply ( const Transform3D& tr ) 
{
	float	matrix [16];
														// build OpenGL matrix on given transform 
														// and multiply current matrix on it
	tr.buildHomogeneousMatrix ( matrix );

	glMultMatrixf ( matrix );
}

void	OpenGlView :: apply ( const Camera& camera )
{
	float		m [16];
	Vector3D	pos ( camera.getPos () );

	camera.buildHomogeneousMatrix ( m );
														// reset current viewport
	glViewport     ( 0, 0, camera.getWidth (), camera.getHeight () );				
	glMatrixMode   ( GL_PROJECTION );					// select projection matrix
	glLoadIdentity ();									// reset projection matrix

														// calculate aspect ratio of the window
	gluPerspective ( camera.getFov (), camera.getAspect (), camera.getZNear (), camera.getZFar () );

	glMatrixMode   ( GL_MODELVIEW );					// select modelview matrix
	glLoadIdentity ();									// reset modelview matrix

	glMultMatrixf ( m );
	glTranslatef  ( -pos.x, -pos.y, -pos.z );
}

int	OpenGlView :: getBitDepth ()
{
   HDC          hDC  = GetDC ( NULL );
   HBITMAP      hBmp = CreateCompatibleBitmap ( hDC, 1, 1 );
   BITMAPINFO * bmi  = (LPBITMAPINFO) malloc ( sizeof (BITMAPINFOHEADER) + 4 * sizeof (RGBQUAD) );

   bmi -> bmiHeader.biBitCount    = (WORD) GetDeviceCaps ( hDC, BITSPIXEL );
   bmi -> bmiHeader.biCompression = BI_BITFIELDS;
   bmi -> bmiHeader.biSize        = sizeof ( BITMAPINFOHEADER );
   bmi -> bmiHeader.biWidth       = 1;
   bmi -> bmiHeader.biHeight      = 1;
   bmi -> bmiHeader.biClrUsed     = 0;
   bmi -> bmiHeader.biPlanes      = 1;

   int	res      = GetDIBits ( hDC, hBmp, 0, 1, NULL, bmi, DIB_RGB_COLORS );
   int	bitCount = bmi->bmiHeader.biBitCount;

   DeleteObject ( hBmp );
   ReleaseDC    ( NULL, hDC );
   DeleteObject ( hBmp );
   free         ( bmi );

   return bitCount;
}

/////////////////////// WndProc for OpenGlView window ////////////////////////////////

static	int	translateKey ( int key )
{
	for ( int i = 0; i < sizeof ( keyTable ) / sizeof ( keyTable [0] ); i++ )
		if ( keyTable [i].vkCode == key )
			return keyTable [i].ourCode;

	return 0;
}

static LRESULT CALLBACK windowProc ( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	OpenGlView * view = (OpenGlView *)GetWindowLong ( hWnd, GWL_USERDATA );

	switch ( msg )												// evaluate window message
	{
		case WM_SYSCOMMAND:										// intercept system commands
		{
			switch (wParam)										// check system calls
			{
				case SC_SCREENSAVE:								// screensaver trying to start?
				case SC_MONITORPOWER:							// monitor trying to enter powersave?
					return 0;									// prevent these from happening
			}

			break;			
		}

		return 0;									

		case WM_CREATE:											// window creation
		{
			CREATESTRUCT * creation = (CREATESTRUCT*)lParam;	// store CREATESTRUCT pointer

			view = (OpenGlView *)creation->lpCreateParams;

			SetWindowLong ( hWnd, GWL_USERDATA, (LONG) view );	// store the OpenGlView pointer
		}

		return 0;												// return

        case WM_CHAR:
			view -> events.putNoRetain ( new CharEvent ( (char)wParam ) );

            return 0;

		case WM_KEYDOWN:
			if ( wParam >= 0 && wParam < 256 )
				view -> keys [wParam] = 1;

			view -> events.putNoRetain ( new KeyEvent ( translateKey ( (int)wParam ), true ) );

			return 0;

		case WM_KEYUP:
			if ( wParam >= 0 && wParam < 256 )
				view -> keys [wParam] = 0;

			view -> events.putNoRetain ( new KeyEvent ( translateKey ( (int)wParam ), false ) );

			return 0;

		case WM_MOUSEMOVE:
			view -> mouseX      = LOWORD ( lParam );
			view -> mouseY      = HIWORD ( lParam );
			view -> mouseLeft   = wParam & MK_LBUTTON;
			view -> mouseRight  = wParam & MK_RBUTTON;
			view -> mouseMiddle = wParam & MK_MBUTTON;

			return 0;

//		case WM_MOUSEWHEEL:
//			return 0;

		case WM_CLOSE:											// closing the window
			PostMessage ( hWnd, WM_QUIT, 0, 0 );				// terminate the application

			return 0;										

		case WM_SIZE:											// resizing action has taken place
			view -> reshape (LOWORD (lParam), HIWORD (lParam));	// resize window 

			switch (wParam)										// evaluate size action
			{
				case SIZE_MINIMIZED:							// was window minimized?
					view -> visible = false;					// set visible to false
					return 0;		

				case SIZE_MAXIMIZED:							// was window maximized?
					view -> visible = true;						// set visible to true
					return 0;									

				case SIZE_RESTORED:								// was window restored?
					view -> visible = true;						// set visible to true
					return 0;									

				default:
					return 0;
			}

			break;				

			case WM_ACTIVATEAPP:
				if ( wParam )									// if we're activating
				{
					if ( view -> fullScreen )					// if we're fullscreen
						view -> changeScreenResolution ( view -> getWidth (), view -> getHeight (), view -> getBitsPerPixel () );

					ShowCursor ( FALSE );
				}
				else
				{
					if ( view -> fullScreen )
						ChangeDisplaySettings ( NULL, 0 );		// switch back to desktop resolution

					ShowCursor ( TRUE );						// show the cursor
				}

				return 0;

			default:
				break;
	}

	return DefWindowProc ( hWnd, msg, wParam, lParam );			// pass nnhandled messages to DefWindowProc
}
