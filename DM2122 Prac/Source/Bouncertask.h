#include "Tasklist.h"

class Bouncertask : public Tasklist 
{
protected:
	Tasklist* task;
public:
	Bouncertask(std::vector<Vocation::Quest>) : task() {};
	std::string Taskstatus(std::vector<Vocation::Quest>);
	std::vector<Vocation::Quest> Addscore(std::vector<Vocation::Quest>);
};