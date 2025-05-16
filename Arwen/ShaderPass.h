//
// Class describing one shader for for OpenGl shader in Arwen project
//
// Author: Alex V. Boreskoff
//
// Last modified: 2/12/2002
//

#ifndef	__SHADER_PASS__
#define	__SHADER_PASS__

#include	"Object.h"
#include	"Vector4D.h"

class	Transform2D;
class	Polygon3D;
class	Mesh3D;
class	Texture;
class	View;
class	Camera;
class	Fog;

class	ShaderPass :public Object
{
protected:
	Texture     * texture;				// blended texture
	Vector4D      color;				// used color
	int	          srcBlend;				// blend modes
	int		      dstBlend;
	bool	      envMapped;			// whether we use environment mapping
	Transform2D * textureTransf;		// texture coordinates transform
public:
	ShaderPass  ( const char * theName, Texture * theTexture, const Vector4D& theColor, bool env = false );
	~ShaderPass ();

	void	setBlendingMode ( int s, int d )
	{
		srcBlend = s;
		dstBlend = d;
	}

	void	setTransform  ( const Transform2D& tr );
	bool	isTransparent () const;

	virtual	void	drawPoly ( View& view, const Polygon3D * poly ) const;
	virtual	void	drawMesh ( View& view, const Camera& camera, const Vector4D& color, 
		                       Fog * fog,  const Mesh3D * mesh ) const;

	static	MetaClass	classInstance;
};

#endif

