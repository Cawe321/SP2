#include "Boss.h"

Boss::Boss()
{
	data = globalData::instance();
	if (!(data->isLoaded()))
		data->setGoodWill(50); // dont set if save found is found
}

Boss::~Boss()
{
}

void Boss::AddGoodwill(int input)
{
	//input is a rough measure of how well a previous task was done
	data->setGoodWill(data->getGoodWill() + input);
	if (data->getGoodWill() >= 100) {
		data->setGoodWill(100);
	}
}

void Boss::LoseGoodwill(int input)
{
	//input is a rough measure of how poorly a previous task was done
	data->setGoodWill(data->getGoodWill() - input);
}

int Boss::GetGoodwill()
{
	return data->getGoodWill();
}

float Boss::getmodifier()
{
	int ModifiableGoodwill;
	float ModifiableValue = 0;
	ModifiableGoodwill = data->getGoodWill() - 50;
	if (ModifiableGoodwill > 0) {
		if (ModifiableGoodwill < 10) {
			return 1;
		}
		else {
			while (ModifiableGoodwill > 9) {
				ModifiableValue++;
				ModifiableGoodwill = ModifiableGoodwill - 10;
			}
		}
	}
	else {
		ModifiableGoodwill = ModifiableGoodwill * (-1);
		if (ModifiableGoodwill < 10) {
			return 1;
		}
		else {
			while (ModifiableGoodwill > 9) {
				ModifiableValue++;
				ModifiableGoodwill = ModifiableGoodwill - 10;
			}
		}
		ModifiableValue = ModifiableValue * (-1);
	}
	ModifiableValue = ModifiableValue / 10;
	ModifiableValue = ModifiableValue + 1;
	return ModifiableValue;
}

std::string Boss::PrintGoodwill()
{
	std::string print;
	print = "Boss's Goodwill: ";
	print.append(std::to_string(data->getGoodWill()));
	return print;
}
