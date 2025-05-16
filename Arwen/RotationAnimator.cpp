//
// Class for doing rotations of VO
//
// Author: Alex V. Boreskoff
//
// Last modified: 2/12/2002
//

#include	"RotationAnimator.h"
#include	"Transform3D.h"

MetaClass	RotationAnimator :: classInstance ( "RotationAnimator", &Animator :: classInstance );

void	RotationAnimator :: animate ( VisualObject * object, float curTime )
{
	object -> animate ( Transform3D :: getRotate ( axis, phase + speed * curTime ) );
}

