//
// A visitor class for to draw Bsp tree
//
// Author: Alex V. Boreskoff
//
// Last modified: 28/11/2002
//

#include	"BspDrawVisitor.h"
#include	"SubScene.h"
#include	"Camera.h"

BspDrawVisitor :: BspDrawVisitor ( View * theView, const SubScene * theScene, const Camera * theCamera, const Frustrum * theFrustrum, Polygon3D * theTempPoly ) : BspNodeVisitor ( camera -> getPos () )
{
	view     = theView;
	camera   = theCamera;
	frustrum = theFrustrum;
	subScene = theScene;
	tempPoly = theTempPoly;
}

bool	BspDrawVisitor :: visit ( BspNode * node )
{
	subScene -> renderPoly ( *view, *camera, node -> facet, *tempPoly, *frustrum, NULL );

	return false;
}

