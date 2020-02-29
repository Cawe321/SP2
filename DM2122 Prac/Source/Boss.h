#pragma once
#include <string>
#include "globalData.h"
/*
This class is meant to be a guage of how much your boss likes you
Once goodwill reaches 0 you are fired
If goodwill is above 60 you will have a small boost to your pay.
Likewise if goodwill is below 40 you will have a small penalty to your pay.
Refer to getmodifier() for math.
*/
class Boss
{
private:
	globalData* data;
public:
	Boss();
	~Boss();
	void AddGoodwill(int);
	void LoseGoodwill(int);
	int GetGoodwill();
	float getmodifier();
	std::string PrintGoodwill();
};

