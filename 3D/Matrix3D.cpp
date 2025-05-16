//
// Basic class for 3d matrices
// Supports all basic matrix operations
//
// Author: Alex V. Boreskoff
//
// Last change: 4/11/2002
//

#include <math.h>
#include "Matrix3D.h"

Matrix3D :: Matrix3D ( float a )
{
	x [0][1] = x [0][2] = x [1][0] =
	x [1][2] = x [2][0] = x [2][1] = 0;
	x [0][0] = x [1][1] = x [2][2] = a;
}

Matrix3D :: Matrix3D ( const Matrix3D& a )
{
	x [0][0] = a.x [0][0];
	x [0][1] = a.x [0][1];
	x [0][2] = a.x [0][2];
	x [1][0] = a.x [1][0];
	x [1][1] = a.x [1][1];
	x [1][2] = a.x [1][2];
	x [2][0] = a.x [2][0];
	x [2][1] = a.x [2][1];
	x [2][2] = a.x [2][2];
}

Matrix3D& Matrix3D :: operator = ( const Matrix3D& a )
{
	x [0][0] = a.x [0][0];
	x [0][1] = a.x [0][1];
	x [0][2] = a.x [0][2];
	x [1][0] = a.x [1][0];
	x [1][1] = a.x [1][1];
	x [1][2] = a.x [1][2];
	x [2][0] = a.x [2][0];
	x [2][1] = a.x [2][1];
	x [2][2] = a.x [2][2];

	return *this;
}

Matrix3D& Matrix3D :: operator = ( float a )
{
	x [0][1] = x [0][2] = x [1][0] =
	x [1][2] = x [2][0] = x [2][1] = 0.0;
	x [0][0] = x [1][1] = x [2][2] = a;

	return *this;
}

Matrix3D& Matrix3D :: operator += ( const Matrix3D& a )
{
	x [0][0] += a.x [0][0];
	x [0][1] += a.x [0][1];
	x [0][2] += a.x [0][2];
	x [1][0] += a.x [1][0];
	x [1][1] += a.x [1][1];
	x [1][2] += a.x [1][2];
	x [2][0] += a.x [2][0];
	x [2][1] += a.x [2][1];
	x [2][2] += a.x [2][2];

	return *this;
}

Matrix3D& Matrix3D :: operator -= ( const Matrix3D& a )
{
	x [0][0] -=a.x [0][0];
	x [0][1] -=a.x [0][1];
	x [0][2] -=a.x [0][2];
	x [1][0] -=a.x [1][0];
	x [1][1] -=a.x [1][1];
	x [1][2] -=a.x [1][2];
	x [2][0] -=a.x [2][0];
	x [2][1] -=a.x [2][1];
	x [2][2] -=a.x [2][2];

	return *this;
}

Matrix3D& Matrix3D :: operator *= ( const Matrix3D& a )
{
	Matrix3D c ( *this );

	x[0][0]=c.x[0][0]*a.x[0][0]+c.x[0][1]*a.x[1][0]+c.x[0][2]*a.x[2][0];
	x[0][1]=c.x[0][0]*a.x[0][1]+c.x[0][1]*a.x[1][1]+c.x[0][2]*a.x[2][1];
	x[0][2]=c.x[0][0]*a.x[0][2]+c.x[0][1]*a.x[1][2]+c.x[0][2]*a.x[2][2];
	x[1][0]=c.x[1][0]*a.x[0][0]+c.x[1][1]*a.x[1][0]+c.x[1][2]*a.x[2][0];
	x[1][1]=c.x[1][0]*a.x[0][1]+c.x[1][1]*a.x[1][1]+c.x[1][2]*a.x[2][1];
	x[1][2]=c.x[1][0]*a.x[0][2]+c.x[1][1]*a.x[1][2]+c.x[1][2]*a.x[2][2];
	x[2][0]=c.x[2][0]*a.x[0][0]+c.x[2][1]*a.x[1][0]+c.x[2][2]*a.x[2][0];
	x[2][1]=c.x[2][0]*a.x[0][1]+c.x[2][1]*a.x[1][1]+c.x[2][2]*a.x[2][1];
	x[2][2]=c.x[2][0]*a.x[0][2]+c.x[2][1]*a.x[1][2]+c.x[2][2]*a.x[2][2];

	return *this;
}

Matrix3D& Matrix3D :: operator *= ( float a )
{
	x [0][0] *= a;
	x [0][1] *= a;
	x [0][2] *= a;
	x [1][0] *= a;
	x [1][1] *= a;
	x [1][2] *= a;
	x [2][0] *= a;
	x [2][1] *= a;
	x [2][2] *= a;

	return *this;
}

Matrix3D& Matrix3D :: operator /= ( float a )
{
	x [0][0] /= a;
	x [0][1] /= a;
	x [0][2] /= a;
	x [1][0] /= a;
	x [1][1] /= a;
	x [1][2] /= a;
	x [2][0] /= a;
	x [2][1] /= a;
	x [2][2] /= a;

	return *this;
};

float	Matrix3D :: det () const
{
	return x [0][0]*(x [1][1]*x [2][2]-x [1][2]*x [2][1]) -
	       x [0][1]*(x [1][0]*x [2][2]-x [1][2]*x [2][0]) +
	       x [0][2]*(x [1][0]*x [2][1]-x [1][1]*x [2][0]);
}

Matrix3D&	Matrix3D :: invert ()
{
	float	 det;
	Matrix3D a;
					// compute a determinant
	det = x [0][0]*(x [1][1]*x [2][2]-x [1][2]*x [2][1]) -
	      x [0][1]*(x [1][0]*x [2][2]-x [1][2]*x [2][0]) +
	      x [0][2]*(x [1][0]*x [2][1]-x [1][1]*x [2][0]);

	a.x [0][0] = (x [1][1]*x [2][2]-x [1][2]*x [2][1]) / det;
	a.x [0][1] = (x [0][2]*x [2][1]-x [0][1]*x [2][2]) / det;
	a.x [0][2] = (x [0][1]*x [1][2]-x [0][2]*x [1][1]) / det;
	a.x [1][0] = (x [1][2]*x [2][0]-x [1][0]*x [2][2]) / det;
	a.x [1][1] = (x [0][0]*x [2][2]-x [0][2]*x [2][0]) / det;
	a.x [1][2] = (x [0][2]*x [1][0]-x [0][0]*x [1][2]) / det;
	a.x [2][0] = (x [1][0]*x [2][1]-x [1][1]*x [2][0]) / det;
	a.x [2][1] = (x [0][1]*x [2][0]-x [0][0]*x [2][1]) / det;
	a.x [2][2] = (x [0][0]*x [1][1]-x [0][1]*x [1][0]) / det;

	return *this = a;
}

Matrix3D&	Matrix3D :: transpose ()
{
	Matrix3D a;

	a.x [0][0] = x [0][0];
	a.x [0][1] = x [1][0];
	a.x [0][2] = x [2][0];
	a.x [1][0] = x [0][1];
	a.x [1][1] = x [1][1];
	a.x [1][2] = x [2][1];
	a.x [2][0] = x [0][2];
	a.x [2][1] = x [1][2];
	a.x [2][2] = x [2][2];

	return *this = a;
}

Matrix3D operator + ( const Matrix3D& a, const Matrix3D& b )
{
	Matrix3D c;

	c.x [0][0] = a.x [0][0] + b.x [0][0];
	c.x [0][1] = a.x [0][1] + b.x [0][1];
	c.x [0][2] = a.x [0][2] + b.x [0][2];
	c.x [1][0] = a.x [1][0] + b.x [1][0];
	c.x [1][1] = a.x [1][1] + b.x [1][1];
	c.x [1][2] = a.x [1][2] + b.x [1][2];
	c.x [2][0] = a.x [2][0] + b.x [2][0];
	c.x [2][1] = a.x [2][1] + b.x [2][1];
	c.x [2][2] = a.x [2][2] + b.x [2][2];

	return c;
}

Matrix3D operator - ( const Matrix3D& a, const Matrix3D& b )
{
	Matrix3D c;

	c.x [0][0] = a.x [0][0] - b.x [0][0];
	c.x [0][1] = a.x [0][1] - b.x [0][1];
	c.x [0][2] = a.x [0][2] - b.x [0][2];
	c.x [1][0] = a.x [1][0] - b.x [1][0];
	c.x [1][1] = a.x [1][1] - b.x [1][1];
	c.x [1][2] = a.x [1][2] - b.x [1][2];
	c.x [2][0] = a.x [2][0] - b.x [2][0];
	c.x [2][1] = a.x [2][1] - b.x [2][1];
	c.x [2][2] = a.x [2][2] - b.x [2][2];

	return c;
}

Matrix3D operator * ( const Matrix3D& a, const Matrix3D& b )
{
	Matrix3D c ( a );

	c.x[0][0]=a.x[0][0]*b.x[0][0]+a.x[0][1]*b.x[1][0]+a.x[0][2]*b.x[2][0];
	c.x[0][1]=a.x[0][0]*b.x[0][1]+a.x[0][1]*b.x[1][1]+a.x[0][2]*b.x[2][1];
	c.x[0][2]=a.x[0][0]*b.x[0][2]+a.x[0][1]*b.x[1][2]+a.x[0][2]*b.x[2][2];
	c.x[1][0]=a.x[1][0]*b.x[0][0]+a.x[1][1]*b.x[1][0]+a.x[1][2]*b.x[2][0];
	c.x[1][1]=a.x[1][0]*b.x[0][1]+a.x[1][1]*b.x[1][1]+a.x[1][2]*b.x[2][1];
	c.x[1][2]=a.x[1][0]*b.x[0][2]+a.x[1][1]*b.x[1][2]+a.x[1][2]*b.x[2][2];
	c.x[2][0]=a.x[2][0]*b.x[0][0]+a.x[2][1]*b.x[1][0]+a.x[2][2]*b.x[2][0];
	c.x[2][1]=a.x[2][0]*b.x[0][1]+a.x[2][1]*b.x[1][1]+a.x[2][2]*b.x[2][1];
	c.x[2][2]=a.x[2][0]*b.x[0][2]+a.x[2][1]*b.x[1][2]+a.x[2][2]*b.x[2][2];

	return c;
}

Matrix3D operator * ( const Matrix3D& a, float b )
{
	Matrix3D c ( a );

	return (c *= b);
}

Matrix3D operator * ( float b, const Matrix3D& a )
{
	Matrix3D c ( a );

	return (c *= b);
}

Matrix3D	Matrix3D :: getIdentityMatrix ()
{
	return Matrix3D ( 1 );
}

Matrix3D Matrix3D :: getScaleMatrix ( const Vector3D& v )
{
	Matrix3D a ( 1 );

	a.x [0][0] = v.x;
	a.x [1][1] = v.y;
	a.x [2][2] = v.z;

	return a;
}

Matrix3D Matrix3D :: getRotateXMatrix ( float angle )
{
	Matrix3D a ( 1 );
	float	 cosine = (float)cos ( angle );
	float	 sine   = (float)sin ( angle );

	a.x [1][1] = cosine;
	a.x [1][2] = sine;
	a.x [2][1] = -sine;
	a.x [2][2] = cosine;

	return a;
}

Matrix3D Matrix3D :: getRotateYMatrix ( float angle )
{
	Matrix3D a ( 1 );
	float	 cosine = (float)cos ( angle );
	float	 sine   = (float)sin ( angle );

	a.x [0][0] = cosine;
	a.x [0][2] = -sine;
	a.x [2][0] = sine;
	a.x [2][2] = cosine;

	return a;
}

Matrix3D Matrix3D :: getRotateZMatrix ( float angle )
{
	Matrix3D a ( 1 );
	float	 cosine = (float)cos ( angle );
	float	 sine   = (float)sin ( angle );

	a.x [0][0] = cosine;
	a.x [0][1] = sine;
	a.x [1][0] = -sine;
	a.x [1][1] = cosine;

	return a;
}

Matrix3D Matrix3D :: getRotateMatrix ( const Vector3D& v, float angle )
{
	Matrix3D a;
	float	 cosine = (float)cos ( angle );
	float	 sine   = (float)sin ( angle );

	a.x [0][0] = v.x *v.x + (1-v.x*v.x) * cosine;
	a.x [0][1] = v.x *v.y * (1-cosine) + v.z * sine;
	a.x [0][2] = v.x *v.z * (1-cosine) - v.y * sine;
	a.x [1][0] = v.x *v.y * (1-cosine) - v.z * sine;
	a.x [1][1] = v.y *v.y + (1-v.y*v.y) * cosine;
	a.x [1][2] = v.y *v.z * (1-cosine) + v.x * sine;
	a.x [2][0] = v.x *v.z * (1-cosine) + v.y * sine;
	a.x [2][1] = v.y *v.z * (1-cosine) - v.x * sine;
	a.x [2][2] = v.z *v.z + (1-v.z*v.z) * cosine;

	return a;
}

Matrix3D Matrix3D :: getRotateYawPitchRollMatrix ( float yaw, float pitch, float roll )
{
    return getRotateYMatrix ( yaw ) * getRotateZMatrix ( roll ) * getRotateXMatrix ( pitch );
/*
	Matrix3D a;
	float    cy = cos ( yaw );
	float    sy = sin ( yaw );
    float    cp = cos ( pitch );
    float    sp = sin ( pitch );
    float    cr = cos ( roll );
    float    sr = sin ( roll );

	a.x [0][0] = cy*cr + sy*sp*sr;
	a.x [1][0] = -cy*sr + sy*sp*cr;
	a.x [2][0] = sy*cp;
	a.x [0][1] = sr * cp;
	a.x [1][1] = cr*cp;
	a.x [2][1] = -sp;
	a.x [0][2] = -sy*cr - cy*sp*sr;
	a.x [1][2] = sr*sy + cy*sp*cr;
	a.x [2][2] = cy*cp;

	return a;
*/
}

Matrix3D Matrix3D :: getMirrorXMatrix ()
{
	Matrix3D a ( 1 );

	a.x [0][0] = -1;

	return a;
}

Matrix3D Matrix3D :: getMirrorYMatrix ()
{
	Matrix3D a ( 1 );

	a.x [1][1] = -1;

	return a;
}

Matrix3D Matrix3D :: getMirrorZMatrix ()
{
	Matrix3D a ( 1 );

	a.x [2][2] = -1;

	return a;
}

