#ifndef __COLLISION_DETECTION__
#define __COLLISION_DETECTION__

#include	"Vector3D.h"

#define  MAX_COL_DEPTH  5		// max. recursion depth of collision detector

class	Array;					// class predeclarations
class	Polygon3D;

struct   ColPacket
{
    Vector3D    pos;			// original position of object
    Vector3D    vel;			// desired movement vector (velocity)
    Vector3D    size;			// ellispoid radii
    int         colFound;		// whether any collision was found
    float       distance;		// distance to collision
    Vector3D    ellIntPoint;	// intersection point on ellispoid
    Vector3D    polyIntPoint;	//intersection point on polygon
    Polygon3D * colPoly;		// polygon we collided with
};

int      checkCollision   ( const Array& polys, ColPacket& col );
Vector3D collideWithWorld ( const Array& polys, const Vector3D& pos, const Vector3D& vel, const Vector3D& size );
Vector3D getPosition      ( const Array& polys, const Vector3D& pos, const Vector3D& vel, const Vector3D& size );

#endif
