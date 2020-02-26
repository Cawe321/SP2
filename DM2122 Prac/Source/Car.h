#ifndef CAR_H
#define CAR_H
#include <string>
class Car
{
private:
	std::string name;
	int price;
	Car* next;
	Car* back;

public:
	Car();
	~Car();

	Car* GetNext() const;
	void SetNext(Car* next);
	Car* GetBack() const;
	void SetBack(Car* back);
	void SetValues(std::string name, int price);
	int GetPrice() const;
	std::string GetName() const;
};

#endif
