//
// Basic class for particle systems
//.
// Author: Alex V. Boreskoff
//
// Last modified: 4/06/2003
//

#include	<windows.h>
#include	<gl/gl.h>

#include	"ParticleSystem.h"
#include	"Pool.h"
#include	"Texture.h"
#include	"View.h"

MetaClass	ParticleSystem :: classInstance ( "ParticleSystem", &VisualObject :: classInstance );

ParticleSystem :: ParticleSystem ( const char * theName, const Vector3D& thePos, int particlesPerSecond ) : VisualObject ( theName, thePos )
{
	particleSizeInBytes = sizeof ( Particle );
	pool                = NULL;
	start               = NULL;
	numParticles        = 0;
	birthPeriod         = particlesPerSecond > 0 ? 1.0f / (float)particlesPerSecond : INFINITY;
	lastCreationTime    = 0;
	srcBlendingMode     = View :: bmSrcAlpha;
	dstBlendingMode     = View :: bmOne;
	saveFlags          |= GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;
	metaClass           = &classInstance;
}

ParticleSystem :: ~ParticleSystem ()
{
	pool -> freeAll ();

	delete pool;
}

int	ParticleSystem :: init ()
{
	if ( pool == NULL )
		pool = new MemoryPool ( particleSizeInBytes, 1000 );

	return 1;
}

void	ParticleSystem :: update ( Controller * controller, float curTime )
{
	VisualObject :: update ( controller, curTime );

												// remove dead particles
	for ( register Particle * cur = start; cur != NULL; )
	{
		register Particle * next = cur -> next;	// save next particle (if cur will be deleted)

		if ( !cur -> isAlive ( curTime ) )
			remove ( cur );

		cur = next;
	}

	if ( birthPeriod < 0 )						// for systems where all particles are created at start
		return;

	//float	delta = curTime - lastCreationTime;

												// create new particles
	if ( lastCreationTime + birthPeriod < curTime )
	{
		for ( float time = lastCreationTime; time + birthPeriod < curTime; time += birthPeriod )
			createParticle ( time );

		lastCreationTime = curTime;
	}
}

void	ParticleSystem :: doDraw ( View& view, const Camera& camera, const Frustrum& frustrum, Fog * fog )
{
	glDepthMask  ( GL_FALSE );

	view.blendFunc ( srcBlendingMode, dstBlendingMode );

	for ( Particle * cur = start; cur != NULL; cur = cur -> next )
		view.drawBillboard ( cur -> pos, cur -> size, color * cur -> color, cur -> texture );

	glDepthMask ( GL_TRUE );
}

void	ParticleSystem :: updateBoundingBox ()
{
	boundingBox.reset ();

	for ( register Particle * cur = start; cur != NULL; cur = cur -> next )
		boundingBox.addVertex ( cur -> pos );

	if ( start != NULL )
		boundingBox.move ( pos );
}

void	ParticleSystem :: insert ( Particle * p )
{
	p -> next = start;
	p -> prev = NULL;

	if ( start != NULL )
		start -> prev = p;

	start = p;

	numParticles++;
}

void	ParticleSystem :: remove ( Particle * p )
{
	if ( p -> prev != NULL )
		p -> prev -> next = p -> next;
	else
		start = p -> next;

	if ( p -> next != NULL )
		p -> next -> prev = p -> prev;

	pool -> freeItem ( p );

	numParticles--;
}
