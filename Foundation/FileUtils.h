//
// Simple file utils
//

#ifndef	__FILE_UTILS__
#define	__FILE_UTILS__

#include	"ObjStr.h"

bool	fileExist ( const String& name );
bool	isDir     ( const String& name );
bool	createDir ( const String& name );

#endif
