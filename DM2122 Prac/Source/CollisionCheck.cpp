#include "CollisionCheck.h"



int CollisionCheck::DistanceCheck(Vector3 obj1, Vector3 obj2)
{
	return sqrt(pow(obj1.x - obj2.x, 2) + pow(obj1.z - obj2.z, 2)); // ignoring y axis
}

bool CollisionCheck::SquareCollisionCheck(Vector3 obj1, float obj1SizeX, float obj1SizeZ, Vector3 obj2, float obj2SizeX, float obj2SizeZ)
{
	bool collisionX = obj1.x + obj1SizeX >= obj2.x && obj2.x + obj2SizeX >= obj1.x;
	bool collisionZ = obj1.z + obj1SizeZ >= obj2.z && obj2.z + obj2SizeZ >= obj1.z;
	// Collision only if on both axes
	return collisionX && collisionZ;
}

bool CollisionCheck::TrueCollisionCheck(Vector3 obj1, float sizeX, float sizeZ)
{
	// TO DO: FINISH THIS Uhsuafhc
	// boundary check
	
	if (obj1.x + sizeX >= 50 || obj1.x - sizeX <= -50 || obj1.z + sizeX >= 50 || obj1.z - sizeX <= -50)
		return false;
	if (obj1.z + sizeZ >= 50 || obj1.z - sizeZ <= -50 || obj1.x + sizeZ >= 50 || obj1.x - sizeZ <= -50)
		return false;
	if (SquareCollisionCheck(obj1, sizeX, sizeZ, Vector3(-41, 1, -14), 4, 49)) // item booths
		return false;
	if (SquareCollisionCheck(obj1, sizeX, sizeZ, Vector3(37.5f, 1, 13.f), 4, 15.25f)) // bench 1
		return false;
	if (SquareCollisionCheck(obj1, sizeX, sizeZ, Vector3(37.5f, 1, -7.f), 4, 15.25f)) // bench 2
		return false;
	if (SquareCollisionCheck(obj1, sizeX, sizeZ, Vector3(37.5f, 1, -27.f), 4, 15.25f)) // bench 3
		return false;
	if (SquareCollisionCheck(obj1, sizeX, sizeZ, Vector3(37.5f, 1, -47.f), 4, 15.25f)) // bench 4
		return false;
	return true;
}



float CollisionCheck::angleBetween2Coords(Vector3 obj1, Vector3 obj2)
{
	float angle = atan2(obj2.z - obj1.z, obj2.x - obj1.x) * (180 / 3.142);
	return angle;
}

CollisionCheck::CollisionCheck()
{
}


CollisionCheck::~CollisionCheck()
{
}
