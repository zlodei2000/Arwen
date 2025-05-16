//
// Class to keep & render a 3d mesh
//
// Author: Alex V. Boreskoff
//
// Last modified: 14/11/2002
//

#include	<stdlib.h>						// for qsort
#include	<windows.h>						// for gl.h
#include	<gl/gl.h>

#include	"Mesh3D.h"
#include	"Vector2D.h"
#include	"Vector3D.h"
#include	"Vector4D.h"
#include	"Texture.h"
#include	"View.h"
#include	"Camera.h"
#include	"Fog.h"

MetaClass	Mesh3D :: classInstance ( "Mesh3D", &Object :: classInstance );

Mesh3D :: Mesh3D ( const char * theName ) : Object ( theName )
{
	maxVertices = 0;
	numVertices = 0;
	numFaces    = 0;
	vertices    = NULL;
	normals     = NULL;
	texCoords   = NULL;
	colors      = NULL;
	faces       = NULL;
	texture     = NULL;
	indices     = NULL;
	smooth      = true;
	metaClass   = &classInstance;
}

Mesh3D :: Mesh3D  ( const char * theName, 
				    Vector3D * theVertices,  Vector3D * theNormals, int theNumVertices,
		            Vector2D * theTexCoords, int theNumTexCoords,
			        Face     * theFaces,     int theNumFaces,
					Texture  * theTexture,   Vector4D * theColors ) : Object ( theName )
{
	metaClass    = &classInstance;
	maxVertices  = theNumVertices;
	numVertices  = theNumVertices;
	numTexCoords = theNumTexCoords;
	numFaces     = theNumFaces;
	texture      = theTexture;
	smooth       = 0;;
	centers      = NULL;
	tempColors   = NULL;
	cachedColor  = Vector4D ( -1, -1, -1, -1 );

	if ( texture != NULL )
		texture -> retain ();

	if ( theVertices != NULL )
		vertices = new Vector3D [numVertices];
	else
		vertices = NULL;

	if ( theNormals != NULL )
		normals = new Vector3D [numVertices];
	else
		normals = NULL;

	if ( theTexCoords != NULL )
		texCoords = new Vector2D [numVertices];
	else
		texCoords = NULL;

	if ( theColors != NULL )
		colors = new Vector4D [numVertices];
	else
		colors = NULL;

	if ( theFaces != NULL )
		faces = new FaceVa [numFaces];
	else
		faces = NULL;

	if ( theVertices != NULL )
		memcpy ( vertices, theVertices, numVertices * sizeof ( Vector3D ) );

	if ( theNormals != NULL )
		memcpy ( normals, theNormals, numVertices * sizeof ( Vector3D ) );

	if ( theColors != NULL )
		memcpy ( colors, theColors, numVertices * sizeof ( Vector4D ) );

//	if ( theTexCoords != NULL )
//		memcpy ( texCoords, theTexCoords, numTexCoords * sizeof ( Vector2D ) );

	if ( faces != NULL )
		rebuildFaces ( theFaces, theTexCoords, theNumTexCoords );

	indices = new FaceOrderingInfo [numFaces];
	centers = new Vector3D         [numFaces];

	if ( colors != NULL )
		tempColors = new Vector4D [numVertices];
			
	for ( int i = 0; i < numFaces; i++ )		// check each face in turn
	{
		centers [i] = Vector3D ( 0, 0, 0 );

		for ( int j = 0; j < 3; j++ )			// process each vertex in turn
			centers [i] += vertices [theFaces [i].vertexIndex [j]];

		centers [i] /= 3;
	}

	buildBoundingBox ();
}

Mesh3D :: ~Mesh3D ()
{
	delete vertices;
	delete normals;
	delete texCoords;
	delete colors;
	delete tempColors;
	delete faces;
	delete indices;
	delete centers;

	texture -> release ();
}

void	Mesh3D :: setTexture ( Texture * theTexture )
{
	if ( texture == theTexture )
		return;

	if ( texture != NULL )
		texture -> release ();

	texture = theTexture;

	if ( texture != NULL )
		texture -> retain ();
}

void	Mesh3D :: draw ( View& view, const Camera& camera, const Vector4D& color, 
						 Fog * fog, Texture * txt, bool transparent ) const
{
	if ( !isOk () )
		return;

	int		   shadeModel;
	Vector4D * colorArray = colors;

	if ( txt != NULL )
		view.bindTexture ( txt );
	else
		view.bindTexture ( texture );

	glGetIntegerv ( GL_SHADE_MODEL, &shadeModel );
	glColor4fv    ( color );

	if ( smooth )
		glShadeModel  ( GL_SMOOTH );
	else
		glShadeModel ( GL_FLAT );

#define	USE_VERTEX_ARRAYS
#ifdef	USE_VERTEX_ARRAYS

										// check whether we need to update tempColors array
	if ( colors != NULL && ( color.x !=1 || color.y != 1 || color.z != -1 || color.w != 1 || color != cachedColor ) )
	{
		for ( int i = 0; i < numVertices; i++ )
			tempColors [i] = color * colors [i];

		cachedColor = color;
		colorArray  = tempColors;
	}

	if ( fog != NULL )					// compute fogging for vertices, using tempColors array
	{
		if ( tempColors == NULL )
			tempColors = new Vector4D [2*numVertices];

		for ( int i = 0;i < numVertices; i++ )
			tempColors [i] = color * fog -> getBlendColor ( camera.getPos (), vertices [i], NULL );

		cachedColor = Vector4D ( -1, -1, -1, -1 );
		colorArray  = tempColors;
	}

	glEnableClientState ( GL_VERTEX_ARRAY );
	glVertexPointer     ( 3, GL_FLOAT, 0, vertices );

	if ( colorArray != NULL )
	{
		glColorPointer      ( 4, GL_FLOAT, 0, colorArray ); 
		glEnableClientState ( GL_COLOR_ARRAY );
	}
	else
		glDisableClientState ( GL_COLOR_ARRAY );

	if ( normals != NULL )
	{
		glNormalPointer     ( GL_FLOAT, 0, normals );
		glEnableClientState ( GL_NORMAL_ARRAY );
	}
	else
		glDisableClientState ( GL_NORMAL_ARRAY );

	if ( texCoords != NULL )
	{
		glTexCoordPointer   ( 2 ,GL_FLOAT, 0, texCoords );
		glEnableClientState ( GL_TEXTURE_COORD_ARRAY );
	}
	else
		glDisableClientState ( GL_TEXTURE_COORD_ARRAY );

	if ( transparent )
	{
											// collect keys for sorting
		for ( int i = 0; i < numFaces; i++ )
		{
			indices [i].index = i;
			indices [i].key   = centers [i] & camera.getViewDir ();
		}

											// sort faces in back-to-fron order
		qsort ( indices, numFaces, sizeof ( FaceOrderingInfo ), compFunc );

											// draw them
		glDepthMask ( GL_FALSE );
		glBegin     ( GL_TRIANGLES );

		for ( int i = 0; i < numFaces; i++ )
		{
			int	index = indices [i].index;

			for ( int j = 0; j < 3; j++ )
				glArrayElement ( faces [index].index [j] );
		}

		glEnd       ();
		glDepthMask ( GL_TRUE );
	}
	else
		glDrawElements ( GL_TRIANGLES, 3*numFaces, GL_UNSIGNED_INT, faces  );

	glDisableClientState ( GL_VERTEX_ARRAY        );
	glDisableClientState ( GL_COLOR_ARRAY         );
	glDisableClientState ( GL_NORMAL_ARRAY        );
	glDisableClientState ( GL_TEXTURE_COORD_ARRAY );

#else
	glBegin ( GL_TRIANGLES );

									// process each face in turn
	for ( int i = 0; i < numFaces; i++ )
	{
									// process each vertex
		for ( int j = 0; j < 3; j++ )
		{
			int	vi = faces [i].index   [j];

			if ( normals != NULL )
				glNormal3fv ( normals [vi] );

			if ( colors != NULL )
				glColor4fv ( colors [vi] );

			if ( texCoords != NULL )
				glTexCoord2fv ( texCoords [vi] );

			glVertex3fv ( vertices [vi] );
		}
	}

	glEnd ();
#endif

	glShadeModel ( shadeModel );
}

void	Mesh3D :: computeNormals ()
{
	Vector3D normal, v [3];

	if ( normals != NULL )						// reallocate normals
		delete normals;

	normals = new Vector3D [numVertices];

												// face normals
	Vector3D * tempNormals = new Vector3D [numFaces];
	
												// process all faces of mesh

	for ( int i = 0; i < numFaces; i++ )
	{
												// extract the 3 points of this face
		v [0] = vertices [faces [i].index [0]];
		v [1] = vertices [faces [i].index [1]];
		v [2] = vertices [faces [i].index [2]];

												// compute face normal
		normal          = (v [2] - v [0]) ^ (v [2] - v [1]);
		tempNormals [i] = normal.normalize ();

	}

												// now compute vertex normals

	Vector3D sum  ( 0, 0, 0 );
	Vector3D zero ( sum );

	for ( int i = 0; i < numVertices; i++ )			// process each vertex in turn
	{
		for ( int j = 0; j < numFaces; j++ )	// pocess each face
		{										// check if the vertex is shared by another face
			if ( faces [j].index [0] == i || faces [j].index [1] == i || faces [j].index [2] == i )
				sum += tempNormals [j];			// add the normal of the shared face
		}      

		normals [i] = sum.normalize ();
		sum         = zero;						// reset the sum
	}

	delete [] tempNormals;							// free face normals
}

void	Mesh3D :: buildBoundingBox ()
{
	boundingBox.reset ();
	boundingBox.addVertices ( vertices, numVertices );
}

void	Mesh3D :: rebuildFaces ( Face * theFaces, Vector2D * theTexCoords, int theNumTexCords )
{
	bool * flags = new bool [numVertices];

	memset ( flags, '\0', numVertices * sizeof ( bool ) );

	for ( int i = 0; i < numFaces; i++ )		// check each face in turn
	{
		for ( int j = 0; j < 3; j++ )			// process each vertex in turn
		{
			int	vi = theFaces [i].vertexIndex   [j];
			int	ti = theFaces [i].texCoordIndex [j];

			if ( flags [vi] )					// vertex already has tex coords assigned
			{									// check whether we're trying to assign 
												// new tex coords
				if ( texCoords [vi] != theTexCoords [ti] )
				{
												// duplicate vertex
					appendVertex ( vi );

					vi = numVertices - 1;
				}
			}
			else
				flags [vi] = true;				// mark vertex as assigned

			texCoords [vi]          = theTexCoords [ti];
			faces     [i].index [j] = vi;
		}
	}

	delete [] flags;
}

template <typename T>
void	reallocate ( T *& ptr, int count, int max )
{
	if ( ptr == NULL )
		return;

	T * newPtr = new T [max];

	memcpy ( newPtr, ptr, count * sizeof ( T ) );

	delete ptr;

	ptr = newPtr;
}

void	Mesh3D :: appendVertex ( int from )
{
	if ( from < 0 || from >= numVertices )
		return;

	if ( numVertices >= maxVertices )
	{
		maxVertices += 100;

		reallocate ( vertices,  numVertices, maxVertices );
		reallocate ( normals,   numVertices, maxVertices );
		reallocate ( colors,    numVertices, maxVertices );
		reallocate ( texCoords, numVertices, maxVertices );
	}

	vertices [numVertices] = vertices [from];

	if ( normals != NULL )
		normals [numVertices] = normals [from];

	if ( colors != NULL )
		colors [numVertices] = colors [from];

	if ( texCoords != NULL )
		texCoords [numVertices] = texCoords [from];

	numVertices++;
}

int __cdecl Mesh3D :: compFunc ( const void * elem1, const void * elem2 )
{
	float	z1 = ((FaceOrderingInfo *) elem1) -> key;
	float	z2 = ((FaceOrderingInfo *) elem2) -> key;

	if ( z1 < z2 )
		return 1;
	else
	if ( z1 > z2 )
		return -1;

	return 0;
}

