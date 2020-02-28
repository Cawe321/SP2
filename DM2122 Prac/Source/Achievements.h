#pragma once
#include <string>
#include "globalData.h"

class Achievements
{
private:
	std::string output;
	globalData* data;
public:
	Achievements();
	std::string carAchievements(int price);
	std::string bouncerAchievements(int current, int max);
	std::string salespersonAchievements(int satisified); //additional condition shld be added to make sure this only appears in day 3
};

