//
// Simple DirectInput wrapper for retriveing user input from mouse & keyboard
//
// Author: Alex V. Boreskoff
//
// Last modified: 17/06/2003
//

#ifndef	__DIRECT_INPUT_OBJECT__
#define	__DIRECT_INPUT_OBJECT__

#include	"dinput.h"

#include	"InputReader.h"

class	DirectXInputReader : public InputReader
{
protected:
#if	DIRECTINPUT_VERSION < 0x0800
	LPDIRECTINPUT		inputObject;
	LPDIRECTINPUTDEVICE	keyboardDevice;
	LPDIRECTINPUTDEVICE	mouseDevice;
#else
	LPDIRECTINPUT8		 inputObject;
	LPDIRECTINPUTDEVICE8 keyboardDevice;
	LPDIRECTINPUTDEVICE8 mouseDevice;
#endif

	HINSTANCE			hInstance;
	HWND				hWindow;
public:
	DirectXInputReader ( View * view, const char * aName );
	~DirectXInputReader ();

	virtual	bool isOk () const
	{
		return inputObject != NULL && keyboardDevice != NULL && mouseDevice != NULL;
	}

	virtual	bool	getKeyboardState ( KeyboardState& );
	virtual	bool	getMouseState    ( MouseState&    );

	static	MetaClass	classInstance;
};

#endif
