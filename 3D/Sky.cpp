//
// Simple class for drawing sky based on cubic maps
//
// Author: Alex V. Boreskoff
//
// Last modified: 20/12/2001
//

#include	<windows.h>
#include	<gl/gl.h>

#include	"Camera.h"
#include	"Sky.h"
#include	"Application.h"
#include	"ResourceManager.h"

static	char * skyExt [6] = { "ft", "bk", "lf", "rt", "up", "dn" };

MetaClass	Sky :: classInstance ( "Sky", &Object :: classInstance );

Sky :: Sky ( const char * theName, const char * ext ) : Object ( theName )
{
	String	texName ( theName );

	metaClass = &classInstance;
	skySize   = 2000.0f;

	for ( int i = 0; i < 6; i++ )
	{
		skyTextures [i] = Application ::instance -> getResourceManager () -> getTexture ( texName + skyExt [i] + ext );

		if ( skyTextures [i] == NULL )
			continue;

		invWidth  [i] = 1.0f / (float) skyTextures [i] -> getWidth  ();
		invHeight [i] = 1.5f / (float) skyTextures [i] -> getHeight ();
	}
}

Sky :: ~Sky ()
{
	for ( int i = 0; i < 6; i++ )
		if ( skyTextures [i] != NULL )
			skyTextures [i] -> release ();
}

bool	Sky :: isOk () const
{
	for ( int i = 0; i < 6; i++ )
		if ( skyTextures [i] == NULL )
			return false;
		else
			if ( !skyTextures [i] -> isOk () )
				return false;

	return true;
}

void	Sky :: draw ( const Camera& camera )
{
	Vector3D	pos      ( camera.getPos () );
	BoundingBox	frontBox ( Vector3D ( pos.x - skySize, pos.y + skySize, pos.z - skySize ), 
						   Vector3D ( pos.x + skySize, pos.y + skySize, pos.z + skySize ) );

	BoundingBox	backBox  ( Vector3D ( pos.x - skySize, pos.y - skySize, pos.z - skySize ), 
						   Vector3D ( pos.x + skySize, pos.y - skySize, pos.z + skySize ) );

	BoundingBox	leftBox  ( Vector3D ( pos.x - skySize, pos.y - skySize, pos.z - skySize ),
						   Vector3D ( pos.x - skySize, pos.y + skySize, pos.z + skySize	) );

	BoundingBox	rightBox ( Vector3D ( pos.x + skySize, pos.y - skySize, pos.z - skySize ),
						   Vector3D ( pos.x + skySize, pos.y + skySize, pos.z + skySize	) );

	BoundingBox	upBox    ( Vector3D ( pos.x - skySize, pos.y - skySize, pos.z + skySize ),
						   Vector3D ( pos.x + skySize, pos.y + skySize, pos.z + skySize ) );

	BoundingBox	downBox  ( Vector3D ( pos.x - skySize, pos.y - skySize, pos.z - skySize ),
						   Vector3D ( pos.x + skySize, pos.y + skySize, pos.z - skySize ) );

	if ( camera.inViewingFrustrum ( frontBox ) )
	{
		glBindTexture ( GL_TEXTURE_2D, skyTextures [0] -> getId () );
		glBegin       ( GL_QUADS );

		glTexCoord2f  ( 1.0f - invWidth [0], invHeight [0] );
		glVertex3d    ( pos.x - skySize, pos.y + skySize, pos.z + skySize );

		glTexCoord2f  ( invWidth [0], invHeight [0] );
		glVertex3f    ( pos.x + skySize, pos.y + skySize, pos.z + skySize );

		glTexCoord2f  ( invWidth [0], 1.0f - invHeight [0] );
		glVertex3f    ( pos.x + skySize, pos.y + skySize, pos.z - skySize );

		glTexCoord2f  ( 1.0f - invWidth [0], 1.0f - invHeight [0] );
		glVertex3f    ( pos.x - skySize, pos.y + skySize, pos.z - skySize );

		glEnd         ();
	}

	if ( camera.inViewingFrustrum ( backBox ) )
	{
		glBindTexture ( GL_TEXTURE_2D, skyTextures [1] -> getId () );
		glBegin       ( GL_QUADS );

		glTexCoord2f  ( invWidth [1], invHeight [1] );
		glVertex3d    ( pos.x - skySize, pos.y - skySize, pos.z + skySize );

		glTexCoord2f  ( invWidth [1], 1.0f - invHeight [1] );
		glVertex3f    ( pos.x - skySize, pos.y - skySize, pos.z - skySize );

		glTexCoord2f  ( 1.0f - invWidth [1], 1.0f - invHeight [1] );
		glVertex3f    ( pos.x + skySize, pos.y - skySize, pos.z - skySize );

		glTexCoord2f  ( 1.0f - invWidth [1], invHeight [1] );
		glVertex3f    ( pos.x + skySize, pos.y - skySize, pos.z + skySize );

		glEnd         ();
	}

	if ( camera.inViewingFrustrum ( leftBox ) )
	{
		glBindTexture ( GL_TEXTURE_2D, skyTextures [2] -> getId () );
		glBegin       ( GL_QUADS );

		glTexCoord2f  ( 1.0f - invWidth [2], invHeight [2] );
		glVertex3d    ( pos.x - skySize, pos.y - skySize, pos.z + skySize );

		glTexCoord2f  ( invWidth [2], invHeight [2] );
		glVertex3f    ( pos.x - skySize, pos.y + skySize, pos.z + skySize );

		glTexCoord2f  ( invWidth [2], 1.0f - invHeight [2] );
		glVertex3f    ( pos.x - skySize, pos.y + skySize, pos.z - skySize );

		glTexCoord2f  ( 1.0f - invWidth [2], 1.0f - invHeight [2] );
		glVertex3f    ( pos.x - skySize, pos.y - skySize, pos.z - skySize );

		glEnd         ();
	}

	if ( camera.inViewingFrustrum ( rightBox ) )
	{
		glBindTexture ( GL_TEXTURE_2D, skyTextures [3] -> getId () );
		glBegin       ( GL_QUADS );

		glTexCoord2f  ( 1.0f - invWidth [3], invHeight [3] );
		glVertex3d    ( pos.x + skySize, pos.y + skySize, pos.z + skySize );

		glTexCoord2f  ( invWidth [3], invHeight [3] );
		glVertex3f    ( pos.x + skySize, pos.y - skySize, pos.z + skySize );

		glTexCoord2f  ( invWidth [3], 1.0f - invHeight [3] );
		glVertex3f    ( pos.x + skySize, pos.y - skySize, pos.z - skySize );

		glTexCoord2f  ( 1.0f - invWidth [3], 1.0f - invHeight [3] );
		glVertex3f    ( pos.x + skySize, pos.y + skySize, pos.z - skySize );

		glEnd         ();
	}

	if ( camera.inViewingFrustrum ( upBox ) )
	{
		glBindTexture ( GL_TEXTURE_2D, skyTextures [4] -> getId () );
		glBegin       ( GL_QUADS );

		glTexCoord2f  ( invWidth [4], 1.0f - invHeight [4] );
		glVertex3d    ( pos.x + skySize, pos.y - skySize, pos.z + skySize );

		glTexCoord2f  ( 1.0f - invWidth [4], 1.0f - invHeight [4] );
		glVertex3f    ( pos.x + skySize, pos.y + skySize, pos.z + skySize );

		glTexCoord2f  ( 1.0f - invWidth [4], invHeight [4] );
		glVertex3f    ( pos.x - skySize, pos.y + skySize, pos.z + skySize );

		glTexCoord2f  ( invWidth [4], invHeight [4] );
		glVertex3f    ( pos.x - skySize, pos.y - skySize, pos.z + skySize );

		glEnd         ();
	}

	if ( camera.inViewingFrustrum ( downBox ) )
	{
		glBindTexture ( GL_TEXTURE_2D, skyTextures [5] -> getId () );
		glBegin       ( GL_QUADS );

		glTexCoord2f  ( invWidth [5], 1.0f - invHeight [5] );
		glVertex3d    ( pos.x - skySize, pos.y - skySize, pos.z - skySize );

		glTexCoord2f  ( 1.0f - invWidth [5], 1.0f - invHeight [5] );
		glVertex3f    ( pos.x - skySize, pos.y + skySize, pos.z - skySize );

		glTexCoord2f  ( 1.0f - invWidth [5], invHeight [5] );
		glVertex3f    ( pos.x + skySize, pos.y + skySize, pos.z - skySize );

		glTexCoord2f  ( invWidth [5], invHeight [5] );
		glVertex3f    ( pos.x + skySize, pos.y - skySize, pos.z - skySize );

		glEnd         ();
	}
}
