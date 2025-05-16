//
// Frustrum class with basic functionality
//
// Author: Alex V. Boreskoff
//
// Last change: 14/11/2002
//

#ifndef __FRUSTRUM__
#define __FRUSTRUM__

#include    "3dDefs.h"
#include    "Plane.h"
#include    "BoundingBox.h"

class	Polygon3D;
class	BoundingBox;

class   Frustrum
{
	Vector3D	org;						// origin of frustrum
	Vector3D    vertices [MAX_VERTICES];	// these vertices and org make the frustrum
	int			numVertices;
	Plane     * farPlane;
	Plane     * nearPlane;
    Plane     * planes [MAX_VERTICES];		// planes, build on vertices and org
    int         numPlanes;
public:
    Frustrum ();
	Frustrum ( const Vector3D& theOrg, int num, const Vector3D * v );
	Frustrum ( const Vector3D& theOrg, const BoundingBox& box );
    Frustrum ( const Frustrum& frustrum );
    ~Frustrum ()
    {
        reset ();
    }

	void	set ( const Vector3D& theOrg, int num, const Vector3D * v );

	Plane * getNearPlane () const
	{
		return nearPlane;
	}

	void	setNearPlane ( const Plane& p )
	{
		delete nearPlane;

		nearPlane = new Plane ( p );
	}

	Plane * getFarPlane () const
	{
		return farPlane;
	}

	void	setFarPlane ( const Plane& p )
	{
		delete farPlane;

		farPlane = new Plane ( p );
	}

    void    reset ()
    {
		delete farPlane;
		delete nearPlane;

        for ( int i = 0; i < numPlanes; i++ )
            delete planes [i];

        numPlanes = 0;
		farPlane  = NULL;
		nearPlane = NULL;

    }

    int getNumPlanes () const
    {
        return numPlanes;
    }

    Plane * getPlane ( int index ) const
    {
        return index >= 0 && index < numPlanes ? planes [index] : NULL;
    }

	bool	contains ( const Vector3D& v ) const
	{
		for ( register int i = 0; i < numPlanes; i++ )
			if ( planes [i] -> classify ( v ) == IN_BACK )
				return false;

		if ( nearPlane != NULL )
			if ( nearPlane -> classify ( v ) == IN_BACK )
				return false;

		if ( farPlane != NULL )
			if ( farPlane -> classify ( v ) == IN_BACK )
				return false;

		return true;
	}

    bool	intersects ( const BoundingBox& box ) const
    {
        for ( register int i = 0; i < numPlanes; i++ )
            if ( box.classify ( * planes [i] ) == IN_BACK )
               return false;

		if ( nearPlane != NULL )
			if ( box.classify ( *nearPlane ) == IN_BACK )
				return false;

		if ( farPlane != NULL )
			if ( box.classify ( *farPlane ) == IN_BACK )
				return false;

        return true;
    }

	bool	containsInside ( const BoundingBox& box ) const
	{
		for ( register int i = 0; i < numPlanes; i++ )
			if ( box.classify ( *planes [i] ) != IN_FRONT )
				return false;

		if ( box.classify ( *nearPlane ) != IN_FRONT )
			return false;

		if ( box.classify ( *farPlane ) != IN_FRONT )
			return false;

		return true;
	}

	bool	isBlockedBy    ( const Polygon3D& poly ) const;
	bool	contains       ( const Frustrum& ) const;
	void	clipByPlane    ( const Plane& );
	void	clipByFrustrum ( const Frustrum& );

private:
	void	buildPlanes ();					// build planes based on org and vertices
};

#endif
