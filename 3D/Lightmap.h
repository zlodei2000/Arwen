#ifndef __LIGHT_MAP__
#define __LIGHT_MAP__

#include	"Object.h"
#include	"Vector2D.h"

#define	LM_DELTA	0.01f				// some delta to ensure remap will get values in [0,1]

class	Texture;

class   Lightmap : public Object
{
private:
	Vector2D  offs;
	Vector2D  scale;
	float	  matrix [4][4];			// OpenGL texture transform matrix
	Texture * map;						// the map itself

public:
	Lightmap  ( const char * theName, const Vector2D& texMin, const Vector2D& texMax, Texture * txt );
	~Lightmap ();
										// remap normal texture coordinates into normalized
										// lightmap coordinates
	Vector2D	remap ( const Vector2D& uv ) const
	{
		return (uv - offs) * scale + Vector2D ( LM_DELTA, LM_DELTA );
	}

	Texture * getTexture () const
	{
		return map;
	}

	const float * getTextureMatrix () const
	{
		return (const float *) matrix;
	}

	static	MetaClass	classInstance;
};

#endif
