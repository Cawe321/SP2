#pragma once
#include "Vocation.h"
#include <string>
class globalData
{
private:
	std::string inGameTime;
	Vocation* Jobs;
	float money;
	static globalData* s_instance;
	globalData();
public:
	std::string getinGameTime();
	void setinGameTime(float time);

	float getJob();
	//void setJob(Vocation* career);

	float getMoney();
	void setMoney(float cash);

	static globalData* instance();
};

