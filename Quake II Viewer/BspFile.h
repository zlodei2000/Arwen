//
// Simple class for loading and processing Quake II bsp file
//
// Author: Alex V. Boreskoff
//
// Last modified: 22/10/2001
//

#ifndef	__BSP_FILE__
#define	__BSP_FILE__

#include	"QuakeII.h"

class	Data;

class	BspFile
{
public:
	Quake2BspHeader * hdr;
	Vector3D        * vertices;
	int				  numVertices;
	Quake2Edge	    * edges;
	int				  numEdges;
	Quake2Face	    * faces;
	int				  numFaces;
	unsigned long   * faceEdges;				// indices of edges for any face
	Quake2BspPlane  * planes;
	int				  numPlanes;
	Quake2BspNode   * nodes;
	int			 	  numNodes;
	Quake2BspLeaf   * leaves;
	int				  numLeaves;
	unsigned short  * leafFaces;				// indices of faces for any leaf
	Quake2TexInfo   * texInfos;
	int				  numTexInfos;
	unsigned char   * vis;						// visibility lump info
	int				  visBytes;
	void            * lightmaps;				// lightmaps lump
	Quake2Hull	    * hulls;
	int				  numHulls;
	char            * entities;

	BspFile ( Data * data );
	~BspFile ();

	int	getNumFaces () const
	{
		return numFaces;
	}

	int	getNumClusters () const
	{
		return * (long *) vis;
	}

	int	getPvsOffset ( int cluster ) const
	{
		return ((Quake2PvsEntry *)(4 + cluster * sizeof (Quake2PvsEntry) + (char *) vis)) -> pvsOffs;
	}

	void * getLightMap ( long offset ) const
	{
		return offset + (unsigned char *) lightmaps;
	}
};

#endif
