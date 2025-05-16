//
// VisualObject class - a decorator for a SubScene.
// Can be model (mesh), particle system or anything like that
//
// Author: Alex. V. Boreskoff
//
// Last modified: 2/12/2002
//

#include	<windows.h>
#include	<gl/gl.h>
#include	"VisualObject.h"
#include	"Animator.h"
#include	"View.h"

MetaClass	VisualObject :: classInstance ( "VisualObject", &Object :: classInstance );

VisualObject :: VisualObject ( const char * theName, const Vector3D& thePos, 
							   const Vector4D& theColor, const Transform3D& theTr ) : Object ( theName )
{
	pos          = thePos;
	color        = theColor;
	transform    = theTr;
	curTransform = transform;
	saveFlags    = 0;
	metaClass    = &classInstance;

	buildBoundingBox ();
}

VisualObject :: VisualObject ( const char * theName, const Vector3D& thePos, 
							   const Vector4D& theColor ) : Object ( theName )
{
	pos          = thePos;
	color        = theColor;
	transform    = Transform3D :: getIdentity ();
	curTransform = transform;
	saveFlags    = 0;
	metaClass    = &classInstance;

	buildBoundingBox ();
}

VisualObject :: VisualObject ( const char * theName, const Vector3D& thePos ) : Object ( theName )
{
	pos          = thePos;
	color        = Vector4D ( 1, 1, 1, 1 );
	transform    = Transform3D :: getIdentity ();
	curTransform = transform;
	saveFlags    = 0;
	metaClass    = &classInstance;

	buildBoundingBox ();
}

void	VisualObject :: update ( Controller * controller, float curTime )
{
	curTransform = transform;					// reset transform

												// let every animator to modify curTransform
	for ( Array :: Iterator it = animators.getIterator (); !it.end (); ++it )
	{
		Animator * animator = (Animator *) it.value ();

		animator -> animate ( this, curTime );
	}

	buildBoundingBox ();
}

void	VisualObject :: draw ( View& view, const Camera& camera, const Frustrum& frustrum, Fog * fog )
{
	preDraw  ( view );
	doDraw   ( view, camera, frustrum, fog );
	postDraw ( view );
}

void	VisualObject :: buildBoundingBox ()
{
	boundingBox.reset     ();
	boundingBox.addVertex ( pos );
}

void	VisualObject :: addAnimator ( Animator * theAnimator )
{
	animators.insert ( theAnimator );
}

bool	VisualObject :: removeAnimator ( const String& name )
{
	return animators.removeObjectWithName ( name );
}

void	VisualObject :: preDraw ( View& view )
{
	if ( saveFlags )
		glPushAttrib ( saveFlags );

	glMatrixMode ( GL_MODELVIEW );
	glPushMatrix ();
	glTranslatef ( pos.x, pos.y, pos.z );

	view.apply ( curTransform );
}

void	VisualObject :: postDraw ( View& view )
{
	glPopMatrix ();

	if ( saveFlags )
		glPopAttrib ();
}
