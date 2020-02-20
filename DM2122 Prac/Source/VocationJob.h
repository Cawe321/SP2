#ifndef VOCATIONJOB_H
#define VOCATIONJOB_H
class VocationJob
{
public:
	enum JOBTYPE
	{
		SALES,
		CLEANER,
		BOUNCER,
		MECHANIC,
		NONE,
	};
	VocationJob(JOBTYPE work);
	VocationJob* getNext();
	VocationJob* getPrevious();
	void setNext(VocationJob* Next);
	void setPrevious(VocationJob* Previous);
	JOBTYPE getJob();
	~VocationJob();
private:
	VocationJob* next;
	VocationJob* previous;
	JOBTYPE job;
};
#endif
