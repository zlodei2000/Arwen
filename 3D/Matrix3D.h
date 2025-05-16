//
// Basic class for 3d matrices
// Supports all basic matrix operations
//
// Author: Alex V. Boreskoff
//
// Last change: 4/11/2002
//

#ifndef __MATRIX3D__
#define __MATRIX3D__

#include	"Vector3D.h"

class Matrix3D
{
public:
	float x [3][3];

	Matrix3D () {}
	Matrix3D ( float );
	Matrix3D ( const Matrix3D& );

	Matrix3D& operator =  ( const Matrix3D& );
	Matrix3D& operator =  ( float );
	Matrix3D& operator += ( const Matrix3D& );
	Matrix3D& operator -= ( const Matrix3D& );
	Matrix3D& operator *= ( const Matrix3D& );
	Matrix3D& operator *= ( float );
	Matrix3D& operator /= ( float );

	const float * operator [] ( int i ) const
	{
		return & x[i][0];
	}

	float * operator [] ( int i )
	{
		return & x[i][0];
	}

	Matrix3D&	invert    ();
	Matrix3D& 	transpose ();
	float		det       () const;

	Matrix3D	getInverse () const
	{
		return Matrix3D ( *this ).invert ();
	}

	static	Matrix3D	getIdentityMatrix           ();
	static	Matrix3D	getScaleMatrix              ( const Vector3D& );
	static	Matrix3D	getRotateXMatrix            ( float );
	static	Matrix3D	getRotateYMatrix            ( float );
	static	Matrix3D	getRotateZMatrix            ( float );
	static	Matrix3D	getRotateMatrix             ( const Vector3D&, float );
	static	Matrix3D	getRotateYawPitchRollMatrix ( float yaw, float pitch, float roll );
	static	Matrix3D	getMirrorXMatrix            ();
	static	Matrix3D	getMirrorYMatrix            ();
	static	Matrix3D	getMirrorZMatrix            ();

	friend Matrix3D operator + ( const Matrix3D&, const Matrix3D& );
	friend Matrix3D operator - ( const Matrix3D&, const Matrix3D& );
	friend Matrix3D operator * ( const Matrix3D&, const Matrix3D& );
	friend Matrix3D operator * ( const Matrix3D&, float );
	friend Matrix3D operator * ( float, const Matrix3D& );
	friend Vector3D operator * ( const Matrix3D&, const Vector3D& );
};

inline Vector3D operator * ( const Matrix3D& a, const Vector3D& b )
{
	return Vector3D ( a.x [0][0]*b.x + a.x [0][1]*b.y + a.x [0][2]*b.z,
	                  a.x [1][0]*b.x + a.x [1][1]*b.y + a.x [1][2]*b.z,
	                  a.x [2][0]*b.x + a.x [2][1]*b.y + a.x [2][2]*b.z );
}

#endif
