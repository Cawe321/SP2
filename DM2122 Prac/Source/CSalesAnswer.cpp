#include "CSalesAnswer.h"
#include "CSalesCustomer.h"
std::string CSalesAnswer::getText()
{
	return Text;
}

CSalesAnswer* CSalesAnswer::getNext()
{
	return Next;
}

CSalesAnswer* CSalesAnswer::getPrevious()
{
	return Previous;
}

void CSalesAnswer::setNext(CSalesAnswer* Next)
{
	this->Next = Next;
}

void CSalesAnswer::setPrevious(CSalesAnswer* Previous)
{
	this->Previous = Previous;
}

void CSalesAnswer::setText(std::string Text)
{
	this->Text = Text;
}

CSalesAnswer::CSalesAnswer()
{
	Text = ""; // init text
}


CSalesAnswer::~CSalesAnswer()
{
}




CSalesGoodAnswer::CSalesGoodAnswer(std::string Text)
{
	setText(Text);
}

CSalesGoodAnswer::~CSalesGoodAnswer()
{
}

void CSalesGoodAnswer::answerEffect(CSalesCustomer* customer)
{
	customer->answered = true;
	customer->count = 0;
	customer->setAppeaseRate(customer->getAppeaseRate() + 1);
	if (customer->getAppeaseRate() >= customer->getSatisfactionRate())
		customer->setPlaceHolderText("Thank you for helping!");
	else
		customer->setPlaceHolderText("You are very helpful!");
}

CSalesBadAnswer::CSalesBadAnswer(std::string Text)
{
	setText(Text);
}

CSalesBadAnswer::~CSalesBadAnswer()
{
}

void CSalesBadAnswer::answerEffect(CSalesCustomer * customer)
{
	customer->answered = true;
	customer->count = 0;
	customer->setAppeaseRate(customer->getAppeaseRate() - 1);
	
	if (customer->getAppeaseRate() <= customer->getSatisfactionRate()-4)
		customer->setPlaceHolderText("I am reporting you to your manager!");
	else
		customer->setPlaceHolderText("Please be more polite with me!");
}

CSalesNeutralAnswer::CSalesNeutralAnswer(std::string Text)
{
	setText(Text);
}

CSalesNeutralAnswer::~CSalesNeutralAnswer()
{
}

void CSalesNeutralAnswer::answerEffect(CSalesCustomer * customer)
{
	customer->count = 0;
	customer->setPlaceHolderText("Um... okay?");
	customer->answered = true;
}

CSalesLeaveAnswer::CSalesLeaveAnswer(std::string Text)
{
	setText(Text);
}

CSalesLeaveAnswer::~CSalesLeaveAnswer()
{
}

void CSalesLeaveAnswer::answerEffect(CSalesCustomer * customer)
{
	customer->setPlaceHolderText("You are useless!");
	customer->setAppeaseRate(customer->getSatisfactionRate() - 4); 
	customer->answered = true;
}
