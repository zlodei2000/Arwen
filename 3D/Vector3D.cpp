//
// Basic class for 3d vectors.
// Supports all basic vector operations
//
// Author: Alex V. Boreskoff
//
// Last change: 13/11/2002
//

#include	<stdlib.h>
#include	"Vector3D.h"
#include	"3DDefs.h"

Vector3D	Vector3D :: getRandomVector ( float len )
{
	Vector3D	v;

	for ( ; ; )
	{
		v.x = 2.0f * float ( rand () ) / float ( RAND_MAX ) - 1.0f;
		v.y = 2.0f * float ( rand () ) / float ( RAND_MAX ) - 1.0f;
		v.z = 2.0f * float ( rand () ) / float ( RAND_MAX ) - 1.0f;

		if ( v.lengthSq () < EPS )
			continue;

		v *= len / v.length ();

		return v;
	}
}

