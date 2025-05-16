//
// Example of a particle system - SmokeTrail
//
// Author: Alex V. Boreskoff
//
// Last modified: 6/12/2002
//

#ifndef	__SMOKE_TRAIL__
#define	__SMOKE_TRAIL__

#include	"ParticleSystem.h"
#include	"Array.h"

class	SmokeTrail : public ParticleSystem
{
private:
	float		lastUpdateTime;					// time the system was last updated or -1
	Vector3D	emitterPos;						// position of particle emitter
	Vector3D	velocity;						// velocity of emitter
	Array		textures;
	float		size;							// starting size
	float		lifeTime;
	float		randomness;						// scattering of particle off the route
	float		decaySpeed;						// speed of diminishing of alpha value
	float		sizeSpeed;						// speed of growing of particle size 
public:
	SmokeTrail ( const char * theName, const Vector3D& thePos, int particlesPerSecond, const Array& theTextures,
	             const Vector3D& theVelocity, float theSize, float theLifeTime );

	void	setRandomness ( float val )
	{
		randomness = val;
	}

	void	setDecaySpeed ( float val )
	{
		decaySpeed = val;
	}

	void	setSizeSpeed ( float val )
	{
		sizeSpeed = val;
	}

	const Vector3D&	getVelocity () const
	{
		return velocity;
	}

	void	setVelocity ( const Vector3D& newVelocity )
	{
		velocity = newVelocity;
	}

	const Vector3D&	getEmitter () const
	{
		return emitterPos;
	}

	void	setEmitter ( const Vector3D& newEmitterPos )
	{
		emitterPos = newEmitterPos;
	}

	virtual	int		init   ();
	virtual	void	update ( Controller * controller, float curTime );

	static	MetaClass	classInstance;

protected:
	virtual	void	createParticle ( float curTime );
};

#endif
