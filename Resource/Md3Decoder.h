//
// Class to load Quake III .md3 models
//
// Author: Alex V. Boreskoff
//
// Last modified: 6/11/2002
//

#ifndef	__MD3_DECODER__
#define	__MD3_DECODER__

#include	"ResourceDecoder.h"
#include	"Vector3D.h"

class	String;

class	Md3Decoder : public ResourceDecoder
{
private:
	Vector3D	normalTable [256][256];

public:
	Md3Decoder ( ResourceManager * rm );

	virtual	Object * decode         ( Data * data );
	virtual	bool     checkExtension ( const String& theName );

	static	MetaClass	classInstance;
};

#endif
