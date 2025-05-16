//
// Simple Quake III-like shader class
//
// Author: Alex V. Boreskoff
//
// Last modified: 2/12/2002
//

#ifndef	__SHADER__
#define	__SHADER__

#include	"ShaderPass.h"
#include	"Array.h"

class	Shader : public Object
{
protected:
	Array	passes;

public:
	Shader ( const char * theName ) : Object ( theName ) 
	{
		metaClass = &classInstance;
	}

	void	addPass ( ShaderPass * pass )
	{
		passes.insert ( pass );
	}

	void	removePass ( const String& theName )
	{
		passes.removeObjectWithName ( theName );
	}

	ShaderPass * getPass ( const String& theName )
	{
		return (ShaderPass *) passes.getObjectWithName ( theName );
	}

	bool	isTransparent () const;

	void	drawPoly ( View& view, const Polygon3D * poly ) const;
	void	drawMesh ( View& view, const Camera& camera, const Vector4D& color, 
	                   Fog * fog,  const Mesh3D * mesh ) const;

	static	MetaClass	classInstance;
};

#endif
