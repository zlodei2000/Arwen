//
// Simple subscene for hardware accelerated graphics
// Uses bsp-tree for transparent faces, stencil buffer to polygon clipping.
// Supports floating and textured portals and mirrors.
// Supports portal transforms.
//
// Author: Alex V. Boreskoff
//
// Last modified: 17/06/2003
//

#include	<windows.h>						// required by gl.h
#include	<gl/gl.h>

#include	"StencilSubScene.h"
#include	"Ray.h"
#include	"View.h"
#include	"Camera.h"
#include	"Frustrum.h"
#include	"Poly3D.h"
#include	"VisualObject.h"
#include	"Sky.h"
#include	"World.h"

#define	MAX_TRANSPARENT_OBJECTS	32				// max transparent visual objects

int			StencilSubScene :: curStencilVal = 0;
MetaClass	StencilSubScene :: classInstance ( "StencilSubScene", &SubScene :: classInstance );

static	int __cdecl objectCompFunc ( const void * elem1, const void * elem2 )
{
	float	v1 = ( (StencilSubScene :: ObjectSortInfo *) elem1 ) -> key;
	float	v2 = ( (StencilSubScene :: ObjectSortInfo *) elem2 ) -> key;

	if ( v1 > v2 )
		return -1;
	else
	if ( v1 < v2 )
		return 1;

	return 0;
}

bool	StencilSubScene :: shouldBeSorted ( const Polygon3D * poly ) const
{
											// any floating polygon (portal, mirror) 
											// should be sorted
	if ( poly -> testFlag ( PF_FLOATING ) )
		return true;

											// any transparent object, which is not a mirror 
											// or portal, should be sorted
	return poly -> isTransparent () && 
		!(poly -> testFlag ( PF_MIRROR ) || poly -> testFlag ( PF_PORTAL ));
}

int		StencilSubScene :: init ()
{
	SubScene :: init ();					// prepare floaters info

	Array	transpFaces ( "transparent facets" );

											// collect transparent faces, mirrors and portals
    for ( Array :: Iterator it = polys.getIterator (); !it.end (); ++it )
    {
        Polygon3D * poly = (Polygon3D *) it.value ();

		if ( shouldBeSorted ( poly ) )
			transpFaces.insert ( poly );
		else
			opaqueFaces.insert ( poly );
	}
		
											// combine them into the bsp-tree
	root = buildBspTree ( transpFaces );

	return 1;
}

void	StencilSubScene :: render ( View& view, const Camera& camera, const Frustrum& viewFrustrum, Array& post ) const
{
    Polygon3D   tempPoly ( "tempPoly", MAX_VERTICES );
	Array		detailPolys;
	Array		lightmappedPolys;

    view.lock  ();
	view.apply ( camera );

	int	sTest = glIsEnabled ( GL_STENCIL_TEST );

	setDefaultStencilOpAndFunc ();

							// render opaque polygons, clipping them via stencil buffer
    for ( Array :: Iterator it = opaqueFaces.getIterator (); !it.end (); ++it )
    {
        Polygon3D * poly = (Polygon3D *) it.value ();

        if ( !poly -> isFrontFacing ( camera.getPos () ) )
           continue;

        if ( poly -> testFlag ( PF_PORTAL ) )
			continue;

								// if not outside of viewing frustrum then draw it
		if ( viewFrustrum.intersects ( poly -> getBoundingBox () ) )
		{
			renderPoly ( view, camera, poly, tempPoly, viewFrustrum, post );

			if ( poly -> getLightmap () != NULL )
				lightmappedPolys.insert ( poly );
												// check for detail texture
			if ( poly -> testFlag ( PF_HAS_DETAIL_TEXTURE ) && World :: detailTexture != NULL )
				if ( poly -> getBoundingBox ().getDistanceTo ( camera.getPos (), camera.getViewDir () ) < World :: detailDistance )
					detailPolys.insert ( poly );
		}
    }

	drawDetails   ( view, detailPolys      );
	drawLightmaps ( view, lightmappedPolys );

	if ( sky != NULL )
		sky -> draw ( camera );

								// now render portals and mirrors
	for (Array::Iterator it = opaqueFaces.getIterator (); !it.end (); ++it )
	{
        Polygon3D * poly = (Polygon3D *) it.value ();

        if ( !poly -> isFrontFacing ( camera.getPos () ) )
           continue;

		if ( !poly -> testFlag ( PF_PORTAL ) )
			continue;

		renderPortal ( view, camera, poly, tempPoly, viewFrustrum, post );
	}

									// now render opaque objects
	for (Array::Iterator it = objects.getIterator (); !it.end (); ++it )
	{
		VisualObject * object = (VisualObject *) it.value ();

		if ( object -> isTransparent () )
			continue;

		if ( viewFrustrum.intersects ( object -> getBoundingBox () ) )
			object -> draw ( view, camera, viewFrustrum, fog );
	}

											// now render transparent polygons
	BspRenderInfo	info ( view, camera, viewFrustrum, tempPoly, post );
	ObjectSortInfo  transpObjects [MAX_TRANSPARENT_OBJECTS];
	int				count = 0;
									// build list of transparent objects
	for (Array::Iterator it = objects.getIterator (); !it.end (); ++it )
	{
		VisualObject * object = (VisualObject *) it.value ();

		if ( object -> isTransparent () && count < MAX_TRANSPARENT_OBJECTS )
		{
			transpObjects [count  ].key    = object -> getPos () & camera.getViewDir ();
			transpObjects [count++].object = object;
		}
	}

									// now sort it
	qsort ( transpObjects, count, sizeof ( ObjectSortInfo ), objectCompFunc );

									// render tree and all transparent objects
	renderTree    ( root, info, transpObjects, count );
	processLights ( camera, viewFrustrum, post );

	if ( !sTest )
		glDisable ( GL_STENCIL_TEST );

    view.unlock ();					// commit drawing
}

void	StencilSubScene :: setDefaultStencilOpAndFunc () const
{
					// enable stencil test
	glEnable      ( GL_STENCIL_TEST );
	glStencilMask ( 0xFF );

					// we do not modify stencil when rendering normal polys
	glStencilOp   ( GL_KEEP, GL_KEEP, GL_KEEP );

					// enable parts where stencil equals to curStencilVal
	glStencilFunc ( GL_EQUAL, curStencilVal, 0xFFFFFFFF );
}

void	StencilSubScene :: incStencil ( const Polygon3D& poly ) const
{
					// save current state
	glPushAttrib ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_ENABLE_BIT | 
		           GL_STENCIL_BUFFER_BIT );

					// setup OpenGL so we write only to stencil buffer in visible pixels 
					// of the poly
	glDisable     ( GL_TEXTURE_2D );
	glColorMask   ( GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE );
	glDepthMask   ( GL_FALSE );

					// set op to increment on both stencil and z pass
	glStencilOp   ( GL_KEEP, GL_KEEP, GL_INCR );
	glStencilFunc ( GL_EQUAL, curStencilVal, 0xFFFFFFFF );

					// now draw the polygon to stencil buffer
					// incrementing stencil when poly is visible
	drawPolygon ( poly );

					// restore attributes
	glPopAttrib ();

	curStencilVal++;
}

void	StencilSubScene :: decStencil ( const Polygon3D& poly ) const
{
					// save current state
	glPushAttrib ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_ENABLE_BIT | 
		           GL_STENCIL_BUFFER_BIT );

					// setup OpenGL so we write only to stencil buffer in visible pixels 
					// of the poly
	glDisable     ( GL_TEXTURE_2D );
	glColorMask   ( GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE );
	glDepthMask   ( GL_FALSE );

					// set op to decrement on stencil pass (zpass or zfail)
	glStencilOp   ( GL_KEEP, GL_DECR, GL_DECR );
	glStencilFunc ( GL_EQUAL, curStencilVal, 0xFFFFFFFF );

					// now draw the polygon to stencil buffer
					// incrementing stencil when poly is visible
	drawPolygon ( poly );

					// restore attributes
	glPopAttrib ();

	curStencilVal--;
}

void	StencilSubScene :: clearDepth ( const Camera& camera, const Polygon3D& poly ) const
{
	int			numVertices ( poly.getNumVertices () );
	Vector3D	org         ( camera.getPos () );
	Vector3D	normal      ( camera.getViewDir () );
	Vector3D	point       ( org + (camera.getZFar () * 0.99f) * normal );
	Plane		farPlane    ( normal, point );
	Vector3D	v;

					// save current state
	glPushAttrib ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_ENABLE_BIT | 
		           GL_STENCIL_BUFFER_BIT );

					// now reset depth to max where stencil is equal
					// to curStencilVal by projecting verices onto the far plane
					// leaving stencil unchanged
	glColorMask   ( GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE );
	glDepthMask   ( GL_TRUE );
	glEnable      ( GL_STENCIL_TEST );
	glStencilFunc ( GL_EQUAL, curStencilVal, 0xFFFFFFFF );
	glStencilOp   ( GL_KEEP, GL_KEEP, GL_KEEP );
	glDepthFunc   ( GL_ALWAYS );

	glBegin ( GL_POLYGON );

	const Vector3D * vertices = poly.getVertices ();

	for ( int i = 0; i < numVertices; i++ )
	{
		Ray	ray ( org, vertices [i] - org );

		v = ray.point ( ray.intersect ( farPlane ) );

		glVertex3fv ( v );
	}

	glEnd ();

							// restore attributes
	glPopAttrib ();
}

void	StencilSubScene :: setDepth ( const Polygon3D& poly ) const
{
					// save current state
	glPushAttrib ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_ENABLE_BIT | 
		           GL_STENCIL_BUFFER_BIT );

					// now reset depth to max where stencil is equal
					// to curStencilVal by projecting verices onto the far plane
					// leaving stencil unchanged
	glDepthMask   ( GL_TRUE );
	glColorMask   ( GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE );
	glEnable      ( GL_STENCIL_TEST );
	glStencilFunc ( GL_EQUAL, curStencilVal, 0xFFFFFFFF );
	glStencilOp   ( GL_KEEP, GL_KEEP, GL_KEEP );
	glDepthFunc   ( GL_ALWAYS );

					// now draw the polygon to depth buffer
					// leaving stencil intact
	drawPolygon ( poly );

					// restore attributes
	glPopAttrib ();
}

void	StencilSubScene :: drawPolygon ( const Polygon3D& poly ) const
{
	glBegin ( GL_POLYGON );

	for ( int i = 0; i < poly.getNumVertices (); i++ )
		glVertex3fv ( poly.getVertices () [i] );

	glEnd ();

}

void StencilSubScene :: renderTree ( BspNode * node, BspRenderInfo& info, 
									 ObjectSortInfo list [], int count ) const
{
    if ( node == NULL )
	{
		for ( int i = 0; i < count; i++ )
			list [i].object -> draw ( *info.view, *info.camera, *info.viewFrustrum, fog );

       return;
	}

										// build list of front and back objects
	ObjectSortInfo front [MAX_TRANSPARENT_OBJECTS];
	ObjectSortInfo back  [MAX_TRANSPARENT_OBJECTS];
	int			   frontCount = 0;
	int			   backCount  = 0;

	for ( int i = 0; i < count; i++ )
		if ( node -> classify ( list [i].object -> getPos () ) == IN_FRONT )
			front [frontCount++] = list [i];
		else
			back  [backCount++]  = list [i];

										// now render the tree
    if ( node -> classify ( info.camera -> getPos () ) == IN_FRONT )
    {
		renderTree ( node -> back, info, back, backCount );

        if ( node -> facet -> isFrontFacing ( info.camera -> getPos () ) )
	        renderPortal ( *info.view, *info.camera, node -> facet, *info.tempPoly, 
			               *info.viewFrustrum, *info.post );

        renderTree ( node -> front, info, front, frontCount );
    }
    else
    {
        renderTree ( node -> front, info, front, frontCount );

        if ( node -> facet -> isFrontFacing ( info.camera -> getPos () ) )
	        renderPortal ( *info.view, *info.camera, node -> facet, *info.tempPoly, 
			               *info.viewFrustrum, *info.post );

        renderTree ( node -> back, info, back, backCount );
    }
}

void	StencilSubScene :: renderPortal ( View& view, const Camera& camera, Polygon3D * poly, 
										  Polygon3D& tempPoly, const Frustrum& viewFrustrum, 
										  Array& post ) const
{
	tempPoly = *poly;			// copy current poly to temp poly

                                // clip against view frustrum
	if ( !tempPoly.clipByFrustrum ( viewFrustrum ) )
		return;

								// now draw portal to stencil using inc op
	incStencil                 ( tempPoly );
	setDefaultStencilOpAndFunc ();

								// for floater clear depth
	if ( poly -> testFlag ( PF_FLOATING ) || poly -> testFlag ( PF_MIRROR ) )
		clearDepth ( camera, tempPoly );
		
								// now render adjacent subscene
    renderPoly ( view, camera, poly, tempPoly, viewFrustrum, post );

								// for floaters and mirrors set depth to that of portal
	if ( poly -> testFlag ( PF_FLOATING ) || poly -> testFlag ( PF_MIRROR ) )
		setDepth ( tempPoly );

								// restore stencil using dec op
	decStencil                 ( tempPoly );
	setDefaultStencilOpAndFunc ();
}

