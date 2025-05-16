//
// Composite VisualObjeect class
//
// Author: Alex V. Boreskoff
//
// Last modified: 12/11/2002
//

#ifndef	__COMPOSITE_OBJECT__
#define	__COMPOSITE_OBJECT__

#include	"VisualObject.h"
#include	"Array.h"

class	Texture;
class	Shader;

class	CompositeObject : public VisualObject
{
protected:
	Array		objects;					// child objects
public:
	CompositeObject ( const char * theName, const Vector3D& thePos, const Vector4D& theColor ) : VisualObject( theName, thePos, theColor ) 
	{
		metaClass = &classInstance;
	}

	CompositeObject ( const char * theName, const Vector3D& thePos ) : VisualObject ( theName, thePos ) 
	{
		metaClass = &classInstance;
	}

	virtual	int	init ();

	virtual	void	update        ( Controller *, float curTime );
	virtual	bool	isTransparent () const;

	void	insert ( VisualObject * obj )
	{
		obj -> init ();

		objects.insert ( obj );
	}

	void	setTexture ( Texture * txt );
	void	setShader  ( Shader  * sh  );

	static	MetaClass	classInstance;

protected:
	virtual	void	buildBoundingBox ();
	virtual	void	doDraw           ( View& view, const Camera& camera, 
									   const Frustrum& frustrum, Fog * fog );
};

#endif
