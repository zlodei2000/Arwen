//
// Basic class for particle systems
//.
// Author: Alex V. Boreskoff
//
// Last modified: 4/06/2003
//

#ifndef	__PARTICLE_SYSTEM__
#define	__PARTICLE_SYSTEM__

#include	"VisualObject.h"

class	MemoryPool;
class	Texture;

struct	Particle
{
	Particle  * next;						// next particle in the list
	Particle  * prev;						// previous particle in the list

											// Physical & visual parameters of the particle:

	Vector3D	pos;						// it's position
	Vector3D	prevPos;					// position for previous frame
	Vector3D	velocity;					// it's velocity
	float		mass;						// it's mass
	Vector4D	color;						// it's color
	float		size;						// it's size
	float		energy;
	Texture   * texture;
	float		timeOfBirth;				// time the particle was born
	float		lifeTime;					// time the particle will live
	float		lastUpdateTime;				// time the particle was last updated

	bool	isAlive ( float curTime ) const	// check whether the particle should be killed
	{
		return (timeOfBirth + lifeTime >= curTime) && (size >= EPS) && (energy >= EPS);
	}

	void	kill ()							// force particle to be killed during next update
	{
		lifeTime = -1;
	}
};

class	ParticleSystem : public VisualObject
{
protected:
	int		     particleSizeInBytes;
	MemoryPool * pool;						// pool used to allocate/free particles
	Particle   * start;						// first particle in the list
	int			 numParticles;
	float		 lastCreationTime;			// time the last particle was created
	float		 birthPeriod;				// period of particle creation
	int			 srcBlendingMode;			// OpenGL blending modes
	int			 dstBlendingMode;

public:
	ParticleSystem ( const char * theName, const Vector3D& thePos, int particlesPerSecond );
	~ParticleSystem ();

	virtual	int	init ();

	virtual	void	update ( Controller * controller, float curTime );

	virtual	bool	isTransparent () const
	{
		return true;						// assume all particle systems are semitransparent
	}

	void	insert ( Particle * );
	void	remove ( Particle * );

	void	setBlendingMode ( int src, int dst )
	{
		srcBlendingMode = src;
		dstBlendingMode = dst;
	}

	int	getNumParticles () const
	{
		return numParticles;
	}

	static	MetaClass	classInstance;

protected:
	virtual	void	doDraw            ( View& view, const Camera& camera, const Frustrum& frustrum, Fog * fog );
	virtual	void	createParticle    ( float curTime ) = 0;
	void			updateBoundingBox ();
};

#endif
