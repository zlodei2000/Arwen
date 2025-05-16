//
// Fog interface file
//
// Author: Alex V. Boreskoff
//
// Last modified: 18/11/2002
//

#include	<windows.h>
#include	<gl/gl.h>

#include	"Fog.h"
#include	"Vector3D.h"
#include	"Vector4D.h"
#include	"Poly3D.h"
#include	"View.h"
#include	"Frustrum.h"
#include	"Plane.h"
#include	"Ray.h"

#define	FOG_DELTA	0.003f

MetaClass	Fog :: classInstance ( "Fog", &Object :: classInstance );

Fog :: Fog ( const char * theName, Plane * thePlane, const Vector3D& theColor, float theDensity ) : Object ( theName )
{
	srcBlend  = View :: bmSrcAlpha;
	dstBlend  = View :: bmOneMinusSrcAlpha;
	color     = theColor;
	numPlanes = 1;
	plane [0] = thePlane;
	density   = theDensity;
	metaClass = &classInstance;
}

Fog :: ~Fog ()
{
	for ( int i = 0; i < numPlanes; i++ )
		delete plane [i];
}

void	Fog :: drawFogPoly ( View& view, const Vector3D& pos, const Frustrum& viewFrustrum, const Polygon3D& poly ) const
{
	Vector3D	offset ( FOG_DELTA * poly.getPlane () -> n );
	Polygon3D	p [2];							// temp polys for splitting
	Vector4D	curColor;
	Vector3D	closestPoint;
	Vector3D	pt;

	poly.split  ( *plane [0], p [0], p [1] );	// p [0] lies in the front (g.e. in the fog)

	poly.getPlane () -> closestPoint ( pos, pt );

	curColor.x = color.x;
	curColor.y = color.y;
	curColor.z = color.z;
	
	glPushAttrib ( GL_COLOR_BUFFER_BIT | GL_ENABLE_BIT | GL_DEPTH_BUFFER_BIT );

	glDisable   ( GL_TEXTURE_2D );
	glDisable   ( GL_LIGHTING   );
	glEnable    ( GL_BLEND      );
	glDepthMask ( GL_FALSE      );

	view.blendFunc ( srcBlend, dstBlend );

	for ( int i = 0; i < 2; i++ )
	{
		if ( p [i].isEmpty () )
			continue;

		p [i].init ();

		if ( !p [i].contains ( pt ) )			// point, closest to the plane is not in the poly
												// find closest point on poly's boundary
			p [i].closestPointToBoundary ( pt, closestPoint );
		else									// closest point already lies in the poly
			closestPoint = pt;

												// move point forward, since due to the innacuracies
												// of OpenGL we sometimes get "lost" points in fog
		closestPoint += offset;

		p [i].translate ( offset );

												// now draw the triangle fan on these points
		curColor.w = getOpacity ( pos, closestPoint, viewFrustrum.getNearPlane () );

		glBegin     ( GL_TRIANGLE_FAN );

		glColor4fv  ( curColor     );
		glVertex3fv ( closestPoint );

		const Vector3D * vertices = p [i].getVertices ();

		for ( int j = 0; j < p [i].getNumVertices (); j++ )
		{
			curColor.w = getOpacity ( pos, vertices [j], viewFrustrum.getNearPlane () );

			glColor4fv  ( curColor     );
			glVertex3fv ( vertices [j] );
		}

		curColor.w = getOpacity ( pos, vertices [0], viewFrustrum.getNearPlane () );

		glColor4fv  ( curColor     );
		glVertex3fv ( vertices [0] );

		glEnd    ();
	}

	glPopAttrib ();
}

bool	Fog :: addFogPlane ( Plane * thePlane )
{
	if ( numPlanes >= MAX_FOG_PLANES )
		return false;

	plane [numPlanes++] = thePlane;

	return true;
}

float	Fog :: getOpacity ( const Vector3D& from, const Vector3D& to, Plane * clipPlane ) const
{
	Ray		ray ( from, to - from );			// ray to sample fog
	float	t1, t2;								// points of entering and leaving fog
	float	t;
												// flag whether ray enters cliPlane 
	t1 = 0.0f;
	t2 = from.distanceTo ( to );

	if ( !traceRay ( ray, t1, t2 ) )
		return 0.0f;

												// now clip segment [t1, t2] to clipPlane
	if ( clipPlane != NULL && clipPlane -> intersectByRay ( ray.getOrigin (), ray.getDir (), t ) )
	{
		bool	clipFlag = clipPlane -> classify ( from ) == IN_BACK;

		if ( clipFlag )							// ray is entering clipPlane
		{
			if ( t > t1 )
				t1 = t;
		}
		else
		{
			if ( t < t2 )
				t2 = t;
		}
	}

	float	opacity = (t2 - t1) * density;

	return (opacity > 1.0f ? 1.0f : opacity);
}

bool	Fog :: traceRay ( const Ray& ray, float& tNear, float& tFar ) const
{
	if ( numPlanes < 1 )
		return false;

	float	t;

	for ( int i = 0; i < numPlanes; i++ )
	{
		int		side = plane [i] -> classify ( ray.getOrigin () );

		if ( !plane [i] -> intersectByRay ( ray.getOrigin (), ray.getDir (), t ) )
		{											// ray is nearly parallel to fog plane
													// so if it's origin lies in the fog
													// we do noting, otherwise return false
			if ( side == IN_BACK )
				return false;

			continue;
		}

		if ( side == IN_FRONT )						// we're casting ray from fog volume
		{
			if ( t > EPS )
				if ( t < tFar )						// compute min ( t )
					tFar = t;
		}
		else
		if ( side == IN_BACK )						// we're casting ray from unfogged region
		{											
			if ( t < EPS )							// ray goes out of fog volume (does not intersect it)
				return false;						// return no intersection

			if ( t > tNear )						// set tNear to point of entering fog volume ( max ( t ) )
				tNear = t;								
		}
		else
			return false;							// we lying in fog plane, return now intersection
	}

	return (tFar > EPS) && (tNear <= tFar);
}
