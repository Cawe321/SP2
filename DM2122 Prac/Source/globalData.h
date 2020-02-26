#pragma once
#include "Vocation.h"
#include <string>
#include <vector>
class globalData
{
private:
	std::string inGameTime;
	int day;
	VocationJob* Jobs;
	float money;
	static globalData* s_instance;
	globalData();
	bool Loaded;
public:
	bool isLoaded();
	bool saveFound();
	void saveGame();
	void loadGame();

	std::string getinGameTime();
	void setinGameTime(float time);

	VocationJob* getJob();
	void setJob(VocationJob* career);
	
	int getDay();
	void NextDay();

	float getMoney();
	void setMoney(float cash);

	std::vector<Vocation::Quest> getMainQuest();

	static globalData* instance();
};

