#ifndef COLLISIONCHECK_H
#define COLLISIONCHECK_H
#include "Vector3.h"
#include <cmath>
class CollisionCheck
{
public:
	static int DistanceCheck(Vector3 obj1, Vector3 obj2);
	static bool SquareCollisionCheck(Vector3 obj1, float obj1SizeX, float obj1SizeZ, Vector3 obj2, float obj2SizeX, float obj2SizeZ);
	static bool TrueCollisionCheck(Vector3 obj1, float obj1SizeX, float obj1SizeZ); // this is the actual collision check for the map | true = no collision detected
	static float angleBetween2Coords(Vector3 obj1, Vector3 obj2);
	CollisionCheck();
	~CollisionCheck();
};

#endif