#pragma once
#include "Tasklist.h"
class Cleanertask : public Tasklist
{
protected:
	Tasklist* task;
public:
	Cleanertask(std::vector<Vocation::Quest>) : task() {};
	std::string Taskstatus(std::vector<Vocation::Quest>);
	std::vector<Vocation::Quest> Addscore(std::vector<Vocation::Quest>);
};

