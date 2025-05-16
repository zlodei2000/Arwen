//
// Example of a particle system - Snow/Rain
//
// Author: Alex V. Boreskoff
//
// Last modified: 28/08/2002
//

#ifndef	__SNOW__
#define	__SNOW__

#include	"ParticleSystem.h"
#include	"Plane.h"

class	Snow : public ParticleSystem
{
private:
	float		lastUpdateTime;					// time the system was last updated or -1
	Vector3D	velocity;
	Vector3D	wind;							// wind direction and speed
	float		windCoeff;
	Vector4D	color;
	Texture   * texture;
	float		size;
	float		lastEmitTime;
	float		radius;
	Plane       bottom;
public:
	Snow ( const char * theName, const Vector3D& thePos, int particlesPerSecond, Texture * theTexture, 
	           const Vector3D& theVelocity, const Vector3D& theWind, 
			   float theSize, float theRadius, const Plane& bottomPlane, const Vector4D& theColor ) : ParticleSystem ( theName, thePos, particlesPerSecond ), bottom ( bottomPlane )
	{
		velocity       = theVelocity;
		wind           = theWind;
		lastUpdateTime = -1.0f;
		lastEmitTime   = -1.0f;
		texture        = theTexture;
		size           = theSize;
		radius         = theRadius;
		windCoeff      = 0.05f;
		color          = theColor;
		metaClass      = &classInstance;

		init ();
	}

	virtual	int		init   ();
	virtual	void	update ( Controller * controller, float curTime );

	static	MetaClass	classInstance;

protected:
	virtual	void	createParticle ( float curTime );
};

#endif
