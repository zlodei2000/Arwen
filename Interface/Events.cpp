//
// Basic events for windows messages
//
// Author: Alex V. Boreskoff
//
// Last modified: 28/11/2002
//

#include	"Events.h"
#include	"Controller.h"

MetaClass	Event     :: classInstance ( "Event",     &Object :: classInstance );
MetaClass	KeyEvent  :: classInstance ( "KeyEvent",  &Event  :: classInstance );
MetaClass	CharEvent :: classInstance ( "CharEvent", &Event  :: classInstance );

int	KeyEvent :: handle ( Controller * controller )
{
	if ( controller != NULL )
		return controller -> handleKey ( keyCode, pressed );

	return controllerContinue;
}

int	CharEvent :: handle ( Controller * controller )
{
	if ( controller != NULL )
		return controller -> handleChar ( ch );

	return controllerContinue;
}

