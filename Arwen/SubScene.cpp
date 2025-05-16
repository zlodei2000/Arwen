//
// Basic subscene class, handles portals and mirrors
//
// Author: Alex V. Boreskoff
//
// Last changed: 5/12/2002
//

#include	<windows.h>
#include	<gl/gl.h>
#include    "SubScene.h"
#include	"Transform3D.h"
#include    "Camera.h"
#include    "Frustrum.h"
#include    "View.h"
#include	"Poly3D.h"
#include	"Portal.h"
#include	"Fog.h"
#include	"Sky.h"
#include	"VisualObject.h"
#include	"Light.h"
#include	"View.h"
#include	"Texture.h"
#include	"World.h"
#include	"Lightmap.h"
#include	"Shader.h"

extern	Texture   * haloTexture;
extern	Texture   * lensFlareTexture [4];

int			SubScene :: mirrorDepth   = 0;
int			SubScene :: polysRendered = 0;
MetaClass	SubScene :: classInstance ( "SubScene", &Object :: classInstance );

int	SubScene :: init ()
{
	boundingBox.reset ();

	for ( Array :: Iterator it = polys.getIterator (); !it.end (); ++it )
    {
        Polygon3D * poly = (Polygon3D *) it.value ();

		if ( poly -> testFlag ( PF_PORTAL ) || poly -> testFlag ( PF_MIRROR ) )
			if ( isFloating ( poly ) )
				poly -> setFlag ( PF_FLOATING );
			
		boundingBox.addVertices ( poly -> getVertices (), poly -> getNumVertices () );
	}

	return Object :: init ();
}

void SubScene :: render ( View& view, const Camera& camera, const Frustrum& viewFrustrum, Array& post ) const
{
                                            // polygon to keep clipped poly
    Polygon3D   tempPoly ( "tempPoly", MAX_VERTICES );
	Array		detailPolys;
	Array		lightmappedPolys;

    view.lock  ();                          // prepare view for drawing
	view.apply ( camera );
                                            // render through portals
    for ( Array :: Iterator it = polys.getIterator (); !it.end (); ++it )
    {
        Polygon3D * poly = (Polygon3D *) it.value ();

        if ( !poly -> isFrontFacing ( camera.getPos () ) && !poly -> isTransparent () )
           continue;

											// if lies outside frustrum => reject it
        if ( !viewFrustrum.intersects ( poly -> getBoundingBox () ) )
           continue;

        renderPoly ( view, camera, poly, tempPoly, viewFrustrum, post );

		if ( poly -> getLightmap () != NULL )
			lightmappedPolys.insert ( poly );
												// check for detail texture
		if ( poly -> testFlag ( PF_HAS_DETAIL_TEXTURE ) && World :: detailTexture != NULL )
			if ( poly -> getBoundingBox ().getDistanceTo ( camera.getPos (), camera.getViewDir () ) < World :: detailDistance )
				detailPolys.insert ( poly );
    }

											// now draw non-transparent visual objects
	for (Array::Iterator it = objects.getIterator (); !it.end (); ++it )
	{
		VisualObject * obj = (VisualObject *) it.value ();

		if ( viewFrustrum.intersects ( obj -> getBoundingBox () ) )
			obj -> draw ( view, camera, viewFrustrum, fog );
	}

	drawDetails   ( view, detailPolys );
	drawLightmaps ( view, lightmappedPolys );

	if ( sky != NULL )
		sky -> draw ( camera );

											// add visible lights to post-process list
	processLights ( camera, viewFrustrum, post );

    view.unlock ();                         // commit drawing
}

void	SubScene :: update ( Controller * controller, float systemTime )
{
	for ( Array :: Iterator it = objects.getIterator (); !it.end (); ++it )
	{
		VisualObject * obj = (VisualObject *) it.value ();
		
		obj -> update ( controller, systemTime );
	}
}

void SubScene :: renderPoly ( View& view, const Camera& camera, Polygon3D * poly, Polygon3D& tempPoly, const Frustrum& viewFrustrum, Array& post ) const
{
    if ( poly -> testFlag ( PF_PORTAL ) )      // this is a portal
    {
        Frustrum    newFrustrum;
		Camera	    newCamera ( camera );
		Portal    * portal   = (Portal *) poly;
		SubScene  * adjScene = portal -> getAdjacentSubScene ( this );

		tempPoly = *poly;						// copy current poly to temp poly

												// clip against view frustrum
		if ( !tempPoly.clipByFrustrum ( viewFrustrum ) )
			return;


												// apply transform
		Transform3D * transform = portal -> getTransform ();

		if ( transform != NULL )
		{
			newCamera.transform ( *transform );
			tempPoly.transform  ( *transform );
		}

												// build frustrum, corresponding to
												// clipped portal
        buildFrustrum ( newCamera.getPos (), tempPoly, newFrustrum );

												// render through portal
        adjScene -> render ( view, newCamera, newFrustrum, post );

		view.apply ( camera );					// restore camera
    }
    else
    if ( poly -> testFlag ( PF_MIRROR ) )
    {
        Frustrum	newFrustrum;                                      
        Camera		mirroredCamera ( camera );
		Transform3D tr             ( Transform3D :: getMirror ( * poly -> getPlane () ) );

												// build mirrored camera
		mirroredCamera.transform ( tr );

												// build corresponding view frustrum
		tempPoly = *poly;						// copy current poly to temp poly

												// clip against view frustrum
		if ( !tempPoly.clipByFrustrum ( viewFrustrum ) )
			return;

												// build frustrum, corresponding to
												// clipped portal
        buildFrustrum ( mirroredCamera.getPos (), tempPoly, newFrustrum );

				                                // render with mirrored camera
		if ( mirrorDepth < World :: maxMirrorDepth )
		{
			mirrorDepth++;

			render ( view, mirroredCamera, newFrustrum, post );

			mirrorDepth--;
		}
												// restore camera
		view.apply ( camera );
    }

												// do not draw portals without texture (shader)
	if ( poly -> getShader () != NULL )
		poly -> getShader () -> drawPoly ( view, poly );
	else
	if ( poly -> getTexture () != NULL || !poly -> testFlag ( PF_PORTAL ) )
		view.draw ( *poly );

	polysRendered++;

	if ( fog != NULL )
		fog -> drawFogPoly ( view, camera.getPos (), viewFrustrum, *poly );
}

void    SubScene :: buildFrustrum ( const Vector3D& pos, const Polygon3D& clipPoly, Frustrum& frustrum ) const
{
    int				  numVertices = clipPoly.getNumVertices ();
	const Vector3D	* vertices    = clipPoly.getVertices    ();

	frustrum.set ( pos, numVertices, vertices );

											// near clipping plane
    Plane	clipPlane ( vertices [0], vertices [1], vertices [2] );

											// flip if camera is in positive halfspace
    if ( clipPlane.classify ( pos ) != IN_BACK )
       clipPlane.flip ();

    frustrum.setNearPlane ( clipPlane );
}

void    SubScene :: addPoly ( Polygon3D * poly )
{
    poly -> computePlane     ();
    poly -> buildBoundingBox ();
    poly -> setOwner         ( this );
    poly -> setId            ( polys.getCount () );

    polys.insert      ( poly );
    boundingBox.merge ( poly -> getBoundingBox () );
}

void    SubScene :: addObject ( VisualObject * obj )
{
	obj -> init     ();
	obj -> setOwner ( this );

    objects.insert    ( obj );
    boundingBox.merge ( obj -> getBoundingBox () );
}

void	SubScene :: setFog ( Fog * theFog )
{
	if ( fog != NULL )
		fog -> release ();

	fog = theFog;
}

void	SubScene :: setSky ( Sky * theSky )
{
	if ( sky != NULL )
		sky -> release ();

	sky = theSky;
}

int SubScene :: contains ( const Vector3D& pos ) const
{
    if ( !boundingBox.contains ( pos ) )
       return 0;

    int        inside = 0;
    Vector3D   dir ( 0, 1, 0 );					// make it go along Oy axis (upward)
	Ray		   ray ( pos, dir );
    Vector3D   p;
	float	   t;

    for ( Array :: Iterator it = polys.getIterator (); !it.end (); ++it )
    {
        Polygon3D * poly = (Polygon3D *) it.value ();

												// do not count two-sided polys
		if ( poly -> testFlag ( PF_TWOSIDED ) )
			continue;

												// plane almost parallel to dir
		if ( fabs ( poly -> getPlane () -> n & dir ) < EPS )
			continue;

        if ( ( t = poly -> intersectByRay ( ray, p ) ) > EPS )
           inside ^= 1;
    }

    return inside & 1;
}

bool	SubScene :: isFloating ( Polygon3D * poly ) const
{
	if ( poly -> testFlag ( PF_PORTAL ) )
		if ( ((Portal *) poly) -> getTransform () != NULL )
			return true;

	Plane	plane ( *poly -> getPlane () );

	for ( Array :: Iterator it = polys.getIterator (); !it.end (); ++it )
    {
        Polygon3D * p = (Polygon3D *) it.value ();
	
		if ( p -> testFlag ( PF_FLOATING ) || p == poly )
			continue;

		int	code = p -> classify ( plane );

		if ( code == IN_BACK || code == IN_BOTH )
			return true;
	}

	return false;
}

float SubScene :: traceRay ( const Ray& ray, float maxLen, Vector3D& pt, Object ** hit ) const
{
	float	    minDist = maxLen;
	Object    * tmp;
	Vector3D	hitPoint ( pt );
	BoundingBox	box;

	box.addVertex ( ray.getOrigin () );
	box.addVertex ( ray.point     ( maxLen ) );

	if ( hit == NULL )
		hit = &tmp;

											// initially no intersection
	*hit = NULL;

	for ( Array :: Iterator it = polys.getIterator (); !it.end (); ++it )
	{
		Polygon3D * poly = (Polygon3D *) it.value ();
		float		t;

		if ( !box.intersects ( poly -> getBoundingBox () ) )
			continue;

		if ( poly -> isTransparent () )		// ignore transparent object
			continue;

											// skip portals
		if ( poly -> testFlag ( PF_PORTAL ) )
			continue;

		if ( ( t = poly -> intersectByRay ( ray, hitPoint ) ) < EPS )
			continue;

		if ( t > maxLen - EPS )				// ignore hit if too far
			continue;

		if ( t < minDist )
		{
			minDist = t;
			pt      = hitPoint;
			*hit    = poly;
		}
	}
											// may be we should add hit test for objects

	if ( *hit == NULL )						// no hit at all
		return -1;							// since initially miDist = maxLen

	return minDist;
}

											// prepare list of post-process objects (now lights)
void	SubScene :: processLights ( const Camera& camera, const Frustrum& viewFrustrum, 
								    Array& post ) const
{
	World     * world = (World *) getOwner ();

	for ( Array :: Iterator it = objects.getIterator (); !it.end (); ++it )
	{
		Light * light = dynamic_cast <Light *> ( it.value () );

		if ( light == NULL )
			continue;
											// it's a light source
											// check for effective distance of light source
		if ( camera.getPos ().distanceTo ( light -> getPos () ) >= light -> getRadius () )
			continue;

											// now check whether it lies within view frustrum
		if ( !viewFrustrum.contains ( light -> getPos () ) )
			continue;
											// trace ray to light to see if it's visible
		if ( world -> pointVisibleFrom ( light -> getPos (), camera.getPos () ) )
			post.insert ( light );
	}
}

void	SubScene :: drawDetails ( View& view, const Array& polyList ) const
{
	if ( World :: detailTexture == NULL )
		return;

	glPushAttrib ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glDepthFunc  ( GL_EQUAL );
	glDepthMask  ( GL_FALSE );
	glEnable     ( GL_BLEND );
	glBlendFunc  ( GL_DST_COLOR, GL_SRC_COLOR );

													// apply detail scale transform
	glMatrixMode ( GL_TEXTURE );
	glPushMatrix ();
	glScalef     ( World :: detailTextureScale, World :: detailTextureScale, 
		           World :: detailTextureScale );
	glColor3f    ( 1, 1, 1 );						// in case it has other value from prev calls

	view.bindTexture ( World :: detailTexture );

	for ( Array :: Iterator it = polyList.getIterator (); !it.end (); ++it )
	{
		Polygon3D * poly = (Polygon3D *) it.value ();

		view.simpleDraw ( *poly, 0 );
	}

	glPopMatrix ();
	glPopAttrib ();
}

void	SubScene :: drawLightmaps ( View& view, const Array& polyList ) const
{
	glPushAttrib ( GL_COLOR_BUFFER_BIT | GL_TEXTURE_BIT | GL_DEPTH_BUFFER_BIT );
	glDepthFunc  ( GL_EQUAL );
	glDepthMask  ( GL_FALSE );
	glEnable     ( GL_BLEND );
	glBlendFunc  ( GL_ZERO, GL_SRC_COLOR );
	glColor3f    ( 1, 1, 1 );						// in case it has other value from prev calls

	for ( Array :: Iterator it = polyList.getIterator (); !it.end (); ++it )
	{
		Polygon3D * poly     = (Polygon3D *) it.value ();
		Lightmap  * lightmap = poly -> getLightmap ();
		
		view.bindTexture ( lightmap -> getTexture () );
		view.simpleDraw  ( *poly, View :: useLightmap );
	}

	glPopAttrib ();
}

void	SubScene :: collectColliders ( const BoundingBox& area, Array& list )
{
	for ( Array :: Iterator it = polys.getIterator (); !it.end (); ++it )
	{
		Polygon3D * poly = (Polygon3D *) it.value ();

		if ( poly -> getBoundingBox ().intersects ( area ) )
			list.insert ( poly );
	}
}
