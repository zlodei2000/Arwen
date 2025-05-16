#include	<windows.h>
#include	"Application.h"

int PASCAL WinMain ( HINSTANCE hCurInstance, HINSTANCE hPrevInstance, LPSTR cmdLine, int cmdShow )
{
	Application	app ( "Arwen", cmdLine );

	if ( app.isOk () )
		return app.run ();

	return 0;
}
