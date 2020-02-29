#pragma once
#include "Vocation.h"
#include <string>
#include <vector>
#include "CarSelection.h"
class globalData
{
private:
	std::string inGameTime;
	int day;
	VocationJob* Jobs;
	int money;
	static globalData* s_instance;
	globalData();
	bool Loaded;
	CarSelection* select;
	int Goodwill;
	bool car1bought;
	bool car2bought;
	bool car3bought;
	bool car4bought;
	bool car5bought;
	bool car6bought;
public:
	bool isLoaded();
	bool saveFound();
	void saveGame();
	void loadGame();

	int getGoodWill();
	void setGoodWill(int Will);

	std::string getinGameTime();
	void setinGameTime(float time);

	VocationJob* getJob();
	void setJob(VocationJob* career);
	
	int getDay();
	void NextDay();

	float getMoney();
	void setMoney(int cash);
	void buycar(int);
	bool owncar(int car);
	bool Deposit(CarSelection* input);
	
	std::vector<Vocation::Quest> getMainQuest();

	static globalData* instance();
};

