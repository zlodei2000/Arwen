//
// Basic functions to render Quake II level
//
// Author: Alex V. Boreskoff
//
// Last modified: 19/11/2001
//

#ifndef	__QUAKE_II_LEVEL__
#define	__QUAKE_II_LEVEL__

#include	"QuakeII.h"
#include	"3DDefs.h"
#include	"Model.h"

class	Camera;
class	BspFile;
class	PakFile;
class	Entities;
class	Frustrum;
class	Texture;
class	Lightmap;
class	View;
class	Polygon3D;
class	ResourceManager;
class	Sky;
class	Data;
class	Vector2D;

extern long frameNo;

class	Quake2Level : public Model
{
private:
	BspFile       * file;
	long          * clusterTable;		// table of visible clusters (cluster if visible if value == frameNo)
	long		  * faceTable;			// table of visible facets (1 bit per face)
	Entities      * entities;
	Polygon3D    ** polys;				// pointers to polygons of the level
	Polygon3D    ** visPolys;			// list of visible polys
	int				numFaces;
	int				numClusters;
	int				numVisibleFaces;
	float			angle;				// angle of player (yaw ?)
	Sky           * sky;
	bool			drawSky;
	long			frameNo;			// current frame number (count of drawn frames)

public:
	Quake2Level  ( Data * data );
	~Quake2Level ();

	virtual	int	init ();

	bool		    loadPolys         ();
	bool		    isLeafInFrustrum  ( const Quake2BspLeaf * leaf, const Frustrum& frustrum ) const;
	Quake2BspLeaf * findLeaf          ( const Vector3D& pos ) const;
	void		    buildClusterTable ( const Vector3D& pos );
	void		    addFacesList      ( int firstFace, int numFaces, const Vector3D& pos );
	void		    buildFacesList    ( const Camera& camera );
	void		    drawFaces         ( View& view, const Camera& camera );
	void		    render            ( View& view, const Camera& camera );
	void		    setLightmap       ( int offset, Polygon3D * poly );

	const Vector3D&	getStartPos () const
	{
		return pos;
	}

	float	getAngle () const
	{
		return angle;
	}

	void	markFace ( int index )
	{
		faceTable [index] = frameNo;
	}

	bool	isFaceMarked ( int index ) const
	{
		return faceTable [index] == frameNo;
	}
};

#endif
