#pragma once
#include <string>
#include <iostream>
#include <vector>
#include "Vocation.h"
class Tasklist
{
public:
	virtual std::string Taskstatus(std::vector<Vocation::Quest> data) = 0;
	virtual std::vector<Vocation::Quest> Addscore(std::vector<Vocation::Quest>) = 0;
};

