//
// Example of a particle system - Exploding (pulsarting) sphere
//
// Author: Alex V. Boreskoff
//
// Last modified: 13/11/2002
//

#ifndef	__MAGIC_SPHERE__
#define	__MAGIC_SPHERE__

#include	"ParticleSystem.h"

class	MagicSphere : public ParticleSystem
{
private:
	Texture   * texture;
	float		startRadius;
	float		endRadius;
	float		speed;
	float		minSize;
	float		maxSize;
	Vector4D	startColor;
	Vector4D	endColor;
	bool		pulsate;
	float		lastUpdateTime;
	float		randomness;
public:
	MagicSphere ( const char * theName, const Vector3D& thePos, int theNumParticles, 
		          float theMinSize, float theMaxSize, float r1, float r2, float theSpeed , 
				  Texture * theTexture, const Vector4D& color1, const Vector4D& color2, bool thePulsate = true, float theRandomness = 0 );

	~MagicSphere ();

	virtual	void	update ( Controller * controller, float curTime );

	static	MetaClass	classInstance;
	
protected:
	virtual	void	createParticle ( float curTime );
};

#endif
