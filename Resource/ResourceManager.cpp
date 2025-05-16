//
// Basic class to manage loading and freeing objects.
// Supports pluggable texture loaders
//
// Author: Alex V. Boreskoff
//
// Last modified: 12/12/2002
//

#include	"ResourceManager.h"
#include	"Log.h"
#include	"Font.h"
#include	"Data.h"
#include	"ResourceDecoder.h"
#include	"Texture.h"

MetaClass	ResourceManager :: classInstance ( "ResourceManager", &Object :: classInstance );

ResourceManager :: ResourceManager ( const char * theName ) : 
	    Object   ( theName ), 
		decoders ( "Registered object decoders" ),
		objects  ( "Loaded objects" )
{
	metaClass = &classInstance;

	defFormats [0].completeFromMasks ( 0xFF, 0xFF00, 0xFF0000 );
	defFormats [1].completeFromMasks ( 0xFF, 0xFF00, 0xFF0000, 0xFF000000 );
	defFormats [2].completeFromMasks ( 0,    0,      0,        0xFF );
}

Font * ResourceManager :: getSystemFont ()
{
	Texture * fontTexture = getTexture ( "../Textures/font.tga" );

	fontTexture -> setMipmap ( false );

	Font * font = new Font ( fontTexture, 16, 16, 16, 16 );

	fontTexture -> release ();

	return font;
}

int	ResourceManager :: registerSource ( ResourceSource * system )
{
	sources.insertNoRetain ( system );

	return 1;
}

int	ResourceManager :: unregisterSource ( const String& theName )
{
	return sources.removeObjectWithName ( theName );
}

int	ResourceManager :: registerDecoder ( ResourceDecoder * decoder )
{
	for ( Array :: Iterator it = decoders.getIterator (); !it.end (); ++it )
		if ( !strcmp ( it.value () -> getName (), decoder -> getName () ) )
			return 0;

	decoders.insertNoRetain ( decoder );

	return 1;
}

int	ResourceManager :: unregisterDecoder ( const String& theName )
{
	return decoders.removeObjectWithName ( theName );
}

Data    * ResourceManager :: getResource ( const String& theName )
{
	Data * data;

	for ( Array :: Iterator it = sources.getIterator (); !it.end (); ++it )
	{
		ResourceSource * source = (ResourceSource *) it.value ();

		if ( ( data = source -> getFile ( theName ) ) != NULL )
			return data;
	}

	(*sysLog) << "Cannot locate " << theName << logEndl;

	return NULL;
}

Texture * ResourceManager :: getTexture ( const String& theName )
{
	Object * object = getObject ( theName );

	return object == NULL ? NULL : dynamic_cast <Texture *> ( object );
}

Object * ResourceManager :: getObject ( const String& theName )
{
	Object * object = objects.getObjectWithName ( theName );

	if ( object != NULL )
		return object -> retain ();

									// the texture has not been loaded yet
	Data * data = getResource ( theName );

	if ( data == NULL )
		return NULL;

	for ( Array :: Iterator it = decoders.getIterator (); !it.end (); ++it )
	{
		ResourceDecoder * decoder = (ResourceDecoder *) it.value ();

		if ( decoder -> checkExtension ( theName ) )
		{
			data -> seekAbs ( 0 );

			if ( ( object = decoder -> decode ( data ) ) != NULL )
			{
				objects.insert ( object );

				object -> setName ( theName );
				data   -> release ();	

				return object;
			}
		}
	}

	for (Array::Iterator it = decoders.getIterator (); !it.end (); ++it )
	{
		ResourceDecoder * decoder = (ResourceDecoder *) it.value ();

		data -> seekAbs ( 0 );

		if ( ( object = decoder -> decode ( data ) ) != NULL )
		{
			object -> setName ( theName );
			data   -> release ();

			objects.insert ( object );

			return object;
		}
	}

	if ( data != NULL )
		data -> release ();

	(*sysLog) << "ResourceManager: cannot load " << theName << logEndl;

	return NULL;
}

void ResourceManager :: freeObject ( const String& theName )
{
	Object * object = objects.getObjectWithName ( theName );

	object -> release ();

	objects.removeObjectWithName ( theName );
}

void	ResourceManager :: upload ()
{
	for ( Array :: Iterator it = objects.getIterator (); !it.end (); ++it )
	{
		Texture * txt = dynamic_cast <Texture *> ( it.value () );

		if ( txt != NULL )
			txt -> upload ();
	}
}

