#include "Bouncertask.h"

std::string Bouncertask::Taskstatus(std::vector<Vocation::Quest> data)
{
	std::string output = "";
	std::string edit = "";
	if (data[2].currentNumber < data[2].maxNumber) {
		output = "Bouncer: Suspicious People to kick out ";
		edit = std::to_string(data[2].currentNumber);
		output.append(edit);
		output.append("/");
		edit = { "" };
		edit = std::to_string(data[2].maxNumber);
		output.append(edit);
	}
	else {
		output = "Bouncer: Completed";
	}
	return output;
}
