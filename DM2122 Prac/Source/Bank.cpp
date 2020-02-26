#include "Bank.h"
#include <iostream>
void Bank::SetMoney()
{
	SalesPersonSalary = 10000;
	CleanerSalary = 9000;
	BouncerSalary = 12000;
	MechanicSalary = 11000;
	if (Vocation::getVocation() == VocationJob::SALES)
		SalesPersonSalary = SalesPersonSalary * 1.2;
	if (Vocation::getVocation() == VocationJob::CLEANER)
		CleanerSalary = CleanerSalary * 1.2;
	if (Vocation::getVocation() == VocationJob::BOUNCER)
		BouncerSalary = BouncerSalary * 1.2;
	if (Vocation::getVocation() == VocationJob::MECHANIC)
		MechanicSalary = MechanicSalary * 1.2;
}

int Bank::GetMoney()
{
	return BankAmount;
}

void Bank::IncreaseMoney(int amount)
{
	//if sales quest is complete
	//BankAmount += SalesPersonSalary
	//if cleaner quest is complete
	//BankAmount += CleanerSalary
	//if bouncer quest is complete
	//BankAmount += BouncerSalary
	//if mechanic quest is complete
	//BankAmount += MechanicSalary
}

bool Bank::Deposit(CarSelection* input)
{
	select = input;
	if (BankAmount >= select->GetIndicator()->GetPrice())
	{
		BankAmount -= select->GetIndicator()->GetPrice();
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
