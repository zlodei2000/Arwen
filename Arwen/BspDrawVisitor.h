//
// A visitor class for to draw Bsp tree
//
// Author: Alex V. Boreskoff
//
// Last modified: 28/11/2002
//

#ifndef	__BSP_DRAW_VISITOR__
#define	__BSP_DRAW_VISITOR__

#include	"BspTree.h"

class	View;
class	SubScene;
class	Camera;
class	Frustrum;
class	Polygon3D;

class	BspDrawVisitor : public BspNodeVisitor
{
protected:
	View            * view;
	const Camera    * camera;
	const Frustrum  * frustrum;
	const SubScene  * subScene;
	Polygon3D       * tempPoly;
public:
	BspDrawVisitor ( View * theView, const SubScene * theScene, const Camera * theCamera, const Frustrum * theFrustrum, Polygon3D * theTempPoly );

	virtual	bool	visit ( BspNode * node );
};
	
#endif
