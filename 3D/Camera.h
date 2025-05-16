//
// Camera class for OpenGL-based rendering
//
// Author: Alex V. Boreskoff
//
// Last change 5/12/2002
//

#ifndef __CAMERA__
#define __CAMERA__

#include    "Object.h"
#include    "Frustrum.h"

class	Plane;
class	Transform3D;

class   Camera : public Object
{
protected:
    Vector3D		pos;				// camera position
    Vector3D		viewDir;			// viewing direction (normalized)
    Vector3D		upDir;				// up direction (normalized)
    Vector3D		rightDir;			// right direction (normalized)
    Matrix3D		transf;				// camera transform (from world to camera space)
    float			fov;				// field of view angle
	float			zNear;				// near clipping z-value
	float			zFar;				// far clipping z-value
	int				width;				// view width
	int				height;				// view height
	float			aspect;				// aspect ratio of camera
    bool			mirrored;			// whether the camera is mirrored
    Frustrum		viewFrustrum;		// vewiwing frustrum

public:
    Camera ( const char * theName, const Vector3D& p, float yaw, float pitch, float roll,
             float aFov = 60.0f, float nearZ = 0.1f, float farZ = 2000.0f );

    Camera ( const Camera& camera );
	Camera ();

    const Vector3D&   getPos () const
    {
        return pos;
    }

    void    setPos ( const Vector3D& newPos )
    {
        pos = newPos;

		computeMatrix ();					// since clipping planes must be rebuild
    }

    const Vector3D&	getViewDir () const
    {
        return viewDir;
    }

    const Vector3D&	getRightDir () const
    {
        return rightDir;
    }

    const Vector3D&	getUpDir () const
    {
        return upDir;
    }

	float	getZNear () const
	{
		return zNear;
	}

	float	getZFar () const
	{
		return zFar;
	}

	float	getAspect () const
	{
		return aspect;
	}

    const Frustrum&   getViewFrustrum () const
    {
        return viewFrustrum;
    }

    bool	inViewingFrustrum ( const Vector3D& v ) const
    {
        return viewFrustrum.contains ( v );
    }

    bool	inViewingFrustrum ( const BoundingBox& box ) const
    {
        return viewFrustrum.intersects ( box );
    }

									// map vector from world space to camera space
	Vector3D	mapFromWorld ( const Vector3D& p ) const
	{
		Vector3D	tmp ( p - pos );

		return Vector3D ( tmp & rightDir, tmp & upDir, tmp & viewDir );
	}

									// map vector from camera space to world space
	Vector3D	mapToWorld ( const Vector3D& p ) const
	{
		return pos + p.x * rightDir + p.y * upDir + p.z * viewDir;
	}

									// map vector to screen space
	Vector3D	mapToScreen ( const Vector3D& p ) const
	{
		Vector3D	scr ( transf * ( p - pos ) );

		scr /= scr.z;				// do perspective transform
		
		return scr;
	}

    bool	isMirrored () const
    {
        return mirrored;
    }

    float   getFov () const
    {
        return fov;
    }

	int	getWidth () const
	{
		return width;
	}

	int	getHeight () const
	{
		return height;
	}

    void    setEulerAngles  ( float theYaw, float thePitch, float theRoll );
	void	setViewSize     ( int theWidth, int theHeight, float theFov );
	void	setAspect       ( float newAspect );
    void    setFov          ( float newFovAngle );
    void    mirror          ( const Plane& );
	void	transform       ( const Transform3D& );

	void	buildHomogeneousMatrix ( float matrix [16] ) const;

	static	MetaClass	classInstance;

private:
    void    computeMatrix ();			// compute vectors, transform matrix and build viewing frustrum
};

#endif
