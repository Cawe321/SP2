#pragma once
#include <string>
class Achievements
{
private:
	std::string output;
public:
	Achievements();
	std::string carAchievements(int price);
	std::string bouncerAchievements(int current, int max);
	std::string salespersonAchievements(int satisified); //additional condition shld be added to make sure this only appears in day 3
};

