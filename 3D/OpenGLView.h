//
// View for rendering via OpenGL
// Supports windowed and full screen mode, various screen resolutions and pixel depths
//
// Author: Alex V. Boreskoff
//
// Last modified: 20/11/2002
//

#ifndef	__OPENGL_VIEW__
#define	__OPENGL_VIEW__

#include	<windows.h>
#include	<gl/gl.h>

#include	"View.h"
#include	"Input.h"
#include	"ObjStr.h"
#include	"Queue.h"

class	Camera;
class	Controller;
class	InputReader;

class	OpenGlView : public View
{
private:
	bool		  fullScreen;
	HINSTANCE	  hInstance;
	HWND		  hWnd;
	HDC			  hDC;
	HGLRC		  hRC;
	float		  zNear;
	float		  zFar;
	unsigned	  lastTextureId;
	char		  keys [256];
	int			  mouseX;
	int			  mouseY;
	int			  mouseLeft, mouseRight, mouseMiddle;
	Camera	    * camera;
	InputReader * reader;
	Queue		  events;
public:
	OpenGlView ( const char * theName, HINSTANCE hInst, Camera * theCamera, int theWidth, int theHeight, int bpp, bool toFullScreen = true );
	~OpenGlView ();

    virtual  bool    isOk () const
    {
        return hWnd != NULL && hDC != NULL && hRC != NULL;
    }

    virtual void    doLock ();
    virtual void    doUnlock ();

	virtual	bool	registerTexture ( Texture * );
    //virtual void    draw            ( const Polygon3D& );
	virtual	void	simpleDraw      ( const Polygon3D& poly, int mask = 0 );
	virtual	void	initGL          ();

	virtual	void	setFullScreen ( bool toFullScreen );
	virtual	void	setSize       ( int newWidth, int newHeight );
	virtual	void	startOrtho    ();
	virtual	void	endOrtho      ();
	virtual	void	drawBillboard ( const Vector3D& pos, float size, const Vector4D& color, Texture * txt = NULL );
	virtual	void	drawBillboard ( const Vector3D& pos, float width, float height, const Vector4D& color, Texture * txt = NULL );
	virtual	void	bindTexture   ( Texture * );
	virtual	void	blendFunc     ( int src, int dst );

	virtual	void	apply ( const Transform3D& );
	virtual	void	apply ( const Camera& );

	virtual	void * getWindow () const 
	{
		return hWnd;
	}

	virtual	Event * getEvent         ();
	virtual	bool	getKeyboardState ( KeyboardState& );
	virtual	bool	getMouseState    ( MouseState& );

	virtual	char  * getVendor   ();
	virtual	char  * getRenderer ();
	virtual	char  * getVersion  ();

	virtual	Texture * getScreenShot () const;

	Camera * getCamera () const
	{
		return camera;
	}

	static	int	getBitDepth ();							// get bit depth of current display mode

	static	MetaClass	classInstance;

protected:
	bool	changeScreenResolution ( int theWidth, int theHeight, int bpp );
	void	reshape                ( int theWidth, int theHeight );
	bool	createWindow           ( int theWidth, int theHeight, int bpp, bool isFullScreen, int depthBits = 32 );
	bool	destroyWindow          ();
	bool	done                   ();					// clear handles and retrun false
	void	getDisplayModes        ();

	friend static LRESULT CALLBACK windowProc ( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
};

#endif
