//
// Frustrum class with basic functionality
//
// Author: Alex V. Boreskoff
//
// Last change: 1/12/2002
//

#include	"Frustrum.h"
#include	"Poly3D.h"
#include	"BoundingBox.h"

Frustrum :: Frustrum ()
{
	numVertices = 0;
    numPlanes   = 0;
	farPlane    = NULL;
	nearPlane   = NULL;
}

Frustrum :: Frustrum ( const Vector3D& theOrg, int num, const Vector3D * v )
{
	org = theOrg;

	if ( num > MAX_VERTICES )
		num = MAX_VERTICES;

	numVertices = num;

	for ( int i = 0; i < num; i++ )
		vertices [i] = v [i];

	farPlane  = NULL;
	nearPlane = NULL;
	numPlanes = 0;						// mark that there's nothing to delete

	buildPlanes ();
}

Frustrum :: Frustrum ( const Vector3D& theOrg, const BoundingBox& box )
{
	bool	faceFlags [6];

							// now determine which faces are front-faced and which back-faced
	if ( theOrg.x < box.getMin ().x )
	{
		faceFlags [0] = true;
		faceFlags [1] = false;
	}
	else
	if ( theOrg.x > box.getMax ().x )
	{
		faceFlags [0] = false;
		faceFlags [1] = true;
	}
	else
	{
		faceFlags [0] = false;
		faceFlags [1] = false;
	}

	if ( theOrg.y < box.getMin ().y )
	{
		faceFlags [2] = true;
		faceFlags [3] = false;
	}
	else
	if ( theOrg.y > box.getMax ().y )
	{
		faceFlags [2] = false;
		faceFlags [3] = true;
	}
	else
	{
		faceFlags [2] = false;
		faceFlags [3] = false;
	}

	if ( theOrg.z < box.getMin ().z )
	{
		faceFlags [4] = true;
		faceFlags [5] = false;
	}
	else
	if ( theOrg.z > box.getMax ().z )
	{
		faceFlags [4] = false;
		faceFlags [5] = true;
	}
	else
	{
		faceFlags [4] = false;
		faceFlags [5] = false;
	}

							// now find edges between front and back faces

	numVertices = 0;
	nearPlane   = NULL;
	farPlane    = NULL;
	numPlanes   = 0;

	buildPlanes ();
}

Frustrum :: Frustrum ( const Frustrum& frustrum )
{
	org         = frustrum.org;
	numVertices = frustrum.numVertices;
    numPlanes   = frustrum.numPlanes;

	for ( int i = 0; i < numVertices; i++ )
		vertices [i] = frustrum.vertices [i];

    for ( int i =0; i < numPlanes; i++ )
        planes [i] = new Plane ( * frustrum.planes [i] );

	if ( frustrum.nearPlane != NULL )
		nearPlane = new Plane ( *frustrum.nearPlane );
	else
		nearPlane = NULL;

	if ( frustrum.farPlane != NULL )
		farPlane = new Plane ( *frustrum.farPlane );
	else
		farPlane = NULL;
}

void	Frustrum :: set ( const Vector3D& theOrg, int num, const Vector3D * v )
{
	org = theOrg;

	if ( num > MAX_VERTICES )
		num = MAX_VERTICES;

	numVertices = num;

	for ( int i = 0; i < num; i++ )
		vertices [i] = v [i];

	delete farPlane;
	delete nearPlane;

	farPlane  = NULL;
	nearPlane = NULL;

	buildPlanes ();
}

void	Frustrum :: buildPlanes ()					// build planes based on arg and vertices
{
	if ( numVertices < 1 )
		return;

	reset ();							// remove previous planes

	int	prev = numVertices - 1;
	int	cur  = 0;

	for ( ; cur < numVertices; cur++ )
	{
		if ( !areCollinear ( org, vertices [prev], vertices [cur] ) )
			planes [numPlanes++] = new Plane ( org, vertices [prev], vertices [cur] );

		prev = cur;
	}

											// now ensure correct plane normal orientation
	Vector3D	c ( vertices [0] );

	for ( int i = 1; i < numVertices; i++ )
		c += vertices [i];

	c /= (float)numVertices;

	for ( int i = 0; i < numPlanes; i++ )
		if ( planes [i] -> classify ( c ) == IN_BACK )
			planes [i] -> flip ();
}

bool	Frustrum :: isBlockedBy ( const Polygon3D& poly ) const
{
	float	t;

	for ( int i = 0; i < numVertices; i++ )
	{
		if ( !poly.getPlane () -> intersectByRay ( org, vertices [i], t ) )
			return false;

		if ( t < EPS )
			return false;

								// get poit of intersection
		Vector3D	v ( org + t * vertices [i] );

		if ( !poly.contains ( v ) )
			return false;
	}

	return true;
}

bool	Frustrum :: contains ( const Frustrum& f ) const
{
	if ( !contains ( f.org ) )
		return false;

	for ( register int i = 0; i < f.numVertices; i++ )
		if ( !contains ( f.vertices [i] ) )
			return false;

	return true;
}
