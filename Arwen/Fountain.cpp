//
// Example of a particle system - Fountain
//
// Author: Alex V. Boreskoff
//
// Last modified: 19/11/2002
//

#include	"Fountain.h"
#include	"Pool.h"
#include	"Math1D.h"

MetaClass	Fountain :: classInstance ( "Fountain", &ParticleSystem :: classInstance );

void	Fountain :: update ( Controller * controller, float curTime )
{
	if ( lastUpdateTime < 0.0f )
		lastUpdateTime = curTime;

	float	delta = curTime - lastUpdateTime;

	ParticleSystem :: update ( controller, curTime );

	for ( Particle * cur = start; cur != NULL; )
	{
		cur -> prevPos        = cur -> pos;
		cur -> pos           += cur -> velocity * delta;
		cur -> velocity      += gravity * delta;
		cur -> lastUpdateTime = curTime;

		cur = cur -> next;
	}

	updateBoundingBox ();

	lastUpdateTime = curTime;
}

void	Fountain :: createParticle ( float curTime )
{
	Particle * p = (Particle *) pool -> allocItem ();

	p -> color          = color;
	p -> lifeTime       = lifeTime + rnd () * 2.0f;
	p -> mass           = 1.0f;
	p -> energy         = 1;
	p -> pos            = Vector3D ( 0, 0, 0 );
	p -> size           = 0.05f + rnd () * 0.025f;
	p -> texture        = texture;
	p -> timeOfBirth    = curTime;
	p -> velocity       = shootDir + Vector3D ( rnd (), rnd (), rnd () ) * dispersion;
	p -> lastUpdateTime = curTime;

	insert ( p );
}
