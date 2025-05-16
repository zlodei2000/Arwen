//
// Class to show explosions a'la RTCW
//
// Author: Alex V. Boreskoff
//
// Last modified: 6/12/2002

#include	"Explosion.h"
#include	"SmokeTrail.h"
#include	"Controller.h"
#include	"ObjStr.h"
#include	"Texture.h"
#include	"ResourceManager.h"
#include	"View.h"

MetaClass	Explosion :: classInstance ( "Explosion", &VisualObject :: classInstance );

Explosion :: Explosion ( const char * theName, const Vector3D& thePos, const Vector4D& theColor, Texture * theTexture, 
						 float thePeriod, int emiters, float theDecaySpeed, float theSizeSpeed, float theRandomness ) :
	VisualObject ( theName, thePos, theColor )
{
	lastUpdateTime = -1;
	texture        = theTexture;
	period         = thePeriod;
	numEmitters    = emiters;
	decaySpeed     = theDecaySpeed;
	sizeSpeed      = theSizeSpeed;
	randomness     = theRandomness;

	if ( texture != NULL )
	{
		texture -> retain ();

		smokeTextures.insert ( texture );
	}
}

Explosion :: ~Explosion ()
{
	if ( texture != NULL )
		texture -> release ();
}

void	Explosion :: update ( Controller * controller, float curTime )
{
	VisualObject :: update ( controller, curTime );

	if ( lastUpdateTime < 0 )
		lastUpdateTime = curTime;

	float	delta = curTime - lastUpdateTime;			// interval since last update
	float	time  = (float)fmod ( curTime, 20 );		// works in periods of 20 sec

	lastUpdateTime = curTime;

	if ( time >= period - 5 )
	{
		emitters.removeAll ();

		return;
	}

	if ( emitters.isEmpty () )							// time to create emitters
	{
		for ( int i = 0; i < numEmitters; i++ )
		{
			SmokeTrail * emitter = new SmokeTrail ( String :: printf ( "trail%02d", i ),
												    pos, 15, smokeTextures, 
				                                    Vector3D :: getRandomVector ( 0.1f ), 0.025f, 7 );

			emitter -> init            ();
			emitter -> setBlendingMode ( View :: bmSrcAlpha, View :: bmOneMinusSrcAlpha );
			emitter -> setColor        ( color );
			emitter -> setDecaySpeed   ( decaySpeed );
			emitter -> setSizeSpeed    ( sizeSpeed );
			emitter -> setRandomness   ( randomness );

			emitters.insert ( emitter );
		}
	}

	for ( Array :: Iterator it = emitters.getIterator (); !it.end (); ++it )
	{
		SmokeTrail * object = (SmokeTrail *) it.value ();

		object -> setEmitter  ( object -> getEmitter  () + object     -> getVelocity () * delta );
		object -> setVelocity ( object -> getVelocity () + controller -> getGravity  () * delta * 0.25f );
		object -> update      ( controller, curTime );
	}
}

void	Explosion :: draw   ( View& view, const Camera& camera, const Frustrum& frustrum, Fog * fog )
{
	for ( Array :: Iterator it = emitters.getIterator (); !it.end (); ++it )
	{
		VisualObject * object = (VisualObject *) it.value ();

		object -> draw ( view, camera, frustrum, fog );
	}
}

void	Explosion :: buildBoundingBox ()
{
	boundingBox.reset ();

	for ( Array :: Iterator it = emitters.getIterator (); !it.end (); ++it )
	{
		VisualObject * object = (VisualObject *) it.value ();

		boundingBox.merge ( object -> getBoundingBox () );
	}
}

