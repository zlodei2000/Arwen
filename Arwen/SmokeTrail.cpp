//
// Example of a particle system - SmokeTrail
//
// Author: Alex V. Boreskoff
//
// Last modified: 6/12/2002
//

#include	"SmokeTrail.h"
#include	"Pool.h"
#include	"Math1D.h"

struct	SmokeParticle : public Particle
{
	int		texNo;
	float	changedTexture;
};

MetaClass	SmokeTrail :: classInstance ( "SmokeTrail", &ParticleSystem :: classInstance );

SmokeTrail :: SmokeTrail ( const char * theName, const Vector3D& thePos, int particlesPerSecond, const Array& theTextures,
	                       const Vector3D& theVelocity, float theSize, float theLifeTime ) : ParticleSystem ( theName, thePos, particlesPerSecond ), textures ( theTextures )
{
	velocity        = theVelocity;
	emitterPos      = pos;
	lastUpdateTime  = -1;
	size            = theSize;
	lifeTime        = theLifeTime;
	randomness      = 0.01f;
	decaySpeed      = 0.25f;						// rate of diminishing of particle
	sizeSpeed       = 0.035f;						// rate of increasing of size of particle
	metaClass       = &classInstance;
}

int		SmokeTrail :: init ()
{
	particleSizeInBytes = sizeof ( SmokeParticle );

	ParticleSystem :: init ();

	return 1;
}

void	SmokeTrail :: update ( Controller * controller, float curTime )
{
	if ( lastUpdateTime < 0 )
		lastUpdateTime = curTime;

	float	delta = curTime - lastUpdateTime;

	ParticleSystem :: update ( controller, curTime );

	for ( SmokeParticle * cur = (SmokeParticle *) start; cur != NULL; )
	{
		cur -> prevPos         = cur -> pos;
		cur -> pos            += cur -> velocity * delta;
		cur -> color.w        -= decaySpeed * delta;
		cur -> size           += sizeSpeed * delta;
		cur -> lastUpdateTime  = curTime;
/*
		if ( curTime - cur -> changedTexture >= lifeTime / textures.getCount () )
			if ( cur -> texNo + 1 < textures.getCount () )
				cur -> texture  = (Texture *) textures.at ( cur -> texNo++ );
*/
		cur = (SmokeParticle *) cur -> next;
	}

	updateBoundingBox ();

	lastUpdateTime = curTime;
}

void	SmokeTrail :: createParticle ( float curTime )
{
	SmokeParticle * p = (SmokeParticle *) pool -> allocItem ();

	p -> color          = color;
	p -> lifeTime       = lifeTime;
	p -> mass           = 1;
	p -> energy         = 1;
	p -> pos            = emitterPos;
	p -> prevPos        = p -> pos;
	p -> size           = size;
	p -> texture        = (Texture *) textures.at ( 0 );
	p -> timeOfBirth    = curTime;
	p -> velocity       = Vector3D :: getRandomVector ( randomness );
	p -> lastUpdateTime = curTime;
	p -> texNo          = 0;
	p -> changedTexture = curTime;
	p -> next           = NULL;
	p -> prev           = NULL;

	insert ( p );
}
