//
// Main file of Arwen portal engine demo 
//
// Last modified 3/06/2003
//
// Author: Alex V. Boreskoff
//

#include	<windows.h>
#include	<stdio.h>
#include	"Application.h"

											// main function of the app
int PASCAL WinMain ( HINSTANCE hCurInstance, HINSTANCE hPrevInstance, LPSTR cmdLine, int cmdShow )
{
	Application	app ( "Arwen", cmdLine );

	if ( app.isOk () )
		return app.run ();

	return 0;
}

