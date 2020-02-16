#include "Vocation.h"


Vocation::JOBTYPE Vocation::VocationJob = NONE;

std::vector<Vocation::Quest> Vocation::getMainQuest(int day)
{
	std::vector<Vocation::Quest> MainQuests;
	
	
	Quest Push;
	Push.currentNumber = 0;

	// Add SALES quest
	Push.job = SALES;
	Push.maxNumber = 10;
	MainQuests.push_back(Push);

	// Add CLEANER quest
	Push.job = CLEANER;
	Push.maxNumber = 10;
	MainQuests.push_back(Push);

	// Add BOUNCER quest
	Push.job = BOUNCER;
	Push.maxNumber = 10;
	MainQuests.push_back(Push);

	// ADD MECHANIC quest
	Push.job = MECHANIC;
	Push.maxNumber = 10;
	MainQuests.push_back(Push);

	return MainQuests;
}

Vocation::JOBTYPE Vocation::getVocation()
{
	return VocationJob;
}

void Vocation::setVocation(JOBTYPE job)
{
	 Vocation::VocationJob = job;
}


