//
// Example of a particle system - Exploding (pulsarting) sphere
//
// Author: Alex V. Boreskoff
//
// Last modified: 19/11/2002
//

#include	"MagicSphere.h"
#include	"3DDefs.h"
#include	"Pool.h"
#include	"Texture.h"
#include	"Math1D.h"

struct	MyParticle : public Particle
{
	Vector3D	origPos;					// original position
};

MetaClass	MagicSphere :: classInstance ( "MagicSphere", &ParticleSystem :: classInstance );

MagicSphere :: MagicSphere ( const char * theName, const Vector3D& thePos, int theNumParticles, float theMinSize, float theMaxSize,
				             float r1, float r2, float theSpeed , Texture * theTexture, 
	                         const Vector4D& color1, const Vector4D& color2, bool thePulsate, float theRandomness ) : ParticleSystem ( theName, thePos, 0 )
{
	texture             = theTexture;
	startRadius         = r1;
	endRadius           = r2;
	speed               = theSpeed;
	startColor          = color1;
	endColor            = color2;
	pulsate             = thePulsate;
	lastUpdateTime      = -1;
	particleSizeInBytes = sizeof ( MyParticle );
	minSize             = theMinSize;
	maxSize             = theMaxSize;
	randomness          = theRandomness;
	metaClass           = &classInstance;

	texture -> retain ();

	init ();

	for ( int i = 0; i < theNumParticles; i++ )
		createParticle ( 0 );
}

MagicSphere :: ~MagicSphere ()
{
	texture -> release ();
}

void	MagicSphere :: update ( Controller * controller, float curTime )
{
	if ( !pulsate && curTime > 1 / speed )
	{
		boundingBox.reset ();				// make it invisible

		return;
	}

	if ( lastUpdateTime < 0.0f )
		lastUpdateTime = curTime;

	float	delta = curTime - lastUpdateTime;

	ParticleSystem :: update ( controller, curTime );

	float		t = 0.5f * ( 1 - (float) cos ( curTime * speed * M_PI ) );
	float		r = startRadius + (endRadius - startRadius) * t;
	Vector4D	color ( startColor * ( 1 - t ) + endColor * t );

	for ( MyParticle * cur = (MyParticle *)start; cur != NULL; )
	{
		float	delta = curTime - cur -> lastUpdateTime;

		if ( randomness > EPS )
		{
			cur -> origPos += Vector3D :: getRandomVector ( randomness );
			cur -> origPos.normalize ();
		}

		cur -> prevPos = cur -> pos;
		cur -> pos     = cur -> origPos * r;
		cur -> color   = color;

		cur = (MyParticle *)cur -> next;
	}

	updateBoundingBox ();

	lastUpdateTime = curTime;
}

void	MagicSphere :: createParticle ( float curTime )
{
	MyParticle * p = (MyParticle *) pool -> allocItem ();

	p -> color          = startColor;
	p -> lifeTime       = INFINITY;
	p -> mass           = 1;
	p -> energy         = 1;
	p -> pos            = Vector3D :: getRandomVector ( startRadius );
	p -> prevPos        = p -> pos;
	p -> size           = rnd ( minSize, maxSize );
	p -> texture        = texture;
	p -> timeOfBirth    = curTime;
	p -> velocity       = p -> pos;
	p -> lastUpdateTime = curTime;
	p -> origPos        = p -> pos;

	insert ( p );
}
