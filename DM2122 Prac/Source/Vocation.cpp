#include "VocationJob.h"
#include <time.h>
#include "globalData.h"
#include "CollisionCheck.h"
// Prevent error of external objeects becoz they are static

VocationJob* Vocation::Sales;
VocationJob* Vocation::Cleaner;
VocationJob* Vocation::Bouncer;
VocationJob* Vocation::Mechanic;
VocationJob* Vocation::None;
std::vector<Vocation::Quest> Vocation::mainQuest[3];
globalData* Vocation::Data = globalData::instance();

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
	else return None;
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
		if (Data->getJob()->getJob()  == VocationJob::SALES)
			Push.maxNumber = 1;
		else
			Push.maxNumber = 2;
		MainQuests.push_back(Push);

		// Add CLEANER quest
		Push.job = VocationJob::CLEANER;
		if (Data->getJob()->getJob() == VocationJob::CLEANER)
			Push.maxNumber = 1;
		else
			Push.maxNumber = 2;
		MainQuests.push_back(Push);

		// Add BOUNCER quest
		Push.job = VocationJob::BOUNCER;
		Push.maxNumber = 1;
		MainQuests.push_back(Push);

		// ADD MECHANIC quest
		Push.job = VocationJob::MECHANIC;
		if (Data->getJob()->getJob() == VocationJob::MECHANIC)
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
		if (Data->getJob()->getJob() == VocationJob::SALES)
			Push.maxNumber = (int)(((rand() % 3) + 2) / 2);
		else
			Push.maxNumber = ((rand() % 3) + 2);

		MainQuests.push_back(Push);

		Push.job = VocationJob::CLEANER;
		if (Data->getJob()->getJob() == VocationJob::CLEANER)
			Push.maxNumber = (int)(((rand() % 3) + 2) / 2);
		else
			Push.maxNumber = ((rand() % 3) + 2);
		MainQuests.push_back(Push);

		Push.job = VocationJob::BOUNCER;
		Push.maxNumber = 1;
		MainQuests.push_back(Push);

		Push.job = VocationJob::MECHANIC;
		if (Data->getJob()->getJob() == VocationJob::MECHANIC)
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
		if (Data->getJob()->getJob() == VocationJob::SALES)
			Push.maxNumber = (int)(((rand() % 4) + 2) / 2);
		else
			Push.maxNumber = ((rand() % 4) + 2);
		MainQuests.push_back(Push);

		Push.job = VocationJob::CLEANER;
		if (Data->getJob()->getJob() == VocationJob::CLEANER)
			Push.maxNumber = (int)(((rand() % 4) + 2) / 2);
		else
			Push.maxNumber = ((rand() % 4) + 2);
		MainQuests.push_back(Push);

		Push.job = VocationJob::BOUNCER;
		Push.maxNumber = 1;
		MainQuests.push_back(Push);

		Push.job = VocationJob::MECHANIC;
		if (Data->getJob()->getJob() == VocationJob::MECHANIC)
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
	return Data->getJob()->getJob();
}

CSalesCustomer::CustomerType Vocation::returnCustomerMood()
{
	int magicNo = rand() % 10;
	if (Data->getJob()->getJob() == VocationJob::SALES)
	{
		if (magicNo >= 0 && magicNo <= 4) // 50%
			return CSalesCustomer::HAPPY;
		else if (magicNo >= 5 && magicNo <= 7) // 30%
			return CSalesCustomer::NEUTRAL;
		else
			return CSalesCustomer::MAD;
	}
	else
	{
		if (magicNo >= 0 && magicNo <= 2) // 30%
			return CSalesCustomer::HAPPY;
		else if (magicNo >= 3 && magicNo <= 6) // 40%
			return CSalesCustomer::NEUTRAL;
		else
			return CSalesCustomer::MAD; // 30 %
	}
}

bool Vocation::isCustomerCurious()
{
	if (Data->getJob()->getJob() == VocationJob::SALES)
	{
		int magicNo = rand() % 5000;
		if (magicNo < 2) // 0.004% Chance
			return true;
		else return false;
	}
	else
	{
		int magicNo = rand() % 1000;
		if (magicNo < 1) // 0.004% Chance
			return true;
		else return false;
	}
}

std::vector<Vector3> Vocation::getLitterLocations(int numberOfLitter)
{
	// MUST CHANGE ONCE CONVENTION IS COMPLETED!!!!!!!!!
	srand(time(NULL));
	std::vector<Vector3> toReturn;
	int MAX_LENGTH = 100, MAX_WIDTH = 100;
	for (int i = 0; i < numberOfLitter; i++)
	{
		Vector3 location = { rand() % 96 - (float)48, -0.5f, rand() % 96 - (float)48 };
		while (!CollisionCheck::NonCharacterCollisionCheck(location, 2.f, 2.f)) // loop until a good coordinate can be used
		{
			location = { rand() % 96 - (float)48, -0.5f, rand() % 96 - (float)48 };
		}
		toReturn.push_back(location);
	}
	return toReturn;
}

Vector3 Vocation::getCustomerLocation()
{
	Vector3 location = { rand() % 96 - (float)48, -0.5f, rand() % 96 - (float)48 };
	while (!CollisionCheck::NonCharacterCollisionCheck(location, 2.f, 2.f)) // loop until a good coordinate can be used
	{
		location = { rand() % 96 - (float)48, -0.5f, rand() % 96 - (float)48 };
	}
	return location;
}

void Vocation::setVocation(VocationJob* job)
{
	Data->setJob(job);
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
		if (Sales != Data->getJob())
			delete Sales;
		if (Cleaner != Data->getJob())
			delete Cleaner;
		if (Bouncer != Data->getJob())
			delete Bouncer;
		if (Mechanic != Data->getJob())
			delete Mechanic;
	}

}

void Vocation::ConnectQuest(int day, std::vector<Quest> quest)
{
	mainQuest[day - 1] = quest;
}

std::vector<Vocation::Quest> Vocation::getConnectedQuest(int day)
{
	return mainQuest[day - 1];
}


