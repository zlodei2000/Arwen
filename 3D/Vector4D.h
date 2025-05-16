//
// Basic class for 4d vectors.
// Supports all basic vector operations
//
// Author: Alex V. Boreskoff
//
// Last change: 26/10/2001
//

#ifndef	__VECTOR4D__
#define	__VECTOR4D__

#include	<math.h>

class	Vector4D
{
public:
	float	x, y, z, w;

	Vector4D () {}
	Vector4D ( float px, float py, float pz, float pw )
	{
		x = px;
		y = py;
		z = pz;
		w = pw;
	}

	Vector4D ( const Vector4D& v )
	{
		x = v.x;
		y = v.y;
		z = v.z;
		w = v.w;
	}

	Vector4D& operator = ( const Vector4D& v )
	{
		x = v.x;
		y = v.y;
		z = v.z;
		w = v.w;

		return *this;
	}

	Vector4D operator + () const
	{
		return *this;
	}

	Vector4D operator - () const
	{
		return Vector4D ( -x, -y, -z, -w );
	}

	Vector4D& operator += ( const Vector4D& v )
	{
		x += v.x;
		y += v.y;
		z += v.z;
		w += v.w;

		return *this;
	}

	Vector4D& operator -= ( const Vector4D& v )
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		w -= v.w;

		return *this;
	}

	Vector4D& operator *= ( const Vector4D& v )
	{
		x *= v.x;
		y *= v.y;
		z *= v.z;
		w *= v.w;

		return *this;
	}

	Vector4D& operator *= ( float f )
	{
		x *= f;
		y *= f;
		z *= f;
		w *= f;

		return *this;
	}

	Vector4D& operator /= ( const Vector4D& v )
	{
		x /= v.x;
		y /= v.y;
		z /= v.z;
		w /= v.w;

		return *this;
	}

	Vector4D& operator /= ( float f )
	{
		x /= f;
		y /= f;
		z /= f;
		w /= f;

		return *this;
	}

	float& operator [] ( int index )
	{
		return * ( index + &x );
	}

	int	operator == ( const Vector4D& v ) const
	{
		return x == v.x && y == v.y && z == v.z && w == v.w;
	}

	int	operator != ( const Vector4D& v ) const
	{
		return x != v.x || y != v.y || z != v.z || w != v.w;
	}

	operator float * ()
	{
		return &x;
	}

	operator const float * () const
	{
		return &x;
	}
	
	float	length () const
	{
		return (float) sqrt ( x * x + y * y + z * z );
	}

    float   lengthSq () const
    {
        return x * x + y * y + z * z;
    }

	Vector4D&	normalize ()
	{
		return (*this) /= length ();
	}

    float   distanceToSq ( const Vector4D& p ) const
    {
        return sqr ( x - p.x ) + sqr ( y - p.y ) + sqr ( z - p.z );
    }

    float   distanceTo ( const Vector4D& p ) const
    {
        return (float)sqrt ( sqr ( x - p.x ) + sqr ( y - p.y ) + sqr ( z - p.z ) );
    }

	friend Vector4D operator + ( const Vector4D&, const Vector4D& );
	friend Vector4D operator - ( const Vector4D&, const Vector4D& );
	friend Vector4D operator * ( const Vector4D&, const Vector4D& );
	friend Vector4D operator * ( float,           const Vector4D& );
	friend Vector4D operator * ( const Vector4D&, float );
	friend Vector4D operator / ( const Vector4D&, float );
	friend Vector4D operator / ( const Vector4D&, const Vector4D& );

private:
    float   sqr ( float x ) const
    {
        return x*x;
    }
};

inline Vector4D operator + ( const Vector4D& u, const Vector4D& v )
{
	return Vector4D ( u.x + v.x, u.y + v.y, u.z + v.z, u.w + v.w );
}

inline Vector4D operator - ( const Vector4D& u, const Vector4D& v )
{
	return Vector4D ( u.x - v.x, u.y - v.y, u.z - v.z, u.w - v.w );
}

inline Vector4D operator * ( const Vector4D& u, const Vector4D& v )
{
	return Vector4D ( u.x*v.x, u.y*v.y, u.z * v.z, u.w * v.w );
}

inline Vector4D operator * ( const Vector4D& v, float a )
{
	return Vector4D ( v.x*a, v.y*a, v.z*a, v.w*a );
}

inline Vector4D operator * ( float a, const Vector4D& v )
{
	return Vector4D ( v.x*a, v.y*a, v.z*a, v.w*a );
}

inline Vector4D operator / ( const Vector4D& u, const Vector4D& v )
{
	return Vector4D ( u.x/v.x, u.y/v.y, u.z/v.z, u.w/v.w );
}

inline Vector4D operator / ( const Vector4D& v, float a )
{
	return Vector4D ( v.x/a, v.y/a, v.z/a, v.w/a );
}

inline Vector4D operator / ( float a, const Vector4D& v )
{
	return Vector4D ( a / v.x, a / v.y, a / v.z, a / v.w );
}

inline float operator & ( const Vector4D& u, const Vector4D& v )
{
	return u.x*v.x + u.y*v.y + u.z*v.z + u.w*v.w;
}

inline	Vector4D lerp ( const Vector4D& a, const Vector4D& b, float t )
{
	return a + t * (b - a);
}

#endif
