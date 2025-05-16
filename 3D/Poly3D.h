//
// 3-dimensional polygon class
// Supports basic operations, textures, lightmaps and vertex colors
//
// Author: Alex V. Boreskoff
//
// Last changed:6/06/2003
//
 
#ifndef __POLYGON3D__
#define __POLYGON3D__

#include    <memory.h>

#include    "Plane.h"
#include    "BoundingBox.h"
#include	"Object.h"
#include	"Vector4D.h"
#include	"Math3D.h"

#define PF_PORTAL				0x0004
#define PF_MIRROR				0x0008		// it's a mirror
#define PF_PASSABLE				0x0010		// player can go through this polygon
#define	PF_NODRAW				0x0020		// don't draw it, used to trigger some actions
#define	PF_FLOATING				0x0040		// floating portal or mirror, requires special handling iwth z-buffering
#define	PF_TWOSIDED				0x0080		// two-sided polygon, draw independent of orientation
#define	PF_DETAIL				0x0100		// it's a detail object
#define	PF_NOT_OCCLUDER			0x0200		// should not be used to cull geometry
#define	PF_HAS_DETAIL_TEXTURE	0x0400		// should be drawn with detail texture
#define	PF_NO_LIGHTMAP			0x0800		// do not generate lightmap for this poly

class	Frustrum;
class	Matrix3D;
class	Transform3D;
class	Texture;
class	Lightmap;
class	Vector2D;
class	Mapping;
class	Shader;

class    Polygon3D : public Object
{
protected:
    Plane     * plane;				// plane through this facet
    int         numVertices;		// current # of vertices
    int         maxVertices;		// storage allocated for maxVertices
    int         id;
    Vector3D  * vertices;			// polygon vertices
	Vector4D  * colors;				// color values ((RGBA) for every vertex
    Vector2D  * uvMap;				// texture coordinates (u,v,1)
    Texture   * texture;			// texture of this polygon
    Lightmap  * lightMap;			// it's light map
    BoundingBox boundingBox;		// polygon's bounding box (AABB)
	Vector4D	color;
	Mapping   * mapping;			// mapping of space coords to texture coords
	Shader    * shader;

public:
    Polygon3D ();
    Polygon3D ( const Polygon3D& poly );
    Polygon3D ( const char * theName, int n );
    Polygon3D ( const char * theName, int n, Vector3D * v, Vector2D * uv );

    ~Polygon3D ();

    virtual bool isOk () const;
	virtual	int	 init ();
	virtual	int	 compare ( const Object * obj ) const;

	virtual	long	hash () const
	{
		return id;
	}

    Polygon3D& operator = ( const Polygon3D& poly );

    int     getId () const
    {
        return id;
    }

    void    setId ( int newId )
    {
        id = newId;
    }

    const Plane * getPlane () const
    {
        return plane;
    }

	void	setPlane ( Plane * thePlane )
	{
		delete plane;

		plane = thePlane;
	}

	void	setPlane ( const Plane& thePlane )
	{
		delete plane;

		plane = new Plane ( thePlane );
	}

	const Vector4D&	getColor () const
	{
		return color;
	}

	void	setColor ( const Vector4D& theColor )
	{
		color = theColor;
	}
	
    int getNumVertices () const
    {
        return numVertices;
    }

    const Vector3D * getVertices () const
    {
        return vertices;
    }

    const Vector2D * getUvMap () const
    {
        return uvMap;
    }

	const Vector4D * getColors () const
	{
		return colors;
	}

	const Mapping * getMapping () const
	{
		return mapping;
	}

    const Vector3D&    getNormal () const
    {
        return plane -> n;
    }

    bool isFrontFacing ( const Vector3D& org ) const
    {
		if ( testFlag ( PF_TWOSIDED ) )
			return true;

        return plane != NULL ? ( plane -> classify ( org ) == IN_FRONT ) : false;
    }

    const BoundingBox& getBoundingBox () const
    {
        return boundingBox;
    }

    Texture * getTexture () const
    {
        return texture;
    }

    Lightmap * getLightmap () const
    {
        return lightMap;
    }

	Shader * getShader () const
	{
		return shader;
	}

	bool	isEmpty () const
	{
		return numVertices < 3;
	}

	void		setMapping         ( const Mapping& );
    void		setTexture         ( Texture * tex );
    void		setLightmap        ( Lightmap * map );		// set ready lightmap
	void		setLightmapTexture ( Texture * txt );		// set texture as lightmap
															// lightmap parameters will be computed automatically
	void		setShader          ( Shader * sp );

    void        transform      ( const Transform3D& );   // apply affine transform
    void        transform      ( const Matrix3D& );      // apply linear transform
    void        translate      ( const Vector3D& );      // translate polygon
    int         classify       ( const Plane& p ) const;
    void        split          ( const Plane& p, Polygon3D& front, Polygon3D& back ) const;
	void		addVertex      ( const Vector3D& v, const Vector2D& uv, const Vector4D& color );
    void        addVertex      ( const Vector3D& v, const Vector2D& uv );
	void		addVertex      ( const Vector3D& v );
    void        delVertex      ( int index );
    bool        contains       ( const Vector3D& v ) const;
    int         intersect      ( const Polygon3D& ) const;
    int         clipByPlane    ( const Plane& );
    int         clipByFrustrum ( const Frustrum& );
    float       intersectByRay ( const Ray& ray, Vector3D& pos );
    int         classify       ( const Polygon3D& ) const;
    float       getSignedArea  () const;
    Vector3D    getCenter      () const;
	bool		isTransparent  () const;

	float		getDistEstimate ( const Vector3D& dir, const Vector3D& from )
	{
		int	mask = computeNearPointMask ( dir );

		return (boundingBox.getVertex ( mask ) & dir) - (from & dir);
	}

	float		getDistEstimate ( const Vector3D& dir, float offset, int nearPointIndex )
	{
		return (boundingBox.getVertex ( nearPointIndex ) & dir) - offset;
	}


	void		projectFromPointOntoPlane ( const Vector3D& org, const Plane& plane );
    float       closestPointToBoundary    ( const Vector3D& p, Vector3D& res ) const;

    void        realloc          ( int newMaxVertices );
    void        computePlane     ();
    void        buildBoundingBox ();
	void		clear            ();

											// map texture coordinates to space
	Vector3D	mapTextureToWorld ( const Vector2D& tex ) const;

											// get range of texture coordinates
	void	getTextureExtent ( Vector2D& texMin, Vector2D& texMax ) const;

	static	MetaClass	classInstance;
};

Vector3D    closestPointToSegment ( const Vector3D& p, const Vector3D& a, const Vector3D& b );

#endif
