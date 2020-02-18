#include "Mechanictask.h"

std::string Mechanictask::Taskstatus(std::vector<Vocation::Quest> data)
{
	std::string output = "";
	std::string edit = "";
	if (data[3].currentNumber < data[3].maxNumber) {
		output = "Mechanic: Robots left to mantain ";
		edit = std::to_string(data[3].currentNumber);
		output.append(edit);
		output.append("/");
		edit = { "" };
		edit = std::to_string(data[3].maxNumber);
		output.append(edit);
	}
	else {
		output = "Mechanic: Completed";
	}
	return output;
}
