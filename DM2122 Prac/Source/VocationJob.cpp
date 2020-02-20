#include "VocationJob.h"





VocationJob::VocationJob(JOBTYPE work)
{
	job = work;
}

VocationJob* VocationJob::getNext()
{
	return next;
}

VocationJob* VocationJob::getPrevious()
{
	return previous;
}

void VocationJob::setNext(VocationJob* Next)
{
	next = Next;
}

void VocationJob::setPrevious(VocationJob* Previous)
{
	previous = Previous;
}

VocationJob::JOBTYPE VocationJob::getJob()
{
	return job;
}


VocationJob::~VocationJob()
{
}
