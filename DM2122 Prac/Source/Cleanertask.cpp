#include "Cleanertask.h"

std::string Cleanertask::Taskstatus(std::vector<Vocation::Quest> data)
{
	std::string output = "";
	std::string edit = "";
	if (data[1].currentNumber < data[1].maxNumber) {
		output = "Cleaning: Trash left to pick up ";
		edit = std::to_string(data[1].currentNumber);
		output.append(edit);
		output.append("/");
		edit = { "" };
		edit = std::to_string(data[1].maxNumber);
		output.append(edit);
	}
	else {
		output = "Cleaning : Completed";
	}
	return output;
}

void Cleanertask::Addscore(std::vector<Vocation::Quest> data)
{
	data[1].currentNumber++;
}
