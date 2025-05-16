//
// Simple 1-D math
//
// Author: Alex V. Boreskoff
//

#ifndef	__MATH_1D__
#define	__MATH_1D__

#include	<math.h>
#include	<stdlib.h>						// for rand

#ifndef	M_PI								// since not all compilers define it						
#define M_PI        3.14159265358979323846f
#endif

inline	float	sqr ( float x )
{
	return x*x;
}

inline	float	rnd ()
{
	return 2 * float ( rand () ) / float ( RAND_MAX ) - 1;
}

inline	float	rnd ( float min, float max )
{
	float	val = (float) rand () / (float) RAND_MAX;

	return min + val * (max - min);
}

inline	int	sign ( float val )
{
	return val < 0 ? -1 : ( val > 0 ? 1 : 0 );
}

inline	float	clamp ( float val, float from = 0, float to = 1 )
{
	if ( val < from )
		return from;
	else
	if ( val > to )
		return to;

	return val;
}

#endif


