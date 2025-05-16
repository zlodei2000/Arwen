//
// Basic 2D affine transform
// in the form:
//      p' = M*p + v
//
// Author: Alex V. Boreskoff
//
// Last changed: 14/11/2002
//

#include	"Transform2D.h"

void	Transform2D :: buildHomogeneousMatrix ( float matrix [16] ) const
{
	matrix [0]  = m [0][0];
	matrix [1]  = m [1][0];
	matrix [2]  = 0;
	matrix [3]  = 0;

	matrix [4]  = m [0][1];
	matrix [5]  = m [1][1];
	matrix [6]  = 0;
	matrix [7]  = 0;

	matrix [8]  = 0;
	matrix [9]  = 0;
	matrix [10] = 1;
	matrix [11] = 0;

	matrix [12] = v.x;
	matrix [13] = v.y;
	matrix [14] = 0;
	matrix [15] = 1;
}

