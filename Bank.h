#ifndef BANK_H
#define BANK_H
#include "Vocation.h"
#include "CarSelection.h"

class Bank
{
private:
	// Total amount of money in bank
	int BankAmount = 10000;
	int SalesPersonSalary;
	int CleanerSalary;
	int BouncerSalary;
	int MechanicSalary;
	CarSelection* select;

public:
	void SetMoney();
	int GetMoney();
	void IncreaseMoney(int amount);
	bool Deposit(CarSelection*);
};

#endif
