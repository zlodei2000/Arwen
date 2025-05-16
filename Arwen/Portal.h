#ifndef __PORTAL__
#define __PORTAL__

#include    "Poly3D.h"
#include	"Transform3D.h"

class	SubScene;

class   Portal : public Polygon3D
{
public:
    SubScene    * scene1, * scene2;
    Transform3D * transf;

	Portal ( const Portal& thePortal ) : Polygon3D ( thePortal )
	{
		scene1 = thePortal.scene1;
		scene2 = thePortal.scene2;
		transf = thePortal.transf != NULL ? new Transform3D ( *thePortal.transf ) : NULL;
		flags  = thePortal.flags;
	}

    Portal ( const char * theName, int polySize, SubScene * s1 = NULL, SubScene * s2 = NULL ) :
        Polygon3D ( theName, polySize )
    {
        scene1 = s1;
        scene2 = s2;
        transf = NULL;

        setFlag ( PF_PORTAL );
    }

    virtual const char * getClassName () const
    {
        return "Portal";
    }

    virtual bool isOk () const
    {
        return scene1 != NULL && scene2 != NULL && Polygon3D :: isOk ();
    }

    SubScene * getSourceScene () const
    {
        return scene1;
    }

    SubScene * getDestScene () const
    {
        return scene2;
    }

    void    setSourceScene ( SubScene * scene )
    {
        scene1 = scene;
    }

    void    setDestScene ( SubScene * scene )
    {
        scene2 = scene;
    }

    Transform3D * getTransform () const
    {
        return transf;
    }

    void    setTransform ( Transform3D * tr )
    {
        delete transf;

        transf = tr;
    }

	SubScene * getAdjacentSubScene ( const SubScene * scene ) const
	{
		return scene1 == scene ? scene2 : scene1;
	}
};

#endif
