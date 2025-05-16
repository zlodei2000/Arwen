//
// Simple subscene for hardware accelerated graphics
// Uses bsp-tree fior transparent faces
//
// Author: Alex V. Boreskoff
//
// Last modified: 28/11/2001
//

#ifndef __ACC_SUB_SCENE__
#define __ACC_SUB_SCENE__

#include "SubScene.h"
#include "BspTree.h"

class  AccSubScene : public SubScene
{
private:
	Array	  opaqueFaces;						// opaque faces are stored here
	BspNode * root;								// transparent are stored in the bsp tree
public:
    AccSubScene ( const char * theName ) : SubScene ( theName ), opaqueFaces ( "opaque Faces" )
    {
        root      = NULL;
		metaClass = &classInstance;
    }

    ~AccSubScene ()
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

	static	MetaClass	classInstance;

protected:
	struct	BspRenderInfo
	{
		View           * view;
		const Camera   * camera;
		const Frustrum * viewFrustrum;
		Polygon3D      * tempPoly;
		Array          * post;
	};

    void	renderTree   ( BspNode * node, BspRenderInfo& info ) const;
    void	collectPolys ( BspNode * node );
};

#endif
