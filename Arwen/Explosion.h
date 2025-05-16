//
// Class to show explosions a'la RTCW
//
// Author: Alex V. Boreskoff
//
// Last modified: 5/12/2002

#ifndef	__EXPLOSION__
#define	__EXPLOSION__

#include	"VisualObject.h"

class	Texture;

class	Explosion : public VisualObject
{
protected:
	Array		smokeTextures;			// used smoke texture
	Array		emitters;				// particle emitters, g.e. SmokeTrail objects
	float		lastUpdateTime;
	Texture   * texture;
	float		period;
	int			numEmitters;
	float		decaySpeed;
	float		sizeSpeed;
	float		randomness;
public:
	Explosion ( const char * name, const Vector3D& thePos, const Vector4D& theColor, Texture * theTexture, 
				float thePeriod, int emmiters, float theDecaySpeed, float theSizeSpeed, float theRandomness );
	~Explosion ();

	virtual	void	update ( Controller * controller, float curTime );
	virtual	void	draw   ( View& view, const Camera& camera, const Frustrum& frustrum, Fog * fog );

	static	MetaClass	classInstance;

protected:
	virtual	void	buildBoundingBox ();
};

#endif
