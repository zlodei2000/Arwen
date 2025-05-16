//
// Example of a particle system - Snow
//
// Author: Alex V. Boreskoff
//
// Last modified: 19/11/2002
//

#include	<stdlib.h>						// for rand

#include	"Snow.h"
#include	"Pool.h"
#include	"Math1D.h"

MetaClass	Snow :: classInstance ( "Snow", &ParticleSystem :: classInstance );

int		Snow :: init ()
{
	return ParticleSystem :: init ();
}

void	Snow :: update ( Controller * controller, float curTime )
{
	if ( lastUpdateTime < 0 )
		lastUpdateTime = curTime;

	float	delta = curTime - lastUpdateTime;

	ParticleSystem :: update ( controller, curTime );

	for ( register Particle * cur = start; cur != NULL; )
	{
		cur -> prevPos   = cur -> pos;
		cur -> pos      += cur -> velocity * delta;
		cur              = cur -> next;
	}

	updateBoundingBox ();

	lastUpdateTime = curTime;
}

void	Snow :: createParticle ( float curTime )
{
	Particle * p = (Particle *) pool -> allocItem ();

	p -> color          = color;
	p -> lifeTime       = 0;						// have no meaning here
	p -> mass           = 1;
	p -> energy         = 1;
	p -> pos            = Vector3D ::getRandomVector ( radius );
	p -> prevPos        = p -> pos;
	p -> size           = size;
	p -> texture        = texture;
	p -> timeOfBirth    = curTime;
	p -> velocity       = velocity;
	p -> lastUpdateTime = curTime;
	p -> lifeTime       = 1e10;						// INFINITY

	insert ( p );
}
