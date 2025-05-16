//
// Simple Quake II structures
//

#ifndef	__QUAKE_II__
#define	__QUAKE_II__

#include	<string.h>					// for strncmp

#pragma pack (push, 1 )

#include	"Vector3D.h"				// use Vector3D class instead of structure

////////////////////// PAK file structure /////////////////////////

struct	PakHeader						// header of Quake II .pak file 
{
	char	magic [4];					// "PACK"
	long	dirOffset;					// offset of directory oin the file
	long	dirSize;					// size of directory (numEntries * 0x40)

	bool	isOk () const
	{
		return !strncmp ( magic, "PACK", 4 );
	}

	int	getNumEntries () const
	{
		return dirSize / 0x40;
	}
};

struct	DirEntry						// entry of a .pak directory
{
	char	fileName [0x38];
	long	offset;				// offset  of resource in the file, from the begining
	long	size;				// size of resource in bytes
};

///////////////////////// BSP file strucures /////////////////////////

#define	BSP_VERSION	38

#define	LUMP_ENTITIES			0		// index of entities lump in BspHeader::dir
#define	LUMP_PLANES				1		// index of planes array lump
#define	LUMP_VERTICES			2		// index of vertices array lump
#define	LUMP_VIS				3		// index of visibility lump
#define	LUMP_NODES				4		// index of bsp nodes array lump
#define	LUMP_TEXINFO			5		// index of texInfo array lump
#define	LUMP_FACES				6		// index of faces array lump
#define	LUMP_LIGHTMAPS			7		// index of lightmaps array lump
#define	LUMP_LEAVES				8		// index of bsp leaves array lump
#define	LUMP_LEAF_FACE_TABLE	9		// index of lookup table for referencing faces from the leaf
#define	LUMP_LEAF_BRUSH_TABLE	10
#define	LUMP_EDGES				11		// index of edges array lump
#define	LUMP_FACE_EDGE_TABLE	12		// index of edge lookup table lump
#define	LUMP_HULLS				13		// index of hulls (models) array lump
#define	LUMP_BRUSHES			14		// index of brushes array lump
#define	LUMP_BRUSH_SIDES		15
#define	LUMP_POP				16
#define	LUMP_AREAS				17
#define	LUMP_AREA_PORTALS		18

struct	Quake2BspEntry					// entry in .bsp file directory
{
	long	offset;						// offset from the start of .bsp file
	long	size;						// sizeof lump in bytes
};

struct	Quake2BspHeader					// header of the .bsp file
{
	char			magic [4];			// signature ("IBSP")
	long			version;
	Quake2BspEntry	dir [19];
};

struct	Quake2Edge						// edge (reside in edegs lump)
{
	unsigned short	firstEdge;
	unsigned short	lastEdge;
};

struct	Quake2Face						// Quake polygon (face) (stored in the faces lump)
{
	unsigned short	plane;				// plane the face is in
	unsigned short	planeSide;			// 0 if plane normal coinsides with facet normal
	long			firstEdge;			// index of 1st of facet in the face edges table
	unsigned short	numEdges;			// number of edges in the face edges table
	unsigned short	texInfo;			// index of texInfo in the text info array
	unsigned char	lightmapStyles [4];
	long			lightmapOffset;		// offset of the lightmap in the lightmap lump
};

#define	PLANE_TYPE_X		0			// axial x-plane
#define	PLANE_TYPE_Y		1			// axial y-plane
#define	PLANE_TYPE_Z		2			// axial z-plane
#define	PLANE_TYPE_MAIN_X	3			// non axial, main axis is Ox
#define	PLANE_TYPE_MAIN_Y	4			// non axial, main axis is Oy
#define	PLANE_TYPE_MAIN_Z	5			// non axial, main axis is Oz

struct	Quake2BspPlane					// Quake planes (stored in the planes lump)
{
	Vector3D	normal;					// plane normal (A,B,C)
	float		dist;					// signed distance along normal (D in the plane aquation Ax+By+Cz-D=0)
	long		type;					// one of the PLANE_TYPE_* constants
};

struct	Quake2BspNode					// internal Bsp tree node (stored in the nodes lump)
{
	long			plane;				// index of plane that makes this node
	long			frontChild;			// index of front child node, if negative then ~leaf
	long			backChild;			// index of back child node, if negative then  ~leaf
	short			mins [3];			// short-based min value of bbox
	short			maxs [3];			// short-based max value of bbox
	unsigned short	firstFace;			// index of the 1ast face of the node (in the face array)
	unsigned short	numFaces;			// number of faces
};

struct	Quake2BspLeaf					// bsp node leaf (stored in the leaves lump)
{
	long			brushOr;			// OR'ed brushes (???)
	unsigned short	cluster;			// -1 (0xFFFF) if no visibility information
	unsigned short	area;
	short			mins [3];			// bbox min value as shorts
	short			maxs [3];			// bbox max values as shorts
	unsigned short	firstLeafFace;		// index of 1st leaf face in the leaf face table
	unsigned short	numLeafFaces;		// # of leaf faces
	unsigned short	firstLeafBrush;
	unsigned short	numLeafBrushes;
};
	
										// texInfo flag bits
#define	SURF_LIGHT		0x01			// value will hold light strength
#define	SUFR_SLICK		0x02
#define	SURF_SKY		0x04
#define	SURF_WARP		0x08			// turbulent water warp
#define	SURF_TRANS33	0x10
#define	SURF_TRANS66	0x20
#define	SURF_FLOWING	0x40
#define	SURF_NODRAW		0x80
#define	SURF_HINT		0x100
#define	SURF_SKIP		0x200

struct	Quake2TexInfo					// texture coordinates generation data (stored in the texinfo lump)
{
	Vector3D		uAxis;
	float			uOffset;
	Vector3D		vAxis;
	float			vOffset;
	unsigned long	flags;				// miptex flags and overrides
	unsigned long	value;				// light emission and etc
	char			texName [32];		// texture name without extension
	long			nextTexture;		// next texture in animation chain or -1
};

struct	Quake2PvsEntry					// directory entry of pvs lump
{
	long	pvsOffs;
	long	phsOffs;
};

struct	Quake2Hull						// bsp hull
{
	Vector3D	mins;					// hull bounding box
	Vector3D	maxs;
	Vector3D	origin;
	long		headNode;
	long		firstFace;
	long		numFaces;
};

#pragma pack (pop)

#endif
