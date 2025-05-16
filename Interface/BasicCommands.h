//
// Some basic console commands
//
// Author: Alex V. Boreskoff
//
// Last modified: 14/11/2002
//

#ifndef	__BASIC_COMMANDS__
#define	__BASIC_COMMANDS__

#include	"ConsoleCommand.h"

class	FullScreenCommand :public ConsoleCommand
{
public:
	FullScreenCommand () : ConsoleCommand ( "fullscreen" ) {}

	virtual	void	execute   ( const Array& argv, Console * console );
	virtual	void	printHelp ( Console * console ) const;

	static	MetaClass	classInstance;
};

class	SetVidModeCommand : public ConsoleCommand
{
public:
	SetVidModeCommand () : ConsoleCommand ( "setmode" ) {}

	virtual	void	execute   ( const Array& argv, Console * console );
	virtual	void	printHelp ( Console * console ) const;

	static	MetaClass	classInstance;
};

class	SetVarCommand : public ConsoleCommand
{
public:
	SetVarCommand () : ConsoleCommand ( "set" ) {}

	virtual	void	execute   ( const Array& argv, Console * console );
	virtual	void	printHelp ( Console * console ) const {}

	static	MetaClass	classInstance;
};

class	SetFovCommand : public ConsoleCommand
{
public:
	SetFovCommand () : ConsoleCommand ( "fov" ) {}

	virtual	void	execute   ( const Array& argv, Console * console );
	virtual	void	printHelp ( Console * console ) const;

	static	MetaClass	classInstance;
};

class	MapCommand : public ConsoleCommand
{
public:
	MapCommand () : ConsoleCommand ( "map" ) {}

	virtual	void	execute   ( const Array& argv, Console * console );
	virtual	void	printHelp ( Console * console ) const;

	static	MetaClass	classInstance;
};

class	ReloadCommand : public ConsoleCommand
{
public:
	ReloadCommand () : ConsoleCommand ( "reload" ) {}

	virtual	void	execute   ( const Array& argv, Console * console );
	virtual	void	printHelp ( Console * console ) const;

	static	MetaClass	classInstance;
};

#endif
