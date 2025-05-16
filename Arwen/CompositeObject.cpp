//
// Composite VisualObjeect class
//
// Author: Alex V. Boreskoff
//
// Last modified: 1/04/2003
//

#include	<windows.h>
#include	<gl/gl.h>

#include	"CompositeObject.h"
#include	"View.h"
#include	"Frustrum.h"
#include	"MeshObject.h"

MetaClass	CompositeObject :: classInstance ( "CompositeObject", &VisualObject :: classInstance );

int	CompositeObject :: init ()
{
	buildBoundingBox ();

	return VisualObject :: init ();
}

void	CompositeObject :: setTexture ( Texture * txt )
{
	for ( Array :: Iterator it = objects.getIterator (); !it.end (); ++it )
	{
		MeshObject * object = dynamic_cast <MeshObject *> ( it.value () );

		if ( object != NULL )
			object -> setTexture ( txt );
		else
		{
			CompositeObject * c = dynamic_cast <CompositeObject *> ( it.value () );

			if ( c != NULL )
				c -> setTexture ( txt );
		}
	}
}

void	CompositeObject :: setShader ( Shader * sh )
{
	for ( Array :: Iterator it = objects.getIterator (); !it.end (); ++it )
	{
		MeshObject * object = dynamic_cast <MeshObject *> ( it.value () );

		if ( object != NULL )
			object -> setShader ( sh );
		else
		{
			CompositeObject * c = dynamic_cast <CompositeObject *> ( it.value () );

			if ( c != NULL )
				c -> setShader ( sh );
		}
	}
}

void	CompositeObject :: buildBoundingBox ()
{
	Vector3D	v;

	boundingBox.reset ();

	for ( Array :: Iterator it = objects.getIterator (); !it.end (); ++it )
	{
		VisualObject  * object = (VisualObject *) it.value ();
		BoundingBox		box ( object -> getBoundingBox () );

		for ( int i = 0; i < 8; i++ )
		{
			v = pos + curTransform.transformPoint ( box.getVertex ( i ) );

			boundingBox.addVertex ( v );
		}
	}
}

void	CompositeObject :: update ( Controller * controller, float curTime )
{
	VisualObject :: update ( controller, curTime );

	for ( Array :: Iterator it = objects.getIterator (); !it.end (); ++it )
		((VisualObject *) it.value ()) -> update ( controller, curTime );
}
	
void	CompositeObject :: doDraw ( View& view, const Camera& camera, const Frustrum& frustrum, Fog * fog )
{
	for ( Array :: Iterator it = objects.getIterator (); !it.end (); ++it )
	{
		VisualObject  * object = (VisualObject *) it.value ();
		BoundingBox		box ( object -> getBoundingBox () );

								// to do frustrum culling we must adjust for transform and position
		box.apply ( curTransform );
		box.move  ( pos );

		if ( frustrum.intersects ( box ) )
			object -> draw ( view, camera, frustrum, fog );
	}
}

bool	CompositeObject :: isTransparent () const
{
	for ( Array :: Iterator it = objects.getIterator (); !it.end (); ++it )
	{
		VisualObject * object = (VisualObject *) it.value ();

		if ( object -> isTransparent () )
			return true;
	}

	return false;
}
