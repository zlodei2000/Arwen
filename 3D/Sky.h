//
// Simple class for drawing sky based on cubic maps
//
// Author: Alex V. Boreskoff
//
// Last modified: 20/12/2001
//

#ifndef	__SKY__
#define	__SKY__

#include	"ObjStr.h"
#include	"Texture.h"

#define	SKY_FRONT	0x01
#define	SKY_BACK	0x02
#define	SKY_LEFT	0x04
#define	SKY_RIGHT	0x08
#define	SKY_UP		0x10
#define	SKY_DOWN	0x20

class	Camera;

class	Sky : public Object
{
protected:
	float	  skySize;
	Texture * skyTextures [6];
	float	  invWidth    [6];
	float	  invHeight   [6];

public:
	Sky ( const char * name, const char * ext = ".tga" );
	~Sky ();

    virtual bool	isOk () const;
	virtual	void	draw ( const Camera& camera );

	static	MetaClass	classInstance;
};

#endif

