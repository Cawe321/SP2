#ifndef VOCATION_H
#define VOCATION_H
#include <vector>
#include "VocationJob.h"
#include "CSalesCustomer.h"

class globalData; // placeholder calss
class Vocation
{
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
	static CSalesCustomer::CustomerType returnCustomerMood();
	static bool isCustomerCurious();
	static std::vector<Vector3> getLitterLocations(int numberOfLitter);
	static Vector3 getCustomerLocation();
	static void setVocation(VocationJob* work);
	static void InitializeJob();
	static void ClearJob(bool programEnd);
	static void ConnectQuest(int day, std::vector<Quest> quest);
	static std::vector<Quest> getConnectedQuest(int day);
private:
	static VocationJob* Sales;
	static VocationJob* Cleaner;
	static VocationJob* Bouncer;
	static VocationJob* Mechanic;
	static VocationJob* None;
	static globalData* Data;
	static std::vector<Quest> mainQuest[3];
};
#endif
