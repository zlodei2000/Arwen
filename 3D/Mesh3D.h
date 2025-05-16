//
// Class to keep & render a 3d mesh
//
// Author: Alex V. Boreskoff
//
// Last modified: 14/11/2002
//

#ifndef	__MESH_3D__
#define	__MESH_3D__

#include	"Object.h"
#include	"BoundingBox.h"
#include	"Texture.h"
#include	"Vector4D.h"

class	Vector2D;
class	Vector3D;
class	Vector4D;
class	Transform3D;
class	Camera;
class	View;
class	Fog;

class	Mesh3D : public Object
{
public:
	struct	Face						// triangular face
	{
		int	vertexIndex   [3];			// indexes to array of vertices
		int	texCoordIndex [3];			// indexes to array of texture coordinates
	};

	struct	FaceVa						// triangular face struct for usage with vertex arrays
	{
			int	index [3];
	};

private:
	struct	FaceOrderingInfo			// struct used to keep info for ordering transparent meshes
	{
		float	key;					// distance
		int		index;					// facet #
	};

	int					maxVertices;
	int			        numVertices;
	int			        numTexCoords;
	int			        numFaces;
	Vector3D          * vertices;
	Vector3D          * normals;
	Vector2D          * texCoords;
	Vector4D          * colors;
	FaceVa            * faces;
	Texture           * texture;
	BoundingBox	        boundingBox;
	bool				smooth;
	FaceOrderingInfo  * indices;
	Vector3D          * centers;		// faces center poins
	mutable Vector4D  * tempColors;		// temp array for color values when in call to draw 
										// color is not ( 1, 1, 1, ) and colors array is present
	mutable Vector4D    cachedColor;	// color for which tempColors is computed,
										// declared mutable since it can be modified in const method draw

public:
	Mesh3D  ( const char * theName = "" );
	Mesh3D  ( const char * theName, 
		      Vector3D * theVertices,  Vector3D * theNormals, int theNumVertices,
		      Vector2D * theTexCoords, int theNumTexCoords,
			  Face     * theFaces,     int theNumFaces,
			  Texture  * theTexture,   Vector4D * theColors = NULL );

	~Mesh3D ();

	virtual	bool	isOk () const
	{
		return vertices != NULL && faces != NULL;
	}

	const BoundingBox& getBoundingBox () const
	{
		return boundingBox;
	}

	const Vector3D * getVertices () const
	{
		return vertices;
	}

	int	getNumVertices () const
	{
		return numVertices;
	}

	Texture * getTexture () const
	{
		return texture;
	}

	void	setSmooth ( bool flag ) 
	{
		smooth = flag;
	}

	bool	getSmooth () const
	{
		return smooth;
	}

	void	setTexture     ( Texture * theTexture );
	void	draw           ( View& view, const Camera& camera, const Vector4D& color, Fog * fog, Texture * txt = NULL, bool transparent = false ) const;
	void	apply          ( const Transform3D& );

	void	computeNormals   ();
	void	buildBoundingBox ();

	static	MetaClass	classInstance;

protected:
	void	rebuildFaces ( Face * theFaces, Vector2D * theTexCoords, int nTexCoords );
	void	appendVertex ( int from );

	static	int __cdecl compFunc ( const void * elem1, const void * elem2 );
};

#endif
