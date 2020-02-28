#include "Achievements.h"


Achievements::Achievements()
{
	output = "";
	data = globalData::instance();
}

std::string Achievements::carAchievements(int carno)
{
	if (data->owncar(carno) == true)
	{
		if (carno == 1)
		{
			output = "CheapSkate Achieved!";
		}
		else if (carno == 3)
		{
			output = "Playing it Safe Achieved!";
		}
		else if (carno == 6)
		{
			output = "Crazy Rich!";
		}
	}
	
	
	else 
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
