#include "Mechanictask.h"

Mechanictask::Mechanictask()
{
	Points = 0;
	Strikes = 0;
}

Mechanictask::~Mechanictask()
{
}

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

std::vector<Vocation::Quest> Mechanictask::Addscore(std::vector<Vocation::Quest> data)
{
	data[3].currentNumber++;
	return data;
}

void Mechanictask::AddStrike()
{
	Strikes = Strikes + 1;
}

void Mechanictask::AddPoints()
{
	Points = Points + 1;
}

int Mechanictask::GetStrike()
{
	return Strikes;
}

int Mechanictask::GetPoints()
{
	return Points;
}


