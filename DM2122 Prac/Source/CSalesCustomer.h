#ifndef CSALESCUSTOMER_H
#define CSALESCUSTOMER_H
#include <string>
#include "CSalesAnswer.h"

// together with CSalesCustomer since the functions affect CSalesCustomer class


class CSalesCustomer
{
public:
	CSalesCustomer();
	~CSalesCustomer();
	//animValues
	int count;
	float debounceTime;
	float elapsedTime; // elapsed time for Customer UI only
	bool answered;

	struct SalesQuestion
	{
		std::string Question;
		CSalesAnswer* GoodAnswer;	// 0
		CSalesAnswer* BadAnswer1;	// 1
		CSalesAnswer* BadAnswer2;	// 2
		CSalesAnswer* NeutralAnswer;// 3
		CSalesAnswer* LeaveAnswer;	// 4
	};
	enum CustomerType // mood of customer
	{
		HAPPY, NEUTRAL, MAD
	};
	int getAppeaseRate();
	int getSatisfactionRate();
	std::string getPlaceHolderText();
	std::string getQuestion();
	std::string getName();
	void setAppeaseRate(int x);
	CSalesAnswer* getPlayerChoice();
	void setPlayerChoice(CSalesAnswer* playerChoice);
	void CustomerUpdate(float dt);
	void setPlaceHolderText(std::string text);
	void isAppeased();
private:
	std::string placeHolderText;
	SalesQuestion QuestionList[5];
	CSalesAnswer* playerChoice;
	CustomerType customerMood;
	int appeaseRate; // the satisfied status of the customer in numbers
	int satisfactionRate; // once appeaseRate >= satisfactionRate, customer is satisfied with service & player wins the minigame
	SalesQuestion ChosenQuestion;
	
	void InitializeQuestions();
	bool satisfactionCheck();
	void ChooseQuestion();
	void RandomizeAnswer(SalesQuestion Question);
};






#endif