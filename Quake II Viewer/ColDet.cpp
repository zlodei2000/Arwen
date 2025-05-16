#include	"Array.h"
#include    "ColDet.h"
#include    "Math3D.h"
#include	"Poly3D.h"
#include	"Portal.h"
#include	"Vector2D.h"

                                  // temp. poly for internal operations
static Polygon3D   tempPoly ( "CollisionDetection poly", MAX_VERTICES );
static int         recDepth;      // current recursion depth

int checkCollision ( const Array& polys, ColPacket& col )
{
    float    velLength          = col.vel.length ();
    Vector3D normalizedVelocity = col.vel / velLength;
    Vector3D ellIntPoint;         // ellipsoid intersection point
    Vector3D planeIntPoint;       // polygon's plane intersection point
    Vector3D polyIntPoint;        // polygon intersection point
    float    dist2PlaneInt;       // distance to plane intersection
    float    dist2EllInt;         // distance to ellipsoid intersection

                                  // check every polygon in the container
    for ( int i = 0; i < polys.getCount (); i++ )
    {
        Polygon3D * poly =  (Polygon3D *) polys.at ( i );
                                  // if some error with poly then skip it
        if ( !poly -> isOk () )
           continue;
                                  // do not check passable polygons
        if ( poly -> testFlag ( PF_PASSABLE ) && !poly -> testFlag ( PF_PORTAL ) )
           continue;

                                  // build scaled polygon, so that in the
                                  // transformed space ellipsoid becomes unit sphere
		tempPoly.clear ();
        for ( int j = 0; j < poly -> getNumVertices (); j++ )
            tempPoly.addVertex ( poly -> getVertices () [j] / col.size, Vector2D ( 0, 0 ) );

//        tempPoly.numVertices = poly -> numVertices;
        tempPoly.computePlane ();

                                  // get plane of poly
        const Plane * plane = tempPoly.getPlane ();

                                  // ignore poly if we're moving away from it
        if ( ( plane -> n & col.vel ) > EPS )
           continue;
                                  // compute intersection point of ellipsoid with plane
        ellIntPoint = col.pos - plane -> n;

                                  // check whether intersection point lies behind the plane
        if ( plane -> classify ( ellIntPoint ) == IN_BACK )
        {                         // we've already hit the plane
            plane -> intersectByRay ( ellIntPoint, plane -> n, dist2PlaneInt );

            planeIntPoint = ellIntPoint + dist2PlaneInt * plane -> n;
            dist2PlaneInt = EPS;  // report possible hitting
        }
        else
        {
			plane -> intersectByRay ( ellIntPoint, normalizedVelocity, dist2PlaneInt );
            planeIntPoint = ellIntPoint + dist2PlaneInt * normalizedVelocity;
        }

        polyIntPoint = planeIntPoint;
        dist2EllInt  = dist2PlaneInt;

                                  // check whther plane intersection point
                                  // lies within the polygon
        if ( !tempPoly.contains ( planeIntPoint ) )
        {                         // if not then find closest point on boundary of polygon
            tempPoly.closestPointToBoundary ( planeIntPoint, polyIntPoint );
                                  // and check whether the eray from it
                                  // intersects the sphere
            dist2EllInt = intersectSphereByRay ( col.pos, 1.0f, polyIntPoint, -normalizedVelocity );

                                  // if intersects compute intersection point on ellipsoid
            if ( dist2EllInt > 0.0f )
               ellIntPoint = polyIntPoint - dist2EllInt * normalizedVelocity;
        }

        if ( (dist2EllInt >= 0.0f) && (dist2EllInt <= velLength) )
           if ( !col.colFound || (dist2EllInt < col.distance) )
           {
                col.distance     = dist2EllInt;
                col.ellIntPoint  = ellIntPoint;
                col.polyIntPoint = polyIntPoint;
                col.colPoly      = poly;
                col.colFound     = 1;
           }
    }


    return col.colFound;
}

Transform3D * collideWithWorld ( const Array& polys, const Vector3D& pos, const Vector3D& vel, const Vector3D& size, Vector3D& res )
{
    if ( vel.length () < EPS )
	{
		res = pos;

		return NULL;
	}

    Vector3D  destPoint = pos + vel;
    ColPacket col;
									// init collision packet
    col.pos      = pos;
    col.vel      = vel;
    col.size     = size;
    col.colFound = 0;
    col.distance = -1.0f;
									// if no intersection then return new point
    if ( !checkCollision ( polys, col ) )
	{
		res = pos + vel;

		return NULL;
	}

									// check for coming through the portal
									// in this case we may need to transform
									// object position
	if ( col.colPoly -> testFlag ( PF_PORTAL ) )
	{
		Transform3D * tr = ((Portal *) col.colPoly) -> getTransform ();

		res = pos + vel;

		return tr;
	}

    Vector3D newPos;

    if ( col.distance >= EPS )    // compute collision point
		newPos = col.pos + col.distance * col.vel / col.vel.length ();
    else
        newPos = col.pos;
                                  // check whther we're too deep in
                                  // recursion. This probably means
                                  // we're stuck
    if ( ++recDepth >= MAX_COL_DEPTH )
	{
		res = newPos;

		return NULL;
	}

                                  // compute sliding direction and try it
    Plane    slidingPlane ( newPos - col.polyIntPoint, col.polyIntPoint );
    float    l;
	
	slidingPlane.intersectByRay ( destPoint, slidingPlane.n, l );

    Vector3D newDestPoint = destPoint + l* slidingPlane.n;
    Vector3D newVelocity  = newDestPoint - col.polyIntPoint;

    return collideWithWorld ( polys, newPos, newVelocity, size, res );
}

Vector3D getPosition ( const Array& polys, const Vector3D& pos, const Vector3D& vel, const Vector3D& size )
{
	Vector3D	res;

    recDepth = 0;                 // initialize recursion depth

    Transform3D * tr = collideWithWorld ( polys, pos / size, vel / size, size, res );

	res *= size;

	return tr != NULL ? tr -> transformPoint ( res ) : res;
}
