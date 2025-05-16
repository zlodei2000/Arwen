//
// Basic subscene class, handles portals and mirrors
//
// Author: Alex V. Boreskoff
//
// Last changed: 5/12/2002
//

#ifndef __SUBSCENE__
#define __SUBSCENE__

#include    "Array.h"
#include	"BoundingBox.h"

class	Vector3D;
class	Transform3D;
class	Polygon3D;
class	Frustrum;
class	Camera;
class	View;
class	Fog;
class	VisualObject;
class	Controller;
class	Sky;
class	Light;
class	Texture;

class   SubScene : public Object
{
protected:
    Array       polys;
	Array		objects;
	Fog       * fog;
	Sky       * sky;
    BoundingBox boundingBox;

public:
    SubScene ( const char * theName ) : Object ( theName ), polys ( "Polys" ), objects ( "objects" )
	{
		fog       = NULL;
		sky       = NULL;
		metaClass = &classInstance;
	}

	virtual	bool	isOk () const
	{
		return polys.isOk ();
	}

    virtual int   init       ();
    virtual void  render     ( View& view, const Camera&, const Frustrum&, Array& post ) const;
    virtual void  renderPoly ( View& view, const Camera&, Polygon3D *, Polygon3D&, const Frustrum&, Array& post ) const;
    virtual int   contains   ( const Vector3D& pos ) const;
	virtual	void  update     ( Controller *, float systemTime );
	virtual	float traceRay   ( const Ray& ray, float maxLen, Vector3D& pt, Object ** hit ) const;

	virtual	void	collectColliders ( const BoundingBox& area, Array& list );
	virtual	void	drawLightmaps    ( View& view, const Array& polyList ) const;
	virtual	void	drawDetails      ( View& view, const Array& polyList ) const;
	
    void	addPoly   ( Polygon3D * poly );
	void	addObject ( VisualObject * object );
	void	setFog    ( Fog * theFog );
	void	setSky    ( Sky * theSky );

    const Array& getPolys () const
    {
        return polys;
    }

	const Array& getObjects () const
	{
		return objects;
	}

	const	BoundingBox&	getBoundingBox () const
	{
		return boundingBox;
	}

	static	int			mirrorDepth;
	static	int			polysRendered;
	static	MetaClass	classInstance;

protected:
    void			buildFrustrum  ( const Vector3D&, const Polygon3D&, Frustrum& ) const;
	virtual	bool	isFloating     ( Polygon3D * poly ) const;
	virtual	void	processLights  ( const Camera& camera, const Frustrum& viewFrustrum, Array& post ) const;
};

#endif
