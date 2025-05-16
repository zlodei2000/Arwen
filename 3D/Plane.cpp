//
// Class for simple plane (p,n) + dist = 0
//
// Author: Alex V. Boreskoff
// Last change: 30/11/2002
//

#include    "Plane.h"
#include	"Transform3D.h"
#include	"Math3D.h"

void	Plane :: apply ( const Transform3D& tr )
{
	Vector3D	p    = tr.transformPoint ( point () );
	Vector3D	norm = tr.transformDir   ( n );

	n             = norm.normalize ();
	nearPointMask = -1;
    dist          = -(p & n);

	computeNearPointMaskAndMainAxis ();
}

void    Plane :: computeNearPointMaskAndMainAxis ()
{
	nearPointMask = computeNearPointMask ( n );
    mainAxis      = n.getMainAxis ();
}

