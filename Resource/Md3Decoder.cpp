//
// Class to load Quake III .md3 models
//
// Author: Alex V. Boreskoff
//
// Last modified: 1/04/2003
//

#include	"Md3Decoder.h"
#include	"ObjStr.h"
#include	"Mesh3D.h"
#include	"Vector2D.h"
#include	"Vector3D.h"
#include	"Array.h"
#include	"Data.h"
#include	"MeshObject.h"
#include	"CompositeObject.h"
#include	"ResourceManager.h"
										// md3 file structures
#pragma pack (push, 1)

struct	Md3Header						// header of .md3 file
{
	char	id [4];						// should be "IDP3"
	int		version;					// should be 15
	char	fileName [68];
	int		numBoneFrames;
	int		numTags;
	int		numMeshes;
	int		numMaxSkins;
	int		headerLength;
	int		tagStart;
	int		tagEnd;
	int		fileSize;
};

struct	Md3BoneFrame
{
	float	mins [3];
	float	maxs [3];
	float	pos  [3];
	float	scale;
	char	creator [16];
};

struct	Md3Tag
{
	char	name [64];
	float	pos [3];
	float	rot [3][3];
};

struct	Md3MeshHeader
{
	char	id [4];
	char	name [68];
	int		numMeshFrames;
	int		numSkins;
	int		numVertices;
	int		numTriangles;
	int		triStart;
	int		skinsStart;
	int		texVectorStart;
	int		vertexStart;
	int		meshSize;
};

struct	Md3Skin
{
	char	name [64];
	int		index;
};

struct	Md3Triangle
{
	int	vertex [3];
};

struct	Md3TexCoord
{
	float	coord [2];
};

struct	Md3Vertex
{
	short			vertex [3];
	unsigned char	normal [2];
};

#pragma	pack (pop)

MetaClass	Md3Decoder :: classInstance ( "Md3Decoder", &ResourceDecoder :: classInstance );

//////////////////////// methods ///////////////////////////////////

Md3Decoder :: Md3Decoder ( ResourceManager * rm ) : ResourceDecoder ( "Md3Loader", rm ) 
{
	metaClass = &classInstance;

	for ( int i = 0; i < 256; i++ )
		for ( int j = 0; j < 256; j++ )
		{
			float	alpha = 2.0f * M_PI * i / 256.0f;
			float	beta  = 2.0f * M_PI * j / 256.0f;

			normalTable [i][j] = Vector3D ( (float)(cos (beta) * sin (alpha)),
											(float)(sin (beta) * sin (alpha)),
											(float) cos (alpha) );
		}
}

bool	Md3Decoder :: checkExtension ( const String& theName )
{
	return String :: getExtension ( theName ).toLower () == "md3";
}

Object * Md3Decoder :: decode ( Data * data )
{
	Md3Header * hdr = (Md3Header *) data -> getPtr ();

	if ( strncmp ( hdr -> id, "IDP3", 4 ) )
		return NULL;

	if ( hdr -> version != 15 )
		return NULL;

					// get bone frames ptr
	Md3BoneFrame * boneFramesPtr = (Md3BoneFrame *) data -> getPtr ( sizeof ( Md3Header ) );

					// get tags ptr
	Md3Tag * tagsPtr = (Md3Tag *) data -> getPtr ( sizeof ( Md3Header ) + hdr -> numBoneFrames * sizeof ( Md3BoneFrame ) );

					// read meshes
	int	meshOffs = sizeof ( Md3Header ) + 
		           hdr -> numBoneFrames * sizeof ( Md3BoneFrame ) + 
		           hdr -> numBoneFrames * hdr -> numTags * sizeof ( Md3Tag );

	CompositeObject * res = new CompositeObject ( data -> getName (), Vector3D ( 0, 0, 0 ) );

	for ( int meshNo = 0; meshNo < hdr -> numMeshes; meshNo++ )
	{
		Md3MeshHeader * meshHdr = (Md3MeshHeader *) data -> getPtr ( meshOffs );

							// get skins ptr
		Md3Skin * skinPtr = (Md3Skin *) data -> getPtr ( meshOffs + sizeof ( Md3MeshHeader ) );

							// get triangles ptr
		Md3Triangle * triPtr = (Md3Triangle *) data -> getPtr ( meshOffs + meshHdr -> triStart );

							// get tex. coords ptr
		Md3TexCoord * texCoordPtr = (Md3TexCoord *) data -> getPtr ( meshOffs + meshHdr -> texVectorStart );

							// get vertices ptr
		Md3Vertex * vertPtr = (Md3Vertex *) data -> getPtr ( meshOffs + meshHdr -> vertexStart );

							// get skins ptr
		Md3Skin * skinsPtr = (Md3Skin *) data -> getPtr ( meshOffs + meshHdr -> skinsStart );

							// now convert meshes to Mesh3D object
		meshOffs += meshHdr -> meshSize;

							// now prepare to mesh from this data
		String	name ( meshHdr -> name );
		int		numSkins     = meshHdr -> numSkins;
		int		numVertices  = meshHdr -> numVertices;
		int		numTexCoords = meshHdr -> numVertices;
		int		numFaces     = meshHdr -> numTriangles;

		Vector3D       * vertices  = new Vector3D [numVertices *  meshHdr -> numMeshFrames];
		Vector2D       * texCoords = new Vector2D [numVertices];
		Mesh3D :: Face * faces     = new Mesh3D :: Face [numFaces];

										// convert vertices, change coordinate system (scale, swap y & z)
		
		for (int i = 0; i < numVertices * meshHdr -> numMeshFrames; i++ )
		{
			vertices [i].x = vertPtr [i].vertex [0] / 64.0f;
			vertices [i].y = vertPtr [i].vertex [2] / 64.0f;
			vertices [i].z = vertPtr [i].vertex [1] / 64.0f;
		}

										// convert uv coordinates
		for (int i = 0; i < numTexCoords; i++ )
		{
			texCoords [i].x = texCoordPtr [i].coord [0];
			texCoords [i].y = texCoordPtr [i].coord [1];
		}

										// now build faces
		for (int i = 0; i < numFaces; i++ )
		{
			faces [i].vertexIndex [0] = triPtr [i].vertex [0];
			faces [i].vertexIndex [1] = triPtr [i].vertex [1];
			faces [i].vertexIndex [2] = triPtr [i].vertex [2];

			faces [i].texCoordIndex [0] = triPtr [i].vertex [0];
			faces [i].texCoordIndex [1] = triPtr [i].vertex [1];
			faces [i].texCoordIndex [2] = triPtr [i].vertex [2];
		}

										// now load skins
										// NB: we use only first skin (if present)
		Texture * texture = NULL;

		if ( numSkins > 0 )
		{
			texture = getResourceManager () -> getTexture ( skinsPtr -> name );

			if ( texture == NULL )		// Quake III quirk: sometimes they use wrong extension
			{
				String	texName ( skinsPtr -> name );

										// try using jpg extension
				String :: setExtension ( texName, "jpg" );

				texture = getResourceManager () -> getTexture ( texName );

				if ( texture == NULL )
				{
										// try using tga extension
					String :: setExtension ( texName, "tga" );

					texture = getResourceManager () -> getTexture ( texName );
				}
			}
		}
										// build mesh
		Mesh3D * triMesh = new Mesh3D ( name, vertices, NULL, numVertices, texCoords, numTexCoords, faces, numFaces, NULL );

		if ( texture != NULL )
		{
			triMesh -> setTexture ( texture );

			texture -> release ();
		}

		triMesh -> computeNormals ();

		MeshObject * mesh = new MeshObject ( name, Vector3D ( 0, 0, 0 ), Vector4D ( 1, 1, 1, 1 ), 
											 Transform3D :: getIdentity (), triMesh );

		mesh -> init ();
		res  -> insert ( mesh );
	
		delete [] vertices;
		delete [] texCoords;
		delete [] faces;
	}

	res -> init ();

	return res;
}
