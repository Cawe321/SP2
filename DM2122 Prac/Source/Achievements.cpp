#include "Achievements.h"


Achievements::Achievements()
{
	output = "";
}

std::string Achievements::carAchievements(int price)
{
	if (price == 15000)
	{
		output = "CheapSkate Achieved!";
	}
	else if(price == 90000)
	{
		output = "Playing it Safe Achieved!";
	}
	else if (price == 225000)
	{
		output = "Crazy Rich!";
	}
	else if (price == 0)
	{
		output = "I Rather Walk Achieved!";
	}
	return output;
}

std::string Achievements::bouncerAchievements(int current, int max)
{
	if (current == max && current != 0) //the current not equal 0 is just a safety net
	{
		output = "Model Citizen Achieved!";
	}
	return output;
}

std::string Achievements::salespersonAchievements(int satisified)
{
	if (satisified == 0)
	{
		output = "No, I Quit! Achieved";
	}
	return output;
}
