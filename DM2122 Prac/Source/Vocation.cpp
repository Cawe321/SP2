#include "Vocation.h"
#include <time.h>

Vocation::JOBTYPE Vocation::VocationJob = NONE;

std::vector<Vocation::Quest> Vocation::getMainQuest(int day)
{
	srand(time(NULL));
	std::vector<Vocation::Quest> MainQuests;
	

	if (day == 1)
	{
		Quest Push;
		Push.currentNumber = 0;

		// Since work is more efficient, lesser number of jobs need to be done by the day.
		// Add SALES quest
		Push.job = SALES;
		if (VocationJob == SALES)
			Push.maxNumber = 1;
		else
			Push.maxNumber = 2;
		MainQuests.push_back(Push);

		// Add CLEANER quest
		Push.job = CLEANER;
		if (VocationJob == CLEANER)
			Push.maxNumber = 1;
		else
			Push.maxNumber = 2;
		MainQuests.push_back(Push);

		// Add BOUNCER quest
		Push.job = BOUNCER;
		if (VocationJob == BOUNCER)
			Push.maxNumber = 0;
		else
			Push.maxNumber = 1;
		MainQuests.push_back(Push);

		// ADD MECHANIC quest
		Push.job = MECHANIC;
		if (VocationJob == MECHANIC)
			Push.maxNumber = 1;
		else 
			Push.maxNumber = 2;
		MainQuests.push_back(Push);
	}
	else if (day == 2)
	{
		// Range is 2-4 quests | Bouncer - 1 Quest of the number from Rand is removed
		Quest Push;
		Push.currentNumber = 0;
		Push.job = SALES;
		if (VocationJob == SALES)
			Push.maxNumber = (int)(((rand() % 3) + 2) / 2);
		else
			Push.maxNumber = ((rand() % 3) + 2);

		MainQuests.push_back(Push);

		Push.job = CLEANER;
		if (VocationJob == CLEANER)
			Push.maxNumber = (int)(((rand() % 3) + 2) / 2);
		else
			Push.maxNumber = ((rand() % 3) + 2);
		MainQuests.push_back(Push);

		Push.job = BOUNCER;
		if (VocationJob == BOUNCER)
			Push.maxNumber = (int)((((rand() % 3) + 2)) / 2) - 1;
		else
			Push.maxNumber = ((rand() % 3) + 2) - 1;
		MainQuests.push_back(Push);

		Push.job = MECHANIC;
		if (VocationJob == MECHANIC)
			Push.maxNumber = (int)(((rand() % 3) + 2) / 2);
		else
			Push.maxNumber = ((rand() % 3) + 2);
		MainQuests.push_back(Push);
	}
	else if (day == 3)
	{
		// Range is 2-5 quests | Bouncer - 1 Quest of the number from Rand is removed
		Quest Push;
		Push.currentNumber = 0;
		Push.job = SALES;
		if (VocationJob == SALES)
			Push.maxNumber = (int)(((rand() % 4) + 2) / 2);
		else
			Push.maxNumber = ((rand() % 4) + 2);
		MainQuests.push_back(Push);

		Push.job = CLEANER;
		if (VocationJob == CLEANER)
			Push.maxNumber = (int)(((rand() % 4) + 2) / 2);
		else
			Push.maxNumber = ((rand() % 4) + 2);
		MainQuests.push_back(Push);

		Push.job = BOUNCER;
		if (VocationJob == BOUNCER)
			Push.maxNumber = (int)((((rand() % 4) + 2)) / 2) - 1;
		else if ((int)((((rand() % 4) + 2)) / 2) - 1 == 0)
			Push.maxNumber = 1; // must have at least 1 Bouncer Quest for day 2
		else
			Push.maxNumber = ((rand() % 4) + 2) - 1;
		MainQuests.push_back(Push);

		Push.job = MECHANIC;
		if (VocationJob == MECHANIC)
			Push.maxNumber = (int)(((rand() % 4) + 2) / 2);
		else
			Push.maxNumber = ((rand() % 4) + 2);
		MainQuests.push_back(Push);
	}
	else printf("Error Generating Main Quest");

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


