//
// 3-dimensional polygon class
// Supports basic operations, textures, lightmaps and vertex colors
//
// Author: Alex V. Boreskoff
//
// Last changed: 13/11/2002
//
 
#include	<assert.h>
#include	<string.h>					// for memmove

#include    "Poly3d.h"
#include    "Math3d.h"
#include	"Texture.h"
#include	"LightMap.h"
#include	"Frustrum.h"
#include	"Vector2D.h"
#include	"Vector4D.h"
#include	"Ray.h"
#include	"Transform3D.h"
#include	"Mapping.h"
#include	"Shader.h"

MetaClass	Polygon3D :: classInstance ( "Polygon3D", &Object :: classInstance );

Polygon3D :: Polygon3D () : Object ( "Unnamed Polygon3D" ), color ( 1.0f, 1.0f, 1.0f, 1.0f )
{
    plane       = NULL;
    vertices    = NULL;
    uvMap       = NULL;
    texture     = NULL;
	colors      = NULL;
    lightMap    = NULL;
	mapping     = NULL;
	shader      = NULL;
    numVertices = 0;
    maxVertices = 0;
    id          = 0;
	metaClass   = &classInstance;
}

Polygon3D :: Polygon3D ( const Polygon3D& poly ) : Object ( poly.name )
{
    plane       = new Plane ( *poly.getPlane () );
    numVertices = poly.numVertices;
    maxVertices = poly.maxVertices;
    vertices    = new Vector3D [maxVertices];
    uvMap       = new Vector2D [maxVertices];
	colors      = NULL;
	mapping     = NULL;
	texture     = NULL;
	lightMap    = NULL;
	shader      = NULL;
    id          = poly.id;
	color       = poly.color;
	metaClass   = &classInstance;

	if ( poly.colors != NULL )
	{
		colors = new Vector4D [maxVertices];

		memcpy ( colors,   poly.colors,   sizeof (Vector4D) * numVertices );
	}

    memcpy ( vertices, poly.vertices, sizeof (Vector3D) * numVertices );
    memcpy ( uvMap,    poly.uvMap,    sizeof (Vector2D) * numVertices );

    if ( poly.getTexture () != NULL )
        texture  = (Texture *)  poly.texture  -> retain ();

    if ( poly.getLightmap () != NULL )
		lightMap = (Lightmap *) poly.lightMap -> retain ();

	if ( poly.getMapping () != NULL )
		mapping = new Mapping ( *poly.getMapping () );

	if ( poly.getShader () != NULL )
		shader = (Shader *) poly.getShader () -> retain ();
}

Polygon3D :: Polygon3D ( const char * theName, int n ) : Object ( theName ), color ( 1.0f, 1.0f, 1.0f, 1.0f )
{
    numVertices = 0;
    maxVertices = ((n + 3) >> 2 ) << 2;
    vertices    = new Vector3D [maxVertices];
    uvMap       = new Vector2D [maxVertices];
	colors      = NULL;
    plane       = NULL;
    texture     = NULL;
    lightMap    = NULL;
	mapping     = NULL;
	shader      = NULL;
    id          = 0;
	metaClass   = &classInstance;
}

Polygon3D :: Polygon3D ( const char * theName, int n, Vector3D * v, Vector2D * uv ) : Object ( theName ), color ( 1.0f, 1.0f, 1.0f, 1.0f )
{
    numVertices = n;
    maxVertices = ((n + 3) >> 2 ) << 2;
    vertices    = new Vector3D [maxVertices];
	colors      = NULL;
    uvMap       = new Vector2D [maxVertices];
    id          = 0;
	metaClass   = &classInstance;

    memcpy ( vertices, v,  sizeof (Vector3D) * numVertices );
    memcpy ( uvMap,    uv, sizeof (Vector2D) * numVertices );

    texture  = NULL;
    lightMap = NULL;
	mapping  = NULL;
	shader   = NULL;

    computePlane ();
}

Polygon3D :: ~Polygon3D ()
{
    delete vertices;
	delete colors;
    delete uvMap;
    delete plane;
	delete mapping;

    if ( texture != NULL )
		texture -> release ();

    if ( lightMap != NULL )
		lightMap -> release ();

	if ( shader != NULL )
		shader -> release ();
}

bool Polygon3D :: isOk () const
{
    return numVertices >= 3 && vertices != NULL && uvMap != NULL && plane != NULL;
}

int	Polygon3D :: init ()
{
	if ( plane != NULL )
	{
		delete plane;

		plane = NULL;
	}

	if ( numVertices < 3 )
		return 0;

	buildBoundingBox ();

	Vector3D	e ( vertices [1] - vertices [0] );

	for ( int i = 2; i < numVertices; i++ )
	{
		Vector3D	n (e ^ (vertices [i] - vertices [0]));

		if ( n.length () >= EPS )
		{
			plane = new Plane ( n, vertices [0] );

			return Object :: init ();
		}
	}

	return 0;			// failed to create plane
}

int		Polygon3D :: compare ( const Object * obj ) const
{
	int	res = Object :: compare ( obj );

	if ( res != 0 )
		return res;

	Polygon3D * poly = (Polygon3D *) obj;

	if ( id < poly -> id )
		return -1;

	if ( id > poly -> id )
		return 1;

	return 0;
}

Polygon3D& Polygon3D :: operator = ( const Polygon3D& poly )
{
    if ( this == &poly )            // check for assignment to itself
        return *this;

									// XXX: this scheme may be dangerous !!!
	if ( poly.maxVertices > maxVertices )
	{
		maxVertices = poly.maxVertices;

		delete vertices;
		delete uvMap;
		delete colors;

		vertices = new Vector3D [maxVertices];

		if ( poly.uvMap != NULL )
			uvMap = new Vector2D [maxVertices];
		else
			uvMap = NULL;

		if ( poly.colors != NULL )
			colors = new Vector4D [maxVertices];
		else
			colors = NULL;
	}

	if ( plane == NULL )
		plane = new Plane ( *poly.getPlane () );
	else
		*plane = *poly.getPlane ();						// assign plane by memberwise copying

    numVertices = poly.numVertices;
    id          = poly.id;
    flags       = poly.flags;
    boundingBox = poly.boundingBox;
	color       = poly.color;

    memcpy ( vertices, poly.vertices, sizeof (Vector3D) * numVertices );

	if ( poly.colors != NULL )
		memcpy ( colors, poly.colors, sizeof (Vector4D) * numVertices );
	else
		colors = NULL;

    if ( poly.uvMap != NULL )
        memcpy ( uvMap, poly.uvMap, sizeof (Vector2D) * numVertices );
    else
        uvMap = NULL;

    setTexture  ( poly.texture  );
    setLightmap ( poly.lightMap );
	setShader   ( poly.getShader () );

    return *this;
}

void	Polygon3D :: setMapping ( const Mapping& m )
{
	delete mapping;

	mapping = new Mapping ( m );
}

void    Polygon3D :: setTexture ( Texture * tex )
{
	if ( texture != NULL )
		texture -> release ();

    if ( tex != NULL )
        texture = (Texture *) tex -> retain ();
    else
        texture = NULL;
}

void    Polygon3D :: setLightmap ( Lightmap * map )
{
     if ( lightMap != NULL )
		lightMap -> release ();

     if ( map != NULL )
	 {
        lightMap = map;

		lightMap -> retain   ();
		lightMap -> setOwner ( this );
	 }
     else
        lightMap = NULL;
}

void	Polygon3D :: setLightmapTexture ( Texture * txt )
{
	if ( uvMap == NULL )
		return;

							// compute range of texture coordinates for poly
	Vector2D	texMin;
	Vector2D	texMax;

	getTextureExtent ( texMin, texMax );

	Lightmap * map = new Lightmap ( txt -> getName (), texMin, texMax, txt );

	setLightmap ( map );

	map -> setOwner ( this );
	map -> release  ();
}

void    Polygon3D :: setShader ( Shader * sh )
{
	if ( shader != NULL )
		shader -> release ();

    if ( sh != NULL )
        shader = (Shader *) sh -> retain ();
    else
        shader= NULL;
}

void	Polygon3D :: transform ( const Transform3D& tr )
{
    for ( register int i = 0; i < numVertices; i++ )
        vertices [i] = tr.transformPoint ( vertices [i] );

	init ();
}

void    Polygon3D :: transform ( const Matrix3D& tr )
{
    for ( register int i = 0; i < numVertices; i++ )
        vertices [i] = tr * vertices [i];

	init ();
}

void    Polygon3D :: translate ( const Vector3D& v )
{
    for ( register int i = 0; i < numVertices; i++ )
        vertices [i] += v;

	init ();
}

int	    Polygon3D :: classify ( const Plane& p ) const
{
	int	positive = 0;
	int	negative = 0;

	for ( int i = 0; i < numVertices; i++ )
	{
		float	res = p.signedDistanceTo ( vertices [i] );

		if ( res > EPS )
			positive++;
		else
		if ( res < -EPS )
			negative++;
	}

	if ( positive > 0 && negative == 0 )
		return IN_FRONT;
	else
	if ( positive == 0 && negative > 0 )
		return IN_BACK;
	else
	if ( positive < 1 && negative < 1 )
		return IN_PLANE;

	return IN_BOTH;
}

void    Polygon3D :: split ( const Plane& p, Polygon3D& front, Polygon3D& back ) const
{
	assert ( uvMap  != NULL );

	Vector3D prevP     = vertices [numVertices - 1];
    Vector2D prevUV    = uvMap    [numVertices - 1];
	float	 prevF     = p.signedDistanceTo ( prevP );

    front.clear ();
    back. clear ();

	if ( colors == NULL )
	{
		for ( int i = 0; i < numVertices; i++ )
		{
			Vector3D curP     ( vertices [i] );
			Vector2D curUV    ( uvMap [i] );
			float	 curF = p.signedDistanceTo ( curP );

			if ( curF > EPS )
			{
				if ( prevF < -EPS )
				{
					float	 t    = - curF / ( prevF - curF );

    				Vector3D sp   = curP + t * ( prevP - curP );
					Vector2D spUV = curUV + t * ( prevUV - curUV );

					front.addVertex ( sp, spUV );
					back.addVertex  ( sp, spUV );
				}

				front.addVertex ( curP, curUV );
			}
			else
			if ( curF < -EPS )
			{
				if ( prevF > EPS )
				{
        			float	 t    = - curF / ( prevF - curF );

    				Vector3D sp   = curP + t * ( prevP - curP );
					Vector2D spUV = curUV + t * ( prevUV - curUV );

					front.addVertex ( sp, spUV );
					back.addVertex  ( sp, spUV );
				}

				back.addVertex ( curP, curUV );
			}
			else            // abs (curF) < EPS
			{
				front.addVertex ( curP, curUV );
				back.addVertex  ( curP, curUV );
			}

	  		prevP     = curP;
			prevF     = curF;
		    prevUV    = curUV;
		}

		return;
	}

	Vector4D prevColor = colors   [numVertices - 1];

	for ( int i = 0; i < numVertices; i++ )
	{
		Vector3D curP     ( vertices [i] );
        Vector2D curUV    ( uvMap [i] );
		Vector4D curColor ( colors [i] );
		float	 curF = p.signedDistanceTo ( curP );

		if ( curF > EPS )
        {
            if ( prevF < -EPS )
            {
			    float	 t    = - curF / ( prevF - curF );

    			Vector3D sp      = curP + t * ( prevP - curP );
                Vector2D spUV    = curUV + t * ( prevUV - curUV );
				Vector4D spColor = curColor + t * (prevColor - curColor );

                front.addVertex ( sp, spUV, spColor );
                back.addVertex  ( sp, spUV, spColor );
            }

            front.addVertex ( curP, curUV, curColor );
        }
		else
		if ( curF < -EPS )
        {
            if ( prevF > EPS )
            {
        	    float	 t    = - curF / ( prevF - curF );

    			Vector3D sp      =  curP + t * ( prevP - curP );
                Vector2D spUV    = curUV + t * ( prevUV - curUV );
				Vector4D spColor = curColor + t * ( prevColor - curColor );

                front.addVertex ( sp, spUV, spColor );
                back.addVertex  ( sp, spUV, spColor );
            }

            back.addVertex ( curP, curUV, curColor );
		}
		else            // abs (curF) < EPS
		{
            front.addVertex ( curP, curUV, curColor );
            back.addVertex  ( curP, curUV, curColor );
		}

	  	prevP     = curP;
		prevF     = curF;
		prevColor = curColor;
        prevUV    = curUV;
	}
}

void    Polygon3D :: addVertex ( const Vector3D& v, const Vector2D& uv, const Vector4D& color )
{
    if ( numVertices + 1 >= maxVertices )
       realloc ( maxVertices + 4 );

    vertices [numVertices] = v;

	if ( colors == NULL )
	{
		colors = new Vector4D [maxVertices];

		for ( int i = 0; i < maxVertices; i++ )
			colors [i] = Vector4D ( 1.0f, 1.0f, 1.0f, 1.0f );
	}

	colors [numVertices] = color;

	if ( uvMap != NULL )
		uvMap [numVertices] = uv;

	numVertices++;
}

void    Polygon3D :: addVertex ( const Vector3D& v, const Vector2D& uv )
{
    if ( numVertices + 1 >= maxVertices )
       realloc ( maxVertices + 4 );

	if ( colors != NULL )
		colors [numVertices] = Vector4D ( 1.0f, 1.0f, 1.0f, 1.0f );

    vertices [numVertices] = v;

	if ( uvMap != NULL )
		uvMap [numVertices] = uv;

	numVertices++;
}

void	Polygon3D :: addVertex ( const Vector3D& v )
{
    if ( numVertices + 1 >= maxVertices )
       realloc ( maxVertices + 4 );

	if ( colors != NULL )
		colors [numVertices] = Vector4D ( 1, 1, 1, 1 );

    vertices [numVertices] = v;

	if ( mapping != NULL )
	{
		if ( uvMap == NULL )
			uvMap = new Vector2D [maxVertices];

		uvMap [numVertices] = mapping -> map ( v );
	}
	else
	if ( uvMap != NULL )
		uvMap [numVertices] = Vector2D ( 0, 0 );

	numVertices++;
}

void    Polygon3D :: delVertex ( int index )
{
    if ( isEmpty () )
       return;

    if ( index < numVertices - 1 )
    {
        memmove ( &vertices [index], &vertices [index+1], sizeof (Vector3D) * (numVertices - index - 1) );

        if ( uvMap != NULL )
            memmove ( &uvMap [index], &uvMap [index+1],   sizeof (Vector2D) * (numVertices - index - 1) );

        if ( colors != NULL )
            memmove ( &colors [index], &colors [index+1], sizeof (Vector4D) * (numVertices - index - 1) );
    }

    numVertices--;
}

void    Polygon3D :: realloc ( int newMaxVertices )
{
    if ( newMaxVertices <= maxVertices )
        return;

    maxVertices = ( (newMaxVertices + 3) >> 2 ) << 2;

    Vector3D * tmp;
	Vector2D * tmp2;

    tmp      = vertices;
    vertices = new Vector3D [maxVertices];

    if ( tmp != NULL )
       memcpy ( vertices, tmp, sizeof (Vector3D) * numVertices );

    delete tmp;

    if ( uvMap != NULL )
    {
        tmp2  = uvMap;
        uvMap = new Vector2D [maxVertices];

        if ( tmp2 != NULL )
           memcpy ( uvMap, tmp2, sizeof (Vector2D) * numVertices );

        delete tmp2;
    }

    if ( colors != NULL )
    {
		Vector4D * tmp;

        tmp    = colors;
        colors = new Vector4D [maxVertices];

        if ( tmp != NULL )
           memcpy ( colors, tmp, sizeof (Vector4D) * numVertices );

        delete tmp;
    }
}

void    Polygon3D :: computePlane ()
{
    assert ( numVertices > 2 );

    delete plane;

    plane = new Plane ( vertices [0], vertices [1], vertices [2] );
}

bool	Polygon3D :: contains ( const Vector3D& v ) const
{
    struct
    {
        float   x;
        float   y;
    } p, point [MAX_VERTICES+1], e0, e1;

    bool	y0;
    bool	inside = false;
    int		i;

    assert ( plane != NULL );
	assert ( numVertices >= 3 );

	if ( numVertices < 3 || plane == NULL )
		return false;

                            // project along main axis
    if ( plane -> getMainAxis () == AXIS_X )
    {
        p.x = v.y;
        p.y = v.z;

        for ( i = 0; i < numVertices; i++ )
        {
            point [i].x = vertices [i].y;
            point [i].y = vertices [i].z;
        }
    }
    else
    if ( plane -> getMainAxis () == AXIS_Y )
    {
        p.x = v.x;
        p.y = v.z;

        for ( i = 0; i < numVertices; i++ )
        {
            point [i].x = vertices [i].x;
            point [i].y = vertices [i].z;
        }
    }
    else
    {
        p.x = v.x;
        p.y = v.y;

        for ( i = 0; i < numVertices; i++ )
        {
            point [i].x = vertices [i].x;
            point [i].y = vertices [i].y;
        }
    }

    e0 = point [numVertices - 1];
    e1 = point [0];
    y0 = e0.y >= p.y;

    for ( i = 1; i <= numVertices; i++ )
    {
        bool	y1 = e1.y >= p.y;

        if ( y0 != y1 )
            if (((e1.y-p.y)*(e0.x-e1.x) >= (e1.x-p.x)*(e0.y-e1.y)) == y1 )
                inside = !inside;

        y0 = y1;
        e0 = e1;
        e1 = point [i];
    }

    return inside != 0;
}

float   Polygon3D :: closestPointToBoundary ( const Vector3D& p, Vector3D& res ) const
{
    Vector3D bestPoint = closestPointToSegment ( p, vertices [0], vertices [numVertices-1] );
    float    bestDist  = p.distanceToSq ( bestPoint );
    float    dist;

    for ( int i = 0; i < numVertices - 1; i++ )
    {
        res  = closestPointToSegment ( p, vertices [i], vertices [i+1] );
        dist = p.distanceToSq ( res );

        if ( dist < bestDist )
        {
            bestDist  = dist;
            bestPoint = res;
        }
    }

    res = bestPoint;

    return (float)sqrt ( bestDist );
}

int Polygon3D :: clipByPlane ( const Plane& plane )
{
    int code = boundingBox.classify ( plane );

    if ( code == IN_FRONT )         // entire poly lies in positive halfspace
        return 1;
    else
    if ( code == IN_BACK )          // entire poly lies in negative halfspace
        return numVertices = 0;
                                    // otherwise do normal clipping
    Vector3D p  [MAX_VERTICES];
    Vector2D uv [MAX_VERTICES];
	Vector4D c  [MAX_VERTICES];

    int      prevIndex = numVertices - 1;
    float    prevF     = plane.signedDistanceTo ( vertices [prevIndex] );
    int      count     = 0;

    assert ( uvMap != NULL );

    for ( register int i = 0; i < numVertices; i++ )
    {
        float   curF = plane.signedDistanceTo ( vertices [i] );

        if ( curF > EPS )
        {
            if ( prevF < -EPS )
            {
                float   t = -curF / (prevF - curF);

				if ( colors != NULL )
					c  [count] = colors [i] + t*(colors [prevIndex] - colors [i]);

                p  [count]   = vertices [i] + t*(vertices [prevIndex] - vertices [i]);
                uv [count++] = uvMap [i] + t*(uvMap [prevIndex] - uvMap [i]);
	        }

			if ( colors != NULL )
				c  [count] = colors   [i];

            p  [count]   = vertices [i];
            uv [count++] = uvMap    [i];
        }
        else
        if ( curF < -EPS )
        {
            if ( prevF > EPS )
            {
                float   t = -curF / (prevF - curF);

				if ( colors != NULL )
					c  [count] = colors [i] + t*(colors [prevIndex] - colors [i]);

                p  [count]   = vertices [i] + t*(vertices [prevIndex] - vertices [i]);
                uv [count++] = uvMap [i] + t*(uvMap [prevIndex] - uvMap [i]);
            }
        }
        else            // abs (curF) < EPS
        {
			if ( colors != NULL )
				c  [count] = colors   [i];

            p  [count]   = vertices [i];
            uv [count++] = uvMap    [i];
        }

        prevF     = curF;
        prevIndex = i;
    }

    realloc ( count );
    memcpy  ( vertices, p,  count * sizeof ( Vector3D ) );
    memcpy  ( uvMap,    uv, count * sizeof ( Vector2D ) );

	if ( colors != NULL )
		memcpy  ( colors,   c,  count * sizeof ( Vector4D ) );

    numVertices = count;

    buildBoundingBox ();                    // rebuild bounding box

    return count >= 3;
}

int Polygon3D :: clipByFrustrum ( const Frustrum& frustrum )
{
	assert ( uvMap  != NULL );
//	assert ( colors != NULL );

    realloc ( MAX_VERTICES );

    Vector3D   p  [MAX_VERTICES];
	Vector4D   c  [MAX_VERTICES];
    Vector2D   uv [MAX_VERTICES];

    Vector3D * curPoint       = vertices;
	Vector4D * curColor       = colors;
    Vector2D * curUV          = uvMap;
    Vector3D * newPoint       = p;
	Vector4D * newColor       = c;
    Vector2D * newUV          = uv;
    int        curNumVertices = numVertices;
    int        newNumVertices = numVertices;    // in case we skipp all planes

    for ( int i = 0; i < frustrum.getNumPlanes (); i++ )
    {                                   // process each plane in turn
        Plane * plane = frustrum.getPlane ( i );
        int     code  = boundingBox.classify ( *plane );

        if ( code == IN_FRONT )         // entire poly lies in positive halfspace
            continue;

        if ( code == IN_BACK )          // entire poly lies in negative halfspace
            return numVertices = 0;

                                        // otherwise do normal clipping
        int      prevIndex = curNumVertices - 1;
        float    prevF     = plane -> signedDistanceTo ( curPoint [prevIndex] );

        newNumVertices = 0;

        for ( register int j = 0; j < curNumVertices; j++ )
        {
            float   curF = plane -> signedDistanceTo ( curPoint [j] );

            if ( curF > EPS )
            {
                if ( prevF < -EPS )
                {
                    float   t = -curF / (prevF - curF);

					if ( colors != NULL )
						newColor [newNumVertices] = curColor [j] + t*(curColor [prevIndex] - curColor [j]);

                    newPoint [newNumVertices]   = curPoint [j] + t*(curPoint [prevIndex] - curPoint [j]);
                    newUV    [newNumVertices++] = curUV [j] + t*(curUV [prevIndex] - curUV [j]);
                }

				if ( colors != NULL )
					newColor [newNumVertices] = curColor [j];

                newPoint [newNumVertices]   = curPoint [j];
                newUV    [newNumVertices++] = curUV    [j];
            }
            else
            if ( curF < -EPS )
            {
                if ( prevF > EPS )
                {
                    float   t = -curF / (prevF - curF);

					if ( colors != NULL )
						newColor [newNumVertices] = curColor [j] + t*(curColor [prevIndex] - curColor [j]);

                    newPoint [newNumVertices]   = curPoint [j] + t*(curPoint [prevIndex] - curPoint [j]);
                    newUV    [newNumVertices++] = curUV [j] + t*(curUV [prevIndex] - curUV [j]);
                }
            }
            else
            {
				if ( colors != NULL )
					newColor [newNumVertices] = curColor [j];

                newPoint [newNumVertices]   = curPoint [j];
                newUV    [newNumVertices++] = curUV    [j];
            }

            prevF     = curF;
            prevIndex = j;
        }

        if ( newNumVertices < 3 )
            return numVertices = 0;

        Vector3D * temp1;
		Vector4D * temp2;
		Vector2D * temp3;
                                            // now swap new & cur pointers
        temp1    = curPoint;
        curPoint = newPoint;
        newPoint = temp1;

        temp3    = curUV;
        curUV    = newUV;
        newUV    = temp3;

		temp2    = curColor;
		curColor = newColor;
		newColor = temp2;

        curNumVertices = newNumVertices;
    }

    numVertices = newNumVertices;

    if ( newPoint == vertices )
    {
        memcpy  ( vertices, curPoint, numVertices * sizeof ( Vector3D ) );
        memcpy  ( uvMap,    curUV,    numVertices * sizeof ( Vector2D ) );

		if ( colors != NULL )
			memcpy  ( colors,   curColor, numVertices * sizeof ( Vector4D ) );
    }

    buildBoundingBox ();                    // rebuild bounding box

    return numVertices >= 3;
}

void    Polygon3D :: buildBoundingBox ()
{
    boundingBox.reset ();

    for ( register int i = 0;i < numVertices; i++ )
        boundingBox.addVertex ( vertices [i] );
}

float   Polygon3D :: intersectByRay ( const Ray& ray, Vector3D& pos )
{
    float   denom = (ray.getDir () & plane -> n);

    if ( fabs ( denom ) < EPS )
        return -1;								// no intersection

    float   numer = -(plane -> dist + (ray.getOrigin () & plane -> n));
    float   t     = numer / denom;

    if ( t < EPS )
       return t;

    if ( contains ( pos = ray.point ( t ) ) )	// check whether polygon contains point of intersection
       return t;

    return -1;
}

float   Polygon3D :: getSignedArea () const
{
    float   polyArea = 0;

    for ( int i = 0; i < numVertices - 2; i++ )
        polyArea += area ( vertices [0], vertices [i+1], vertices [i+2] );

    return polyArea * 0.5f;
}

Vector3D Polygon3D :: getCenter () const
{
    Vector3D center ( 0, 0, 0 );

    for ( int i = 0; i < numVertices; i++ )
        center += vertices [i];

    return center / (float)numVertices;
}

int Polygon3D :: intersect ( const Polygon3D& poly ) const
{
    return IN_BOTH;
}


void	Polygon3D :: clear ()
{
	numVertices = 0;

	if ( texture != NULL )
	{
		texture -> release ();

		texture = NULL;
	}

	if ( lightMap != NULL )
	{
		lightMap -> release ();

		lightMap = NULL;
	}
}

void	Polygon3D :: projectFromPointOntoPlane ( const Vector3D& org, const Plane& thePlane )
{
	if ( thePlane.classify ( org ) == IN_PLANE )		// check for error
		return;

	for ( int i = 0; i < numVertices; i++ )
	{
		Ray		ray ( org, vertices [i] - org );
		float	t = ray.intersect ( thePlane );

		vertices [i] = ray.point ( t );
	}

	delete plane;

	plane = new Plane ( thePlane );
}

bool	Polygon3D :: isTransparent () const
{
	if ( texture != NULL && texture -> isTransparent () )
		return true;

	if ( colors != NULL )
		for ( int i = 0; i < numVertices; i++ )
			if ( colors [i].w < 1.0f - EPS )
				return true;

	return color.w < 1 - EPS;
}

Vector3D	Polygon3D :: mapTextureToWorld ( const Vector2D& tex ) const
{
	if ( mapping == NULL || uvMap == NULL || vertices == NULL )
		return Vector3D ( 0, 0, 0 );

	return mapping -> unmap ( tex, *plane );
}

void	Polygon3D :: getTextureExtent ( Vector2D& texMin, Vector2D& texMax ) const
{
	texMin.x =
	texMin.y = INFINITY;
	texMax.x =
	texMax.y = -INFINITY;

	if ( uvMap == NULL || vertices == NULL )
		return;

	for ( int i = 0; i < numVertices; i++ )
	{
		Vector2D	tex ( uvMap [i] );

		if ( tex.x < texMin.x )
			texMin.x = tex.x;

		if ( tex.x > texMax.x )
			texMax.x = tex.x;

		if ( tex.y < texMin.y )
			texMin.y = tex.y;

		if ( tex.y > texMax.y )
			texMax.y = tex.y;
	}
}

