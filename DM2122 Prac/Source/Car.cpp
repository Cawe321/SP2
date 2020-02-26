#include "Car.h"

Car::Car()
	: name("0"), price(0), next(nullptr), back(nullptr)
{

}

Car::~Car()
{
	//do nothing
}

Car* Car::GetNext() const
{
	return next;
}

void Car::SetNext(Car* next)
{
	this->next = next;
}

Car* Car::GetBack() const
{
	return back;
}

void Car::SetBack(Car* back)
{
	this->back = back;
}

void Car::SetValues(std::string name, int price)
{
	this->name = name;
	this->price = price;
}

int Car::GetPrice() const
{
	return price;
}

std::string Car::GetName() const
{
	return name;
}
