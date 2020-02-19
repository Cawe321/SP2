#ifndef VOCATION_H
#define VOCATION_H
#include <vector>
#include "VocationJob.h"
class Vocation
{
private:
	static VocationJob* Sales;
	static VocationJob* Cleaner;
	static VocationJob* Bouncer;
	static VocationJob* Mechanic;
	static VocationJob* None;
public:
	static VocationJob* getVocationJobClass(VocationJob::JOBTYPE theType);
	struct Quest
	{
		VocationJob::JOBTYPE job; // job type
		int maxNumber; // if currentNumber == maxNumber, player finished the quest.
		int currentNumber; // use it when job is finished, to count the number of times the player finished this job.
	};
	static std::vector<Vocation::Quest> getMainQuest(int day);
	static VocationJob::JOBTYPE getVocation();
	static void setVocation(VocationJob* work);
	static void InitializeJob();
	static void ClearJob(bool programEnd);
private:
	static VocationJob* VocationWork;
};
#endif
