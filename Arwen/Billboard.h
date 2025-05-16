//
// Billboard class
//
// Author: Alex V. Boreskoff
//
// Last modified: 31/09/2002
//

#ifndef	__BILLBOARD__
#define	__BILLBOARD__

#include	"VisualObject.h"

class	Texture;

class	Billboard : public VisualObject
{
protected:
	Texture * texture;
	float	  width, height;

public:
	Billboard ( const char * theName, const Vector3D& thePos, const Vector4D& theColor, 
		        Texture * theTexture, float theWidth, float theHeight );

	~Billboard ();

	float	getWidth () const
	{
		return width;
	}

	float	getHeight () const
	{
		return height;
	}

	Texture * getTexture () const
	{
		return texture;
	}

	virtual	void	draw ( View& view, const Camera& camera, const Frustrum& frustrum, Fog * fog );

	static	MetaClass	classInstance;

protected:
	virtual	void	buildBoundingBox ();
};

#endif
