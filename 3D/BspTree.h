//
// Basic class to handle Bsp trees
//
// Author: Alex V. Boreskoff
//
// Last modified: 10/11/2002
//

#ifndef	__BSP_TREE__
#define	__BSP_TREE__

#include	"Plane.h"
#include	"Poly3D.h"

class	Array;
class	BspNodeVisitor;

class	BspNode : public Plane
{
public:
	Polygon3D * facet;
	BspNode   * front;		// subtree in positive halfspace (front)
	BspNode   * back;		// subtree in negative halfspace (back)
	BoundingBox	box;

	BspNode ( Polygon3D * poly ) : Plane ( *poly -> getPlane () )
	{
        facet = poly;
		front = NULL;
		back  = NULL;
	}

	bool	visitInorder   ( BspNodeVisitor& );		// front-to-back traversal
	bool	visitPostorder ( BspNodeVisitor& );		// back-to-front traversal

	const BoundingBox& getBoundingBox () const
	{
		return box;
	}

	void	setBoundingBox ( const BoundingBox& theBox )
	{
		box = theBox;
	}
};


class	BspNodeVisitor
{
protected:
	Vector3D	pos;

public:
	BspNodeVisitor ( const Vector3D& v) : pos ( v ) {}
	virtual	~BspNodeVisitor () {}

	const Vector3D&	getPos () const
	{
		return pos;
	}

	virtual	bool	wantsNode ( const BspNode * node )
	{
		return true;
	}

	virtual	bool	visit ( BspNode * node ) = 0;
};
	
BspNode * buildBspTree  ( Array& facets );
void      deleteBspTree ( BspNode * tree );

#endif
