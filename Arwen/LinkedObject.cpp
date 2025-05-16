//
// Linked object - like ones used in Quake 3
//
// Author :Alex V. Boreskoff
//

#include	"ObjStr.h"
#include	"LinkedObject.h"
#include	"Frustrum.h"

class	LinkedObjectItem : public Object
{
private:
	VisualObject  * object;
	Transform3D		transform;

public:
	LinkedObjectItem ( const char * theName, VisualObject * theObject, const Transform3D& theTransform ) : Object ( theName ), transform ( theTransform )
	{
		object    = theObject;
		metaClass = &classInstance;
	}

	~LinkedObjectItem ()
	{
		object -> release ();
	}

	VisualObject * getObject () const
	{
		return object;
	}

	const Transform3D& getTransform () const
	{
		return transform;
	}

	static	MetaClass	classInstance;
};

MetaClass	LinkedObjectItem :: classInstance ( "LinkedObjectItem", &Object       :: classInstance );
MetaClass	LinkedObject     :: classInstance ( "LinkedObject",     &VisualObject :: classInstance );

LinkedObject ::LinkedObject ( const char * theName, VisualObject * theObject ) : VisualObject ( theName, Vector3D ( 0, 0, 0 ) )
{
	mainObject = theObject;
	metaClass  = &classInstance;

	mainObject -> retain ();
}

LinkedObject :: ~LinkedObject ()
{
	mainObject -> release ();
}

int	LinkedObject :: init ()
{
	Vector3D	v;

	boundingBox = mainObject -> getBoundingBox ();

	for ( Array :: Iterator it = objects.getIterator (); !it.end (); ++it )
	{
		LinkedObjectItem  * object = (LinkedObjectItem *) it.value ();
		Transform3D		    tr     = object -> getTransform () * curTransform;
		BoundingBox			box ( object -> getObject () -> getBoundingBox () );

		for ( int i = 0; i < 8; i++ )
			boundingBox.addVertex ( pos + tr.transformPoint ( box.getVertex ( i ) ) );
	}

	return VisualObject :: init ();
}

void	LinkedObject :: update ( Controller * controller, float curTime )
{
	VisualObject :: update ( controller, curTime );

	mainObject -> update ( controller, curTime );

	for ( Array :: Iterator it = objects.getIterator (); !it.end (); ++it )
		((LinkedObjectItem *) it.value ()) -> getObject () -> update ( controller, curTime );
}
	
void	LinkedObject :: doDraw ( View& view, const Camera& camera, const Frustrum& frustrum, Fog * fog )
{
	mainObject -> draw ( view, camera, frustrum, fog );


										// setup coordinate system for mainObject and draw children in it
	for ( Array :: Iterator it = objects.getIterator (); !it.end (); ++it )
	{
		LinkedObjectItem  * item   = (LinkedObjectItem *) it.value ();
		VisualObject      * object = item -> getObject ();
		BoundingBox		    box ( object -> getBoundingBox () );
		Transform3D			tr ( curTransform * item -> getTransform () );

								// to do frustrum culling we must adjust for transform and position
		box.move  ( pos );
		box.apply ( curTransform );

		if ( frustrum.intersects ( box ) )
		{
			object -> animate ( tr );
			object -> draw ( view, camera, frustrum, fog );
		}
	}
}

bool	LinkedObject :: isTransparent () const
{
	if ( mainObject -> isTransparent () )
		return true;

	for ( Array :: Iterator it = objects.getIterator (); !it.end (); ++it )
	{
		LinkedObjectItem * object = (LinkedObjectItem *) it.value ();

		if ( object -> getObject () -> isTransparent () )
			return true;
	}

	return false;
}

void	LinkedObject :: setLink ( const String& tag, VisualObject * object, const Transform3D& transform )
{
	if ( objects.getObjectWithName ( tag ) != NULL )
		objects.removeObjectWithName ( tag );

	objects.insertNoRetain ( new LinkedObjectItem ( tag, object, transform ) );
}
