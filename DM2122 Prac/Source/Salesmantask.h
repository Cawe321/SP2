#pragma once
#include "Tasklist.h"
class Salesmantask :
	public Tasklist
{
protected:
	Tasklist* task;
public:
	Salesmantask(std::vector<Vocation::Quest>) : task() {};
	std::string Taskstatus(std::vector<Vocation::Quest>);
	std::vector<Vocation::Quest> Addscore(std::vector<Vocation::Quest>);
};

