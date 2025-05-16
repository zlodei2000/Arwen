//
// Generic View object for Model/View/Controller pattern
//
// Author: Alex V. Boreskoff
//
// Last modified: 20/11/2002
//

#include	"View.h"
#include	"Poly3D.h"

MetaClass	VideoMode :: classInstance ( "VideoMode", &Object :: classInstance );
MetaClass	View      :: classInstance ( "View",      &Object :: classInstance );

void	View :: draw ( const Polygon3D& poly )
{
	bindTexture ( poly.getTexture () );
	simpleDraw  ( poly, useColors  );
}
