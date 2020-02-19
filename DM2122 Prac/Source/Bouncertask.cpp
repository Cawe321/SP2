#include "Bouncertask.h"

std::string Bouncertask::Taskstatus(std::vector<Vocation::Quest> data)
{
	std::string output = "";
	std::string edit = "";
	if (data[2].currentNumber < data[2].maxNumber) {
		output = "Bouncer: Suspects to kick out "; //sorry had to change to suspects or else it wont fit the screen
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

std::vector<Vocation::Quest> Bouncertask::Addscore(std::vector<Vocation::Quest> data)
{
	data[2].currentNumber++;
	return data;
}


