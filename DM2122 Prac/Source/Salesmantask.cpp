#include "Salesmantask.h"

std::string Salesmantask::Taskstatus(std::vector<Vocation::Quest> data)
{
	std::string output = "";
	std::string edit = "";
	output = "Sales: People you've conned: ";
	edit = std::to_string(data[0].currentNumber);
	output.append(edit);
	output.append("/");
	edit = { "" };
	edit = std::to_string(data[0].maxNumber);
	output.append(edit);
	return output;
}
