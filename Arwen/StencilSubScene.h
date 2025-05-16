//
// Simple subscene for hardware accelerated graphics
// Uses bsp-tree fior transparent faces, stecnil buffer to polygon clipping.
// Supports floating and textured portals and mirrors.
// Supports portal transforms.
//
// Author: Alex V. Boreskoff
//
// Last modified: 5/12/2002
//

#ifndef __STENCIL_SUB_SCENE__
#define __STENCIL_SUB_SCENE__

#include "SubScene.h"
#include "BspTree.h"

class  StencilSubScene : public SubScene
{
private:
	Array	  opaqueFaces;						// opaque and floating faces/mirrors/portals are stored here
	BspNode * root;								// transparent faces/mirrors are stored in the bsp tree

public:
    StencilSubScene ( const char * theName ) : SubScene ( theName ), opaqueFaces ( "opaque Faces" )
    {
        root      = NULL;
		metaClass = &classInstance;
    }

    ~StencilSubScene ()
    {
        if ( root != NULL )
			deleteBspTree ( root );
    }

    virtual bool isOk () const
    {
        return opaqueFaces.isOk () && SubScene :: isOk ();
    }

    virtual int		init           ();
    virtual void	render         ( View& view, const Camera& camera, const Frustrum& frustrum, Array& post ) const;
	virtual	bool	shouldBeSorted ( const Polygon3D * poly ) const;

	static	int			curStencilVal;
	static	MetaClass	classInstance;

protected:
	class	BspRenderInfo
	{
	public:
		View           * view;
		const Camera   * camera;
		const Frustrum * viewFrustrum;
		Polygon3D      * tempPoly;
		Array          * post;

		BspRenderInfo ( View& theView, const Camera& theCamera, const Frustrum& theFrustrum,
						Polygon3D& theTempPoly, Array& thePost )
		{
			view         = &theView;
			camera       = &theCamera;
			viewFrustrum = &theFrustrum;
			tempPoly     = &theTempPoly;
			post         = &thePost;
		}
	};

	struct	ObjectSortInfo
	{
		VisualObject * object;
		float		   key;
	};

	friend static	int __cdecl objectCompFunc ( const void * elem1, const void * elem2 );

	void	drawPolygon ( const Polygon3D& ) const;
    void	renderTree  ( BspNode * node, BspRenderInfo& info, ObjectSortInfo list [], int count ) const;

					// draw poly to stencil only, using inc op on 
					// depth and stencil pass
					// draws only to stencil buffer
	void	incStencil ( const Polygon3D& ) const;

					// restore stencil by using dec op on stencil pass
					// draws only to stencil buffer
	void	decStencil ( const Polygon3D& ) const;

					// set depth values to that of given polygon on
					// stencil pass
					// modifies only depth and stencil buffers
	void	setDepth ( const Polygon3D& ) const;

					// clear depth values to max depth in visible points of poly
					// draws only on depth buffer, does not touch frame buffer,
					// sets stencil to visible points of poly
	void	clearDepth    ( const Camera&, const Polygon3D& ) const;

					// set default values for drawing polygons
	void	setDefaultStencilOpAndFunc () const;

	void	renderPortal ( View& view, const Camera& camera, Polygon3D * poly, Polygon3D& tempPoly, 
						   const Frustrum& viewFrustrum, Array& post ) const;
};

#endif
