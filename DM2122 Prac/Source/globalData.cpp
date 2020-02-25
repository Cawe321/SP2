#include "globalData.h"



std::string globalData::getinGameTime()
{
	return inGameTime;
}

void globalData::setinGameTime(float time)
{
	inGameTime = std::to_string(time);
}

float globalData::getJob()
{
	return Jobs->getVocation();
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
