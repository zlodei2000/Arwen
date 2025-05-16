//
// Basic constants for a 3D engine
//
// Author: Alex V. Boreskoff
//
// Last modified: 13/11/2002
//

#ifndef __3D_DEFS__
#define __3D_DEFS_

#define EPS         1e-3f				// threshold for comparisions
#define	MAX_COORD   1e7f				// max value for x, y and z coordinates
#define	INFINITY	1e7f

#define MAX_VERTICES    20

				// classification codes (point/poly to plane)
#define	IN_PLANE    0
#define IN_FRONT    1
#define	IN_BACK     2
#define IN_BOTH     3

#define AXIS_X      0
#define AXIS_Y      1
#define AXIS_Z      2

#ifndef NULL
	#define NULL    0
#endif

#endif
