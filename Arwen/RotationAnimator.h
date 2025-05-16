//
// Class for doing rotations of VO
//
// Author: Alex V. Boreskoff
//
// Last modified: 2/12/2002
//

#ifndef	__ROTATION_ANIMATOR__
#define	__ROTATION_ANIMATOR__

#include	"Animator.h"
#include	"Vector3D.h"

class	RotationAnimator : public Animator
{
private:
	Vector3D	  axis;
	float		  speed;
	float		  phase;
public:
	RotationAnimator ( const char * theName, const Vector3D& theAxis, float theSpeed, float thePhase = 0 ) : Animator ( theName ) 
	{
		axis      = theAxis;
		speed     = theSpeed;
		phase     = thePhase;
		metaClass = &classInstance;
	}

	virtual	void	animate ( VisualObject * object, float curTime );

	static	MetaClass	classInstance;
};

#endif
