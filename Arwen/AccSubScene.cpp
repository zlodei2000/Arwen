//
// Simple subscene for hardware accelerated graphics
// Uses bsp-tree fior transparent faces
//
// Author: Alex V. Boreskoff
//
// Last modified: 2/12/2001
//

#include	"AccSubScene.h"
#include	"Camera.h"
#include	"View.h"
#include	"Poly3D.h"
#include	"Portal.h"
#include	"VisualObject.h"
#include	"Sky.h"
#include	"BspDrawVisitor.h"

MetaClass	AccSubScene :: classInstance ( "AccSubScene", &SubScene :: classInstance );

bool	AccSubScene :: shouldBeSorted ( const Polygon3D * poly ) const
{
											// any floating polygon (portal, mirror) should be sorted
	if ( poly -> testFlag ( PF_FLOATING ) )
		return true;

											// any transparent object, which is not a mirror or portal,
											// should be sorted
	return poly -> isTransparent () && !(poly -> testFlag ( PF_MIRROR ) || poly -> testFlag ( PF_PORTAL ));
}

int AccSubScene :: init ()
{
	SubScene :: init ();					// prepare floating info

	Array	transpFaces ( "transparent facets" );

											// collect transparent faces
	for ( Array :: Iterator it = polys.getIterator () ; !it.end(); ++it )
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

void AccSubScene :: collectPolys ( BspNode * node )
{
	polys.insert ( node -> facet );

    if ( node -> front != NULL )
		collectPolys ( node -> front );

    if ( node -> back != NULL )
        collectPolys ( node -> back );
}

void  AccSubScene :: render ( View& view, const Camera& camera, const Frustrum& viewFrustrum, Array& post ) const
{
    Polygon3D   tempPoly ( "tempPoly", MAX_VERTICES );

    view.lock  ();
	view.apply ( camera );

                                            // render opaque facets
    for ( int i = 0; i < opaqueFaces.getCount (); i++ )
    {
        Polygon3D * poly = (Polygon3D *) opaqueFaces.at ( i );

        if ( !poly -> isFrontFacing ( camera.getPos () ) )
           continue;

                                            // check against view frustrum
        if ( !viewFrustrum.intersects ( poly -> getBoundingBox () ) )
           continue;

        renderPoly ( view, camera, poly, tempPoly, viewFrustrum, post );
    }

	if ( sky != NULL )
		sky -> draw ( camera );

											// now draw non-transparent visual objects
	for ( Array :: Iterator it = objects.getIterator (); !it.end (); ++it )
	{
		VisualObject * obj = (VisualObject *) it.value ();

		if ( viewFrustrum.intersects ( obj -> getBoundingBox () ) )
			obj -> draw ( view, camera, viewFrustrum, fog );
	}

											// now render transparent polygons
	BspRenderInfo	info;

	info.view = &view;
	info.viewFrustrum = &viewFrustrum;
	info.camera       = &camera;
	info.tempPoly     = &tempPoly;
	info.post         = &post;

	renderTree    ( root, info );
	processLights ( camera, viewFrustrum, post );

    view.unlock ();							// commit drawing
}

void AccSubScene :: renderTree ( BspNode * node, BspRenderInfo& info ) const
{
    if ( node == NULL )
       return;
										// now render the tree
    if ( node -> classify ( info.camera -> getPos () ) == IN_FRONT )
    {
        if ( node -> back != NULL )
           renderTree ( node -> back, info );

        if ( node -> facet -> isFrontFacing ( info.camera -> getPos () ) )
	        renderPoly ( *info.view, *info.camera, node -> facet, *info.tempPoly, *info.viewFrustrum, *info.post );

        if ( node -> front != NULL )
           renderTree ( node -> front, info );
    }
    else
    {
        if ( node -> front != NULL )
           renderTree ( node -> front, info );

        if ( node -> facet -> isFrontFacing ( info.camera -> getPos () ) )
	        renderPoly ( *info.view, *info.camera, node -> facet, *info.tempPoly, *info.viewFrustrum, *info.post );

        if ( node -> back != NULL )
           renderTree ( node -> back, info );
    }
}
