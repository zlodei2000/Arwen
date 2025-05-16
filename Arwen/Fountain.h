//
// Example of a particle system - Fountain
//
// Author: Alex V. Boreskoff
//
// Last modified: 1/08/2002
//

#ifndef	__FOUNTAIN__
#define	__FOUNTAIN__

#include	"ParticleSystem.h"

class	Fountain : public ParticleSystem
{
private:
	float		lastUpdateTime;					// time the system was last updated or -1
	Vector3D	shootDir;						// shooting direction
	Vector3D	gravity;						// gravity acceleration vector
	Vector3D	wind;							// wind direction and speed
	float		dispersion;						// measure of randomness
	float		lifeTime;
	Texture   * texture;
	Vector4D	color;
public:
	Fountain ( const char * theName, const Vector3D& thePos, int particlesPerSecond, Texture * theTexture, 
	           const Vector3D& theShootDir, const Vector3D& theGravity,
               const Vector3D& theWind, 
			   float theDispersion, float theLifeTime, const Vector4D& theColor ) : ParticleSystem ( theName, thePos, particlesPerSecond )
	{
		shootDir       = theShootDir;
		gravity        = theGravity;
		wind           = theWind;
		dispersion     = theDispersion;
		lifeTime       = theLifeTime;
		lastUpdateTime = -1.0f;
		texture        = theTexture;
		color          = theColor;
		metaClass      = &classInstance;
	}

	virtual	void	update ( Controller * controller, float curTime );

	static	MetaClass	classInstance;

protected:
	virtual	void	createParticle ( float curTime );
};

#endif
