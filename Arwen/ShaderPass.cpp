//
// Class describing one shader for for OpenGl shader in Arwen project
//
// Author: Alex V. Boreskoff
//
// Last modified: 14/11/2002
//

#include	<windows.h>
#include	<gl/gl.h>

#include	"ShaderPass.h"
#include	"Transform2D.h"
#include	"Texture.h"
#include	"Poly3D.h"
#include	"Mesh3D.h"
#include	"View.h"

MetaClass	ShaderPass :: classInstance ( "ShaderPass", &Object :: classInstance );

ShaderPass :: ShaderPass ( const char * theName, Texture * theTexture, const Vector4D& theColor, bool env ) : Object ( theName )
{
	texture       = theTexture;
	color         = theColor;
	envMapped     = env;
	srcBlend      = View :: bmNone;
	dstBlend      = View :: bmNone;
	textureTransf = NULL;
	metaClass     = &classInstance;

	if ( texture != NULL )
		texture -> retain ();
}

ShaderPass :: ~ShaderPass ()
{
	if ( texture != NULL )
		texture -> release ();

	delete textureTransf;
}

bool	ShaderPass :: isTransparent () const
{
							// if we use som blending mode (other that replacing)
							// then we assume we're tranparent
	if ( srcBlend != View :: bmNone && dstBlend != View :: bmNone )
		return true;

	return false;
}

void	ShaderPass :: setTransform ( const Transform2D& tr )
{
	delete textureTransf;
	
	textureTransf = new Transform2D ( tr );
}

void	ShaderPass :: drawPoly ( View& view, const Polygon3D * poly ) const
{
	glPushAttrib ( GL_COLOR_BUFFER_BIT | GL_ENABLE_BIT | GL_TRANSFORM_BIT );

	if ( textureTransf != NULL )		// setup texture matrix
	{
		float	m [16];

		textureTransf -> buildHomogeneousMatrix ( m );

		glMatrixMode  ( GL_TEXTURE );
		glPushMatrix  ();
		glMultMatrixf ( m );
	}

	if ( envMapped )					// setup spherical env. mapping
	{
		glTexGeni ( GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP    );
		glTexGeni ( GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP    );
		glEnable  ( GL_TEXTURE_GEN_S );
		glEnable  ( GL_TEXTURE_GEN_T );
	}

	if ( srcBlend != View :: bmNone && dstBlend != View :: bmNone )
	{
		glEnable ( GL_BLEND );

		view.blendFunc ( srcBlend, dstBlend );
	}
										// draw polygon
	view.bindTexture ( texture );

	Vector4D	c;
	Vector4D	clr ( color * poly -> getColor () );
	int			numVertices = poly -> getNumVertices ();
	float		invWidth    = 1.0f / (float) texture -> getWidth  ();
	float		invHeight   = 1.0f / (float) texture -> getHeight ();

	const Vector2D * uvMap    = poly -> getUvMap    ();
	const Vector3D * vertices = poly -> getVertices ();
	const Vector4D * colors   = poly -> getColors   ();

	glBegin ( GL_POLYGON );

	for ( int i = 0; i < numVertices; i++ )
	{
		if ( colors != NULL )
			c = colors [i] * clr ;
		else
			c = clr;

		glColor4fv    ( c            );
		glTexCoord2fv ( uvMap [i]    );
		glVertex3fv   ( vertices [i] );
	}

	glEnd();

										// restore texture matrix
	if ( textureTransf != NULL )
		glPopMatrix  ();

	glPopAttrib ();
}

void	ShaderPass :: drawMesh ( View& view, const Camera& camera, const Vector4D& theColor, Fog * fog, const Mesh3D * mesh ) const
{
	bool	transp = srcBlend != View :: bmNone && dstBlend != View :: bmNone;

	glPushAttrib ( GL_COLOR_BUFFER_BIT | GL_ENABLE_BIT | GL_TRANSFORM_BIT );

	if ( textureTransf != NULL )		// setup texture matrix
	{
		float	m [16];

		textureTransf -> buildHomogeneousMatrix ( m );

		glMatrixMode  ( GL_TEXTURE );
		glPushMatrix  ();
		glMultMatrixf ( m );
	}

	if ( envMapped )					// setup spherical env. mapping
	{
		glTexGeni ( GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP    );
		glTexGeni ( GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP    );
		glEnable  ( GL_TEXTURE_GEN_S );
		glEnable  ( GL_TEXTURE_GEN_T );
	}

	if ( transp )
	{
		glEnable ( GL_BLEND );
		
		view.blendFunc ( srcBlend, dstBlend );
	}
										// draw mesh
	mesh -> draw ( view, camera, color * theColor, fog, texture, transp );

										// restore texture matrix
	if ( textureTransf != NULL )
		glPopMatrix  ();

	glPopAttrib ();						// restore all other attributes
}

