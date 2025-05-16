//
// Simple Quake-like console
//
// Author: Alex V. Boreskoff
//
// Last modified: 22/08/2002
//

#ifndef	__CONSOLE__
#define	__CONSOLE__

#include	"Object.h"
#include	"ObjStr.h"
#include	"Set.h"
										// console states
enum
{
	consoleInactive = 0,
	consoleOpening,
	consoleActive,
	consoleClosing
};

class	Array;							// class predeclarations
class	View;
class	Texture;
class	Font;
class	ConsoleCommand;
class	Controller;
class	ResourceManager;
class	Timer;

class	Console : public Object
{
private:
	Texture         * background;		// shaded background
	float		      bottom;			// coordinate of bottom of console in 0..1 range
	Array           * text;				// text in console
	Array           * history;
	int			      curLine;			// text top line in console
	String			  saveStr;
	int			      state;			// internal state of console - active, inactive, opening or closing
	String		      str;				// currently typed string
	Font            * font;				// font used for drawing the text
	float		      cursorOnTime;		// time in millisecs since cursor was on
	Set			      commands;			// a set of ConsoleCommand objects
	Controller      * controller;
	ResourceManager * resourceManager;
	Timer           * timer;
	int				  cursorPos;
	int				  linePos;			// position of current line in histoy
	String			  textureName;
public:
	Console ( const String& textureName, Controller * theController );
	~Console ();
										// overriden Object methods
    virtual bool     isOk () const;
	virtual	Object * clone () const;

	void	draw        ( View * );					// draw the console on top of screene
	int		handleChar  ( int ch );					// handle character, returns non-zero if console is active
	int		handleKey   ( int key, bool pressed );	// handle key, returns non-zero if console is active
	void	addCommand  ( ConsoleCommand * command );
	void	addString   ( const String& );
	void	execute     ( const String& str );
	bool	executeHelp ( const Array& argv );

	int	getState () const
	{
		return state;
	}

	Controller * getController () const
	{
		return controller;
	}

	static	MetaClass	classInstance;
};

#endif
