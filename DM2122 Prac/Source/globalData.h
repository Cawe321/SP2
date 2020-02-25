#pragma once
#include "Vocation.h"
#include <string>
class globalData
{
private:
	std::string inGameTime;
	VocationJob* Jobs;
	float money;
	static globalData* s_instance;
	globalData();
public:
	std::string getinGameTime();
	void setinGameTime(float time);

	VocationJob* getJob();
	void setJob(VocationJob* career);

	float getMoney();
	void setMoney(float cash);

	static globalData* instance();
};

