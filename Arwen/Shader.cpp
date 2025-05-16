//
// Simple Quake III-like shader class
//
// Author: Alex V. Boreskoff
//
// Last modified: 2/12/2002
//

#include	<windows.h>
#include	<gl/gl.h>

#include	"Shader.h"

MetaClass	Shader :: classInstance ( "Shader", &Object :: classInstance );

void	Shader :: drawPoly ( View& view, const Polygon3D * poly ) const
{
							// save OpenGL state
	glPushAttrib ( GL_COLOR_BUFFER_BIT | GL_ENABLE_BIT | GL_LIGHTING_BIT | GL_TEXTURE_BIT );

							// perform passes
	for ( Array :: Iterator it = passes.getIterator (); !it.end (); ++it )
	{
		ShaderPass * pass = (ShaderPass *) it.value ();

		pass -> drawPoly ( view, poly );
	}

							// restore OpenGL state
	glPopAttrib ();
}

void	Shader :: drawMesh ( View& view, const Camera& camera, const Vector4D& color, 
	                         Fog * fog,  const Mesh3D * mesh ) const
{
							// save OpenGL state
	glPushAttrib ( GL_COLOR_BUFFER_BIT | GL_ENABLE_BIT | GL_LIGHTING_BIT | GL_TEXTURE_BIT );

							// perform passes
	for ( Array :: Iterator it = passes.getIterator (); !it.end (); ++it )
	{
		ShaderPass * pass = (ShaderPass *) it.value ();

		pass -> drawMesh ( view, camera, color, fog, mesh );
	}

							// restore OpenGL state
	glPopAttrib ();
}

bool	Shader :: isTransparent () const
{
								// assume initially that shader is transparent
	for ( Array :: Iterator it = passes.getIterator (); !it.end (); ++it )
	{
		ShaderPass * pass = (ShaderPass *) it.value ();

								// if we've encountered opaque pass then shader is opaque
		if ( !pass -> isTransparent () )
			return false;
	}

	return true;
}
