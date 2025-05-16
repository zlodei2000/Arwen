//
// Decoder class to load scene from .sc file
//
// Author: Alex V. Boreskoff
//
// Last modified: 12/11/2002
//

#include	<stdio.h>					// for sscanf

#include	"SceneDecoder.h"
#include	"ObjStr.h"
#include	"Data.h"
#include	"World.h"
#include	"StencilSubScene.h"
#include	"Vector2D.h"
#include	"Vector3D.h"
#include	"Vector4D.h"
#include	"Transform3D.h"
#include	"Transform2D.h"
#include	"Mapping.h"
#include	"Poly3D.h"
#include	"Portal.h"
#include	"Texture.h"
#include	"ResourceManager.h"
#include	"Log.h"
#include	"Billboard.h"
#include	"Fountain.h"
#include	"Snow.h"
#include	"MagicSphere.h"
#include	"Fire.h"
#include	"Fog.h"
#include	"LinearFog.h"
#include	"Mesh3D.h"
#include	"MeshObject.h"
#include	"CompositeObject.h"
#include	"RotationAnimator.h"
#include	"FileSystem.h"
#include	"PakFileSystem.h"
#include	"ZipFileSystem.h"
#include	"Shader.h"
#include	"Sky.h"
#include	"View.h"					// for blend modes
#include	"Light.h"
#include	"FileUtils.h"
#include	"Lightmap.h"

MetaClass	SceneDecoder :: classInstance ( "SceneDecoder", &ResourceDecoder :: classInstance );

bool	SceneDecoder :: checkExtension ( const String& theName )
{
	return String :: getExtension ( theName ).toLower () == "sc";
}

Object * SceneDecoder :: decode ( Data * data )
{
	if ( !data -> isAscii () )
		return NULL;

	Context	context;
	String	str;
	String	cmd;
	String	args;

	(*sysLog) << "Decoding scene file " << data -> getName () << logEndl;

	context.world = new World ( data -> getName () );

	for ( context.line = 1; getString ( context, data, str ); )
	{
		String :: parseString ( str, cmd, args );

		if ( cmd == "camera" )
		{
			if ( !getCamera ( context, args ) )
			{
				context.world -> release ();

				return NULL;
			}
		}
		else
		if ( cmd == "source" )
		{
			if ( !getSource ( context, args ) )
			{
				context.world -> release ();

				return NULL;
			}
		}
		else
		if ( cmd == "subscene" )
		{
			if ( !getSubScene ( context, args, data ) )
			{
				context.world -> release ();

				return NULL;
			}
		}
		else
		if ( cmd == "shader" )
		{
			Shader * shader = getShader ( context, data, args );

			if ( shader == NULL )
			{
				context.world -> release ();

				return NULL;
			}

			context.shaders.insertNoRetain ( shader );
		}
		else
		if ( cmd == "ambient" )
		{
			if ( !getVector3D ( context, World :: ambient, args ) )
			{
				context.world -> release ();

				return NULL;
			}
		}
		else
		if ( cmd != "" )
		{
			(*sysLog) << "Line " << context.line << " Illegal command: " << cmd << logEndl;

			context.world -> release ();

			return NULL;
		}

	}

	if ( !resolvePendingPortals ( context ) )
	{
		context.world -> release ();

		return NULL;
	}

	(*sysLog) << "Loading scene done." << logEndl;

	return context.world;
}

bool	SceneDecoder :: getString ( Context& context, Data * data, String& str )
{
	context.line++;

	if ( !data -> getString ( str ) )
		return false;

	int	pos = str.find ( "#" );

	if ( pos != -1 )
		str.setLength ( pos );

	str.trim ();

	if ( ( pos = str.find ( ";" ) ) != -1 )
		str.setLength ( pos );

	str.trim ();

	return true;
}

bool	SceneDecoder :: getCamera ( Context& context, const String& args )
{
	float	x, y, z;
	float	yaw, pitch, roll;

	if ( sscanf ( args, "position ( %f, %f, %f ) angles ( %f, %f, %f )", &x, &y, &z, &yaw, &pitch, &roll ) != 6 )
	{
		(*sysLog) << "Error reading camera command" << logEndl;

		return false;
	}

	context.world -> setStart ( Vector3D ( x, y, z ), yaw, pitch, roll );

	return true;
}

bool	SceneDecoder :: getSource ( Context& context, const String& srcName )
{
	String	name ( srcName );

	name.dequote ();

	ResourceSource * fs = NULL;
	String	         ext ( String :: getExtension ( name ).toLower () );

	if ( ext == "pak" )
		fs = new PakFileSystem ( name );
	else
	if ( ext == "zip" || ext == "pk3" )
		fs = new ZipFileSystem ( name );
	else
		fs = new OsFileSystem ( name );

	if ( !fs -> isOk () )
	{
		fs -> release ();

		(*sysLog) << "Source: line " << context.line << ", error creating filesystem " << name << logEndl;

		return false;
	}

	getResourceManager () -> registerSource ( fs );

	return true;
}

bool	SceneDecoder :: getSubScene ( Context& context, const String& name, Data * data )
{
	SubScene * scene = new StencilSubScene ( name );

	String	str, cmd, args;

	if ( !getString ( context, data, str ) )
	{
		(*sysLog) << "Unexpected end of file found. " << logEndl;

		scene -> release ();

		return false;
	}

	if ( str != "{" )
	{
		(*sysLog) << "Line " << context.line << " \'{\' expected." << logEndl;

		scene -> release ();

		return false;
	}

	for ( ; ; )
	{
		if ( !getString ( context, data, str ) )
		{
			(*sysLog) << "Unexpected end of file found. " << logEndl;

			scene -> release ();

			return false;
		}

		if ( str == "}" )
			break;

		bool	res = false;

		String :: parseString ( str, cmd, args );

		if ( cmd == "polygon" )
			res = getPolygon ( context, args, data, scene );
		else
		if ( cmd == "portal" )
			res = getPortal ( context, args, data, scene );
		else
		if ( cmd == "light" )
			res = getLight ( context, args, data, scene );
		else
		if ( cmd == "billboard" )
			res = getBillboard ( context, args, data, scene );
		else
		if ( cmd == "fog" )
			res = getFog ( context, args, data, scene );
		else
		if ( cmd == "linear_fog" )
			res = getLinearFog ( context, args, data, scene );
		else
		if ( cmd == "fountain" )
			res = getFountain ( context, args, data, scene );
		else
		if ( cmd == "magic-sphere" )
			res = getMagicSphere ( context, args, data, scene );
		else
		if ( cmd == "fire" )
			res = getFire ( context, args, data, scene );
		else
		if ( cmd == "snow" )
			res = getSnow ( context, args, data, scene );
		else
		if ( cmd == "mesh" )
			res = getMesh ( context, args, data, scene );
		else
		if ( cmd == "sky" )
		{
			Sky * sky = new Sky ( args, "tga" );

			if ( !sky -> isOk () )
			{
				sky -> release ();

				(*sysLog) << "Line " << context.line << " error loading sky" << logEndl;
			}
			else
			{
				scene -> setSky ( sky );

				res = true;
			}
		}
		else
		if ( cmd == "" )
			res = true;
		else
			(*sysLog) << "Unknown command at line " << context.line << " : " << cmd << logEndl;

		if ( !res )
		{
			scene -> release ();

			return false;
		}
	}

	context.world -> addScene ( scene );

	return true;
}

bool	SceneDecoder :: getPolygon ( Context& context, const String& name, Data * data, SubScene * scene )
{
	Polygon3D * poly   = new Polygon3D ( name, 4 );
	String	    str, cmd, args;

	poly -> setFlag ( PF_HAS_DETAIL_TEXTURE );

	if ( !getString ( context, data, str ) )
	{
		(*sysLog) << "Unexpected end of file found. " << logEndl;

		return false;
	}

	if ( str != "{" )
	{
		(*sysLog) << "Line " << context.line << " \'{\' expected." << logEndl;

		return false;
	}

	for ( ; ; )
	{
		if ( !getString ( context, data, str ) )
		{
			(*sysLog) << "Unexpected end of file found. " << logEndl;

			scene -> release ();

			return false;
		}

		if ( str == "}" )
			break;

		String :: parseString ( str, cmd, args );

		bool	res = false;

		if ( cmd == "texture" )
			res = getTexture ( context, poly, args );
		else
		if ( cmd == "shader" )
		{
			Shader    * shader;

			if ( ( shader = (Shader *)context.shaders.getObjectWithName ( args ) ) == NULL )
			{
				(*sysLog) << "Line " << context.line << " shader " << args << " not found." << logEndl;

				return false;
			}

			poly -> setShader ( shader );
		}
		else
		if ( cmd == "mapping" )
			res = getMapping ( context, poly, args );
		else
		if ( cmd == "color" )
		{
			Vector4D	color;

			if ( ( res = getVector4D ( context, color, args ) ) )
				poly -> setColor ( color );
		}
		else
		if ( cmd == "vertex" )
			res = getVertex ( context, poly, args );
		else
		if ( cmd == "style" )
			res = getStyle ( context, poly, args );
		else
		if ( cmd == "" )
			res = true;

		if ( !res )
		{
			poly -> release ();

			return false;
		}
	}

	scene -> addPoly ( poly );

	if ( !poly -> testFlag ( PF_NO_LIGHTMAP ) && !poly -> testFlag ( PF_MIRROR ) )
	{
		String	lightmapName ( context.world -> getLightmapNameForPoly ( poly ) );

		if ( fileExist ( lightmapName ) )
		{
			Texture * txt = getTexture ( context, lightmapName );

			if ( txt != NULL )
			{
				poly -> setLightmapTexture ( txt );
				txt  -> release            ();
			}
		}
	}

	return true;
}

bool	SceneDecoder :: getPortal ( Context& context, const String& name, Data * data, SubScene * scene )
{
	Portal  * poly = new Portal ( name, 4, scene );

	String	str, cmd, args;

	if ( !getString ( context, data, str ) )
	{
		(*sysLog) << "Unexpected end of file found. " << logEndl;

		return false;
	}

	if ( str != "{" )
	{
		(*sysLog) << "Line " << context.line << " \'{\' expected." << logEndl;

		return false;
	}

	for ( ; ; )
	{
		if ( !getString ( context, data, str ) )
		{
			(*sysLog) << "Unexpected end of file found. " << logEndl;

			scene -> release ();

			return false;
		}

		if ( str == "}" )
			break;

		String :: parseString ( str, cmd, args );

		bool	res = false;

		if ( cmd == "connect" )
			res = getConnection ( context, poly, args );
		else
		if ( cmd == "texture" )
			res = getTexture ( context, poly, args );
		else
		if ( cmd == "shader" )
		{
			Shader    * shader;

			if ( ( shader = (Shader *)context.shaders.getObjectWithName ( args ) ) == NULL )
			{
				(*sysLog) << "Line " << context.line << " shader " << args << " not found." << logEndl;

				return false;
			}

			poly -> setShader ( shader );
		}
		else
		if ( cmd == "mapping" )
			res = getMapping ( context, poly, args );
		else
		if ( cmd == "color" )
		{
			Vector4D	color;

			if ( ( res = getVector4D ( context, color, args ) ) )
				poly -> setColor ( color );
		}
		else
		if ( cmd == "vertex" )
			res = getVertex ( context, poly, args );
		else
		if ( cmd == "style" )
			res = getStyle ( context, poly, args );
		else
		if ( cmd == "transform" )
		{
			Transform3D	tr;

			if ( ( res = getTransform ( context, tr, args ) ) )
				poly -> setTransform ( new Transform3D ( tr ) );
		}
		else
		if ( cmd == "" )
			res = true;

		if ( !res )
		{
			poly -> release ();

			return false;
		}
	}
	
	scene -> addPoly    ( poly );

	return true;
}

bool	SceneDecoder :: getBillboard ( Context& context, const String& name, Data * data, SubScene * scene )
{
	Vector3D	pos   ( 0, 0, 0 );
	Vector4D	color ( 1, 1, 1, 1 );
	Texture	  * texture = NULL;
	float		width   = -1;
	float		height  = -1;

	String	str, cmd, args;

	if ( !getString ( context, data, str ) )
	{
		(*sysLog) << "Unexpected end of file found. " << logEndl;

		return false;
	}

	if ( str != "{" )
	{
		(*sysLog) << "Line " << context.line << " \'{\' expected." << logEndl;

		return false;
	}

	for ( ; ; )
	{
		if ( !getString ( context, data, str ) )
		{
			(*sysLog) << "Unexpected end of file found. " << logEndl;

			scene -> release ();

			return false;
		}

		if ( str == "" )
			continue;

		if ( str == "}" )
			break;

		String :: parseString ( str, cmd, args );

		if ( cmd == "" )
			continue;

		if ( cmd == "texture" )
			texture = getTexture ( context, args );
		else
		if ( cmd == "size" )
			width = height = args.toFloat ();
		else
		if ( cmd == "width" )
			width = args.toFloat ();
		else
		if ( cmd == "height" )
			height = args.toFloat ();
		else
		if ( cmd == "pos" )
		{
			if ( !getVector3D ( context, pos, args ) )
				return false;
		}
		else
		if ( cmd == "color" )
		{
			if ( !getVector4D ( context, color, args ) )
				return false;
		}
		else
		{
			(*sysLog) << "Line " << context.line << " Illegal command " << cmd << logEndl;

			return false;
		}
	}

	if ( texture == NULL || width < 0.0f || height < 0.0f )
	{
		(*sysLog) << "Line " << context.line << " missing billboard parameters" << logEndl;

		return false;
	}

	scene -> addObject ( new Billboard ( name, pos, color, texture, width, height ) );

	return true;
}

bool	SceneDecoder :: getFog ( Context& context, const String& name, Data * data, SubScene * scene )
{
	Vector3D	color ( 1, 1, 1 );
	Vector3D	n     ( 0, 0, 0 );
	float		dist;
	float		density  = -1;
	int			srcBlend = View :: bmSrcAlpha;
	int			dstBlend = View :: bmOneMinusSrcAlpha;

	String	str, cmd, args;

	if ( !getString ( context, data, str ) )
	{
		(*sysLog) << "Unexpected end of file found. " << logEndl;

		return false;
	}

	if ( str != "{" )
	{
		(*sysLog) << "Line " << context.line << " \'{\' expected." << logEndl;

		return false;
	}

	for ( ; ; )
	{
		if ( !getString ( context, data, str ) )
		{
			(*sysLog) << "Unexpected end of file found. " << logEndl;

			scene -> release ();

			return false;
		}

		if ( str == "}" )
			break;

		String :: parseString ( str, cmd, args );

		if ( cmd == "" )
			continue;

		if ( cmd == "density" )
			density = args.toFloat ();
		else
		if ( cmd == "color" )
		{
			if ( !getVector3D ( context, color, args ) )
				return false;
		}
		else
		if ( cmd == "plane" )
		{
			if ( sscanf ( args, "(%f, %f, %f) %f", &n.x, &n.y, &n.z, &dist ) != 4 )
			{
				(*sysLog) << "Line " << context.line << " illegal plane command" << logEndl;

				return false;
			}
		}
		else
		if ( cmd == "src-blend" )
		{
			if ( ( srcBlend = getBlendingMode ( args ) ) == -1 )
			{
				(*sysLog) << "Line " << context.line << " illegal blending mode: " << args << logEndl;

				return NULL;
			}
		}
		else
		if ( cmd == "dst-blend" )
		{
			if ( ( dstBlend = getBlendingMode ( args ) ) == -1 )
			{
				(*sysLog) << "Line " << context.line << " illegal blending mode: " << args << logEndl;

				return NULL;
			}
		}
		else
		{
			(*sysLog) << "Line " << context.line << " Illegal command " << cmd << logEndl;

			return false;
		}
	}

	if ( density < 0 || n.length () < EPS )
	{
		(*sysLog) << "Line " << context.line << " missing fog parameters" << logEndl;

		return false;
	}

	Fog * fog = new Fog ( name, new Plane ( n, dist ), color, density );

	fog   -> setSrcBlend ( srcBlend );
	fog   -> setDstBlend ( dstBlend );
	scene -> setFog      ( fog );

	return true;
}

bool	SceneDecoder :: getLinearFog ( Context& context, const String& name, Data * data, SubScene * scene )
{
	Vector3D	color ( 1, 1, 1 );
	Vector3D	grad  ( 0, 0, 0 );
	Vector3D	n     ( 0, 0, 0 );
	float		dist;
	float		offset;

	String	str, cmd, args;

	if ( !getString ( context, data, str ) )
	{
		(*sysLog) << "Unexpected end of file found. " << logEndl;

		return false;
	}

	if ( str != "{" )
	{
		(*sysLog) << "Line " << context.line << " \'{\' expected." << logEndl;

		return false;
	}

	for ( ; ; )
	{
		if ( !getString ( context, data, str ) )
		{
			(*sysLog) << "Unexpected end of file found. " << logEndl;

			scene -> release ();

			return false;
		}

		if ( str == "}" )
			break;

		String :: parseString ( str, cmd, args );

		if ( cmd == "" )
			continue;

		if ( cmd == "offset" )
			offset = args.toFloat ();
		else
		if ( cmd == "color" )
		{
			if ( !getVector3D ( context, color, args ) )
				return false;
		}
		else
		if ( cmd == "gradient" )
		{
			if ( !getVector3D ( context, grad, args ) )
				return false;
		}
		else
		if ( cmd == "plane" )
		{
			if ( sscanf ( args, "(%f, %f, %f) %f", &n.x, &n.y, &n.z, &dist ) != 4 )
			{
				(*sysLog) << "Line " << context.line << " illegal plane command" << logEndl;

				return false;
			}
		}
		else
		{
			(*sysLog) << "Line " << context.line << " Illegal command " << cmd << logEndl;

			return false;
		}
	}

	if ( grad.length () < EPS || n.length () < EPS )
	{
		(*sysLog) << "Line " << context.line << " missing fog parameters" << logEndl;

		return false;
	}

	scene -> setFog ( new LinearFog ( name, new Plane ( n, dist ), color, grad, offset ) );

	return true;
}

bool	SceneDecoder :: getMesh ( Context& context, const String& name, Data * data, SubScene * scene )
{
	Vector3D	pos   ( 0, 0, 0 );
	Vector4D	color ( 1, 1, 1, 1 );
	String		file;
	Transform3D	tr ( Transform3D :: getIdentity () );
	Texture   * texture     = NULL;
	Animator  * animator    = NULL;
	Shader    * shader      = NULL;
	float		scale       = 1;

	String	str, cmd, args;

	if ( !getString ( context, data, str ) )
	{
		(*sysLog) << "Unexpected end of file found. " << logEndl;

		return false;
	}

	if ( str != "{" )
	{
		(*sysLog) << "Line " << context.line << " \'{\' expected." << logEndl;

		return false;
	}

	for ( ; ; )
	{
		if ( !getString ( context, data, str ) )
		{
			(*sysLog) << "Unexpected end of file found. " << logEndl;

			scene -> release ();

			return false;
		}

		if ( str == "}" )
			break;

		String :: parseString ( str, cmd, args );

		if ( cmd == "" )
			continue;

		if ( cmd == "file" )
			file = args.trim ();
		else
		if ( cmd == "texture" )
			texture = getTexture ( context, args );
		else
		if ( cmd == "pos" )
		{
			if ( !getVector3D ( context, pos, args ) )
				return false;
		}
		else
		if ( cmd == "scale" )
			scale = args.toFloat ();
		else
		if ( cmd == "transform" )
		{
			if ( !getTransform ( context, tr, args ) )
				return false;
		}
		else
		if ( cmd == "color" )
		{
			if ( !getVector4D ( context, color, args ) )
				return false;
		}
		else
		if ( cmd == "animator" )
		{
			if ( ( animator = getAnimator ( args ) ) == NULL )
				return false;
		}
		else
		if ( cmd == "shader" )
		{
			if ( ( shader = (Shader *)context.shaders.getObjectWithName ( args ) ) == NULL )
			{
				(*sysLog) << "Line " << context.line << " shader " << args << " not found." << logEndl;

				return false;
			}
		}
		else
		{
			(*sysLog) << "Line " << context.line << " Illegal command " << cmd << logEndl;

			return false;
		}
	}

	if ( file == "" )
	{
		(*sysLog) << "Line " << context.line << " missing mesh file" << logEndl;

		return false;
	}

	Object * object = getResourceManager () -> getObject ( file.dequote () );

	if ( object == NULL )
	{
		(*sysLog) << "Line " << context.line << " cannot load mesh " << file << logEndl;

		return false;
	}
											// now try to figure out what we've got
	Mesh3D       * mesh  = dynamic_cast <Mesh3D *>       ( object );
	VisualObject * vis   = dynamic_cast <VisualObject *> ( object );
	Array        * array = dynamic_cast <Array *>        ( object );

	if ( array != NULL )
	{
		mesh = dynamic_cast <Mesh3D *> ( array -> at ( 0 ) );
	}
	
	if ( mesh != NULL )						// it's a Mesh3D object
	{
		MeshObject * meshObject = new MeshObject ( name, pos, color, tr * Transform3D ::getScale ( scale ), mesh );

		if ( animator != NULL )
			meshObject -> addAnimator ( animator );

		if ( shader != NULL )
			meshObject -> setShader ( shader );
		else
			mesh  -> setTexture ( texture );

		scene -> addObject  ( meshObject );
	}
	else
	if ( vis != NULL )						// it's a VisualObject
	{
		vis -> setName      ( name );
		vis -> setPos       ( pos );
		vis -> setColor     ( color );
		vis -> setTransform ( tr * Transform3D :: getScale ( scale ) );
		vis -> init         ();

		if ( animator != NULL )
			vis -> addAnimator ( animator );

											// try to apply texture/shader on object
		if ( texture != NULL || shader != NULL )	
		{
			MeshObject      * m = dynamic_cast <MeshObject      *> ( object );
			CompositeObject * c = dynamic_cast <CompositeObject *> ( object );

			if ( m != NULL )
			{
				if ( shader != NULL )
					m -> setShader ( shader );
				else
					m -> setTexture ( texture );
			}
			else
			{
				if ( shader != NULL )
					c -> setShader ( shader );
				else
					c -> setTexture ( texture );
			}
		}

		scene -> addObject ( vis );
	}
	else									// we don't know what it is
	{
		(*sysLog) << "Line " << context.line << " cannot get mesh out of object" << logEndl;

		object -> release ();

		return false;
	}

	return true;
}

bool	SceneDecoder :: getFountain ( Context& context, const String& name, Data * data, SubScene * scene )
{
	Vector3D	pos     ( 0, 0, 0 );
	Vector3D	dir     ( 0, 1, 0 );
	Vector3D	wind    ( 0, 0, 0 );
	Vector3D	gravity ( 0, -0.05f, 0 );
	Vector4D	color   ( 1, 1, 1, 1 );

	Texture	  * texture            = NULL;
	int			particlesPerSecond = -1;
	int			srcBlend           = View :: bmSrcAlpha;
	int			dstBlend           = View :: bmOne;
	float		dispersion         = -1;
	float		lifeTime           = -1;

	String	str, cmd, args;

	if ( !getString ( context, data, str ) )
	{
		(*sysLog) << "Unexpected end of file found. " << logEndl;

		return false;
	}

	if ( str != "{" )
	{
		(*sysLog) << "Line " << context.line << " \'{\' expected." << logEndl;

		return false;
	}

	for ( ; ; )
	{
		if ( !getString ( context, data, str ) )
		{
			(*sysLog) << "Unexpected end of file found. " << logEndl;

			scene -> release ();

			return false;
		}

		if ( str == "}" )
			break;

		String :: parseString ( str, cmd, args );

		if ( cmd == "" )
			continue;

		if ( cmd == "texture" )
			texture = getTexture ( context, args );
		else
		if ( cmd == "particles-per-second" )
			particlesPerSecond = args.toInt ();
		else
		if ( cmd == "life-time" )
			lifeTime = args.toFloat ();
		else
		if ( cmd == "dispersion" )
			dispersion = args.toFloat ();
		else
		if ( cmd == "pos" )
		{
			if ( !getVector3D ( context, pos, args ) )
				return false;
		}
		else
		if ( cmd == "dir" )
		{
			if ( !getVector3D ( context, dir, args ) )
				return false;
		}
		else
		if ( cmd == "wind" )
		{
			if ( !getVector3D ( context, wind, args ) )
				return false;
		}
		else
		if ( cmd == "color" )
		{
			if ( !getVector4D ( context, color, args ) )
				return false;
		}
		else
		if ( cmd == "src-blend" )
		{
			if ( ( srcBlend = getBlendingMode ( args ) ) == -1 )
				return false;

		}
		else
		if ( cmd == "dst-blend" )
		{
			if ( ( dstBlend = getBlendingMode ( args ) ) == -1 )
				return false;

		}
		else
		{
			(*sysLog) << "Fountain loader : line " << context.line << " Illegal command " << cmd << logEndl;

			return false;
		}
	}

	if ( texture == NULL || particlesPerSecond < 0 || dispersion < 0 || lifeTime < 0 )
	{
		(*sysLog) << "Line " << context.line << " missing fountain parameters" << logEndl;

		return false;
	}

	ParticleSystem * ps = new Fountain ( name, pos, particlesPerSecond, texture, dir, gravity, wind, dispersion, lifeTime, color );

	ps    -> setBlendingMode ( srcBlend, dstBlend );
	scene -> addObject ( ps );

	return true;
}

bool	SceneDecoder :: getSnow ( Context& context, const String& name, Data * data, SubScene * scene )
{
	Vector3D	pos   ( 0, 0, 0 );
	Vector3D	wind  ( 0, 0, 0 );
	Vector3D	vel   ( 0, -0.025f, 0 );
	Vector3D	n     ( 0, 0, 0 );
	Vector4D	color ( 1, 1, 1, 1 );

	Texture	  * texture = NULL;
	int			particlesPerSecond = -1;
	int			srcBlend           = View :: bmSrcAlpha;
	int			dstBlend           = View :: bmOne;
	float		radius  = -1;
	float		size    = -1;
	float		dist;

	String	str, cmd, args;

	if ( !getString ( context, data, str ) )
	{
		(*sysLog) << "Unexpected end of file found. " << logEndl;

		return false;
	}

	if ( str != "{" )
	{
		(*sysLog) << "Line " << context.line << " \'{\' expected." << logEndl;

		return false;
	}

	for ( ; ; )
	{
		if ( !getString ( context, data, str ) )
		{
			(*sysLog) << "Unexpected end of file found. " << logEndl;

			scene -> release ();

			return false;
		}

		if ( str == "}" )
			break;

		String :: parseString ( str, cmd, args );

		if ( cmd == "" )
			continue;

		if ( cmd == "pos" )
		{
			if ( !getVector3D ( context, pos, args ) )
				return false;
		}
		else
		if ( cmd == "texture" )
			texture = getTexture ( context, args );
		else
		if ( cmd == "velocity" )
		{
			if ( !getVector3D ( context, vel, args ) )
				return false;
		}
		else
		if ( cmd == "wind" )
		{
			if ( !getVector3D ( context, wind, args ) )
				return false;
		}
		else
		if ( cmd == "size" )
			size = args.toFloat ();
		else
		if ( cmd == "radius" )
			radius = args.toFloat ();
		else
		if ( cmd == "particles-per-second" )
			particlesPerSecond = args.toInt ();
		else
		if ( cmd == "plane" )
		{
			if ( sscanf ( args, "( %f, %f, %f ) %f", &n.x, &n.y, &n.z, &dist ) != 4 )
			{
				(*sysLog) << "Line " << context.line << " illegal plane command" << logEndl;

				return false;
			}
		}
		else
		if ( cmd == "color" )
		{
			if ( !getVector4D ( context, color, args ) )
				return false;
		}
		else
		if ( cmd == "src-blend" )
		{
			if ( ( srcBlend = getBlendingMode ( args ) ) == -1 )
				return false;

		}
		else
		if ( cmd == "dst-blend" )
		{
			if ( ( dstBlend = getBlendingMode ( args ) ) == -1 )
				return false;

		}
		else
		{
			(*sysLog) << "Line " << context.line << " Illegal command " << cmd << logEndl;

			return false;
		}
	}

	if ( texture == NULL || particlesPerSecond < 0 || radius < 0 || size < 0 )
	{
		(*sysLog) << "Line " << context.line << " missing snow parameters" << logEndl;

		return false;
	}

	ParticleSystem * ps = new Snow ( name, pos, particlesPerSecond, texture, vel, wind, size, radius, Plane ( n, dist ), color );

	ps    -> setBlendingMode ( srcBlend, dstBlend );
	scene -> addObject ( ps );

	return true;
}

bool	SceneDecoder :: getFire ( Context& context, const String& name, Data * data, SubScene * scene )
{
	Vector3D	pos     ( 0, 0, 0 );
	Vector3D	dir     ( 0, 1, 0 );
	Vector3D	wind    ( 0, 0, 0 );
	Vector3D	gravity ( 0, -0.025f, 0 );

	Texture	  * texture            = NULL;
	int			particlesPerSecond = -1;
	float		lifeTime           = -1;
	float		radius             = -1;
	float		speed              = -1;
	int			srcBlend           = View :: bmSrcAlpha;
	int			dstBlend           = View :: bmOne;
	Vector4D	color1 ( 1, 1, 0, 1 );
	Vector4D	color2 ( 0.5f, 0, 0, 0.3f );

	String	str, cmd, args;

	if ( !getString ( context, data, str ) )
	{
		(*sysLog) << "Unexpected end of file found. " << logEndl;

		return false;
	}

	if ( str != "{" )
	{
		(*sysLog) << "Line " << context.line << " \'{\' expected." << logEndl;

		return false;
	}

	for ( ; ; )
	{
		if ( !getString ( context, data, str ) )
		{
			(*sysLog) << "Unexpected end of file found. " << logEndl;

			scene -> release ();

			return false;
		}

		if ( str == "}" )
			break;

		String :: parseString ( str, cmd, args );

		if ( cmd == "" )
			continue;

		if ( cmd == "texture" )
			texture = getTexture ( context, args );
		else
		if ( cmd == "particles-per-second" )
			particlesPerSecond = args.toInt ();
		else
		if ( cmd == "life-time" )
			lifeTime = args.toFloat ();
		else
		if ( cmd == "radius" )
			radius = args.toFloat ();
		else
		if ( cmd == "speed" )
			speed = args.toFloat ();
		else
		if ( cmd == "pos" )
		{
			if ( !getVector3D ( context, pos, args ) )
				return false;
		}
		else
		if ( cmd == "src-color" )
		{
			if ( !getVector4D ( context, color1, args ) )
				return false;
		}
		else
		if ( cmd == "dst-color" )
		{
			if ( !getVector4D ( context, color2, args ) )
				return false;
		}
		else
		if ( cmd == "src-blend" )
		{
			if ( ( srcBlend = getBlendingMode ( args ) ) == -1 )
				return false;

		}
		else
		if ( cmd == "dst-blend" )
		{
			if ( ( dstBlend = getBlendingMode ( args ) ) == -1 )
				return false;

		}
		else
		{
			(*sysLog) << "Line " << context.line << " Illegal command " << cmd << logEndl;

			return false;
		}
	}

	if ( texture == NULL || particlesPerSecond < 0 || radius < 0 || lifeTime < 0 || speed < 0 )
	{
		(*sysLog) << "Line " << context.line << " missing fountain parameters" << logEndl;

		return false;
	}

	ParticleSystem * ps = new Fire ( name, pos, particlesPerSecond, lifeTime, radius, speed, texture, color1, color2 );

	ps    -> setBlendingMode ( srcBlend, dstBlend );
	scene -> addObject ( ps );

	return true;
}

bool	SceneDecoder :: getMagicSphere ( Context& context, const String& name, Data * data, SubScene * scene )
{
	Vector3D	pos     ( 0, 0, 0 );
	Vector3D	dir     ( 0, 1, 0 );
	Vector3D	wind    ( 0, 0, 0 );
	Vector3D	gravity ( 0, -0.025f, 0 );

	Texture	  * texture      = NULL;
	int			numParticles = -1;
	int			srcBlend     = View :: bmSrcAlpha;
	int			dstBlend     = View :: bmOne;
	float		startRadius  = -1;
	float		endRadius    = -1;
	float		speed        = -1;
	float		minSize      = -1;
	float		maxSize      = -1;
	bool		pulsate      = true;
	float		randomness   = 0;
	Vector4D	color1 ( 1, 1, 1, 1 );
	Vector4D	color2 ( 0, 0, 0, 0 );

	String	str, cmd, args;

	if ( !getString ( context, data, str ) )
	{
		(*sysLog) << "Unexpected end of file found. " << logEndl;

		return false;
	}

	if ( str != "{" )
	{
		(*sysLog) << "Line " << context.line << " \'{\' expected." << logEndl;

		return false;
	}

	for ( ; ; )
	{
		if ( !getString ( context, data, str ) )
		{
			(*sysLog) << "Unexpected end of file found. " << logEndl;

			scene -> release ();

			return false;
		}

		if ( str == "}" )
			break;

		String :: parseString ( str, cmd, args );

		if ( cmd == "" )					// skip empty lines
			continue;

		if ( cmd == "texture" )
			texture = getTexture ( context, args );
		else
		if ( cmd == "num-particles" )
			numParticles = args.toInt ();
		else
		if ( cmd == "start-radius" )
			startRadius = args.toFloat ();
		else
		if ( cmd == "end-radius" )
			endRadius = args.toFloat ();
		else
		if ( cmd == "speed" )
			speed = args.toFloat ();
		else
		if ( cmd == "min-size" )
			minSize = args.toFloat ();
		else
		if ( cmd == "max-size" )
			maxSize = args.toFloat ();
		else
		if ( cmd == "randomness" )
			randomness = args.toFloat ();
		else
		if ( cmd == "pulsate" )
			pulsate = (args == "yes" );
		else
		if ( cmd == "pos" )
		{
			if ( !getVector3D ( context, pos, args ) )
				return false;
		}
		else
		if ( cmd == "start-color" )
		{
			if ( !getVector4D ( context, color1, args ) )
				return false;
		}
		else
		if ( cmd == "end-color" )
		{
			if ( !getVector4D ( context, color2, args ) )
				return false;
		}
		else
		if ( cmd == "src-blend" )
		{
			if ( ( srcBlend = getBlendingMode ( args ) ) == -1 )
				return false;

		}
		else
		if ( cmd == "dst-blend" )
		{
			if ( ( dstBlend = getBlendingMode ( args ) ) == -1 )
				return false;

		}
		else
		{
			(*sysLog) << "Line " << context.line << " Illegal command " << cmd << logEndl;

			return false;
		}
	}

	if ( texture == NULL || numParticles < 0 || startRadius < 0 || endRadius < 0 || speed < 0 )
	{
		(*sysLog) << "Line " << context.line << " missing magic sphere  parameters" << logEndl;

		return false;
	}

	ParticleSystem * ps = new MagicSphere ( name, pos, numParticles, minSize, maxSize, startRadius, endRadius, speed, texture, color1, color2, pulsate, randomness );
	
	ps    -> setBlendingMode ( srcBlend, dstBlend );
	scene -> addObject ( ps );

	return true;
}
/*
bool	SceneDecoder :: getExplosion ( const String& name, Data * data, SubScene * scene )
{
	Vector3D	pos   ( 0, 0, 0 );
	Vector4D	color ( 1, 1, 1, 1 );

	Texture	  * texture      = NULL;
	float		period       = -1;
	float		decaySpeed   = -1;
	float		sizeSpeed    = -1;
	float		randomness   = -1;
	int			numEmitters  = -1;

	String	str, cmd, args;

	if ( !getString ( data, str ) )
	{
		(*sysLog) << "Unexpected end of file found. " << logEndl;

		return false;
	}

	if ( str != "{" )
	{
		(*sysLog) << "Line " << line << " \'{\' expected." << logEndl;

		return false;
	}

	for ( ; ; )
	{
		if ( !getString ( data, str ) )
		{
			(*sysLog) << "Unexpected end of file found. " << logEndl;

			scene -> release ();

			return false;
		}

		if ( str == "}" )
			break;

		String :: parseString ( str, cmd, args );

		if ( cmd == "" )					// skip empty lines
			continue;

		if ( cmd == "texture" )
			texture = getTexture ( args );
		else
		if ( cmd == "num-emitters" )
			numEmitters = args.toInt ();
		else
		if ( cmd == "period" )
			period = args.toFloat ();
		else
		if ( cmd == "decay-speed" )
			decaySpeed = args.toFloat ();
		else
		if ( cmd == "size-speed" )
			sizeSpeed = args.toFloat ();
		else
		if ( cmd == "randomness" )
			randomness = args.toFloat ();
		else
		if ( cmd == "pos" )
		{
			if ( !getVector3D ( pos, args ) )
				return false;
		}
		else
		if ( cmd == "color" )
		{
			if ( !getVector4D ( color, args ) )
				return false;
		}
		else
		{
			(*sysLog) << "Line " << line << " Illegal command " << cmd << logEndl;

			return false;
		}
	}

	if ( lmGen )
		return true;

	if ( texture == NULL || numEmitters < 0 || period < 0 || decaySpeed < 0 || sizeSpeed < 0 || randomness < 0 )
	{
		(*sysLog) << "Line " << line << " missing magic sphere  parameters" << logEndl;

		return false;
	}

	Explosion * ex = new Explosion ( name, pos, color, texture, period, numEmitters, decaySpeed, sizeSpeed, randomness );
	
	scene -> addObject ( ex );

	return true;
}
*/

bool	SceneDecoder :: getLight ( Context& context, const String& name, Data * data, SubScene * scene )
{
	Vector3D	pos   ( 0, 0, 0 );
	Vector4D	color ( 1, 1, 1, 1 );
	float		brightness = 1;
	float		radius     = 10;
	bool		hasHalo    = false;
	bool		hasFlare   = false;
	float		constant   = 1;
	float		linear     = 0;
	float		square     = 1;

	String	str, cmd, args;

	if ( !getString ( context, data, str ) )
	{
		(*sysLog) << "Unexpected end of file found. " << logEndl;

		return false;
	}

	if ( str != "{" )
	{
		(*sysLog) << "Line " << context.line << " \'{\' expected." << logEndl;

		return false;
	}

	for ( ; ; )
	{
		if ( !getString ( context, data, str ) )
		{
			(*sysLog) << "Unexpected end of file found. " << logEndl;

			scene -> release ();

			return false;
		}

		if ( str == "}" )
			break;

		String :: parseString ( str, cmd, args );

		if ( cmd == "" )					// skip empty lines
			continue;

		if ( cmd == "brightness" )
			brightness = args.toFloat ();
		else
		if ( cmd == "radius" )
			radius = args.toFloat ();
		else
		if ( cmd == "halo" )
			hasHalo = (args == "yes" || args == "on" );
		else
		if ( cmd == "lens-flare" )
			hasFlare = (args == "yes" || args == "on" );
		else
		if ( cmd == "constant" )
			constant = args.toFloat ();
		else
		if ( cmd == "linear" )
			linear = args.toFloat ();
		else
		if ( cmd == "square" )
			square = args.toFloat ();
		else
		if ( cmd == "pos" )
		{
			if ( !getVector3D ( context, pos, args ) )
				return false;
		}
		else
		if ( cmd == "color" )
		{
			if ( !getVector4D ( context, color, args ) )
				return false;
		}
		else
		{
			(*sysLog) << "Line " << context.line << " Illegal command " << cmd << logEndl;

			return false;
		}
	}

	Light * light = new Light ( name, pos, color, brightness, radius, constant, linear, square );

	if ( hasHalo )
		light -> setFlag ( LF_HAS_HALO );

	if ( hasFlare )
		light -> setFlag ( LF_HAS_LENS_FLARE );

	scene -> addObject ( light );

	return true;
}

bool	SceneDecoder :: getVector2D ( Context& context, Vector2D& v, const String& str )
{
	if ( sscanf ( str, "(%f, %f)", &v.x, &v.y ) == 2 )
		return true;

	if ( sscanf ( str, "(%f %f)", &v.x, &v.y ) == 2 )
		return true;

	(*sysLog) << "Line " << context.line << " illegal Vector2D" << logEndl;

	return false;
}

bool	SceneDecoder :: getVector3D ( Context& context, Vector3D& v, const String& str )
{
	if ( sscanf ( str, "(%f, %f, %f)", &v.x, &v.y, &v.z ) == 3 )
		return true;

	if ( sscanf ( str, "(%f %f %f)", &v.x, &v.y, &v.z ) == 3 )
		return true;

	(*sysLog) << "Line " << context.line << " illegal Vector3D" << logEndl;

	return false;
}

bool	SceneDecoder :: getVector4D ( Context& context, Vector4D& v, const String& str )
{
	if (  sscanf ( str, "(%f, %f, %f, %f)", &v.x, &v.y, &v.z, &v.w ) == 4 )
		return true;

	if (  sscanf ( str, "(%f %f %f %f)", &v.x, &v.y, &v.z, &v.w ) == 4 )
		return true;

	(*sysLog) << "Line " << context.line << " illegal Vector4D" << logEndl;

	return false;
}

Texture * SceneDecoder :: getTexture ( Context& context, const String& theName )
{
	String	name ( theName );

	Texture * txt = getResourceManager () -> getTexture ( name.dequote () );

	if ( txt == NULL )
		(*sysLog) << "Line " << context.line << " Texture " << name << " not found." << logEndl;

	return txt;
}

bool	SceneDecoder :: getTexture ( Context& context, Polygon3D * poly, const String& str )
{
	Texture * texture = getTexture ( context, str );

	if ( texture == NULL )
		return false;

	poly -> setTexture ( texture );

	return true;
}

bool	SceneDecoder :: getMapping ( Context& context, Polygon3D * poly, const String& str )
{
	Vector3D	u, v;
	float		uOffs, vOffs;

	if ( sscanf ( str, "(%f %f %f) %f (%f %f %f) %f",  
				  &u.x, &u.y, &u.z, &uOffs,
				  &v.x, &v.y, &v.z, &vOffs ) != 8 )
	{
		(*sysLog) << "Line " << context.line << " mapping comand missing parameters" << logEndl;

		return false;
	}

	Mapping	  mapping ( u, uOffs, v, vOffs );

	poly -> setMapping ( mapping );

	return true;
}

bool	SceneDecoder :: getVertex ( Context& context, Polygon3D * poly, const String& str )
{
	Vector3D	vertex;
	Vector4D	color;
	Vector2D	uv;

	if ( sscanf ( str, "(%f, %f, %f),(%f, %f),(%f, %f, %f, %f)", &vertex.x, &vertex.y, &vertex.z,
														         &uv.x, &uv.y,
																 &color.x, &color.y, &color.z, &color.w ) == 9 )
	{
		poly -> addVertex ( vertex, uv, color );

		return true;
	}

	if ( sscanf ( str, "(%f %f %f),(%f %f),(%f %f %f %f)", &vertex.x, &vertex.y, &vertex.z,
														   &uv.x, &uv.y,
														   &color.x, &color.y, &color.z, &color.w ) == 9 )
	{
		poly -> addVertex ( vertex, uv, color );

		return true;
	}

	if ( sscanf ( str, "(%f, %f, %f),(%f,%f)", &vertex.x, &vertex.y, &vertex.z, &uv.x, &uv.y ) == 5 )
	{
		poly -> addVertex ( vertex, uv );

		return true;
	}

	if ( sscanf ( str, "(%f %f %f),(%f %f)", &vertex.x, &vertex.y, &vertex.z, &uv.x, &uv.y ) == 5 )
	{
		poly -> addVertex ( vertex, uv );

		return true;
	}

	if ( sscanf ( str, "(%f, %f, %f)", &vertex.x, &vertex.y, &vertex.z ) == 3 )
	{
		poly -> addVertex ( vertex );

		return true;
	}

	if ( sscanf ( str, "(%f %f %f)", &vertex.x, &vertex.y, &vertex.z ) == 3 )
	{
		poly -> addVertex ( vertex );

		return true;
	}

	(*sysLog) << "Line " << context.line << " invalid vertex definition" << logEndl;

	return false;
}

bool	SceneDecoder :: getStyle ( Context& context, Polygon3D * poly, const String& str )
{
	String	s ( str );
	String	style, res;
	int		st = PF_HAS_DETAIL_TEXTURE;

	for ( ; ; )
	{
		String :: parseString ( s, style, res );

		if ( style == "" )
			break;

		s = res;
		s.trim ();

		if ( style == "two-sided" )
			st |= PF_TWOSIDED;
		else
		if ( style == "floating" )
			st |= PF_FLOATING;
		else
		if ( style == "passable" )
			st |= PF_PASSABLE;
		else
		if ( style == "mirror" )
			st |= PF_MIRROR;
		else
		if ( style == "no-detail" )
			st &= ~PF_HAS_DETAIL_TEXTURE;
		else
		if ( style == "no-lightmap" )
			st |= PF_NO_LIGHTMAP;
		else
		{
			(*sysLog) << "Line " << context.line << " - illegal style: " << style << logEndl;

			return false;
		}
	}

	poly -> setFlag ( st );

	return true;
}

bool	SceneDecoder :: getTransform ( Context& context, Transform3D& tr, const String& s )
{
	String	str ( s );
	String	cmd, args;
	int		count = 0;

	tr = Transform3D :: getIdentity ();

	while ( str != "" )
	{
		String :: parseString ( str, cmd, args );

		if ( cmd == "translate" )
		{
			Vector3D	v;

			if ( sscanf ( args, "( %f, %f, %f )%n", &v.x, &v.y, &v.z, &count ) != 3 )
			{
				(*sysLog) << "Line " << context.line << " cannot load translate params" << logEndl;

				return false;
			}

			tr = tr * Transform3D :: getTranslate ( v );
		}
		else
		if ( cmd == "rotate_x" )
		{
			float	angle;

			if ( sscanf ( args, "%f%n", &angle, &count ) != 1 )
			{
				(*sysLog) << "Line " << context.line << " cannot load nagle in rotate" << logEndl;

				return false;
			}

			tr = tr * Transform3D :: getRotateX ( angle * M_PI / 180.0f );
		}
		else
		if ( cmd == "rotate_y" )
		{
			float	angle;

			if ( sscanf ( args, "%f%n", &angle, &count ) != 1 )
			{
				(*sysLog) << "Line " << context.line << " cannot load nagle in rotate" << logEndl;

				return false;
			}

			tr = tr * Transform3D :: getRotateY ( angle * M_PI / 180.0f );
		}
		else
		if ( cmd == "rotate_z" )
		{
			float	angle;

			if ( sscanf ( args, "%f%n", &angle, &count ) != 1 )
			{
				(*sysLog) << "Line " << context.line << " cannot load nagle in rotate" << logEndl;

				return false;
			}

			tr = tr * Transform3D :: getRotateZ ( angle * M_PI / 180.0f );
		}
		else
		{
			(*sysLog) << "Line " << context.line << " illegal command in getTransform : " << cmd << logEndl;

			return false;
		}

		str = args.substr ( count );
		str.trim ();
	}

	return true;
}

bool	SceneDecoder :: getTransform2D ( Context& context, Transform2D& tr, const String& s )
{
	String	str ( s );
	String	cmd, args;
	int		count = 0;

	tr = Transform2D :: getIdentity ();

	while ( str != "" )
	{
		String :: parseString ( str, cmd, args );

		if ( cmd == "translate" )
		{
			Vector2D	v;

			if ( sscanf ( args, "( %f, %f)%n", &v.x, &v.y, &count ) != 3 )
			{
				(*sysLog) << "Line " << context.line << " cannot load translate params" << logEndl;

				return false;
			}

			tr = tr * Transform2D :: getTranslate ( v );
		}
		else
		if ( cmd == "rotate" )
		{
			float	angle;

			if ( sscanf ( args, "%f%n", &angle, &count ) != 2 )
			{
				(*sysLog) << "Line " << context.line << " cannot load nagle in rotate" << logEndl;

				return false;
			}

			tr = tr * Transform2D :: getRotate ( angle * M_PI / 180.0f );
		}
		else
		if ( cmd == "scale" )
		{
			Vector2D	v;

			if ( sscanf ( args, "( %f, %f)%n", &v.x, &v.y, &count ) != 3 )
			{
				(*sysLog) << "Line " << context.line << " cannot load translate params" << logEndl;

				return false;
			}

			tr = tr * Transform2D :: getScale ( v );
		}
		else
		{
			(*sysLog) << "Line " << context.line << " illegal command in getTransform : " << cmd << logEndl;

			return false;
		}

		str = args.substr ( count );
		str.trim ();
	}

	return true;
}

bool	SceneDecoder :: getConnection ( Context& context, Portal * portal, const String& name )
{
	SubScene * scene =context. world -> getSubScene ( name );

	if ( scene != NULL )
		portal -> setDestScene ( scene );
	else
	{											// pending portal
		portal -> setDestScene ( (SubScene *) new String ( name ) );	// it's a dirty hack off course
																		// but it works ok

		context.pending.insert ( portal );
	}

	return true;
}

bool	SceneDecoder :: resolvePendingPortals ( Context& context )
{
	for ( Array :: Iterator it = context.pending.getIterator (); !it.end (); ++it )
	{
		Portal   * portal    = (Portal *) it.value ();
		String   * sceneName = (String *) portal -> getDestScene ();
		SubScene * scene     = context.world -> getSubScene ( *sceneName );

		if ( scene == NULL )
		{
			(*sysLog) << "Unresolved portal reference: Portal " << portal -> getName () << " ref " << *sceneName << logEndl;

			return false;
		}

		portal    -> setDestScene ( scene );
		sceneName -> release ();
	}

	context.pending.removeAll ();

	return true;
}

Animator * SceneDecoder :: getAnimator ( const String& str )
{
	Vector3D	axis;
	float		speed;
	float		phase;

	if ( sscanf ( str, "( %f, %f, %f ) %f %f", &axis.x, &axis.y, &axis.z, &speed, &phase ) == 5 )
		return new RotationAnimator ( "animator", axis, speed, phase );

	if ( sscanf ( str, "( %f, %f, %f ) %f", &axis.x, &axis.y, &axis.z, &speed ) == 4 )
		return new RotationAnimator ( "animator", axis, speed );

	return NULL;
}

Shader * SceneDecoder :: getShader ( Context& context, Data * data, const String& name )
{
	Shader	* shader = new Shader ( name );
	String	  str, cmd, args;

	if ( !getString ( context, data, str ) )
	{
		(*sysLog) << "Unexpected end of file found. " << logEndl;

		return NULL;
	}

	if ( str != "{" )
	{
		(*sysLog) << "Line " << context.line << " \'{\' expected." << logEndl;

		return NULL;
	}

	for ( ; ; )
	{
		if ( !getString ( context, data, str ) )
		{
			(*sysLog) << "Unexpected end of file found. " << logEndl;

			return NULL;
		}

		if ( str == "}" )
			break;

		String :: parseString ( str, cmd, args );

		if ( cmd == "pass" )
		{
			ShaderPass * pass = getShaderPass ( context, data, args );

			if ( pass == NULL )
			{
				shader -> release ();

				return NULL;
			}

			shader -> addPass ( pass );
		}
		else
		if ( cmd != "" )
		{
			(*sysLog) << "Line " << context.line << " illegal command " << cmd << logEndl;

			shader -> release ();

			return NULL;
		}
	}

	return shader;
}

ShaderPass * SceneDecoder :: getShaderPass ( Context& context, Data * data, const String& name )
{
	Texture	  * texture  = NULL;
	bool		env      = false;
	int			srcBlend = View :: bmNone;
	int			dstBlend = View :: bmNone;
	Vector4D	color     ( 1, 1, 1, 1 );
	Transform2D	transform ( Transform2D :: getIdentity () );

	String	str, cmd, args;

	if ( !getString ( context, data, str ) )
	{
		(*sysLog) << "Unexpected end of file found. " << logEndl;

		return NULL;
	}

	if ( str != "{" )
	{
		(*sysLog) << "Line " << context.line << " \'{\' expected." << logEndl;

		return NULL;
	}

	for ( ; ; )
	{
		if ( !getString ( context, data, str ) )
		{
			(*sysLog) << "Line " << context.line << " unexpected end of file found. " << logEndl;

			return NULL;
		}

		if ( str == "}" )
			break;

		String :: parseString ( str, cmd, args );

		if ( cmd == "texture" )
			texture = getTexture ( context, args );
		else
		if ( cmd == "mapping" )
		{
			if ( args == "env" )			// only one mode supported now
				env = true;
			else
			{
				(*sysLog) << "Line " << context.line << " - unexpected mapping mode " << args << "found." << logEndl;
	
				return NULL;
			}
		}
		else
		if ( cmd == "color" )
		{
			if ( !getVector4D ( context, color, args ) )
				return NULL;
		}
		else
		if ( cmd == "transform" )
		{
			if ( !getTransform2D ( context, transform, args ) )
			{
				(*sysLog) << "Line " << context.line << " ShaderPass: error getting transform" << logEndl;

				return NULL;
			}
		}
		else
		if ( cmd == "src-blend" )
		{
			if ( ( srcBlend = getBlendingMode ( args ) ) == -1 )
			{
				(*sysLog) << "Line " << context.line << " illegal blending mode: " << args << logEndl;

				return NULL;
			}
		}
		else
		if ( cmd == "dst-blend" )
		{
			if ( ( dstBlend = getBlendingMode ( args ) ) == -1 )
			{
				(*sysLog) << "Line " << context.line << " illegal blending mode: " << args << logEndl;

				return NULL;
			}
		}
		else
		{
			(*sysLog) << "Line " << context.line << " Illegal command " << cmd << logEndl;

			return NULL;
		}
	}

	if ( texture == NULL  )
	{
		(*sysLog) << "Line " << context.line << " missing shader pass parameters" << logEndl;

		return NULL;
	}

	ShaderPass * pass = new ShaderPass ( name, texture, color, env );

	pass -> setBlendingMode ( srcBlend, dstBlend );
	pass -> setTransform    ( transform );

	return pass;
}	

int	SceneDecoder :: getBlendingMode ( const String& str )
{
	if ( str == "zero" )
		return View :: bmZero;

	if ( str == "one" )
		return View :: bmOne;

	if ( str == "src-alpha" )
		return View :: bmSrcAlpha;

	if ( str == "one-minus-src-alpha" )
		return View :: bmOneMinusSrcAlpha;

	if ( str == "src-color" )
		return View :: bmSrcColor;

	if ( str == "one-minus-src-color" )
		return View :: bmOneMinusSrcColor;

	if ( str == "dst-alpha" )
		return View :: bmDstAlpha;

	if ( str == "one-minus-dst-alpha" )
		return View :: bmOneMinusDstAlpha;

	if ( str == "dst-color" )
		return View :: bmDstColor;

	if ( str == "one-minus-dst-color" )
		return View :: bmOneMinusDstColor;

	return View :: bmNone;
}

bool	SceneDecoder :: getPlane ( Context& context, Plane& plane, const String& str )
{
	Vector3D	n;
	float		dist;

	if ( sscanf ( str, "(%f, %f, %f) %f", &n.x, &n.y, &n.z, &dist ) != 4 )
	{
		(*sysLog) << "Line " << context.line << " illegal plane command" << logEndl;

		return false;
	}

	plane = Plane ( n, dist );

	return true;
}
/*
bool	SceneDecoder :: getQ3Player    ( const String& name, Data * data, SubScene * scene )
{
	String		str, cmd, args;
	String		file;
	Vector3D	pos;
	float		scale = 1;

	if ( !getString ( data, str ) )
	{
		(*sysLog) << "Unexpected end of file found. " << logEndl;

		return NULL;
	}

	if ( str != "{" )
	{
		(*sysLog) << "Line " << line << " \'{\' expected." << logEndl;

		return NULL;
	}

	for ( ; ; )
	{
		if ( !getString ( data, str ) )
		{
			(*sysLog) << "Unexpected end of file found. " << logEndl;

			return NULL;
		}

		if ( str == "}" )
			break;

		String :: parseString ( str, cmd, args );

		if ( cmd == "file" )
			file = args;
		else
		if ( cmd == "pos" )
		{
			if ( !getVector3D ( pos, args ) )
			{
				(*sysLog) << "Line " << line << " invalid pos" << logEndl;

				return false;

			}
		}
		else
		if ( cmd == "scale" )
			scale = args.toFloat ();
		else
		if ( cmd != "" )
		{
			(*sysLog) << "Line " << line << " illegal command " << cmd << logEndl;

			return NULL;
		}
	}

	String	headName  ( file + "/head.md3" );
	String	lowerName ( file + "/lower.md3" );
	String	upperName ( file + "/upper.md3" );

	VisualObject * head  = dynamic_cast <VisualObject *> ( getResourceManager () -> getObject ( headName .dequote () ) );
	VisualObject * lower = dynamic_cast <VisualObject *> ( getResourceManager () -> getObject ( lowerName.dequote () ) );
	VisualObject * upper = dynamic_cast <VisualObject *> ( getResourceManager () -> getObject ( upperName.dequote () ) );

	if ( head == NULL || lower == NULL || upper == NULL )
	{
		head  -> release ();
		upper -> release ();
		lower -> release ();

		(*sysLog) << "Line " << line << " error loading Q3player from " << file << logEndl;

		return false;
	}

	LinkedObject * lowerObject = new LinkedObject ( name, lower );
	LinkedObject * upperObject = new LinkedObject ( name + "-upper", upper );

	lowerObject -> setLink ( "tag_torso", upperObject );
	upperObject -> setLink ( "tag_head",  head );

	lowerObject -> setTransform ( Transform3D ::getScale ( scale ) );

	scene -> addObject ( lowerObject );
}

*/