//
// Decoder class to load scene from .sc file
//
// Author: Alex V. Boreskoff
//
// Last modified: 12/11/2002
//

#ifndef	__SCENE_DECODER__
#define	__SCENE_DECODER__

#include	"ResourceDecoder.h"
#include	"Array.h"

class	SubScene;
class	World;
class	Vector2D;
class	Vector3D;
class	Vector4D;
class	Transform3D;
class	Transform2D;
class	Polygon3D;
class	Portal;
class	Texture;
class	Animator;
class	Shader;
class	ShaderPass;
class	Plane;

class	SceneDecoder : public ResourceDecoder 
{
protected:
	struct	Context						// we put all context in object to be allocated on stack
	{
		Array	pending;				// array of pending (unresolved) portals
		Array	shaders;				// array of registered shaders
		World * world;
		int	    line;
	};

public:
	SceneDecoder ( ResourceManager * rm ) : ResourceDecoder ( "Scene Decoder", rm ) 
	{
		metaClass = &classInstance;
	}

	virtual	Object * decode         ( Data * data );
	virtual	bool	 checkExtension ( const String& theName );

	static	MetaClass	classInstance;

protected:
	bool	getString   ( Context& context, Data * data, String& str );

	bool	getCamera   ( Context& context, const String& args );
	bool	getSource   ( Context& context, const String& args );
	bool	getSubScene ( Context& context, const String& name, Data * data );

	bool	getPolygon     ( Context& context, const String& name, Data * data, SubScene * scene );
	bool	getPortal      ( Context& context, const String& name, Data * data, SubScene * scene );
	bool	getBillboard   ( Context& context, const String& name, Data * data, SubScene * scene );
	bool	getFog         ( Context& context, const String& name, Data * data, SubScene * scene );
	bool	getLinearFog   ( Context& context, const String& name, Data * data, SubScene * scene );
	bool	getFountain    ( Context& context, const String& name, Data * data, SubScene * scene );
	bool	getMagicSphere ( Context& context, const String& name, Data * data, SubScene * scene );
	bool	getFire        ( Context& context, const String& name, Data * data, SubScene * scene );
	bool	getSnow        ( Context& context, const String& name, Data * data, SubScene * scene );
	bool	getExplosion   ( Context& context, const String& name, Data * data, SubScene * scene );
	bool	getMesh        ( Context& context, const String& name, Data * data, SubScene * scene );
	bool	getLight       ( Context& context, const String& name, Data * data, SubScene * scene );
//	bool	getQ3Player    ( const String& name, Data * data, SubScene * scene );

	bool	getVector2D ( Context& context, Vector2D& v, const String& str );
	bool	getVector3D ( Context& context, Vector3D& v, const String& str );
	bool	getVector4D ( Context& context, Vector4D& v, const String& str );

	bool	getTexture       ( Context& context, Polygon3D * poly, const String& str );
	bool	getMapping       ( Context& context, Polygon3D * poly, const String& str );
	bool	getVertex        ( Context& context, Polygon3D * poly, const String& str );
	bool	getStyle         ( Context& context, Polygon3D * poly, const String& str );
	bool	getTransform     ( Context& context, Transform3D& tr,  const String& str );
	bool	getTransform2D   ( Context& context, Transform2D& tr, const String& str );   
	bool	getConnection    ( Context& context, Portal * portal,  const String& str );
	bool	getPlane         ( Context& context, Plane& plane,     const String& str );
	int		getBlendingMode  ( const String& str );

	Texture    * getTexture            ( Context& context, const String& name );
	Shader     * getShader             ( Context& context, Data * data, const String& name );
	ShaderPass * getShaderPass         ( Context& context, Data * data, const String& name );
	bool	     resolvePendingPortals ( Context& context );
	Animator   * getAnimator           ( const String& str );
};

#endif
