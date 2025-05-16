//
// Basic 2D affine transform
// in the form:
//      p' = M*p + v
//
// Author: Alex V. Boreskoff
//
// Last changed: 14/11/2002
//

#ifndef __TRANSFORM_2D__
#define __TRANSFORM_2D__

#include "Matrix2D.h"
#include "Vector2D.h"

class    Transform2D
{
public:
    Matrix2D m;         // linear transform matrix
    Vector2D v;         // translation vector

    Transform2D () {}
    Transform2D ( const Matrix2D matrix, const Vector2D& vector ) : m ( matrix ), v ( vector ) {}
    Transform2D ( const Transform2D& tr ) : m ( tr.m ), v ( tr.v ) {}
	Transform2D ( const Matrix2D& matrix ) : m ( matrix )
	{
		v.x =
		v.y = 0;
	}

    Transform2D& invert ()
    {
        m.invert ();

        v = -(m * v);

        return *this;
    }

	Transform2D	getInverse () const
	{
		return Transform2D ( *this ).invert ();
	}
										// transform a point in space
    Vector2D  transformPoint ( const Vector2D& p ) const
    {
        return Vector2D ( v.x + m.x [0][0]*p.x + m.x [0][1]*p.y, v.y + m.x [1][0]*p.x + m.x [1][1]*p.y );
    }

										// transform a direction
										// can change length
	Vector2D	transformDir ( const Vector2D& p ) const
	{
        return Vector2D ( m.x [0][0]*p.x + m.x [0][1]*p.y, m.x [1][0]*p.x + m.x [1][1]*p.y );
    }
		
	void	buildHomogeneousMatrix ( float matrix [16] ) const;

	const Vector2D& getTranslation () const
	{
		return v;
	}

	const Matrix2D& getLinearPart () const
	{
		return m;
	}

	static	Transform2D getIdentity ()
	{
		return Transform2D ( Matrix2D :: getIdentityMatrix () );
	}

	static	Transform2D	getTranslate ( const Vector2D& v )
	{
		return Transform2D ( Matrix2D :: getIdentityMatrix (), v );
	}

	static	Transform2D	getScale ( const Vector2D& s )
	{
		return Transform2D ( Matrix2D :: getScaleMatrix ( s ) );
	}

	static	Transform2D	getScale ( float factor )
	{
		return Transform2D ( Matrix2D :: getScaleMatrix ( Vector2D ( factor, factor ) ) );
	}

	static	Transform2D	getRotate ( float angle )
	{
		return Transform2D ( Matrix2D :: getRotateMatrix ( angle ) );
	}
};

inline Transform2D operator * ( const Transform2D& t1, const Transform2D& t2 )
{
    return Transform2D ( t1.m * t2.m, t1.m * t2.v + t1.v );
}

#endif
