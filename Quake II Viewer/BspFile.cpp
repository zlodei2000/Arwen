#include	<malloc.h>
#include	"BspFile.h"
#include	"Data.h"

BspFile :: BspFile ( Data * data )
{
	void * ptr = malloc ( data -> getLength () );

	if ( ptr == NULL )
		return;

	data -> getBytes ( ptr, data -> getLength () );

	hdr         = (Quake2BspHeader *) ptr;

	vertices    = (Vector3D *)(hdr -> dir [LUMP_VERTICES].offset + (char *) ptr);
	numVertices = hdr -> dir [LUMP_VERTICES].size / sizeof ( Vector3D );

	edges       = (Quake2Edge *)(hdr -> dir [LUMP_EDGES].offset + (char *) ptr);
	numEdges    = hdr -> dir [LUMP_EDGES].size / sizeof ( Quake2Edge );

	faces       = (Quake2Face *)(hdr -> dir [LUMP_FACES].offset + (char *) ptr);
	numFaces    = hdr -> dir [LUMP_FACES].size / sizeof ( Quake2Face );

	planes      = (Quake2BspPlane *)(hdr -> dir [LUMP_PLANES].offset + (char *) ptr);
	numPlanes   = hdr -> dir [LUMP_PLANES].size / sizeof ( Quake2BspPlane );

	nodes       = (Quake2BspNode *)(hdr -> dir [LUMP_NODES].offset + (char *) ptr);
	numNodes    = hdr -> dir [LUMP_NODES].size / sizeof ( Quake2BspNode );

	leaves      = (Quake2BspLeaf *)(hdr -> dir [LUMP_LEAVES].offset + (char *) ptr);
	numLeaves   = hdr -> dir [LUMP_LEAVES].size / sizeof ( Quake2BspLeaf );

	leafFaces   = (unsigned short *)(hdr -> dir [LUMP_LEAF_FACE_TABLE].offset + (char *) ptr);

	texInfos    = (Quake2TexInfo *)(hdr -> dir [LUMP_TEXINFO].offset + (char *) ptr);
	numTexInfos = hdr -> dir [LUMP_TEXINFO].size / sizeof ( Quake2TexInfo );

	vis         = (unsigned char *)(hdr -> dir [LUMP_VIS].offset + (char *) ptr);
	visBytes    = hdr -> dir [LUMP_VIS].size;

	lightmaps   = hdr -> dir [LUMP_LIGHTMAPS].offset + (char *) ptr;

	hulls       = (Quake2Hull *)(hdr -> dir [LUMP_HULLS].offset + (char *) ptr);
	numHulls    = hdr -> dir [LUMP_HULLS].size /  sizeof ( Quake2Hull );

	entities    = (char *)(hdr -> dir [LUMP_ENTITIES].offset + (char *) ptr);

	faceEdges   = (unsigned long *)(hdr -> dir [LUMP_FACE_EDGE_TABLE].offset + (char *) ptr);
}

BspFile :: ~BspFile ()
{
	free ( hdr );
}
