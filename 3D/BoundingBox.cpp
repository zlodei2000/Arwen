//
// Axis-aligned bounding box
//
// Author: Alex V. Boreskoff
//
// Last change: 8/10/20002
//

#include	"BoundingBox.h"
#include	"Transform3D.h"

static	int	edgeIndices [12][2] =
{
	{ 0, 1 },
	{ 0, 2 },
	{ 0, 4 },
	{ 1, 3 },
	{ 1, 5 },
	{ 2, 3 },
	{ 2, 6 },
	{ 3, 7 },
	{ 4, 5 },
	{ 4, 6 },
	{ 5, 6 },
	{ 6, 7 }
};

static	int	faceIndices [6][4] =
{
	{ 2, 3, 1, 0 },                 // bottom side
    { 4, 5, 7, 6 },                 // top side
	{ 0, 1, 5, 4 },                 // front side
    { 6, 7, 3, 2 },                 // far side
    { 1, 3, 7, 5 },                 // right side
	{ 4, 6, 2, 0 }                  // left side
};

void	BoundingBox :: apply ( const Transform3D& tr )
{
	Vector3D	v [8];

	for ( int i = 0; i < 8; i++ )
		v [i] = tr.transformPoint ( getVertex ( i ) );

	reset ();

	for ( int i = 0; i < 8; i++ )
		addVertex ( v [i] );
}

const int * BoundingBox :: getEdge ( int index ) const
{
	if ( index >= 0 && index < 12 )
		return &edgeIndices [index][0];

	return NULL;
}

const int * BoundingBox :: getFace ( int index ) const
{
	if ( index >= 0 && index < 6 )
		return &faceIndices [index][0];

	return NULL;
}

