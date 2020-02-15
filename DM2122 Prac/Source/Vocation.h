#ifndef VOCATION_H
#define VOCATION_H
#include <vector>
class Vocation
{

public:
	enum JOBTYPE
	{
		SALES,
		CLEANER,
		BOUNCER,
		MECHANIC,
	};
	struct Quest
	{
		JOBTYPE job; // job type
		int maxNumber; // if currentNumber == maxNumber, player finished the quest.
		int currentNumber; // use it when job is finished, to count the number of times the player finished this job.
	};
	static std::vector<Vocation::Quest> getMainQuest(int day);
	static JOBTYPE getVocation();
	static void setVocation(JOBTYPE job);
	Vocation();
	~Vocation();
private:
	static JOBTYPE VocationJob;
};
#endif
