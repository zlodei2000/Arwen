//
// Simple DirectInput wrapper for retriveing user input from mouse & keyboard
//
// Author: Alex V. Boreskoff
//
// Last modified: 12/07/2002
//

#include	"View.h"
#include	"DirectXInputReader.h"
#include	"Input.h"
#include	"Keys.h"

struct	KeyTable
{
	int	dikCode;
	int	code;
};

static	KeyTable	keyTable [] =
{
	{ DIK_ESCAPE,     keyEsc          },
	{ DIK_0,          key0            },
	{ DIK_1,          key1            },
	{ DIK_2,          key2            },
	{ DIK_3,          key3            },
	{ DIK_4,          key4            },
	{ DIK_5,          key5            },
	{ DIK_6,          key6            },
	{ DIK_7,          key7            },
	{ DIK_8,          key8            },
	{ DIK_9,          key9            },
	{ DIK_MINUS,      keyMinus        },
	{ DIK_EQUALS,     keyEquals       },
	{ DIK_BACK,       keyBs           },
	{ DIK_TAB,        keyTab          },
	{ DIK_LBRACKET,   keyOpenBracket  },
	{ DIK_RBRACKET,   keyCloseBracket },
	{ DIK_LCONTROL,   keyLeftControl  },
	{ DIK_SEMICOLON,  keySemiColon    },
	{ DIK_APOSTROPHE, keySingleQuote  },
	{ DIK_LSHIFT,     keyLeftShift    },
	{ DIK_BACKSLASH,  keyBackSlash    },
	{ DIK_COMMA,      keyComma        },
	{ DIK_PERIOD,     keyPeriod       },
	{ DIK_SLASH,      keySlash        },
	{ DIK_RSHIFT,     keyRightShift   },
	{ DIK_MULTIPLY,   keyNumPadMult   },
	{ DIK_LMENU,      keyLeftAlt      },
	{ DIK_SPACE,      keySpace        },
	{ DIK_CAPITAL,    keyCapsLock     },
	{ DIK_F1,         keyF1           },
	{ DIK_F2,         keyF2           },
	{ DIK_F3,         keyF3           },
	{ DIK_F4,         keyF4           },
	{ DIK_F5,         keyF5           },
	{ DIK_F6,         keyF6           },
	{ DIK_F7,         keyF7           },
	{ DIK_F8,         keyF8           },
	{ DIK_F9,         keyF9           },
	{ DIK_F10,        keyF10          },
	{ DIK_F11,        keyF11          },
	{ DIK_F12,        keyF12          },
	{ DIK_NUMLOCK,    keyNumLock      },
	{ DIK_SCROLL,     keyScrollLock   },
	{ DIK_SUBTRACT,   keyNumPadMinus  },
	{ DIK_ADD,        keyNumPadPlus   },
	{ DIK_NUMPAD0,    keyNumPad0      },
	{ DIK_NUMPAD1,    keyNumPad1      },
	{ DIK_NUMPAD2,    keyNumPad2      },
	{ DIK_NUMPAD3,    keyNumPad3      },
	{ DIK_NUMPAD4,    keyNumPad4      },
	{ DIK_NUMPAD5,    keyNumPad5      },
	{ DIK_NUMPAD6,    keyNumPad6      },
	{ DIK_NUMPAD7,    keyNumPad7      },
	{ DIK_NUMPAD8,    keyNumPad8      },
	{ DIK_NUMPAD9,    keyNumPad9      },
	{ DIK_DECIMAL,    keyNumPadDot    },
	{ DIK_DIVIDE,     keyNumPadSlash  },
	{ DIK_RMENU,      keyRightAlt     },
	{ DIK_PAUSE,      keyPause        },
	{ DIK_HOME,       keyHome         },
	{ DIK_UP,         keyUp           },
	{ DIK_PRIOR,      keyPgUp         },
	{ DIK_LEFT,       keyLeft         },
	{ DIK_RIGHT,      keyRight        },
	{ DIK_END,        keyEnd          },
	{ DIK_DOWN,       keyDown         },
	{ DIK_NEXT,       keyPgDown       },
	{ DIK_INSERT,     keyIns          },
	{ DIK_DELETE,     keyDel          },
	{ DIK_RETURN,     keyEnter        },
	{ DIK_A,          keyA            },
	{ DIK_B,          keyB            },
	{ DIK_C,          keyC            },
	{ DIK_D,          keyD            },
	{ DIK_E,          keyE            },
	{ DIK_F,          keyF            },
	{ DIK_G,          keyG            },
	{ DIK_H,          keyH            },
	{ DIK_I,          keyI            },
	{ DIK_J,          keyJ            },
	{ DIK_K,          keyK            },
	{ DIK_L,          keyL            },
	{ DIK_M,          keyM            },
	{ DIK_N,          keyN            },
	{ DIK_O,          keyO            },
	{ DIK_P,          keyP            },
	{ DIK_Q,          keyQ            },
	{ DIK_R,          keyR            },
	{ DIK_S,          keyS            },
	{ DIK_T,          keyT            },
	{ DIK_U,          keyU            },
	{ DIK_V,          keyV            },
	{ DIK_W,          keyW            },
	{ DIK_X,          keyX            },
	{ DIK_Y,          keyY            },
	{ DIK_Z,          keyZ            }
};

int	keyMap [256];

MetaClass	DirectXInputReader :: classInstance ( "DirectXInputReader", &InputReader :: classInstance );

DirectXInputReader :: DirectXInputReader ( View * view, const char * aName ) : InputReader ( view, aName )
{
	hWindow        = (HWND) view -> getWindow ();
	hInstance      = (HINSTANCE) GetModuleHandle ( NULL ); //GetWindowLong ( hWindow, GWL_HINSTANCE );
	inputObject    = NULL;
	keyboardDevice = NULL;
	mouseDevice    = NULL;
	metaClass      = &classInstance;

	HRESULT	hr;

#if	DIRECTINPUT_VERSION < 0x0800
	hr = DirectInputCreate ( hInstance, DIRECTINPUT_VERSION, &inputObject, NULL );
#else
	hr = DirectInput8Create ( hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (VOID**)&inputObject, NULL );
#endif

	if ( hr != DI_OK )
	{
		switch ( hr )
		{
			case DIERR_BETADIRECTINPUTVERSION:
				MessageBox ( NULL, "Beta", "DirectInpurt error", MB_OK );
				break;

			case DIERR_INVALIDPARAM:
				MessageBox ( NULL, "Invalid version", "DirectInpurt error", MB_OK );
				break;

			case DIERR_OLDDIRECTINPUTVERSION:
				MessageBox ( NULL, "Old version", "DirectInpurt error", MB_OK );
				break;

			case DIERR_OUTOFMEMORY:
				MessageBox ( NULL, "out of memory", "DirectInpurt error", MB_OK );
				break;

			default:
				MessageBox ( NULL, "other", "DirectInpurt error", MB_OK );
				break;
		}

		return;
	}

	if ( inputObject -> CreateDevice ( GUID_SysKeyboard, &keyboardDevice, NULL ) != DI_OK )
		return;

	if ( keyboardDevice == NULL )
		return;

	if ( keyboardDevice -> SetDataFormat ( &c_dfDIKeyboard ) != DI_OK )
		return;

	if ( keyboardDevice -> SetCooperativeLevel ( hWindow, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND ) != DI_OK )
		return;

	keyboardDevice -> Acquire ();

	if ( inputObject -> CreateDevice ( GUID_SysMouse, &mouseDevice, NULL ) != DI_OK )
		return;

	if ( mouseDevice == NULL )
		return;

	if ( mouseDevice -> SetDataFormat ( &c_dfDIMouse/*2*/ ) != DI_OK )
	{
		mouseDevice -> Release ();			// to force isOk to return false
		mouseDevice = NULL;

		return;
	}

	if ( mouseDevice -> SetCooperativeLevel ( hWindow, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND ) != DI_OK )
	{
		mouseDevice -> Release ();			// to force isOk to return false
		mouseDevice = NULL;

		return;
	}

	mouseDevice -> Acquire ();


											// now build keyMap
	memset ( keyMap, '\0', sizeof ( keyMap ) );

	for ( int i= 0; i < sizeof ( keyTable ) / sizeof ( keyTable [0] ); i++ )
		keyMap [keyTable [i].dikCode] = keyTable [i].code;
}

DirectXInputReader :: ~DirectXInputReader ()
{
	if ( keyboardDevice != NULL )
	{
		keyboardDevice -> Unacquire ();
		keyboardDevice -> Release ();
	}

	if ( mouseDevice != NULL )
	{
		mouseDevice -> Unacquire ();
		mouseDevice -> Release ();
	}

	if ( inputObject != NULL )
		inputObject -> Release ();
}

bool	DirectXInputReader :: getKeyboardState ( KeyboardState& keyState )
{
	if ( keyboardDevice == NULL )
		return false;

	char	keys [256];

	for ( ; ; )
	{
		HRESULT	hr = keyboardDevice -> GetDeviceState ( 256, keys );
		
		if ( SUCCEEDED ( hr ) )
			break;
		
		if ( hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED )
		{
			if ( keyboardDevice -> Acquire () != DI_OK )
				return false;
		}
		else
			return false;
	}

	memset ( keyState.getKeys (), '\0', 256 );

	for ( int i = 0; i < 256; i++ )
		if ( keys [i] )
			keyState.getKeys () [keyMap [i]] = 1;

	return true;
}

bool	DirectXInputReader :: getMouseState ( MouseState& mouse )
{
	if ( mouseDevice == NULL )
		return false;

	DIMOUSESTATE	ms;

	ZeroMemory ( &ms, sizeof ( ms ) );

	for ( ; ; )
	{
		HRESULT	hr = mouseDevice -> GetDeviceState ( sizeof ( ms ), &ms );
		
		if ( SUCCEEDED ( hr ) )
			break;
		
		if ( hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED )
		{
			if ( mouseDevice -> Acquire () != DI_OK )
				while ( mouseDevice -> Acquire () == DIERR_INPUTLOST )
					;
				//return false;
		}
		else
			return false;
	}

	mouse.setState ( ms.lX, ms.lY, ms.lZ, ms.rgbButtons [0] != 0, ms.rgbButtons [1] != 0, ms.rgbButtons [2] != 0, ms.rgbButtons [3] != 0 );
	
	return true;
}
