//
// Basic world model for subscene-based portal engine
//
// Author: Alex V. Boreskoff
//
// Last modified: 5/12/2002
//

#ifndef __WORLD__
#define __WORLD__

#include	"SubScene.h"
#include	"Model.h"
#include	"ObjStr.h"

class	Texture;
class	Vector2D;
class	Vector4D;

class   World : public Model
{
protected:
    Array             scenes;
	SubScene        * currentSubScene;
	Vector3D		  updateSize;				// defines size of b/box for updating scenes
	Array			  polys;					// all the polys in the scene
	Array			  lights;					// all the lights in the scene
	String			  lightmapDir;				// directory where lightmaps are kept

public:
    World  ( const char * theName );
	~World ();

												// methods for rendering and getting 
												// potential colliders
	virtual	void	render           ( View& view, const Camera& camera );
	virtual	void	getColliders     ( const BoundingBox& area, Array& colliders );
	virtual	void	update           ( Controller *, float systemTime );
	virtual	bool	pointVisibleFrom ( const Vector3D& from, const Vector3D& to ) const;

	bool       addScene        ( SubScene * scene      );
	Texture  * getTexture      ( const String& theName ) const;
    SubScene * getSubScene     ( const Vector3D& pos   ) const;

    SubScene * getSubScene ( const String& theName ) const
	{
		return (SubScene *)scenes.getObjectWithName ( theName );
	}

	bool	buildAllLightmaps    ();
	bool	buildLightmapForPoly ( const Polygon3D * poly );

	String	getLightmapNameForPoly ( const Polygon3D * poly ) const;

	static	Texture	  * detailTexture;
	static	float		detailTextureScale;
	static	float		detailDistance;
	static	Vector3D	ambient;
	static	int			maxMirrorDepth;
	static	MetaClass	classInstance;

protected:
	bool	getLightmapSample      ();
	void	drawPostObjects        ( View& view, const Camera& camera, const Array& post ) const;
	bool	buildSampleOnPoly      ( const Polygon3D * poly, const Vector2D& tex, const Vector2D& mid, Vector3D& sample ) const;
	void	draw2dRect             ( View& view, const Vector3D& center, const Vector2D& size, const Vector4D& color, Texture * texture ) const;
};

extern	Texture   * haloTexture;
extern	Texture   * lensFlareTexture [4];

#endif
