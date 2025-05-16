//
// Generic model for 3D-scene.
// Provides methods for rendering, animation and 
// performing collision detection
//
// Author: Alex V. Boreskoff
//
// Last modified: 31/10/2002
//

#ifndef	__MODEL_3D__
#define	__MODEL_3D__

#include	"Object.h"
#include	"Vector3D.h"
										// class predeclarations
class	Array;
class	BoundingBox;
class	Camera;
class	View;
class	Controller;

class	Model : public Object
{
protected:
	Vector3D	pos;					// starting pos of the viewer in the model
	float		yaw, pitch, roll;		// euler angles of the viewer
public:
	Model ( const char * theName ) : Object ( theName ) {}

										// methods for rendering and getting potential colliders
	virtual	void	render       ( View& view, const Camera& camera ) {}
	virtual	void	getColliders ( const BoundingBox& area, Array& colliders ) {}
	virtual	void	update       ( Controller *, float systemTime ) {}

	const	Vector3D&	getPos () const
	{
		return pos;
	}

	float	getYaw () const
	{
		return yaw;
	}

	float	getPitch () const
	{
		return pitch;
	}

	float	getRoll () const
	{
		return roll;
	}

	void	setStart ( const Vector3D& v, float y, float p, float r )
	{
		pos   = v;
		yaw   = y;
		pitch = p;
		roll  = r;
	}

	static	MetaClass	classInstance;
};

#endif
