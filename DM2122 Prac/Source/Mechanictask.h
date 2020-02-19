#pragma once
#include "Tasklist.h"
class Mechanictask : public Tasklist
{
protected:
	Tasklist* task;
public:
	Mechanictask(std::vector<Vocation::Quest>) : task() {};
	std::string Taskstatus(std::vector<Vocation::Quest>);
	void Addscore(std::vector<Vocation::Quest>);
};

