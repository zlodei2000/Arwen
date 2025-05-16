#ifndef __PORTAL__
#define __PORTAL__

#include    "Poly3D.h"
#include	"Transform3D.h"

class	SubScene;

class   Portal : public Polygon3D
{
protected:
    SubScene    * srcScene;
	SubScene    * dstScene;
    Transform3D * transf;

public:
	Portal ( const Portal& thePortal ) : Polygon3D ( thePortal )
	{
		srcScene = thePortal.srcScene;
		dstScene = thePortal.dstScene;
		transf   = thePortal.transf != NULL ? new Transform3D ( *thePortal.transf ) : NULL;
		flags    = thePortal.flags;
	}

    Portal ( const char * theName, int polySize, SubScene * s1 = NULL, SubScene * s2 = NULL ) :
        Polygon3D ( theName, polySize )
    {
        srcScene = s1;
        dstScene = s2;
        transf   = NULL;

        setFlag ( PF_PORTAL );
    }

    virtual bool isOk () const
    {
        return srcScene != NULL && dstScene != NULL && Polygon3D :: isOk ();
    }

    SubScene * getSourceScene () const
    {
        return srcScene;
    }

    SubScene * getDestScene () const
    {
        return dstScene;
    }

    void    setSourceScene ( SubScene * scene )
    {
        srcScene = scene;
    }

    void    setDestScene ( SubScene * scene )
    {
        dstScene = scene;
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
		return srcScene == scene ? dstScene : srcScene;
	}

	static	MetaClass	classInstance;
};

#endif
