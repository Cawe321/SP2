#pragma once
#include <string>
#include <iostream>
#include <vector>
#include "Vocation.h"
class Tasklist
{
public:
	virtual std::string Taskstatus(std::vector<Vocation::Quest> data) = 0;
};

