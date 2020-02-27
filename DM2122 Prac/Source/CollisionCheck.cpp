#include "CollisionCheck.h"



int CollisionCheck::DistanceCheck(Vector3 obj1, Vector3 obj2)
{
	return sqrt(pow(obj1.x - obj2.x, 2) + pow(obj1.z - obj2.z, 2)); // ignoring y axis
}

bool CollisionCheck::SquareCollisionCheck(Vector3 obj1, float obj1SizeX, float obj1SizeZ, Vector3 obj2, float obj2SizeX, float obj2SizeZ)
{
	bool collisionX = obj1.x + obj1SizeX >= obj2.x && obj2.x + obj2SizeX >= obj1.x;
	// Collision y-axis?
	bool collisionZ = obj1.z + obj1SizeZ >= obj2.z && obj2.z + obj2SizeZ >= obj1.z;
	// Collision only if on both axes
	return collisionX && collisionZ;
}

bool CollisionCheck::TrueCollisionCheck(Vector3 obj1, float sizeX, float sizeZ)
{
	// TO DO: FINISH THIS Uhsuafhc
	// boundary check
	if (obj1.x + sizeX >= 46 || obj1.x - sizeX <= -46)
		return false;
	if (obj1.z + sizeZ >= 46 || obj1.z - sizeZ <= -46)
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
