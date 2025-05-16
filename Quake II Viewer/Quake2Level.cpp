//
// Basic functions to render Quake II level
//
// Author: Alex V. Boreskoff
//
// Last modified: 9/10/2002
//

#include	<windows.h>
#include	<gl/gl.h>
#include	<stdio.h>

#include	"Quake2Level.h"
#include	"BspFile.h"
#include	"Entities.h"
#include	"Camera.h"
#include	"Texture.h"
#include	"LightMap.h"
#include	"View.h"
#include	"Poly3D.h"
#include	"ResourceManager.h"
#include	"Sky.h"
#include	"Log.h"
#include	"Vector2D.h"
#include	"Data.h"
#include	"Application.h"
#include	"Mapping.h"

Quake2Level :: Quake2Level ( Data * data ) : Model ( data -> getName () )
{
	bool	posFound = false;

	data -> seekAbs ( 0 );

	file            = new BspFile ( data );
	numFaces        = file -> getNumFaces ();
	numClusters     = file -> getNumClusters ();
	clusterTable    = new long [numClusters];
	faceTable       = new long [numFaces];
	numVisibleFaces = 0;
	polys           = new Polygon3D * [numFaces];
	sky             = NULL;
	visPolys        = new Polygon3D * [numFaces];
	yaw   = 0;
	pitch = 0;
	roll  = 0;

	loadPolys ();

	memset ( clusterTable, '\xFF', numClusters * sizeof ( long ) );
	memset ( faceTable,    '\xFF', numFaces    * sizeof ( long ) );

/*
{
	MutableData	d ( "", strlen ( file -> entities ) );
	d.putBytes ( file -> entities, strlen ( file -> entities ) );
	d.saveToFile ( "entities" );
}
*/
	entities = new Entities ( file -> entities );

					// now get the location of player
	for ( int i = 0; i < entities -> getNumGroups (); i++ )
	{
		EntityGroup * group  = entities -> getGroup ( i );

		if ( group == NULL )
			continue;

		String	clsName = group -> getValue ( "classname" );

		if ( clsName == "info_player_coop" && !posFound )
		{
			String	posStr = group -> getValue ( "origin" );
			String	angStr = group -> getValue ( "angle"  );

			if ( sscanf ( posStr.c_str (), "%f %f %f", &pos.x, &pos.y, &pos.z ) < 3 )
				continue;

			if ( sscanf ( angStr.c_str (), "%f", &angle ) < 1 )
				continue;

			angle   *= M_PI / 180.0f;
			posFound = true;
		}
		else
		if ( clsName == "worldspawn" )
		{
			String	skyStr = group -> getValue ( "sky" );

			if ( skyStr != "" )
				sky = new Sky ( String ( "env/" ) + skyStr, ".tga" );
		}
	}

	yaw   = 0;
	pitch = 0;
	roll  = angle;
}

Quake2Level :: ~Quake2Level ()
{
	if ( sky != NULL )
		sky -> release ();

	for ( int i = 0; i < numFaces; i++ )
		if ( polys [i] != NULL )
			polys [i]-> release ();

	delete [] visPolys;
	delete    file;
	delete [] clusterTable;
	delete [] faceTable;
	delete [] polys;
}

int	Quake2Level :: init ()
{
	Model :: init ();

									// now upload lightmap textures
	for ( int i = 0; i < numFaces; i++ )
	{
		Lightmap * map = polys [i] -> getLightmap ();

		if ( map == NULL )
			continue;

		map -> getTexture () -> upload ();
	}

	return 1;
}

bool	Quake2Level :: loadPolys ()
{
	String	dir ( "textures/" );

	for ( int i = 0; i < numFaces; i++ )
	{
		Quake2Face     * face    = &file -> faces    [i];
		Quake2TexInfo  * texInfo = &file -> texInfos [face -> texInfo];
		Quake2BspPlane * qPlane  = &file -> planes   [face -> plane];
		String	         texName = texInfo -> texName;

		Texture * texture   = Application ::instance -> getResourceManager () -> getTexture ( dir + texName + ".wal" );
		float	  txtWidth  = (float)texture -> getWidth  ();
		float	  txtHeight = (float)texture -> getHeight ();

		Mapping	mapping ( texInfo -> uAxis / txtWidth, texInfo -> uOffset / txtWidth, texInfo -> vAxis / txtHeight, texInfo -> vOffset / txtHeight );

		polys [i] = new Polygon3D ( "", face -> numEdges );

		polys [i] -> setTexture ( texture );
		polys [i] -> setMapping ( mapping );

		for ( int j = 0; j < face -> numEdges; j++ )
		{
			int	edge = file -> faceEdges [face -> firstEdge + j];
			int	vertex;

			if ( edge < 0 )
				vertex = file -> edges [-edge].lastEdge;
			else
				vertex = file -> edges [edge].firstEdge;

			polys [i] -> addVertex ( file -> vertices [vertex] );
		}

		polys [i] -> init ();

		setLightmap ( face -> lightmapOffset, polys [i] );

			// we need to set our plane due to some glitches in original QII file
		Plane	plane ( qPlane -> normal.x, qPlane -> normal.y, qPlane -> normal.z, -qPlane -> dist );

		if ( face -> planeSide )
			plane.flip ();

		polys [i] -> setPlane ( plane );
	}

	return true;
}

bool	Quake2Level :: isLeafInFrustrum ( const Quake2BspLeaf * leaf, const Frustrum& frustrum ) const
{
	BoundingBox	box ( Vector3D ( leaf -> mins [0], leaf -> mins [1], leaf -> mins [2] ), 
			          Vector3D ( leaf -> maxs [0], leaf -> maxs [1], leaf -> maxs [2] ) );

	return frustrum.intersects ( box ) != 0;
}

Quake2BspLeaf * Quake2Level :: findLeaf ( const Vector3D& pos ) const
{
	int	index = file -> hulls [0].headNode;

	while ( index >= 0 )
	{
		Quake2BspNode  * node  = &file -> nodes [index];
		Quake2BspPlane * plane = &file -> planes [node  -> plane];

		if ( (plane -> normal & pos) >= plane -> dist )
			index = node -> frontChild;
		else
			index = node -> backChild;
	}

	index = -(index + 1);			// ~index

	if ( index < 0 || index >= file -> numLeaves )
		return NULL;

	return &file -> leaves [index];
}

void	Quake2Level :: buildClusterTable ( const Vector3D& pos )
{
	Quake2BspLeaf * leaf = findLeaf ( pos );

	if ( leaf == NULL || leaf -> cluster >= numClusters )
		return;

	long			offs = file -> getPvsOffset ( leaf -> cluster );
	unsigned char * pvs  = offs + (unsigned char *) file -> vis;

	for ( int i = 0, cl = 0; cl < numClusters; i++ )
		if ( pvs [i] == 0 )			// RLE'd zeros
			cl += 8 * pvs [++i];
		else
			for ( unsigned char bit = 1; bit != 0; bit *= 2, cl++ )
				if ( pvs [i] & bit )
					clusterTable [cl] = frameNo;
}

void	Quake2Level :: addFacesList ( int firstFace, int count, const Vector3D& pos )
{
	for ( int i = 0; i < count; i++ )
	{
		int	faceNo = file -> leafFaces [firstFace + i];			// get face no
																// add it, if it's a front face

		if ( !polys [faceNo] -> isOk () )						// check whether plane is defined
			continue;

		if ( polys [faceNo] -> isFrontFacing ( pos ) )
			if ( !isFaceMarked ( faceNo ) )						// if not already marked then mark
				markFace ( faceNo );
	}
}

void	Quake2Level :: buildFacesList ( const Camera& camera )
{
	Quake2BspLeaf * leaf = file -> leaves;

	for ( int i = 0; i < file -> numLeaves; i++, leaf++ )
		if ( clusterTable [leaf -> cluster] == frameNo )				// cluster is marked as visible
			if ( isLeafInFrustrum ( leaf, camera.getViewFrustrum () ) )	// if leaf is in frustrum then add its faces
				addFacesList ( leaf -> firstLeafFace, leaf -> numLeafFaces, camera.getPos () );

	numVisibleFaces = 0;
	drawSky         = false;

	for ( int i = 0; i < numFaces; i++ )
		if ( isFaceMarked ( i ) )
		{
			Quake2Face * face = &file -> faces [i];

			if ( file -> texInfos [face -> texInfo].flags & SURF_SKY )
				drawSky = true;
			else
				visPolys [numVisibleFaces++] = polys [i];
		}

}

void	Quake2Level :: drawFaces ( View& view, const Camera& camera )
{
	view.lock  ();
	view.apply ( camera );

									// draw polys
	for ( int i = 0; i < numVisibleFaces; i++ )
		view.draw ( *visPolys [i] );


									// draw lightmaps
	glPushAttrib ( GL_COLOR_BUFFER_BIT | GL_TEXTURE_BIT | GL_DEPTH_BUFFER_BIT );

	glDepthFunc  ( GL_LEQUAL );
	glDepthMask  ( GL_FALSE );
	glEnable     ( GL_BLEND );
	glBlendFunc  ( GL_ONE, GL_SRC_COLOR );
	glColor4f    ( 1, 1, 1, 1 );						// in case it has other value from prev calls

	for ( int i = 0; i < numVisibleFaces; i++ )
	{
		Polygon3D * poly = visPolys [i];
		Lightmap  * lightmap = poly -> getLightmap ();
		
		view.bindTexture ( lightmap -> getTexture () );
		view.simpleDraw  ( *poly, View :: useLightmap );
	}

	glPopAttrib ();

	if ( drawSky )
		sky -> draw ( camera );

	view.unlock ();
}

void	Quake2Level :: render ( View& view, const Camera& camera )
{
	view.apply ( camera );

	buildClusterTable ( camera.getPos () );					// build a list of visible clusters
	buildFacesList    ( camera );							// compute a list of all potentially visible faces
	drawFaces         ( view, camera );						// draw potentially visible faces

	frameNo++;
}

void	Quake2Level :: setLightmap ( int offset, Polygon3D * poly )
{
	Texture * polyTexture = poly -> getTexture ();

	if ( polyTexture == NULL )
		return;

	int	txtWidth  = polyTexture -> getWidth  ();
	int	txtHeight = polyTexture -> getHeight ();

	Vector2D uvMin, uvMax;				// normalized (0..1) texture extent

	poly -> getTextureExtent ( uvMin, uvMax );

										// extent in texture coordinates
	Vector2D	texMin ( uvMin.x * txtWidth, uvMin.y * txtHeight );
	Vector2D	texMax ( uvMax.x * txtWidth, uvMax.y * txtHeight );

										// compute size of lightmap based on texture extent
	int			width  = 1 + ((int)(ceil ( texMax.x / 16 ) - floor ( texMin.x / 16 )));
	int			height = 1 + ((int)(ceil ( texMax.y / 16 ) - floor ( texMin.y / 16 )));
	PixelFormat	rgbFormat ( 0xFF, 0xFF00, 0xFF0000 );
	Texture   * texture = new Texture ( "", width, height, rgbFormat );

	memcpy ( (void *)texture -> getData (), file -> getLightMap ( offset ), 3*width*height );

										// set 
	poly    -> setLightmap ( new Lightmap ( "", uvMin, uvMax, texture ) );
	texture -> release     ();
}

