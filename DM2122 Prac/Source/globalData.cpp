#include "globalData.h"
#include <fstream>

globalData* globalData::s_instance = nullptr;

globalData::globalData()
{
	// check if save file exists first only once, which is only at the start of the game, otherwise, init all private data members as 0
	Loaded = false;
	setinGameTime(600);
	day = 1;
	Jobs == nullptr;
	money = 0;
	
	car1bought = false;
	car2bought = false;
	car3bought = false;
	car4bought = false;
	car5bought = false;
	car6bought = false; 
}

bool globalData::isLoaded()
{
	return Loaded;
}

bool globalData::saveFound()
{
	std::ifstream infile("saveFile.txt");
	return infile.good();
}

void globalData::saveGame()
{
	std::string saveDay = "Day:" + std::to_string(day) + "\n";
	std::string saveTime = "Time:" + inGameTime + "\n";


	std::string saveJob = "Vocation:";
	if (Jobs->getJob() == VocationJob::SALES)
		saveJob.append("SALES\n");
	else if (Jobs->getJob() == VocationJob::CLEANER)
		saveJob.append("CLEANER\n");
	else if (Jobs->getJob() == VocationJob::BOUNCER)
		saveJob.append("BOUNCER\n");
	else if (Jobs->getJob() == VocationJob::MECHANIC)
		saveJob.append("MECHANIC\n");
	
	std::string saveMoney = "Money:" + std::to_string(money) + "\n";

	std::vector<Vocation::Quest> Day1 = Vocation::getConnectedQuest(1);
	std::string saveDay1Quest = "DAY1:";
	for (int i = 0; i < Day1.size(); ++i)
	{
		std::string toAppend = "";
		if (Day1[i].job == VocationJob::SALES)
			toAppend.append("S");
		else if (Day1[i].job == VocationJob::CLEANER)
			toAppend.append("C");
		else if (Day1[i].job == VocationJob::BOUNCER)
			toAppend.append("B");
		else if (Day1[i].job == VocationJob::MECHANIC)
			toAppend.append("M");
		toAppend = toAppend + std::to_string(Day1[i].currentNumber) + "/" + std::to_string(Day1[i].maxNumber) + "|";
		saveDay1Quest.append(toAppend);
	}
	saveDay1Quest.append("\n");

	std::vector<Vocation::Quest> Day2 = Vocation::getConnectedQuest(2);
	std::string saveDay2Quest = "DAY2:";
	for (int i = 0; i < Day2.size(); ++i)
	{
		std::string toAppend = "";
		if (Day2[i].job == VocationJob::SALES)
			toAppend.append("S");
		else if (Day2[i].job == VocationJob::CLEANER)
			toAppend.append("C");
		else if (Day2[i].job == VocationJob::BOUNCER)
			toAppend.append("B");
		else if (Day2[i].job == VocationJob::MECHANIC)
			toAppend.append("M");
		toAppend = toAppend + std::to_string(Day2[i].currentNumber) + "/" + std::to_string(Day2[i].maxNumber) + "|";
		saveDay2Quest.append(toAppend);
	}
	saveDay2Quest.append("\n");

	std::vector<Vocation::Quest> Day3 = Vocation::getConnectedQuest(3);
	std::string saveDay3Quest = "DAY3:";
	for (int i = 0; i < Day3.size(); ++i)
	{
		std::string toAppend = "";
		if (Day3[i].job == VocationJob::SALES)
			toAppend.append("S");
		else if (Day3[i].job == VocationJob::CLEANER)
			toAppend.append("C");
		else if (Day3[i].job == VocationJob::BOUNCER)
			toAppend.append("B");
		else if (Day3[i].job == VocationJob::MECHANIC)
			toAppend.append("M");
		toAppend = toAppend + std::to_string(Day3[i].currentNumber) + "/" + std::to_string(Day3[i].maxNumber) + "|";
		saveDay3Quest.append(toAppend);
	}
	saveDay3Quest.append("\n");

	std::ofstream out;
	out.open("saveFile.txt");
	if (out.is_open())
	{
		out << "Futuristic MotorShow Save File\n"; // first line
		out << saveDay + saveTime + saveJob + saveMoney + saveDay1Quest + saveDay2Quest + saveDay3Quest;
		out.close();
	}
	else
	{
		std::ofstream out;
		while (!out.is_open())
		{
			out.open("saveFile.txt"); // loop until it's open
		}
		out << "Futuristic MotorShow Save File\n"; // first line
		out << saveDay + saveTime + saveJob + saveMoney + saveDay1Quest + saveDay2Quest + saveDay3Quest;
		out.close();
	}
}

void globalData::loadGame()
{
	std::ifstream myfile;
	myfile.open("saveFile.txt");
	std::string line;
	while (!myfile.is_open()) {} // wait until file is open
	if (myfile.is_open())
	{
		getline(myfile, line);
		if (line != "Futuristic MotorShow Save File") // incorrect file with same name
		{
			Loaded = false;
			return;
		}
		while (getline(myfile, line))		// reads the information of map 1
		{
			if (line.substr(0, 4) == "Day:")
			{
				day = line[4] - '0';
			}
			else if (line.substr(0, 5) == "Time:")
			{
				inGameTime = line.substr(5, line.length() - 5);
			}
			else if (line.substr(0, 9) == "Vocation:")
			{
				if (line.substr(9, line.length() - 9) == "SALES")
					Jobs = Vocation::getVocationJobClass(VocationJob::SALES);
				else if (line.substr(9, line.length() - 9) == "CLEANER")
					Jobs = Vocation::getVocationJobClass(VocationJob::CLEANER);
				else if (line.substr(9, line.length() - 9) == "BOUNCER")
					Jobs = Vocation::getVocationJobClass(VocationJob::BOUNCER);
				else if (line.substr(9, line.length() - 9) == "MECHANIC")
					Jobs = Vocation::getVocationJobClass(VocationJob::MECHANIC);
			}
			else if (line.substr(0, 6) == "Money:")
			{
				money = std::stoi(line.substr(6, line.length() - 6));
			}
			else if (line.substr(0, 5) == "DAY1:")
			{
				std::string temp = "", newLine = line.substr(5, line.length()-5);
				std::vector<Vocation::Quest> questList;
				bool current = true;
				Vocation::Quest toPush;
				for (int i = 0; newLine[i] != '\0'; ++i)
				{
					if (newLine[i] == '|')
					{
						toPush.maxNumber = std::stoi(temp);
						temp = "";
						questList.push_back(toPush);
					}
					else if (newLine[i] == 'S')
					{
						toPush.job = VocationJob::SALES;
					}
					else if (newLine[i] == 'C')
					{
						toPush.job = VocationJob::CLEANER;
					}
					else if (newLine[i] == 'B')
					{
						toPush.job = VocationJob::BOUNCER;
					}
					else if (newLine[i] == 'M')
					{
						toPush.job = VocationJob::MECHANIC;
					}
					else if (newLine[i] == '/')
					{
						
							toPush.currentNumber = std::stoi(temp);
							temp = "";
					}
					else
					{
						temp.push_back(newLine[i]);
					}

					
				}
				
				Vocation::ConnectQuest(1, questList);
			}
			else if (line.substr(0, 5) == "DAY2:")
			{
				std::string temp = "", newLine = line.substr(5, line.length() - 5);
				std::vector<Vocation::Quest> questList;
				bool current = true;
				Vocation::Quest toPush;
				for (int i = 0; newLine[i] != '\0'; ++i)
				{
					if (newLine[i] == '|')
					{
						toPush.maxNumber = std::stoi(temp);
						temp = "";
						questList.push_back(toPush);
					}
					else if (newLine[i] == 'S')
					{
						toPush.job = VocationJob::SALES;
					}
					else if (newLine[i] == 'C')
					{
						toPush.job = VocationJob::CLEANER;
					}
					else if (newLine[i] == 'B')
					{
						toPush.job = VocationJob::BOUNCER;
					}
					else if (newLine[i] == 'M')
					{
						toPush.job = VocationJob::MECHANIC;
					}
					else if (newLine[i] == '/')
					{

						toPush.currentNumber = std::stoi(temp);
						temp = "";
					}
					else
					{
						temp.push_back(newLine[i]);
					}


				}

				Vocation::ConnectQuest(2, questList);
			}
			else if (line.substr(0, 5) == "DAY3:")
			{
			std::string temp = "", newLine = line.substr(5, line.length() - 5);
			std::vector<Vocation::Quest> questList;
			bool current = true;
			Vocation::Quest toPush;
			for (int i = 0; newLine[i] != '\0'; ++i)
			{
				if (newLine[i] == '|')
				{
					toPush.maxNumber = std::stoi(temp);
					temp = "";
					questList.push_back(toPush);
				}
				else if (newLine[i] == 'S')
				{
					toPush.job = VocationJob::SALES;
				}
				else if (newLine[i] == 'C')
				{
					toPush.job = VocationJob::CLEANER;
				}
				else if (newLine[i] == 'B')
				{
					toPush.job = VocationJob::BOUNCER;
				}
				else if (newLine[i] == 'M')
				{
					toPush.job = VocationJob::MECHANIC;
				}
				else if (newLine[i] == '/')
				{

					toPush.currentNumber = std::stoi(temp);
					temp = "";
				}
				else
				{
					temp.push_back(newLine[i]);
				}


			}

			Vocation::ConnectQuest(3, questList);
			}
		}
		Loaded = true;
		myfile.close();
	}
}

void globalData::buycar(int a)
{
	if (a == 1) {
		car1bought = true;
	}
	else if (a == 2) {
		car2bought = true;
	}
	else if (a == 3) {
		car3bought = true;
	}
	else if (a == 4) {
		car4bought = true;
	}
	else if (a == 5) {
		car5bought = true;
	}
	else if (a == 6) {
		car6bought = true;
	}
}

std::string globalData::getinGameTime()
{
	return inGameTime;
}

void globalData::setinGameTime(float time)
{
	inGameTime = std::to_string(time);
}

VocationJob* globalData::getJob()
{
	if (Jobs == nullptr)
		Jobs = Vocation::getVocationJobClass(VocationJob::NONE);
	return Jobs; // returns the VocationJob* class of the Job
}

void globalData::setJob(VocationJob* career)
{
	Jobs = career;
}

int globalData::getDay()
{
	return day;
}

void globalData::NextDay()
{

	if (std::stof(getinGameTime()) < 0)
	{
		day++;
	}
	
}

float globalData::getMoney()
{
	return money;
}

void globalData::setMoney(int cash)
{
	money = cash;
}

bool globalData::Deposit(CarSelection* input)
{
	select = input;
	if (money >= select->GetIndicator()->GetPrice())
	{
		money -= select->GetIndicator()->GetPrice();
		std::cout << "Bought." << " ";
		std::cout << select->GetIndicator()->GetName() << " " << select->GetIndicator()->GetPrice() << " ";
		return true;
	}
	else
	{
		std::cout << "Not enough." << " ";
		return false;

	}
}

std::vector<Vocation::Quest> globalData::getMainQuest()
{
	return std::vector<Vocation::Quest>();
}

globalData* globalData::instance()
{
	if (!s_instance)
	{
		s_instance = new globalData;
		return s_instance;
	}
	else
	{
		return s_instance;
	}
}
