//
// Basic class to handle Bsp trees
//
// Author: Alex V. Boreskoff
//
// Last modified: 10/11/2002
//

#include    "BspTree.h"
#include	"Array.h"
#include	"Poly3D.h"

bool	BspNode :: visitInorder ( BspNodeVisitor& visitor )
{
	if ( !visitor.wantsNode ( this ) )
		return true;

	int	cl = classify ( visitor.getPos () );

	if ( cl == IN_FRONT )
	{
		if ( front != NULL )
			if ( front -> visitInorder ( visitor ) )
				return true;

		if ( visitor.visit ( this ) )
			return true;

		if ( back != NULL )
			if ( back -> visitInorder ( visitor ) )
				return true;
	}
	else
	{
		if ( back != NULL )
			if ( back -> visitInorder ( visitor ) )
				return true;

		if ( visitor.visit ( this ) )
			return true;

		if ( front != NULL )
			if ( front -> visitInorder ( visitor ) )
				return true;
	}

	return false;
}

bool	BspNode :: visitPostorder ( BspNodeVisitor& visitor )
{
	if ( !visitor.wantsNode ( this ) )
		return true;

	int	cl = classify ( visitor.getPos () );

	if ( cl == IN_BACK )
	{
		if ( front != NULL )
			if ( front -> visitInorder ( visitor ) )
				return true;

		if ( visitor.visit ( this ) )
			return true;

		if ( back != NULL )
			if ( back -> visitInorder ( visitor ) )
				return true;
	}
	else
	{
		if ( back != NULL )
			if ( back -> visitInorder ( visitor ) )
				return true;

		if ( visitor.visit ( this ) )
			return true;

		if ( front != NULL )
			if ( front -> visitInorder ( visitor ) )
				return true;
	}

	return false;
}

int findOptimalSplitter ( const Array& polys )
{
    int bestSplitter  = 0;
    int bestNumSplits = polys.getCount ();

    for ( int i = 0; i < polys.getCount (); i++ )
    {
        Polygon3D   * splitter  = (Polygon3D *) polys.at ( i );
        const Plane * plane     = splitter -> getPlane ();
        int           numSplits = 0;

        for ( int j = 0; j < polys.getCount (); j++ )
        {
            if ( i == j )
               continue;

            Polygon3D * poly = (Polygon3D *) polys.at ( j );

            if ( poly -> classify ( *plane ) == IN_BOTH )
               numSplits++;
        }

        if ( numSplits < bestNumSplits )
        {
            bestNumSplits = numSplits;
            bestSplitter  = i;
        }
    }

    return bestSplitter;
}

BspNode * buildBspTree ( Array& polys )
{
	if ( polys.isEmpty () )
		return NULL;

    int         index     = findOptimalSplitter ( polys );
    Polygon3D * splitter  = (Polygon3D *) polys.at ( index );
    BspNode   * root      = new BspNode ( splitter );
    Polygon3D * frontPoly = NULL;
    Polygon3D * backPoly  = NULL;

    Array       front ( "Front Polys" );
    Array       back  ( "Back Polys"  );
	BoundingBox	box;

	for ( int i = 0; i < polys.getNumItems (); i++ )
		box.merge ( ( (Polygon3D *) polys.at ( i ) ) -> getBoundingBox () );

	root -> setBoundingBox ( box );

    splitter -> retain  ();     // since it'll be release'd in next stmt
    polys.removeAtIndex ( index );

    while ( polys.getCount () > 0 )
    {
        Polygon3D * poly = (Polygon3D *) polys.at ( 0 );

        poly -> retain ();
        polys.removeAtIndex ( 0 );

        switch ( poly -> classify ( *splitter -> getPlane () ) )
        {
            case IN_PLANE:
            case IN_FRONT:
                 front.insert ( poly );
                 break;

            case IN_BACK:
                 back.insert ( poly );
                 break;

            case IN_BOTH:
                 frontPoly = new Polygon3D ( poly -> getName (), MAX_VERTICES );
                 backPoly  = new Polygon3D ( poly -> getName (), MAX_VERTICES );

                 poly -> split ( *splitter -> getPlane (), *frontPoly, *backPoly );

                 front.insert ( frontPoly );
                 back.insert  ( backPoly  );

                 frontPoly -> release ();
                 backPoly  -> release ();
                 break;
        }

        poly -> release ();
    }

	root -> front = (front.getCount () > 0 ? buildBspTree (front) : NULL );
	root -> back  = (back.getCount  () > 0 ? buildBspTree (back)  : NULL );

  	return root;
}

void	deleteBspTree ( BspNode * tree )
{
    tree -> facet -> release ();

    if ( tree -> front != NULL )
        deleteBspTree ( tree -> front );

    if ( tree -> back != NULL )
        deleteBspTree ( tree -> back );

    delete tree;
}
