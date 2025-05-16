//
// Example of a particle system - Fire
//
// Author: Alex V. Boreskoff
//
// Last modified: 19/11/2002
//

#ifndef	__FIRE__
#define	__FIRE__

#include	"ParticleSystem.h"

class	Fire : public ParticleSystem
{
private:
	Texture   * texture;
	float		startRadius;
	float		speed;
	Vector4D	startColor;
	Vector4D	endColor;
	float		lastUpdateTime;
	float		lifeTime;
	float		dispersion;
	float		minSize;
	float		maxSize;
public:
	Fire ( const char * theName, const Vector3D& thePos, int particlesPerSecond, float theLifeTime,
		   float radius, float theSpeed , Texture * theTexture, 
	       const Vector4D& color1, const Vector4D& color2 );

	~Fire ();

	virtual	void	update ( Controller * controller, float curTime );

	static	MetaClass	classInstance;

protected:
	virtual	void	createParticle ( float curTime );

	void	setParticleColor ( Particle * cur, float curTime )
	{
		float		t = (curTime - cur -> timeOfBirth) / lifeTime;

		cur -> color = startColor * ( 1 - t ) + endColor * t;
	}
};

#endif
