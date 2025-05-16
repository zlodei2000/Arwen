//
// Mesh VisualObject
//
// Author: Alex V. Boreskoff
//
// Last modified: 12/11/2002
//

#include	<windows.h>
#include	<gl/gl.h>

#include	"Mesh3D.h"
#include	"MeshObject.h"
#include	"View.h"
#include	"Shader.h"

MetaClass	MeshObject :: classInstance ( "MeshObject", &VisualObject :: classInstance );

MeshObject :: MeshObject ( const char * theName, const Vector3D& thePos, const Vector4D& theColor, const Transform3D& theTr, Mesh3D * theMesh ) :
	VisualObject ( theName, thePos, theColor, theTr )
{
	metaClass = &classInstance;
	mesh      = theMesh;
	shader    = NULL;

	if ( mesh != NULL )
		mesh -> retain ();

	buildBoundingBox ();
}

MeshObject :: ~MeshObject ()
{
	if ( mesh != NULL )
		mesh -> release ();
}

void	MeshObject :: setShader ( Shader * sp )
{
	if ( shader != NULL )
		shader -> release ();

	shader = sp;

	if ( shader != NULL )
		shader -> retain ();
}

void	MeshObject :: setTexture ( Texture * txt )
{
	if ( mesh != NULL )
		mesh -> setTexture ( txt );
}

void	MeshObject :: doDraw ( View& view, const Camera& camera, const Frustrum& frustrum, Fog * fog )
{
	if ( shader == NULL )
		mesh -> draw ( view, camera, color, fog );
	else
		shader -> drawMesh ( view, camera, color, fog, mesh );
}

void	MeshObject :: setMesh ( Mesh3D * theMesh )
{
	if ( mesh != NULL )
		mesh -> release ();

	mesh = theMesh;

	if ( mesh != NULL )
		mesh -> retain ();

	buildBoundingBox ();
}

void	MeshObject :: buildBoundingBox ()
{
	boundingBox.reset ();

	if ( mesh == NULL )
		return;

	const Vector3D * vertices    = mesh -> getVertices    ();
	int				 numVertices = mesh -> getNumVertices ();

	for ( int i = 0; i < numVertices; i++ )
		boundingBox.addVertex ( pos + curTransform.transformPoint ( vertices [i] ) );
}

bool	MeshObject :: isTransparent () const
{
	if ( shader != NULL )
		return shader -> isTransparent ();

	if ( mesh -> getTexture () != NULL )
		return mesh -> getTexture () -> isTransparent ();

	return false;
}

