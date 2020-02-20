#pragma once
#include "Tasklist.h"
class Mechanictask : public Tasklist
{
private:
	int Points;
	int Strikes;
protected:
	Tasklist* task;
public:
	Mechanictask();
	~Mechanictask();
	Mechanictask(std::vector<Vocation::Quest>) : task() {};
	std::string Taskstatus(std::vector<Vocation::Quest>);
	std::vector<Vocation::Quest> Addscore(std::vector<Vocation::Quest>);
	void AddStrike();
	void AddPoints();
	int GetStrike();
	int GetPoints();
};

