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
	obj1.x += 2.5; // center
	// boundary check
	
	if (obj1.x + sizeX >= 50 || obj1.x - sizeX <= -50 || obj1.z + sizeX >= 47 || obj1.z - sizeX <= -50)
		return false;
	if (obj1.z + sizeZ >= 47 || obj1.z - sizeZ <= -50 || obj1.x + sizeZ >= 50 || obj1.x - sizeZ <= -50)
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
	if (DistanceCheck(obj1, Vector3(5.f, 0, 30.f)) < 4.5f + sizeX) // information counter
		return false;
	if (DistanceCheck(obj1, Vector3(-10, 0, 30)) < 3 + sizeX) // display car
		return false;
	if (DistanceCheck(obj1, Vector3(1, 0, 35)) < 0.25f + sizeX) // information stand
		return false;
	if (DistanceCheck(obj1, Vector3(45, 0, 45)) < 0.25f + sizeX) // information stand
		return false;
	for (int i = 20; i >= -30; i -= 10)
	{
		if (DistanceCheck(obj1, Vector3(-5, 0, i)) < (sizeX)) // information stand
			return false;
		if (DistanceCheck(obj1, Vector3(5, 0, i)) < (sizeX)) // information stand
			return false;
	}
	if (SquareCollisionCheck(obj1, sizeX, sizeZ, Vector3(-20.f, 0, -40), 17.f, 12.7f))
		return false;
	if (SquareCollisionCheck(obj1, sizeX, sizeZ, Vector3(-20.5, 0, -29), 14.5, 13))
		return false;
	if (SquareCollisionCheck(obj1, sizeX, sizeZ, Vector3(-16, 0, -16), 7, 6)) 
		return false;
	if (SquareCollisionCheck(obj1, sizeX, sizeZ, Vector3(-16, 0, -4), 10.5, 27))
		return false;
	if (SquareCollisionCheck(obj1, sizeX, sizeZ, Vector3(4.4f, 0, -11), 8.f, 8.f)) 
		return false;
	if (SquareCollisionCheck(obj1, sizeX, sizeZ, Vector3(6.5f, 0, -1), 12.2f, 12.f))
		return false;
	if (SquareCollisionCheck(obj1, sizeX, sizeZ, Vector3(11.f, 0, 14.f), 5.f, 8.f))
		return false;
	if (SquareCollisionCheck(obj1, sizeX, sizeZ, Vector3(7.f, 0, 19.5f), 8.f, 7.f))
		return false;
	if (SquareCollisionCheck(obj1, sizeX, sizeZ, Vector3(-45.f, 0, -45.f), 2.f, 5.f))
		return false;

	return true;
}

bool CollisionCheck::NonCharacterCollisionCheck(Vector3 obj1, float obj1SizeX, float obj1SizeZ)
{
	obj1.x -= 2.5; // center
	if (!TrueCollisionCheck(obj1, obj1SizeX, obj1SizeZ))
		return false;

	if (DistanceCheck(obj1, Vector3(45, 0, 45)) < 20) 
		return false;
	if (DistanceCheck(obj1, Vector3(-45, 0, -40)) < 20)
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
