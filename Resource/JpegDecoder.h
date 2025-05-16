//
// Class to laod jpeg files.
// Based on libjpeg.
//
// Author: Alex V. Boreskoff
//
// Last modified: 6/11/2002
//

#ifndef	__JPEG_LOADER__
#define	__JPEG_LOADER__

#include	"ResourceDecoder.h"

class	JpegDecoder : public ResourceDecoder
{
private:
	int             palette [256];
	int             width;
	int             height;
public:
	JpegDecoder ( ResourceManager * rm ) : ResourceDecoder ( "JepgResourceDecoder", rm ) 
	{
		metaClass = &classInstance;
	}

	virtual	Object * clone () const
	{
		return new JpegDecoder ( getResourceManager () );
	}

	virtual	Object * decode         ( Data * );
	virtual	bool	 checkExtension ( const String& theName );

	static	MetaClass	classInstance;
};

#endif
