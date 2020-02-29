#ifndef CARSELECTION_H
#define CARSELECTION_H
#include "Car.h"

class CarSelection
{
private:
	Car* first, * last, * indicator;
	int noOfCars;
	int count;

public:
	CarSelection();
	~CarSelection();

	void Up();
	void Down();
	void AddCar(std::string name, int price);
	Car* GetIndicator();
	void Printing();
	std::string PrintName(int);
	std::string PrintPrice(int);
	std::string PrintSelection(CarSelection*);
};
#endif
