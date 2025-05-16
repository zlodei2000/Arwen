//
// Example of a particle system - Fire
//
// Author: Alex V. Boreskoff
//
// Last modified: 19/11/2002
//


#include	"Fire.h"
#include	"3DDefs.h"
#include	"Pool.h"
#include	"Texture.h"
#include	"Math1D.h"

MetaClass	Fire :: classInstance ( "Fire", &ParticleSystem :: classInstance );

Fire :: Fire ( const char * theName, const Vector3D& thePos, int particlesPerSecond, float theLifeTime,
			   float radius, float theSpeed , Texture * theTexture, 
	           const Vector4D& color1, const Vector4D& color2 ) : ParticleSystem ( theName, thePos, particlesPerSecond )
{
	texture        = theTexture;
	startRadius    = radius;
	speed          = theSpeed;
	startColor     = color1;
	endColor       = color2;
	lastUpdateTime = -1;
	lifeTime       = theLifeTime;
	dispersion     = 0.01f;
	minSize        = 0.02f;
	maxSize        = 0.05f;
	metaClass      = &classInstance;

	texture -> retain ();
}

Fire :: ~Fire ()
{
	texture -> release ();
}

void	Fire :: update ( Controller * controller, float curTime )
{
	if ( lastUpdateTime < 0 )
		lastUpdateTime = curTime;

	float	delta = curTime - lastUpdateTime;

	ParticleSystem :: update ( controller, curTime );

	for ( Particle * cur = start; cur != NULL; )
	{
		cur -> prevPos        = cur -> pos;
		cur -> pos           += cur -> velocity * delta;
		cur -> velocity.x    *= 0.8f;
		cur -> velocity.z    *= 0.8f;
		cur -> lastUpdateTime = curTime;

		setParticleColor ( cur, curTime );

		cur = cur -> next;
	}

	updateBoundingBox ();

	lastUpdateTime = curTime;
}

void	Fire :: createParticle ( float curTime )
{
	Particle * p = (Particle *) pool -> allocItem ();

	p -> color          = startColor;
	p -> lifeTime       = lifeTime;
	p -> mass           = 1;
	p -> energy         = 1;
	p -> pos            = Vector3D :: getRandomVector ( startRadius );
	p -> prevPos        = p -> pos;
	p -> size           = rnd ( minSize, maxSize );
	p -> texture        = texture;
	p -> timeOfBirth    = curTime + lifeTime * (1 + rnd ()) * 0.2f;		// for some randomness in color
	p -> lastUpdateTime = curTime;

	if ( p -> pos.y < 0 )
		p -> pos.y = 0;

	p -> velocity = (Vector3D ( 0, 1, 0 ) + p -> pos * dispersion) * speed;

	insert ( p );
}
