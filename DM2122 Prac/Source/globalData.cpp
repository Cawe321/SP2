#include "globalData.h"


globalData* globalData::s_instance = nullptr;

globalData::globalData()
{
	// check if save file exists first only once, which is only at the start of the game, otherwise, init all private data members as 0
}

std::string globalData::getinGameTime()
{
	return inGameTime;
}

void globalData::setinGameTime(float time)
{
	inGameTime = std::to_string(time);
}

VocationJob* globalData::getJob()
{
	if (Jobs == nullptr)
		Jobs = Vocation::getVocationJobClass(VocationJob::NONE);
	return Jobs; // returns the VocationJob* class of the Job
}

void globalData::setJob(VocationJob* career)
{
}



float globalData::getMoney()
{
	return money;
}

void globalData::setMoney(float cash)
{
	money = cash;
}

globalData* globalData::instance()
{
	if (!s_instance)
	{
		s_instance = new globalData;
		return s_instance;
	}
	else
	{
		return s_instance;
	}
}
