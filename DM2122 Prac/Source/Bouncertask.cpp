#include "Bouncertask.h"

std::string Bouncertask::Taskstatus(std::vector<Vocation::Quest> data)
{
	std::string output = "";
	std::string edit = "";
	output = "Bouncer: Suspicious People to kick out ";
	edit = std::to_string(data[2].currentNumber);
	output.append(edit);
	output.append("/");
	edit = { "" };
	edit = std::to_string(data[2].maxNumber);
	output.append(edit);
	return output;
}
