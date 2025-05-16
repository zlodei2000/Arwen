//
// Mesh VisualObject
//
// Author: Alex V. Boreskoff
//
// Last modified: 14/11/2002
//

#ifndef	__MESH_OBJECT__
#define	__MESH_OBJECT__

#include	"VisualObject.h"

class	Mesh3D;
class	Shader;

class	MeshObject : public VisualObject
{
protected:
	Mesh3D * mesh;
	Shader * shader;

public:
	MeshObject  ( const char * theName, const Vector3D& thePos, const Vector4D& theColor, const Transform3D& theTr, Mesh3D * theMesh = NULL );
	~MeshObject ();

	void	setShader  ( Shader * sp );
	void	setTexture ( Texture * txt );

	Mesh3D * getMesh () const
	{
		return mesh;
	}

	void	setMesh ( Mesh3D * theMesh );

	virtual	bool	isTransparent () const;

	static	MetaClass	classInstance;

protected:
	virtual	void	buildBoundingBox ();
	virtual	void	doDraw           ( View& view, const Camera& camera, const Frustrum& frustrum, Fog * fog );
};

#endif
