//
// VisualObject class - a decorator for a SubScene.
// Can be model (mesh), particle system or anything like that
//
// Author: Alex. V. Boreskoff
//
// Last modified: 2/12/2002
//

#ifndef	__VISUAL_OBJECT__
#define	__VISUAL_OBJECT__

#include	"Object.h"
#include	"BoundingBox.h"
#include	"Vector3D.h"
#include	"Vector4D.h"
#include	"Transform3D.h"
#include	"Array.h"

class	Camera;
class	View;
class	Frustrum;
class	Animator;
class	Fog;
class	Controller;

class	VisualObject : public Object
{
protected:
	Vector3D	 pos;					// position of the object
	Vector4D	 color;					// color to modulate object
	Transform3D	 transform;				// original transform applied to objects 
										// (before animators)
	Transform3D	 curTransform;			// current transform (build from original by animators)
	Array		 animators;
	BoundingBox	 boundingBox;			// bounding box of object
	int			 saveFlags;				// what state values to save

public:
	VisualObject ( const char * theName, const Vector3D& thePos, const Vector4D& theColor, 
		           const Transform3D& theTr );
	VisualObject ( const char * theName, const Vector3D& thePos, const Vector4D& theColor );
	VisualObject ( const char * theName, const Vector3D& thePos );

	const Vector3D&	getPos () const
	{
		return pos;
	}

	void	setPos ( const Vector3D& thePos )
	{
		pos = thePos;

		buildBoundingBox ();
	}

	const Vector4D&	getColor () const
	{
		return color;
	}

	void	setColor ( const Vector4D& theColor )
	{
		color = theColor;
	}

	const Transform3D	getTransform () const
	{
		return transform;
	}

	void	setTransform ( const Transform3D& theTr )
	{
		transform    = theTr;
		curTransform = theTr;

		buildBoundingBox ();
	}

	const BoundingBox& getBoundingBox () const
	{
		return boundingBox;
	}

	void	animate ( const Transform3D& tr )
	{
		curTransform = tr * curTransform;
	}

	void	addAnimator    ( Animator * theAnimator );
	bool	removeAnimator ( const String& name );

	virtual	void	update ( Controller * controller, float curTime );
	virtual	void	draw   ( View& view, const Camera& camera, const Frustrum& frustrum, Fog * fog );

	virtual	bool	isTransparent () const
	{
		return false;
	}

	static	MetaClass	classInstance;

protected:
	virtual	void	buildBoundingBox ();

	virtual	void	doDraw   ( View& view, const Camera& camera, const Frustrum& frustrum, Fog * fog ) {}
	virtual	void	preDraw  ( View& view );
	virtual	void	postDraw ( View& view );
};

#endif

