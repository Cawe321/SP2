#include "Vocation.h"
#include "VocationJob.h"
#include <time.h>

// Prevent error of external objeects becoz they are static
VocationJob* Vocation::VocationWork = new VocationJob(VocationJob::NONE);
VocationJob* Vocation::Sales;
VocationJob* Vocation::Cleaner;
VocationJob* Vocation::Bouncer;
VocationJob* Vocation::Mechanic;
VocationJob* Vocation::None;


VocationJob* Vocation::getVocationJobClass(VocationJob::JOBTYPE theType)
{
	if (theType == VocationJob::SALES)
		return Sales;
	else if (theType == VocationJob::CLEANER)
		return Cleaner;
	else if (theType == VocationJob::BOUNCER)
		return Bouncer;
	else if (theType == VocationJob::MECHANIC)
		return Mechanic;
}
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
		Push.job = VocationJob::SALES;
		if (VocationWork->getJob() == VocationJob::SALES)
			Push.maxNumber = 1;
		else
			Push.maxNumber = 2;
		MainQuests.push_back(Push);

		// Add CLEANER quest
		Push.job = VocationJob::CLEANER;
		if (VocationWork->getJob() == VocationJob::CLEANER)
			Push.maxNumber = 1;
		else
			Push.maxNumber = 2;
		MainQuests.push_back(Push);

		// Add BOUNCER quest
		Push.job = VocationJob::BOUNCER;
		if (VocationWork->getJob() == VocationJob::BOUNCER)
			Push.maxNumber = 0;
		else
			Push.maxNumber = 1;
		MainQuests.push_back(Push);

		// ADD MECHANIC quest
		Push.job = VocationJob::MECHANIC;
		if (VocationWork->getJob() == VocationJob::MECHANIC)
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
		Push.job = VocationJob::SALES;
		if (VocationWork->getJob() == VocationJob::SALES)
			Push.maxNumber = (int)(((rand() % 3) + 2) / 2);
		else
			Push.maxNumber = ((rand() % 3) + 2);

		MainQuests.push_back(Push);

		Push.job = VocationJob::CLEANER;
		if (VocationWork->getJob() == VocationJob::CLEANER)
			Push.maxNumber = (int)(((rand() % 3) + 2) / 2);
		else
			Push.maxNumber = ((rand() % 3) + 2);
		MainQuests.push_back(Push);

		Push.job = VocationJob::BOUNCER;
		if (VocationWork->getJob() == VocationJob::BOUNCER)
			Push.maxNumber = (int)((((rand() % 3) + 2)) / 2) - 1;
		else
			Push.maxNumber = ((rand() % 3) + 2) - 1;
		MainQuests.push_back(Push);

		Push.job = VocationJob::MECHANIC;
		if (VocationWork->getJob() == VocationJob::MECHANIC)
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
		Push.job = VocationJob::SALES;
		if (VocationWork->getJob() == VocationJob::SALES)
			Push.maxNumber = (int)(((rand() % 4) + 2) / 2);
		else
			Push.maxNumber = ((rand() % 4) + 2);
		MainQuests.push_back(Push);

		Push.job = VocationJob::CLEANER;
		if (VocationWork->getJob() == VocationJob::CLEANER)
			Push.maxNumber = (int)(((rand() % 4) + 2) / 2);
		else
			Push.maxNumber = ((rand() % 4) + 2);
		MainQuests.push_back(Push);

		Push.job = VocationJob::BOUNCER;
		if (VocationWork->getJob() == VocationJob::BOUNCER)
			Push.maxNumber = (int)((((rand() % 4) + 2)) / 2) - 1;
		else if ((int)((((rand() % 4) + 2)) / 2) - 1 == 0)
			Push.maxNumber = 1; // must have at least 1 Bouncer Quest for day 2
		else
			Push.maxNumber = ((rand() % 4) + 2) - 1;
		MainQuests.push_back(Push);

		Push.job = VocationJob::MECHANIC;
		if (VocationWork->getJob() == VocationJob::MECHANIC)
			Push.maxNumber = (int)(((rand() % 4) + 2) / 2);
		else
			Push.maxNumber = ((rand() % 4) + 2);
		MainQuests.push_back(Push);
	}
	else printf("Error Generating Main Quest");

	return MainQuests;
}

VocationJob::JOBTYPE Vocation::getVocation()
{
	return VocationWork->getJob();
}

void Vocation::setVocation(VocationJob* job)
{
	Vocation::VocationWork = job;
}

void Vocation::InitializeJob()
{
	Sales = new VocationJob(VocationJob::SALES);
	Cleaner = new VocationJob(VocationJob::CLEANER);
	Bouncer = new VocationJob(VocationJob::BOUNCER);
	Mechanic = new VocationJob(VocationJob::MECHANIC);
	Sales->setNext(Cleaner);
	Cleaner->setNext(Bouncer);
	Bouncer->setNext(Mechanic);
	Mechanic->setNext(Sales);

	Sales->setPrevious(Mechanic);
	Cleaner->setPrevious(Sales);
	Bouncer->setPrevious(Cleaner);
	Mechanic->setPrevious(Bouncer);

	None = new VocationJob(VocationJob::NONE);
	None->setNext(nullptr);
	None->setPrevious(nullptr);

}


/* remember to call this line when ending program!!!!!!!!!!!!!!
programEnd = True when program is ending to prevent memory leaks
programEnd = False when program is still running
*/
void Vocation::ClearJob(bool programEnd) 
{
	if (programEnd)
	{
		if (Sales != nullptr)
			delete Sales;
		if (Cleaner != nullptr)
			delete Cleaner;
		if (Bouncer != nullptr)
			delete Bouncer;
		if (Mechanic != nullptr)
			delete Mechanic;
		delete None;
	}
	else
	{
		if (Sales != VocationWork)
			delete Sales;
		if (Cleaner != VocationWork)
			delete Cleaner;
		if (Bouncer != VocationWork)
			delete Bouncer;
		if (Mechanic != VocationWork)
			delete Mechanic;
	}

}


