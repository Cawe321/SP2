#include "Cleanertask.h"

std::string Cleanertask::Taskstatus(std::vector<Vocation::Quest> data)
{
	std::string output = "";
	std::string edit = "";
	output = "Cleaning: Trash left to pick up ";
	edit = std::to_string(data[1].currentNumber);
	output.append(edit);
	output.append("/");
	edit = {""};
	edit = std::to_string(data[1].maxNumber);
	output.append(edit);
	return output;
}
