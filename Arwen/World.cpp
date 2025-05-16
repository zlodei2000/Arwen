//
// Basic world model for subscene-based portal engine
//
// Author: Alex V. Boreskoff
//
// Last modified: 6/06/2003
//

#include	<windows.h>
#include	<gl/gl.h>
#include	<stdio.h>

#include	"ResourceManager.h"
#include    "World.h"
#include	"Camera.h"
#include	"Poly3D.h"
#include	"Controller.h"
#include	"View.h"
#include	"Light.h"
#include	"Vector2D.h"
#include	"Texture.h"
#include	"MutableData.h"
#include	"TgaEncoder.h"
#include	"Application.h"
#include	"Mapping.h"
#include	"FileUtils.h"
#include	"Log.h"

#define	STEP		16								// step used for building lightmaps
#define	HALF_STEP	((STEP)/2)
#define	NUDGE_STEPS	6								// number of steps to "nudge" l/m sample on poly

Texture   * haloTexture          = NULL;
Texture   * lensFlareTexture [4] = { NULL, NULL, NULL, NULL };

MetaClass	World :: classInstance ( "World", &Model :: classInstance );
Vector3D	World :: ambient       ( 0.25f, 0.25f, 0.25f );
Texture	  * World :: detailTexture      = NULL;
float		World :: detailTextureScale = 50;
float		World :: detailDistance     = 3;
int			World :: maxMirrorDepth     = 5;

World :: World ( const char * theName ) : Model ( theName ), scenes ( "SubScenes" )
{
	currentSubScene = NULL;
	updateSize      = Vector3D ( 100, 100, 100 );
	lightmapDir     = "lightmaps";
	metaClass       = &classInstance;

	if ( haloTexture == NULL )
		haloTexture  = getTexture ( "../Textures/halo.bmp" );

	if ( lensFlareTexture [0] == NULL )
	{
		lensFlareTexture [0] = getTexture ( "../Textures/lens1.jpg" );
		lensFlareTexture [1] = getTexture ( "../Textures/lens2.jpg" );
		lensFlareTexture [2] = getTexture ( "../Textures/lens3.jpg" );
		lensFlareTexture [3] = getTexture ( "../Textures/lens4.jpg" );
	}
}

World :: ~World ()
{

}

bool	World :: addScene ( SubScene * scene )
{
    if ( getSubScene ( scene -> getName () ) )			// already exists
       return false;

    if ( !scene -> init () )
       return false;

	scene -> setOwner ( this );

    scenes.insertNoRetain ( scene );

	for ( Array :: Iterator it = scene -> getObjects ().getIterator (); !it.end (); ++it )
	{
		Light * light = dynamic_cast <Light *> ( it.value () );

		if ( light != NULL )
			lights.insert ( light );
	}

    return true;
}

Texture * World :: getTexture ( const String& theName ) const
{
	return Application :: instance -> getResourceManager () -> getTexture ( theName );
}

SubScene * World :: getSubScene ( const Vector3D& pos ) const
{
    for ( Array :: Iterator it = scenes.getIterator (); !it.end (); ++it )
    {
        SubScene * scene = (SubScene *) it.value ();

        if ( scene -> contains ( pos ) )
           return scene;
    }

    return NULL;
}

void	World :: render ( View& view, const Camera& camera )
{
	currentSubScene = getSubScene ( camera.getPos () );

	SubScene :: polysRendered = 0;

	Array	postObjects ( "PostProcessed Object List" );

	view.lock ();

    if ( currentSubScene != NULL )
		currentSubScene -> render ( view, camera, camera.getViewFrustrum (), postObjects );

	drawPostObjects ( view, camera, postObjects );

	view.unlock ();
}

void	World :: getColliders ( const BoundingBox& area, Array& colliders )
{
	colliders.removeAll ();

	if ( currentSubScene != NULL )
		currentSubScene -> collectColliders ( area, colliders );
}

void  World :: update ( Controller * controller, float systemTime )
{
	Vector3D	pos       ( controller -> getPos () );
	BoundingBox	updateBox ( pos - updateSize, pos + updateSize );

	for ( Array :: Iterator it = scenes.getIterator (); !it.end (); ++it )
	{
		SubScene * scene = (SubScene *) it.value ();

		if ( scene -> getBoundingBox ().intersects ( updateBox ) )
			scene -> update ( controller, systemTime );
	}
}

struct	FlareDesc
{
	int		index;				// texture index
	float	lengthScale;
	float	imageScale;
};

static	FlareDesc	flareTable [7] = 
{
	{ 0, 1,      1     },		// primary flare
	{ 1, 0.5f,   0.5f  },		// first halo
	{ 2, 0.33f,  0.25f },		// small birst
	{ 3, 0.125f, 1     },		// next halo
	{ 2, -0.5f,  0.5f, },		// next birst
	{ 3, -0.25f, 0.25f },		// next halo
	{ 2, -0.18f, 0.25f }		// next birst
};

void	World :: drawPostObjects ( View& view, const Camera& camera, const Array& post ) const
{
	float		haloSize  = 20;
	float		flareSize = 30;

											// save current state
	glPushAttrib ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_ENABLE_BIT | GL_STENCIL_BUFFER_BIT );
	glEnable     ( GL_BLEND );
	glDisable    ( GL_DEPTH_TEST );
	glDepthMask  ( GL_FALSE );

											// setup drawing mode
	view.startOrtho ();
	view.blendFunc  ( View :: bmSrcAlpha, View :: bmOne );

	for ( Array :: Iterator it = post.getIterator (); !it.end (); ++it )
	{
		Light * light = dynamic_cast <Light *> ( it.value () );

		if ( light == NULL )
			continue;
											// it's a light source, now
											// get screen coordinates of it
		Vector3D	loc ( camera.mapToScreen ( light -> getPos () ) );
		float		dist = camera.getPos ().distanceTo ( light -> getPos () );

											// draw the halo
		if ( light -> testFlag ( LF_HAS_HALO ) )
		{
			Vector4D	color ( light -> getColor () / ( 1 + 0.3f * dist ) );

			color.w = 0.4f;					// make it not too bright

			draw2dRect ( view, loc, Vector2D ( haloSize, haloSize ), color, haloTexture );
		}

		if ( light -> testFlag ( LF_HAS_LENS_FLARE ) )
		{
			Vector3D	c ( view.getWidth () * 0.5f, view.getHeight () * 0.5f, 1 );
			Vector3D	v ( c - loc );

			for ( int i = 0; i < sizeof ( flareTable ) / sizeof ( flareTable [0] ); i++ )
			{
				Vector3D	p     ( c + flareTable [i].lengthScale * v );
				Vector4D	color ( light -> getColor () );
				float		size  ( flareSize * flareTable [i].imageScale );

				color.w = 0.6f;					// make it not too bright

				draw2dRect ( view, p, Vector2D ( size, size ), color, lensFlareTexture [flareTable [i].index] );
			}
		}
	}

	view.endOrtho ();

	glPopAttrib ();
}

#define	EPS1	0.05f

bool	World :: pointVisibleFrom ( const Vector3D& from, const Vector3D& to ) const
{
	Ray			ray ( from, to - from );
	float		len ( from.distanceTo ( to ) );
	float		t;
	Vector3D	isect;
	BoundingBox	box;

	box.addVertex ( from );
	box.addVertex ( to   );
	
	for ( Array :: Iterator it = scenes.getIterator (); !it.end (); ++it )
	{
		SubScene * scene = (SubScene *) it.value ();

		if ( !box.intersects ( scene -> getBoundingBox () ) )
			continue;

		t = scene -> traceRay ( ray, len, isect, NULL );

		if ( t < len - EPS1 && t > EPS1 )
			return false;
	}

	return true;
}

String	World :: getLightmapNameForPoly ( const Polygon3D * poly ) const
{
	String	sceneName;

	if ( poly -> getOwner () != NULL )
		sceneName = poly -> getOwner () -> getName ();

	return lightmapDir + "/" + sceneName + "_" + poly -> getName () + ".tga";
}

bool	World :: buildSampleOnPoly ( const Polygon3D * poly, const Vector2D& srcTex, const Vector2D& mid, Vector3D& sample ) const
{
	float		xStep = (float) HALF_STEP / (float) poly -> getTexture () -> getWidth  ();
	float		yStep = (float) HALF_STEP / (float) poly -> getTexture () -> getHeight (); 
	Vector2D	tex ( srcTex );

	for ( int step = 0; step < NUDGE_STEPS; step++ )
	{
		sample = poly -> mapTextureToWorld ( tex );

		if ( poly -> contains ( sample ) )
			return true;

								// now try to "nudge" point towards polygon
		if ( step & 1 )
		{
			if ( tex.x > mid.x )
			{
				tex.x -= xStep;

				if ( tex.x < mid.x )
					tex.x = mid.x;
			}
			else
			{
				tex.x += xStep;

				if ( tex.x > mid.x )
					tex.x = mid.x;
			}
		}
		else
		{
			if ( tex.y > mid.y )
			{
				tex.y -= yStep;

				if ( tex.y < mid.y )
					tex.y = mid.y;
			}
			else
			{
				tex.y += yStep;

				if ( tex.y > mid.y )
					tex.y = mid.y;
			}
		}
	}

	return false;
}

bool	World :: buildLightmapForPoly ( const Polygon3D * poly )
{
	String	lightmapName ( getLightmapNameForPoly ( poly ) );

	(*sysLog) << "Building lightmap " << lightmapName << logEndl;

	if ( poly -> getMapping () == NULL || poly -> getTexture () == NULL )
		return true;

	if ( poly -> testFlag ( PF_PORTAL ) || poly -> testFlag ( PF_MIRROR ) )
		return true;

	int	texWidth  = poly -> getTexture () -> getWidth  ();
	int	texHeight = poly -> getTexture () -> getHeight ();

							// compute range of texture coordinates for poly
	Vector2D	texMin;
	Vector2D	texMax;

	poly -> getTextureExtent ( texMin, texMax );

							// find middle point
	Vector2D	mid  ( 0.5f * (texMin + texMax) );

	texMin.x *= texWidth;
	texMin.y *= texHeight;
	texMax.x *= texWidth;
	texMax.y *= texHeight;

							// get size of lightmap
	int	width  = (int)ceil ( texMax.x / STEP ) - (int)floor ( texMin.x / STEP ) + 1;
	int	height = (int)ceil ( texMax.y / STEP ) - (int)floor ( texMin.y / STEP ) + 1;

							// create texture object to hold lightmap with 24 bit format
	Vector2D	step      ( texMax - texMin );
	PixelFormat	rgbFormat ( 0xFF, 0xFF00, 0xFF0000 );
	Texture *   lightmap = new Texture ( lightmapName, width, height, rgbFormat );

	step.x /= width;
	step.y /= height;
							// create buffer to encode pixel data
	long * buf = new long [width];

	for ( int i = 0; i < height; i++ )
	{
		for ( int j = 0; j < width; j++ )
		{
			Vector2D	tex;
			Vector3D	sample;
			Vector3D	color ( ambient );

			tex.x = (texMin.x + j * step.x) / texWidth;
			tex.y = (texMin.y + i * step.y) / texHeight;

							// check whether we can nudge sample point to lay within poly
			if ( buildSampleOnPoly ( poly, tex, mid, sample ) )
			{

							// now sample is our sample point on poly,
							// accumulate light on it
				for ( Array :: Iterator it = lights.getIterator (); !it.end (); ++it )
				{
					Light * light = (Light *) it.value ();

					if ( poly -> isFrontFacing ( light -> getPos () ) )
						if ( pointVisibleFrom ( sample, light -> getPos () ) )
							color += light -> getLightAt ( sample, poly -> getNormal () );
				}
			}

							// clamp each component to [0,1]
			color.clamp ( 0, 1 );

							// now we have our light value in this point
							// write it to buffer
			buf [j] = rgbToInt ( (int)(255*color.z), (int)(255*color.y), (int)(255*color.x) );
		}

							// now put buf as a line in lightmap texture
		lightmap -> writeLine ( height - 1 - i, buf );
	}
							// write lightmap to file
	int			 size = width * height * 3;				// size of image in bytes
	MutableData	 data ( "lightmap", size + 1024 );		// placeholder for result
														// (add space for header)
	TgaEncoder	 encoder;

	encoder.encode     ( lightmap, &data );
	data.   saveToFile ( lightmapName );

														// free allocated resources
	delete [] buf;
	delete lightmap;

	return true;
}

bool	World :: buildAllLightmaps ()
{
	if ( !createDir ( lightmapDir ) )
		return false;

					// count all lightmaps
	int		numPolys = 0;
	int		curPoly  = 0;
	DWORD	count;
	char	buf [256];

	for ( Array :: Iterator scIt = scenes.getIterator (); !scIt.end (); ++scIt )
	{
		SubScene * scene = (SubScene *) scIt.value ();

		numPolys += scene -> getPolys ().getNumItems ();
	}

	AllocConsole ();

	sprintf_s      ( buf, "\nBuilding lightmaps for %d polys.\n", numPolys );
	WriteConsole ( GetStdHandle ( STD_OUTPUT_HANDLE ), buf, strlen ( buf ), &count, NULL );
	sprintf_s      ( buf, "Done: 0  %%" );
	WriteConsole ( GetStdHandle ( STD_OUTPUT_HANDLE ), buf, strlen ( buf ), &count, NULL );

	for (Array::Iterator scIt = scenes.getIterator (); !scIt.end (); ++scIt )
	{
		SubScene * scene = (SubScene *) scIt.value ();

		for ( Array :: Iterator it = scene -> getPolys ().getIterator (); !it.end (); ++it )
		{
			Polygon3D * poly = (Polygon3D *) it.value ();

			sprintf_s      ( buf, "\rDone: %3d %%", (100 * curPoly) / numPolys );
			WriteConsole ( GetStdHandle ( STD_OUTPUT_HANDLE ), buf, strlen ( buf ), &count, NULL );

			curPoly++;

			if ( !buildLightmapForPoly ( poly ) )
			{
				FreeConsole ();

				return false;
			}
		}
	}

	FreeConsole ();

	return true;
}

void	World :: draw2dRect ( View& view, const Vector3D& center, const Vector2D& size, const Vector4D& color, Texture * texture ) const
{
	if ( texture != NULL )
		view.bindTexture ( haloTexture );

	glBegin      ( GL_QUADS );
	glColor4fv   ( color );

	glTexCoord2f ( 0, 0 );
	glVertex2f   ( center.x - size.x, center.y - size.y );

	glTexCoord2f ( 0, 1 );
	glVertex2f   ( center.x - size.x, center.y + size.y );

	glTexCoord2f ( 1, 1 );
	glVertex2f   ( center.x + size.x, center.y + size.y );

	glTexCoord2f ( 1, 0 );
	glVertex2f   ( center.x + size.x, center.y - size.y );

	glEnd        ();
}
