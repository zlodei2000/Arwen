//
// Simple Quake-like console
//
// Author: Alex V. Boreskoff
//
// Last modified: 20/11/2002
//

#include	<windows.h>
#include	<gl/gl.h>

#include	"Console.h"
#include	"Array.h"
#include	"Poly3D.h"
#include	"OpenGLView.h"
#include	"Texture.h"
#include	"Font.h"
#include	"ResourceManager.h"
#include	"BasicCommands.h"
#include	"Timer.h"
#include	"Controller.h"
#include	"Vector2D.h"
#include	"Application.h"

MetaClass	Console :: classInstance ( "Console", &Object :: classInstance );

Console :: Console ( const String& theTextureName, Controller * theController ) : Object ( "Main console" )
{
	controller  = theController;
	textureName = theTextureName; 
	background  = Application :: instance -> getResourceManager () -> getTexture ( textureName );
	font        = Application :: instance -> getResourceManager () -> getSystemFont ();
	text        = new Array ( "Console text"    );
	history     = new Array ( "Console history" );
	curLine     = 0;
	bottom      = 0.5f;
	state       = consoleInactive;
	timer       = NULL;
	cursorPos   = 0;
	linePos     = 0;
	metaClass   = &classInstance;

	addCommand ( new FullScreenCommand () );
	addCommand ( new SetVidModeCommand () );
	addCommand ( new SetVarCommand     () );
	addCommand ( new SetFovCommand     () );
	addCommand ( new MapCommand        () );
	addCommand ( new ReloadCommand     () );
}

Console :: ~Console ()
{
	background -> release ();
	text       -> release ();
	history    -> release ();

	delete font;
}

bool	Console :: isOk () const
{
	return background != NULL && background -> isOk () && text != NULL && text -> isOk ();
}

Object * Console :: clone () const
{
	return new Console ( textureName, controller );
}

void	Console :: draw ( View * view )
{
	if ( state == consoleInactive )
		return;

												// update state
	if ( state == consoleOpening )
		if ( (bottom += 0.05f) >= 0.7f )
		{
			state  = consoleActive;
			bottom = 0.7f;
		}

	if ( state == consoleClosing )
		if ( (bottom -= 0.05f) <= 0.0f )
		{
			state  = consoleInactive;
			bottom = 0.0f;

			timer -> release ();

			timer = NULL;

			return;
		}

													// draw the poly
	float		tWidth  = (float) background -> getWidth ();
	float		tHeight = (float) background -> getHeight ();
	float		vWidth  = (float) view -> getWidth ();
	float		vHeight = (float) view -> getHeight ();
	float		yStart  = (1.0f - bottom) * vHeight;
	float		alpha   = bottom / 0.7f;
	Polygon3D	poly ( "", 4 );

	poly.setTexture ( background );
	poly.addVertex  ( Vector3D ( 0,      yStart,           0 ), Vector2D ( 0, 0 ), Vector4D ( 1, 1, 1, alpha ) );
	poly.addVertex  ( Vector3D ( vWidth, yStart,           0 ), Vector2D ( 1, 0 ), Vector4D ( 1, 1, 1, alpha ) );
	poly.addVertex  ( Vector3D ( vWidth, yStart + vHeight, 0 ), Vector2D ( 1, 1 ), Vector4D ( 1, 1, 1, alpha ) );
	poly.addVertex  ( Vector3D ( 0,      yStart + vHeight, 0 ), Vector2D ( 0, 1 ), Vector4D ( 1, 1, 1, alpha ) );
	
	view -> startOrtho ();
	view -> draw       ( poly );
	view -> endOrtho   ();
													// now draw the text in console
	int	textStart = (int)(vHeight * bottom ) - 16;

	font -> setColor ( Vector4D ( 1.0f, 1.0f, 0.0f, alpha ) );

	font -> print ( view, 10, textStart, "]%s", str.c_str () );
	
	for ( int i = text -> getCount () - 1, y = textStart - 16; i >= 0; i--, y -= 16 )
	{
		String * s = (String *) text -> at ( i );

		font -> print ( view, 10, y, s -> c_str () );
	}

	float	curTime = timer -> getTime ();

	if ( curTime - cursorOnTime < 0.5f )				// draw the cursor
	{
		float	cursorPosX = 10.0f + (cursorPos + 1) * 16.0f;
		float	cursorPosY = vHeight - (float) textStart;

		view -> startOrtho ();

		glDisable  ( GL_TEXTURE_2D );
		glColor4f  ( 1.0f, 1.0f, 0.0f, alpha );
		glBegin    ( GL_QUADS );
		glVertex2f ( cursorPosX,      cursorPosY );
		glVertex2f ( cursorPosX + 12, cursorPosY );
		glVertex2f ( cursorPosX + 12, cursorPosY + 15.0f );
		glVertex2f ( cursorPosX,      cursorPosY + 15.0f );
		glEnd      ();
		glEnable   ( GL_TEXTURE_2D );

		view -> endOrtho ();
	}

	if ( curTime - cursorOnTime >= 1.0f )
		cursorOnTime = curTime;
}

int	Console :: handleChar ( int key )
{
	if ( key == '~' || key == '`' )
	{
		switch ( state )
		{
			case consoleInactive:
					state        = consoleOpening;
					bottom       = 0.0f;
					timer        = getTimer ();
					cursorOnTime = timer -> getTime ();

					break;

			case consoleOpening:
					state = consoleClosing;

					break;

			case consoleActive:
					state  = consoleClosing;
					bottom = 0.7f;

					break;

			case consoleClosing:
					state = consoleOpening;

					break;
			default:
				break;
		}

		return state != consoleInactive;
	}

	if ( state == consoleInactive )		// skip all chars when inactive (except '~')
		return 0;

	if ( key == '\r' )					// do the command
	{
		addString ( str );
		execute   ( str );

		str = "";
	}
	else
	if ( key == '\b' )					// erase last character in string
	{
		if ( cursorPos > 0 )
		{
			str.cut ( cursorPos - 1, 1 );
			cursorPos --;
		}
	}
	else
	if ( key >= 32 )
	{
		str.insert ( cursorPos, (char) key );
		cursorPos++;
	}

	return state != consoleInactive;
}

int	Console :: handleKey ( int key, bool pressed )
{
	if ( state == consoleInactive )		// skip all chars when inactive (except '~')
		return 0;

	if ( !pressed )
		return state != consoleInactive;

	if ( key == keyLeft )
	{
		if ( cursorPos > 0 )
			cursorPos--;
	}
	else
	if ( key == keyRight )
	{
		if ( cursorPos < str.getLength () - 1 )
			cursorPos++;
	}
	else
	if ( key == keyDel )
	{
		if ( cursorPos < str.getLength () )
			str.cut ( cursorPos, 1 );
	}
	else
	if ( key == keyHome )
		cursorPos = 0;
	else
	if ( key == keyEnd )
		cursorPos = str.getLength ();
	else
	if ( key == keyDown )
	{
		if ( ++curLine >= history -> getNumItems () )
			str = saveStr;
		else
			str = * (String *) history -> at ( curLine );

		if ( cursorPos > str.getLength () )
			cursorPos = str.getLength ();
	}
	else
	if ( key == keyUp )
	{
		if ( curLine >= history -> getNumItems () )
			saveStr = str;

		if ( curLine > 0 )
			str = * (String *) history -> at ( --curLine );

		if ( cursorPos > str.getLength () )
			cursorPos = str.getLength ();
	}

	return state != consoleInactive;
}

void	Console :: addString ( const String& command )
{
	text -> insertNoRetain ( new String ( command ) );
}

void	Console :: execute ( const String& str )
{
	Array	argv;
	String	buf;

	history -> insertNoRetain ( new String ( str ) );

	curLine++;
	cursorPos = 0;

	for ( int i = 0; i < str.getLength (); i++ )
	{
		char	ch = str [i];

		if ( ch == ' ' || ch == '\t' )
		{
			if ( buf.getLength () > 0 )
				argv.insertNoRetain ( new String ( buf ) );

			buf = "";
		}
		else
			buf += ch;
	}

	if ( buf.getLength () > 0 )
		argv.insertNoRetain ( new String ( buf ) );

	if ( !executeHelp ( argv ) )
		for ( Set :: Iterator it = commands.getIterator (); !it.end (); ++it )
		{
			ConsoleCommand * com = (ConsoleCommand *) it.value ();

			com -> execute ( argv, this );
		}
}

bool	Console :: executeHelp ( const Array& argv )
{
	String * key = (String *) argv.at ( 0 );

	if ( *key != "help" )
		return false;

	if ( argv.getCount () != 2 )				// return list of console commands
	{
		String	curStr;

		addString ( "Available commands:" );

		for ( Set :: Iterator it = commands.getIterator (); !it.end (); ++it )
		{
			ConsoleCommand * command = (ConsoleCommand *) it.value ();

			curStr += String ().printf ( "%-15s", command -> getName () );

			if ( curStr.getLength () >= 75 )	// overflow
			{
				addString ( curStr );

				curStr = "";
			}
		}

		if ( !curStr.isEmpty () )
			addString ( curStr );

		addString ( "Type \"help command-name\" for help on any command" );
	}
	else
	{
		String * arg = (String *) argv.at ( 1 );

		for ( Set :: Iterator it = commands.getIterator (); !it.end (); ++it )
		{
			ConsoleCommand * com = (ConsoleCommand *) it.value ();

			if ( *arg == com -> getName () )
			{
				com -> printHelp ( this );

				return true;
			}
		}

		addString ( String ().printf ( "Unknown command:%s", arg -> c_str () ) );
	}

	return true;
}

void	Console :: addCommand ( ConsoleCommand * command )
{
	commands.insert ( command );
}
