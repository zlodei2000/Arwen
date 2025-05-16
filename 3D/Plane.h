//
// Class for simple plane (p,n) + dist = 0
//
// Author: Alex V. Boreskoff
// Last change: 30/09/2002
//
#ifndef __PLANE__
#define __PLANE__

#include    "3dDefs.h"
#include    "Vector3D.h"
#include    "Matrix3D.h"
#include	"Ray.h"

class	Transform3D;

class   Plane
{
public:
    Vector3D n;                 // normal vector
    float    dist;              // signed distance along n
                                // build plane from normal and signed distance
    int      nearPointMask;     // -1 if not initialized
    int      mainAxis;          // index of main axis

    Plane ( const Vector3D& normal, float d ) : n (normal), nearPointMask ( -1 )
    {
        n.normalize ();

        dist = d;

        computeNearPointMaskAndMainAxis ();
    }
                                // build plane from plane equation
    Plane ( float nx, float ny, float nz, float d ) : n (nx, ny, nz), nearPointMask ( -1 )
    {
        n.normalize ();

        dist = d;

        computeNearPointMaskAndMainAxis ();
    }
                                // build plane from normal and point on plane
    Plane ( const Vector3D& normal, const Vector3D& point ) : n ( normal ), nearPointMask ( -1 )
    {
        n.normalize ();

        dist = -(point & n);

        computeNearPointMaskAndMainAxis ();
    }
                                // build plane from 3 points
    Plane ( const Vector3D& p1, const Vector3D& p2, const Vector3D& p3 ) : nearPointMask ( -1 )
    {
        n = (p2 - p1) ^ (p3 - p1 );

        n.normalize ();

        dist = -(p1 & n);

        computeNearPointMaskAndMainAxis ();
    }

    Plane ( const Plane& plane ) : n ( plane.n ), dist ( plane.dist )
    {
        nearPointMask = plane.nearPointMask;
        mainAxis      = plane.mainAxis;
    }

    float    signedDistanceTo ( const Vector3D& v ) const
    {
        return (v & n) + dist;
    }

	float	distanceTo ( const Vector3D& v ) const
	{
		return (float)fabs ( signedDistanceTo ( v ) );
	}
	                            // get point on plane
    Vector3D    point () const
    {
        return (-dist) * n;
    }
                            // classify point
    int	classify ( const Vector3D& p ) const
    {
        float   v = signedDistanceTo ( p );

        if ( v > EPS )
            return IN_FRONT;
        else
        if ( v < -EPS )
            return IN_BACK;

        return IN_PLANE;
    }

                            // mirror position (point), depends on plane posit.
    void    reflectPos ( Vector3D& v ) const
    {
        v -= (2.0f*((v & n) + dist)) * n;
    }

                            // mirror direction, depends only on plane normal
    void    reflectDir ( Vector3D& v ) const
    {
        v -= (2.0f*(v & n)) * n;
    }

    void    reflectPlane ( Plane& plane ) const
    {
        Vector3D p (-plane.dist * plane.n);        // point on plane

        reflectDir ( plane.n );
        reflectPos ( p );

        plane.dist = -(p & plane.n);

        plane.computeNearPointMaskAndMainAxis ();
    }

    void    rotate ( const Matrix3D& rot )
    {
        Vector3D    p ( - dist*n );

        n    = rot * n;
        dist = - (p & n);

        computeNearPointMaskAndMainAxis ();
    }

    void    flip ()
    {
        n    = -n;
        dist = -dist;

        computeNearPointMaskAndMainAxis ();
    }

    float   closestPoint ( const Vector3D& p, Vector3D& res ) const
    {
        float   distanceToPlane = - dist - (p & n);

        res = p + distanceToPlane * n;

        return distanceToPlane;
    }

    bool	intersectByRay ( const Vector3D& org, const Vector3D& dir, float& t ) const
    {
        float   numer = - (dist + (org & n));
        float   denom = dir & n;

        if ( fabs ( denom ) < EPS )
           return false;

		t = numer / denom;

        return true;
    }

	bool	intersectByRay ( const Ray& ray, float& t ) const
	{
		float	numer = -(dist + (ray.getOrigin () & n));
		float	denom = ray.getDir () & n;

		if ( fabs ( denom ) < EPS )
			return false;

		t = numer / denom;

		return true;
	}

    Vector3D    makeNearPoint ( const Vector3D& minPoint, const Vector3D& maxPoint ) const
    {
        return Vector3D ( nearPointMask & 1 ? maxPoint.x : minPoint.x,
                          nearPointMask & 2 ? maxPoint.y : minPoint.y,
                          nearPointMask & 4 ? maxPoint.z : minPoint.z );
    }

    Vector3D    makeFarPoint ( const Vector3D& minPoint, const Vector3D& maxPoint ) const
    {
        return Vector3D ( nearPointMask & 1 ? minPoint.x : maxPoint.x,
                          nearPointMask & 2 ? minPoint.y : maxPoint.y,
                          nearPointMask & 4 ? minPoint.z : maxPoint.z );
    }

    int getMainAxis () const
    {
        return mainAxis;
    }

	void	apply ( const Transform3D& );

protected:
    void    computeNearPointMaskAndMainAxis ();
};

#endif
