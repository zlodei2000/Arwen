//
// Basic class for 2d matrices
// Supports all basic matrix operations
//
// Author: Alex V. Boreskoff
//
// Last change: 14/11/2002
//

#include <math.h>
#include "Matrix2D.h"

Matrix2D :: Matrix2D ( float a )
{
	x [0][1] = x [1][0] = 0;
	x [0][0] = x [1][1] = a;
}

Matrix2D :: Matrix2D ( const Matrix2D& a )
{
	x [0][0] = a.x [0][0];
	x [0][1] = a.x [0][1];
	x [1][0] = a.x [1][0];
	x [1][1] = a.x [1][1];
}

Matrix2D& Matrix2D :: operator = ( const Matrix2D& a )
{
	x [0][0] = a.x [0][0];
	x [0][1] = a.x [0][1];
	x [1][0] = a.x [1][0];
	x [1][1] = a.x [1][1];

	return *this;
}

Matrix2D& Matrix2D :: operator = ( float a )
{
	x [0][1] = x [1][0] = 0;
	x [0][0] = x [1][1] = a;

	return *this;
}

Matrix2D& Matrix2D :: operator += ( const Matrix2D& a )
{
	x [0][0] += a.x [0][0];
	x [0][1] += a.x [0][1];
	x [1][0] += a.x [1][0];
	x [1][1] += a.x [1][1];

	return *this;
}

Matrix2D& Matrix2D :: operator -= ( const Matrix2D& a )
{
	x [0][0] -=a.x [0][0];
	x [0][1] -=a.x [0][1];
	x [1][0] -=a.x [1][0];
	x [1][1] -=a.x [1][1];

	return *this;
}

Matrix2D& Matrix2D :: operator *= ( const Matrix2D& a )
{
	Matrix2D c ( *this );

	x [0][0] = c.x[0][0]*a.x[0][0]+c.x[0][1]*a.x[1][0];
	x [0][1] = c.x[0][0]*a.x[0][1]+c.x[0][1]*a.x[1][1];
	x [1][0] = c.x[1][0]*a.x[0][0]+c.x[1][1]*a.x[1][0];
	x [1][1] = c.x[1][0]*a.x[0][1]+c.x[1][1]*a.x[1][1];

	return *this;
}

Matrix2D& Matrix2D :: operator *= ( float a )
{
	x [0][0] *= a;
	x [0][1] *= a;
	x [1][0] *= a;
	x [1][1] *= a;

	return *this;
}

Matrix2D& Matrix2D :: operator /= ( float a )
{
	x [0][0] /= a;
	x [0][1] /= a;
	x [1][0] /= a;
	x [1][1] /= a;

	return *this;
};

Matrix2D&	Matrix2D :: invert ()
{
	float	 d = det ();
	Matrix2D a;

	a.x [0][0] =  x [1][1] / d;
	a.x [0][1] = -x [0][1] / d;
	a.x [1][0] = -x [1][0] / d;
	a.x [1][1] =  x [0][0] / d;

	return *this = a;
}

Matrix2D&	Matrix2D :: transpose ()
{
	Matrix2D a;

	a.x [0][0] = x [0][0];
	a.x [0][1] = x [1][0];
	a.x [1][0] = x [0][1];
	a.x [1][1] = x [1][1];

	return *this = a;
}

Matrix2D operator + ( const Matrix2D& a, const Matrix2D& b )
{
	Matrix2D c;

	c.x [0][0] = a.x [0][0] + b.x [0][0];
	c.x [0][1] = a.x [0][1] + b.x [0][1];
	c.x [1][0] = a.x [1][0] + b.x [1][0];
	c.x [1][1] = a.x [1][1] + b.x [1][1];

	return c;
}

Matrix2D operator - ( const Matrix2D& a, const Matrix2D& b )
{
	Matrix2D c;

	c.x [0][0] = a.x [0][0] - b.x [0][0];
	c.x [0][1] = a.x [0][1] - b.x [0][1];
	c.x [1][0] = a.x [1][0] - b.x [1][0];
	c.x [1][1] = a.x [1][1] - b.x [1][1];

	return c;
}

Matrix2D operator * ( const Matrix2D& a, const Matrix2D& b )
{
	Matrix2D c;

	c.x [0][0] = a.x [0][0]*b.x [0][0] + a.x [0][1]*b.x [1][0];
	c.x [0][1] = a.x [0][0]*b.x [0][1] + a.x [0][1]*b.x [1][1];
	c.x [1][0] = a.x [1][0]*b.x [0][0] + a.x [1][1]*b.x [1][0];
	c.x [1][1] = a.x [1][0]*b.x [0][1] + a.x [1][1]*b.x [1][1];

	return c;
}

Matrix2D operator * ( const Matrix2D& a, float b )
{
	Matrix2D c ( a );

	return (c *= b);
}

Matrix2D operator * ( float b, const Matrix2D& a )
{
	Matrix2D c ( a );

	return (c *= b);
}

Matrix2D	Matrix2D :: getIdentityMatrix ()
{
	return Matrix2D ( 1 );
}

Matrix2D Matrix2D :: getScaleMatrix ( const Vector2D& v )
{
	Matrix2D a ( 1 );

	a.x [0][0] = v.x;
	a.x [1][1] = v.y;

	return a;
}

Matrix2D Matrix2D :: getRotateMatrix ( float angle )
{
	Matrix2D a ( 1 );
	float	 cosine = (float)cos ( angle );
	float	 sine   = (float)sin ( angle );

	a.x [0][0] = cosine;
	a.x [0][1] = sine;
	a.x [1][0] = -sine;
	a.x [1][1] = cosine;

	return a;
}

Matrix2D Matrix2D :: getMirrorXMatrix ()
{
	Matrix2D a ( 1 );

	a.x [0][0] = -1;

	return a;
}

Matrix2D Matrix2D :: getMirrorYMatrix ()
{
	Matrix2D a ( 1 );

	a.x [1][1] = -1;

	return a;
}

