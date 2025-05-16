//
// Class to load Quake II .md2 models
//
// Author: Alex V. Boreskoff
//
// Last modified: 6/11/2002
//

#include	"Md2Decoder.h"
#include	"ObjStr.h"
#include	"Mesh3D.h"
#include	"Vector2D.h"
#include	"Vector3D.h"
#include	"Array.h"
#include	"Data.h"
										// defintions of md2 file structs
#pragma pack (push, 1)

struct	Md2Header
{
	int	magic;							// magic used to identify file
	int	version;						// version number of the file, must be 8
	int	skinWidth;
	int	skinHeight;
	int	frameSize;
	int	numSkins;
	int	numVertices;
	int	numTexCoords;
	int	numTriangles;
	int	numGlCommands;
	int	numFrames;
	int	offsetOfSkins;
	int	offsetOfTexCoords;
	int	offsetOfTriangles;
	int	offsetOfFrames;
	int	offsetOfGlCommands;
	int	offsetEnd;						// end of file offset
};

struct	Md2AliasTriangle
{
	unsigned char	vertex [3];
	unsigned char	lightNormalIndex;
};

struct	Md2Vertex
{
	float	vertex [3];
	float	normal [3];
};

struct	Md2Face
{
	short	vertexIndex   [3];
	short	texCoordIndex [3];
};

struct	Md2TexCoord
{
	short	u, v;
};

struct	Md2AliasFrame
{
	float				scale     [3];
	float				translate [3];
	char				name      [16];
	Md2AliasTriangle	aliasVertices [1];
};

struct	Md2Frame
{
	char	      name [16];
	Md2Vertex	* vertices;
};

typedef	char	Md2Skin [64];

#pragma	pack (pop)

MetaClass	Md2Decoder :: classInstance ( "Md2Decoder", &ResourceDecoder :: classInstance );

////////////////////////// here goes the methods ////////////////////////////

bool	Md2Decoder :: checkExtension ( const String& theName )
{
	return String :: getExtension ( theName ).toLower () == "md2";
}

Object * Md2Decoder :: decode ( Data * data )
{
	Md2Header * hdr = (Md2Header *) data -> getPtr ();

	if ( hdr -> version != 8 )
		return NULL;

	Md2Skin     * md2Skins     = (Md2Skin     *) data -> getPtr ( hdr -> offsetOfSkins     );
	Md2TexCoord * md2TexCoords = (Md2TexCoord *) data -> getPtr ( hdr -> offsetOfTexCoords );
	Md2Face     * md2Faces     = (Md2Face     *) data -> getPtr ( hdr -> offsetOfTriangles );
	Md2Frame    * frames       = new Md2Frame [hdr -> numFrames];

	int	frameOffs = hdr -> offsetOfFrames;

								// now get all the frames
	for ( int i = 0; i < hdr -> numFrames; i++ )
	{
		Md2AliasFrame * aliasFrame = (Md2AliasFrame *) data -> getPtr ( frameOffs );
		Md2Vertex     * vertices   = new Md2Vertex [hdr -> numVertices];

		strcpy ( frames [i].name, aliasFrame -> name );

		frames [i].vertices = vertices;

		for ( int j = 0; j < hdr -> numVertices; j++ )
		{
			vertices [j].vertex [0] = (aliasFrame -> aliasVertices [j].vertex [0] * aliasFrame -> scale [0] + aliasFrame -> translate [0]);
			vertices [j].vertex [2] = (aliasFrame -> aliasVertices [j].vertex [1] * aliasFrame -> scale [1] + aliasFrame -> translate [1]) * (-1);
			vertices [j].vertex [1] = (aliasFrame -> aliasVertices [j].vertex [2] * aliasFrame -> scale [2] + aliasFrame -> translate [2]);
		}

		frameOffs += hdr -> frameSize;
	}

	int	numVertices  = hdr -> numVertices;
	int	numTexCoords = hdr -> numTexCoords;
	int	numFaces     = hdr -> numTriangles;

	Array * array = new Array;

								// build meshes for every frame
	for ( int frame = 0; frame < hdr -> numFrames; frame++ )
	{
								// build vertices
		Vector3D * vertices = new Vector3D [numVertices];
		int i;
		for ( i = 0; i < numVertices; i++ )
		{
			vertices [i].x = frames [frame].vertices [i].vertex [0];
			vertices [i].y = frames [frame].vertices [i].vertex [1];
			vertices [i].z = frames [frame].vertices [i].vertex [2];
		}

								// build tex coords
		Vector2D * texCoords = new Vector2D [numTexCoords];

		for ( i = 0; i < numTexCoords; i++ )
		{
			texCoords [i].x =  md2TexCoords [i].u / (float) hdr -> skinWidth;
			texCoords [i].y = -md2TexCoords [i].v / (float) hdr -> skinHeight;
		}

								// build faces
		Mesh3D :: Face * faces = new Mesh3D :: Face [numFaces];

		for ( i = 0; i < numFaces; i++ )
		{
			for ( int j = 0; j < 3; j++ )
			{
				faces [i].vertexIndex   [j] = md2Faces [i].vertexIndex   [j];
				faces [i].texCoordIndex [j] = md2Faces [i].texCoordIndex [j];
			}
		}

		Mesh3D * mesh = new Mesh3D ( frames [frame].name, vertices, NULL, numVertices, 
			                         texCoords, numTexCoords, faces, numFaces, NULL, NULL );

		mesh  -> computeNormals   ();
		mesh  -> buildBoundingBox ();
		array -> insert           ( mesh );

		delete frames [frame].vertices;
		delete [] vertices;
		delete [] texCoords;
		delete [] faces;
	}

	delete [] frames;

	return array;
}

