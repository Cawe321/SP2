#ifndef CSALESANSWER_H
#define CSALESANSWER_H
#include <string>
class CSalesCustomer;

class CSalesAnswer
{
	std::string Text;
	CSalesAnswer* Next;
	CSalesAnswer* Previous;
public:
	std::string getText();
	CSalesAnswer* getNext();
	CSalesAnswer* getPrevious();
	void setNext(CSalesAnswer* Next);
	void setPrevious(CSalesAnswer* Previous);
	void setText(std::string Text);
	virtual void answerEffect(CSalesCustomer* customer) = 0;
	CSalesAnswer();
	~CSalesAnswer();
};

class CSalesGoodAnswer : public CSalesAnswer
{
public:
	CSalesGoodAnswer(std::string Text);
	~CSalesGoodAnswer();
	void answerEffect(CSalesCustomer* customer);
};

class CSalesBadAnswer : public CSalesAnswer
{
public:
	CSalesBadAnswer(std::string Text);
	~CSalesBadAnswer();
	void answerEffect(CSalesCustomer* customer);
};

class CSalesNeutralAnswer : public CSalesAnswer
{
public:
	CSalesNeutralAnswer(std::string Text);
	~CSalesNeutralAnswer();
	void answerEffect(CSalesCustomer* customer);
};

class CSalesLeaveAnswer : public CSalesAnswer
{
public:
	CSalesLeaveAnswer(std::string Text);
	~CSalesLeaveAnswer();
	void answerEffect(CSalesCustomer* customer);
};


#endif