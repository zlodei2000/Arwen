//
// Class for automatical animation of Visual Objects
//
// Author: Alex V. Boreskoff
//
// Last modified: 2/12/2002
//

#ifndef	__ANIMATOR__
#define	__ANIMATOR__

#include	"VisualObject.h"

class	VisualObject;

class	Animator : public Object
{
public:
	Animator ( const char * theName ) : Object ( theName ) 
	{
		metaClass = &classInstance;
	}

	virtual	void	animate ( VisualObject * object, float curTime ) {}

	static	MetaClass	classInstance;
};

#endif
