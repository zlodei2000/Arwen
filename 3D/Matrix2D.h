//
// Basic class for 2d matrices
// Supports all basic matrix operations
//
// Author: Alex V. Boreskoff
//
// Last change: 14/11/2002
//

#ifndef __MATRIX2D__
#define __MATRIX2D__

#include	"Vector2D.h"

class Matrix2D
{
public:
	float x [2][2];

	Matrix2D () {}
	Matrix2D ( float );
	Matrix2D ( const Matrix2D& );

	Matrix2D& operator =  ( const Matrix2D& );
	Matrix2D& operator =  ( float );
	Matrix2D& operator += ( const Matrix2D& );
	Matrix2D& operator -= ( const Matrix2D& );
	Matrix2D& operator *= ( const Matrix2D& );
	Matrix2D& operator *= ( float );
	Matrix2D& operator /= ( float );

	const float * operator [] ( int i ) const
	{
		return & x[i][0];
	}

	float * operator [] ( int i )
	{
		return & x[i][0];
	}

	Matrix2D&	invert    ();
	Matrix2D& 	transpose ();

	float	det () const
	{
		return x [0][0] * x [1][1] - x [0][1] * x [1][0];
	}

	Matrix2D	getInverse () const
	{
		return Matrix2D ( *this ).invert ();
	}

	static	Matrix2D	getIdentityMatrix ();
	static	Matrix2D	getScaleMatrix    ( const Vector2D& );
	static	Matrix2D	getRotateMatrix   ( float );
	static	Matrix2D	getMirrorXMatrix  ();
	static	Matrix2D	getMirrorYMatrix  ();

	friend Matrix2D operator + ( const Matrix2D&, const Matrix2D& );
	friend Matrix2D operator - ( const Matrix2D&, const Matrix2D& );
	friend Matrix2D operator * ( const Matrix2D&, const Matrix2D& );
	friend Matrix2D operator * ( const Matrix2D&, float );
	friend Matrix2D operator * ( float, const Matrix2D& );
	friend Vector2D operator * ( const Matrix2D&, const Vector2D& );
};

inline Vector2D operator * ( const Matrix2D& a, const Vector2D& b )
{
	return Vector2D ( a.x [0][0]*b.x + a.x [0][1]*b.y, a.x [1][0]*b.x + a.x [1][1]*b.y );
}

#endif
