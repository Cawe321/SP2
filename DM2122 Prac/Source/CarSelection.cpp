#include "CarSelection.h"
#include <iostream>

CarSelection::CarSelection()
{
	first = last = indicator = nullptr;
	noOfCars = 12;
	count = 0;
}

CarSelection::~CarSelection()
{
	//
}

void CarSelection::Up()
{
	if (indicator->GetBack() != nullptr)
		indicator = indicator->GetBack();
}

void CarSelection::Down()
{
	if (indicator->GetNext() != nullptr)
		indicator = indicator->GetNext();
}

void CarSelection::AddCar(std::string name, int price)
{
	Car* newNode = new Car();
	if (count < noOfCars)
	{
		if ((first == NULL) && (last == NULL))
		{
			last = new Car();
			last->SetValues(name, price);
			first = last;
		}
		else
		{
			newNode->SetValues(name, price);
			newNode->SetBack(last);
			last->SetNext(newNode);
			last = newNode;
		}
		++count;
	}
	indicator = first;
}

Car* CarSelection::GetIndicator()
{
	return indicator;
}

void CarSelection::Printing()
{
	Car* temp = first;
	while (temp != nullptr)
	{
		std::cout << temp->GetName() << "										$" << temp->GetPrice() << "\n";
		temp = temp->GetNext();
		if (temp == nullptr)
			break;
	}
}

std::string CarSelection::PrintName(int position)
{
	Car* temp = first;

	int localposition = 0;
	while (temp != nullptr)
	{
		if (localposition == position) {
			return temp->GetName();
		}
		else {
			localposition++;
			temp = temp->GetNext();
		}
	}
}

std::string CarSelection::PrintPrice(int position)
{
	Car* temp = first;

	int localposition = 0;
	while (temp != nullptr)
	{
		if (localposition == position)
			return std::to_string(temp->GetPrice());
		else 
		{
			localposition++;
			temp = temp->GetNext();
		}
	}
}

std::string CarSelection::PrintSelection(CarSelection* Sel)
{
	Car* Temp;
	Temp = Sel->GetIndicator();
	return Temp->GetName();
}

