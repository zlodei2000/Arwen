//
// Linked object - like ones used in Quake 3
//
// Author :Alex V. Boreskoff
//

#ifndef	__LINKED_OBJECT__
#define	__LINKED_OBJECT__

#include	"VisualObject.h"
#include	"Array.h"

class	LinkedObject : public VisualObject
{
private:
	VisualObject * mainObject;
	Array		   objects;

public:
	LinkedObject ( const char * name, VisualObject * theObject );
	~LinkedObject ();

	virtual	int	init ();

	virtual	void	update        ( Controller *, float curTime );
	virtual	bool	isTransparent () const;

	void	setLink ( const String& tag, VisualObject * object, const Transform3D& transform );

	static	MetaClass	classInstance;

protected:
	virtual	void	doDraw ( View& view, const Camera& camera, const Frustrum& frustrum, Fog * fog );
};

#endif
