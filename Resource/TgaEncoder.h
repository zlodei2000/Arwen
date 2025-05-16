//
// Class to encode textures as tga files
//
// Author: Alex V. Boreskoff
//
// Last modified: 20/11/2002
//

#ifndef	__TGA_ENCODER__
#define	__TGA_ENCODER__

#include	"Object.h"
#include	"TgaStructs.h"

class	String;
class	MutableData;
class	Dictionary;

class	TgaEncoder : public Object
{
private:
	TgaHeader  hdr;
	int	       width;
	int	       height;
public:
	TgaEncoder () : Object ( "TgaEncoder" ) 
	{
		metaClass = &classInstance;
	}

	virtual	bool	encode ( const Object * object, MutableData * data, Dictionary * hints = NULL );

	static	MetaClass	classInstance;

private:
	bool	encodeRgb24 ( MutableData *, byte * );
	bool	encodeRgb32 ( MutableData *, byte * );
};

#endif
