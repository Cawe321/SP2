#include "CSalesCustomer.h"
#include "Application.h"
#include "Vocation.h"


int CSalesCustomer::getAppeaseRate()
{
	return appeaseRate;
}
int CSalesCustomer::getSatisfactionRate()
{
	return satisfactionRate;
}
void CSalesCustomer::setAppeaseRate(int x)
{
	appeaseRate = x;
}
std::string CSalesCustomer::getQuestion()
{
	return ChosenQuestion.Question;
}
CSalesCustomer::CSalesCustomer(Vector3 coordinates)
{
	this->coordinates = coordinates;
	answered = false;
	debounceTime = 0;
	elapsedTime = 0;
	customerMood = Vocation::returnCustomerMood();
	if (customerMood == HAPPY)
		satisfactionRate = 1;
	else if (customerMood == NEUTRAL)
		satisfactionRate = 2;
	else
		satisfactionRate = 3;

	InitializeQuestions();
	ChooseQuestion();
}

CSalesCustomer::~CSalesCustomer()
{
}

CSalesAnswer* CSalesCustomer::getPlayerChoice()
{
	return playerChoice;
}

void CSalesCustomer::setPlayerChoice(CSalesAnswer* playerChoice)
{
	this->playerChoice = playerChoice;
}

void CSalesCustomer::CustomerUpdate(float dt)
{
	elapsedTime += dt;
	if  (elapsedTime - debounceTime > 0.25f && Application::IsKeyPressed('Q') && answered == false)
	{
		debounceTime = elapsedTime;
		playerChoice = playerChoice->getPrevious();
	}
	else if (elapsedTime - debounceTime > 0.25f && Application::IsKeyPressed('E') && answered == false)
	{
		debounceTime = elapsedTime;
		playerChoice = playerChoice->getNext();
	}
	if (Application::IsKeyPressed('I') && answered == false)
		playerChoice->answerEffect(this);

}

void CSalesCustomer::setPlaceHolderText(std::string text)
{
	placeHolderText = text;
}

std::string CSalesCustomer::getPlaceHolderText()
{
	return placeHolderText;
}

void CSalesCustomer::isAppeased()
{
	if (satisfactionCheck())
	{
		// do something to show customer is satisfied and won the minigame
		soundEngine->play2D("audio\\yay.mp3", false);
		delete this;
	}
	else if (appeaseRate <= (satisfactionRate - 4))
	{
		// do something to show customer is disatisfied  and lostthe minigame
		soundEngine->play2D("audio\\negativebeep.mp3", false);
		delete this;
	}
	else
	{
		ChooseQuestion();
	}
}

std::string CSalesCustomer::getName()
{
	if (customerMood == HAPPY)
		return "Happy Customer";
	else if (customerMood == NEUTRAL)
		return "Customer";
	else return "Mad Customer";
}





void CSalesCustomer::InitializeQuestions()
{
	QuestionList[0].Question = "I am curious about this car. Will you help me?";
	QuestionList[0].GoodAnswer = new CSalesGoodAnswer("Sure! I will do my best to help.");
	QuestionList[0].BadAnswer1 = new CSalesBadAnswer("I am too busy for you, you won't be buying anyways.");
	QuestionList[0].BadAnswer2 = new CSalesBadAnswer("I would like to help but you have no money");
	QuestionList[0].NeutralAnswer = new CSalesNeutralAnswer("I am not sure what you mean.");
	QuestionList[0].LeaveAnswer = new CSalesLeaveAnswer("Sorry! I am currently busy. I will get someone else to help");

	QuestionList[1].Question = "I have questions about this fine car...";
	QuestionList[1].GoodAnswer = new CSalesGoodAnswer("I am willing to assist you in any way.");
	QuestionList[1].BadAnswer1 = new CSalesBadAnswer("Sureeeeeeee");
	QuestionList[1].BadAnswer2 = new CSalesBadAnswer("Noted with thanks!");
	QuestionList[1].NeutralAnswer = new CSalesNeutralAnswer("Hmm...");
	QuestionList[1].LeaveAnswer = new CSalesLeaveAnswer("Sorry! I am currently busy. I will get someone else to help!");

	QuestionList[2].Question = "I would like to know more about this car.";
	QuestionList[2].GoodAnswer = new CSalesGoodAnswer("What would you like to know?");
	QuestionList[2].BadAnswer1 = new CSalesBadAnswer("Just read the description...");
	QuestionList[2].BadAnswer2 = new CSalesBadAnswer("This car is a car.");
	QuestionList[2].NeutralAnswer = new CSalesNeutralAnswer("This car has 4 wheels, not 4 legs.");
	QuestionList[2].LeaveAnswer = new CSalesLeaveAnswer("Sorry, I am not sure. Bye!");

	QuestionList[3].Question = "Is this car worth the money?";
	QuestionList[3].GoodAnswer = new CSalesGoodAnswer("I would highly recommend it!");
	QuestionList[3].BadAnswer1 = new CSalesBadAnswer("It's worth the money which you don't have.");
	QuestionList[3].BadAnswer2 = new CSalesBadAnswer("Find me only if you have the money...");
	QuestionList[3].NeutralAnswer = new CSalesNeutralAnswer("I am not sure what you mean.");
	QuestionList[3].LeaveAnswer = new CSalesLeaveAnswer("Sorry! I am currently busy.");

	QuestionList[4].Question = "What do you think of this car?";
	QuestionList[4].GoodAnswer = new CSalesGoodAnswer("It's an awesome car worth it's money.");
	QuestionList[4].BadAnswer1 = new CSalesBadAnswer("Not worth your time.");
	QuestionList[4].BadAnswer2 = new CSalesBadAnswer("Ask me only if you have the money");
	QuestionList[4].NeutralAnswer = new CSalesNeutralAnswer("I don't I can answer that question.");
	QuestionList[4].LeaveAnswer = new CSalesLeaveAnswer("Sorry! I am currently busy. I will get someone else to help");
}

bool CSalesCustomer::satisfactionCheck()
{
	if (appeaseRate >= satisfactionRate)
		return true;
	else return false;
}

void CSalesCustomer::ChooseQuestion()
{
	count = 0;
	// random question number generator
	int i = rand() % 5; // replace number with number of Questions
	ChosenQuestion = QuestionList[i];
	RandomizeAnswer(QuestionList[i]);
}

void CSalesCustomer::RandomizeAnswer(SalesQuestion Question)
{
	CSalesAnswer* temp = playerChoice;
	int store[4] = { 5,5,5,5};
	for (int i = 0; i < 4; ++i)
	{
		int randomNo;
		CSalesAnswer* temp2 = temp;
		while (true)
		{
			randomNo = rand() % 4;
			if (store[0] != randomNo && store[1] != randomNo && store[2] != randomNo && store[3] != randomNo)
			{
				store[i] = randomNo;
				break;
			}
		}


		if (randomNo == 0)
		{
			if (i == 0) // if first loop
			{
				playerChoice = temp = Question.GoodAnswer;
			}
			else
			{
				temp->setNext(Question.GoodAnswer);
				temp = temp->getNext();
				temp->setPrevious(temp2);
			}
		}
		else if (randomNo == 1)
		{
			if (i == 0) // if first loop
			{
				playerChoice = temp = Question.BadAnswer1;
			}
			else
			{
				temp->setNext(Question.BadAnswer1);
				temp = temp->getNext();
				temp->setPrevious(temp2);
			}
		}
		else if (randomNo == 2)
		{
			if (i == 0) // if first loop
			{
				playerChoice = temp = Question.BadAnswer2;
			}
			else
			{
				temp->setNext(Question.BadAnswer2);
				temp = temp->getNext();
				temp->setPrevious(temp2);
			}
		}
		else if (randomNo == 3)
		{
			if (i == 0) // if first loop
			{
				playerChoice = temp = Question.NeutralAnswer;
			}
			else
			{
				temp->setNext(Question.NeutralAnswer);
				temp = temp->getNext();
				temp->setPrevious(temp2);
			}
		}
	}
	temp->setNext(Question.LeaveAnswer);
	temp->getNext()->setPrevious(temp);
	temp->getNext()->setNext(playerChoice);
	
	playerChoice->setPrevious(Question.LeaveAnswer); //since wrap around
}





