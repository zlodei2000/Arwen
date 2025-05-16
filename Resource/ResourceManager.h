//
// Basic class to manage loading and freeing of resource objects (texures, sounds & etc).
// Supports pluggable texture loaders
//
// Author: Alex V. Boreskoff
//
// Last modified: 10/11/2002
//

#ifndef	__RESOURCE_MANAGER__
#define	__RESOURCE_MANAGER__

#include	"Array.h"
#include	"ResourceSource.h"
#include	"ObjStr.h"
#include	"PixelFormat.h"

class	Font;
class	Data;
class	ResourceDecoder;
class	Texture;

class	ResourceManager : public Object
{
private:
	Array		sources;				// file systems used to retrieve data
	Array		decoders;				// registered object loaders
	Array		objects;				// loaded objects
	PixelFormat	defFormats [3];			// Formats for RGB, RGBA and single-component textures
public:
	ResourceManager ( const char * theName );

	int		  registerSource    ( ResourceSource  * system );
	int		  unregisterSource  ( const String& theName );
	int	      registerDecoder   ( ResourceDecoder * loader );
	int		  unregisterDecoder ( const String& theName );
	Data    * getResource       ( const String& theName );
	Object  * getObject         ( const String& theName );
	Texture * getTexture        ( const String& theName );
	Font    * getSystemFont     ();
	void	  freeObject        ( const String& theName );
	void	  upload            ();

	void	setRgbFormat ( const PixelFormat& format )
	{
		defFormats [0] = format;
	}

	void	setRgbaFormat ( const PixelFormat& format )
	{
		defFormats [1] = format;
	}
	
	void	setMonoFormat ( const PixelFormat& format )
	{
		defFormats [2] = format;
	}

	const PixelFormat&	getRgbFormat () const
	{
		return defFormats [0];
	}

	const PixelFormat& getRgbaFormat () const
	{
		return defFormats [1];
	}

	const PixelFormat& getMonoFormat () const
	{
		return defFormats [2];
	}

	static	MetaClass	classInstance;
};

#endif
