#include "SceneText.h"
#include "GL\glew.h"
#include "Application.h"
#include <Mtx44.h>
#include "shader.hpp"
#include "MeshBuilder.h"
#include "Utility.h"
#include "LoadTGA.h"
#include "Cleanertask.h"
#include "Salesmantask.h"
#include "Bouncertask.h"
#include "Achievements.h"
#include "CarSelection.h"
#include "CollisionCheck.h"

#define ROT_LIMIT 45.f;
#define SCALE_LIMIT 5.f;
#define LSPEED 10.f

SceneText::SceneText()
{
	for (int i = 0; i < NUM_GEOMETRY; ++i)
	{
		meshList[i] = NULL;
	}
	Selection = new CarSelection();
	timeData->getMoney();
	if (timeData->isLoaded())
	{
		Day1 = Vocation::getConnectedQuest(1);
		Day2 = Vocation::getConnectedQuest(2);
		Day3 = Vocation::getConnectedQuest(3);
	}
	else
	{
		Day1 = Vocation::getMainQuest(1);
		Day2 = Vocation::getMainQuest(2);
		Day3 = Vocation::getMainQuest(3);
	}
	for (int i = 0; i < Day1.size(); i++)
	{
		if (Day1[i].job == VocationJob::CLEANER)
			Day1Litter = Vocation::getLitterLocations(Day1[i].maxNumber);
	}
	for (int i = 0; i < Day2.size(); i++)
	{
		if (Day2[i].job == VocationJob::CLEANER)
			Day2Litter = Vocation::getLitterLocations(Day2[i].maxNumber);
	}
	for (int i = 0; i < Day3.size(); i++)
	{
		if (Day3[i].job == VocationJob::CLEANER)
			Day3Litter = Vocation::getLitterLocations(Day3[i].maxNumber);
	}
	BossOpinion = new Boss();
	MechanicGameScore = new Mechanictask();
	MechanicGame = false;
	FreezeMovement = false;
	hasmissed = false;
	hasreset = false;

	BankOpen = false;
	NotEnough = false;
	HasCars = false;
	gameover = false;

	CleanerGame = false;

	for (int i = 0; i < 10; i++) {
		game[i] = '-';
	}
	
	Price = 0; //for me to render out the achievements
	
	escapeanimation = false;
	secondescpaeanimation = false;
	thirdescapeanimation = false;
	AchievementScene = false;
	GameScene = true;
	
	no = 1;
	SalesPersonSalary = 10000;
	CleanerSalary = 9000;
	BouncerSalary = 12000;
	MechanicSalary = 11000;

	// init values for customer
	salesCustomer = nullptr; //new CSalesCustomer
	srand(time(NULL));
	for (int i = 0; i < 10; i++)
	{
		customerLocations[i].position = Vocation::getCustomerLocation();
		customerLocations[i].angle = rand() % 360;
		customerLocations[i].customerType = rand() % 2;
		customerLocations[i].inRange = false;
		customerLocations[i].isCurious = false;
		customerLocations[i].selected = false;
	}

	if (timeData->getJob()->getJob() == VocationJob::SALES)
		SalesPersonSalary = SalesPersonSalary * 1.2;
	if (timeData->getJob()->getJob() == VocationJob::CLEANER)
		CleanerSalary = CleanerSalary * 1.2;
	if (timeData->getJob()->getJob() == VocationJob::BOUNCER)
		BouncerSalary = BouncerSalary * 1.2;
	if (timeData->getJob()->getJob() == VocationJob::MECHANIC)
		MechanicSalary = MechanicSalary * 1.2;

}

SceneText::~SceneText()
{
}

void SceneText::Init()
{
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	// Generate a default VAO for now
	glGenVertexArrays(1, &m_vertexArrayID);
	glBindVertexArray(m_vertexArrayID);
	glEnable(GL_CULL_FACE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	camera.Init(Vector3(0, 0, 10), Vector3(0, 0, 0), Vector3(0, 1, 0));
	if (CleanerGame == true)
	{
		camera.Init(Vector3(15, 0, 0), Vector3(0, 0, 0), Vector3(0, 1, 0));
		camera.cameraLock = true;
	}
	Mtx44 projection;
	projection.SetToPerspective(45.f, 4.f / 3.f, 0.1f, 1000.f);
	projectionStack.LoadMatrix(projection);
	
	m_programID = LoadShaders("Shader//Texture.vertexshader", "Shader//Text.fragmentshader");
	
	//m_programID = LoadShaders("Shader//Texture.vertexshader", "Shader//Texture.fragmentshader"); 

	m_parameters[U_MVP] = glGetUniformLocation(m_programID, "MVP");
	m_parameters[U_MODELVIEW] = glGetUniformLocation(m_programID, "MV");
	m_parameters[U_MODELVIEW_INVERSE_TRANSPOSE] = glGetUniformLocation(m_programID, "MV_inverse_transpose");
	m_parameters[U_MATERIAL_AMBIENT] = glGetUniformLocation(m_programID, "material.kAmbient");
	m_parameters[U_MATERIAL_DIFFUSE] = glGetUniformLocation(m_programID, "material.kDiffuse");
	m_parameters[U_MATERIAL_SPECULAR] = glGetUniformLocation(m_programID, "material.kSpecular");
	m_parameters[U_MATERIAL_SHININESS] = glGetUniformLocation(m_programID, "material.kShininess");
	m_parameters[U_LIGHTENABLED] = glGetUniformLocation(m_programID, "lightEnabled");
	m_parameters[U_NUMLIGHTS] = glGetUniformLocation(m_programID, "numLights");


	m_parameters[U_LIGHT0_POSITION] = glGetUniformLocation(m_programID, "lights[0].position_cameraspace");
	m_parameters[U_LIGHT0_COLOR] = glGetUniformLocation(m_programID, "lights[0].color");
	m_parameters[U_LIGHT0_POWER] = glGetUniformLocation(m_programID, "lights[0].power");
	m_parameters[U_LIGHT0_KC] = glGetUniformLocation(m_programID, "lights[0].kC");
	m_parameters[U_LIGHT0_KL] = glGetUniformLocation(m_programID, "lights[0].kL");
	m_parameters[U_LIGHT0_KQ] = glGetUniformLocation(m_programID, "lights[0].kQ");
	m_parameters[U_LIGHT0_TYPE] = glGetUniformLocation(m_programID, "lights[0].type");
	m_parameters[U_LIGHT0_SPOTDIRECTION] = glGetUniformLocation(m_programID, "lights[0].spotDirection");
	m_parameters[U_LIGHT0_COSCUTOFF] = glGetUniformLocation(m_programID, "lights[0].cosCutoff");
	m_parameters[U_LIGHT0_COSINNER] = glGetUniformLocation(m_programID, "lights[0].cosInner");
	m_parameters[U_LIGHT0_EXPONENT] = glGetUniformLocation(m_programID, "lights[0].exponent");

	m_parameters[U_LIGHT1_POSITION] = glGetUniformLocation(m_programID, "lights[1].position_cameraspace");
	m_parameters[U_LIGHT1_COLOR] = glGetUniformLocation(m_programID, "lights[1].color");
	m_parameters[U_LIGHT1_POWER] = glGetUniformLocation(m_programID, "lights[1].power");
	m_parameters[U_LIGHT1_KC] = glGetUniformLocation(m_programID, "lights[1].kC");
	m_parameters[U_LIGHT1_KL] = glGetUniformLocation(m_programID, "lights[1].kL");
	m_parameters[U_LIGHT1_KQ] = glGetUniformLocation(m_programID, "lights[1].kQ");
	m_parameters[U_LIGHT1_TYPE] = glGetUniformLocation(m_programID, "lights[1].type");
	m_parameters[U_LIGHT1_SPOTDIRECTION] = glGetUniformLocation(m_programID, "lights[1].spotDirection");
	m_parameters[U_LIGHT1_COSCUTOFF] = glGetUniformLocation(m_programID, "lights[1].cosCutoff");
	m_parameters[U_LIGHT1_COSINNER] = glGetUniformLocation(m_programID, "lights[1].cosInner");
	m_parameters[U_LIGHT1_EXPONENT] = glGetUniformLocation(m_programID, "lights[1].exponent");

	m_parameters[U_LIGHT2_POSITION] = glGetUniformLocation(m_programID, "lights[2].position_cameraspace");
	m_parameters[U_LIGHT2_COLOR] = glGetUniformLocation(m_programID, "lights[2].color");
	m_parameters[U_LIGHT2_POWER] = glGetUniformLocation(m_programID, "lights[2].power");
	m_parameters[U_LIGHT2_KC] = glGetUniformLocation(m_programID, "lights[2].kC");
	m_parameters[U_LIGHT2_KL] = glGetUniformLocation(m_programID, "lights[2].kL");
	m_parameters[U_LIGHT2_KQ] = glGetUniformLocation(m_programID, "lights[2].kQ");
	m_parameters[U_LIGHT2_TYPE] = glGetUniformLocation(m_programID, "lights[2].type");
	m_parameters[U_LIGHT2_SPOTDIRECTION] = glGetUniformLocation(m_programID, "lights[2].spotDirection");
	m_parameters[U_LIGHT2_COSCUTOFF] = glGetUniformLocation(m_programID, "lights[2].cosCutoff");
	m_parameters[U_LIGHT2_COSINNER] = glGetUniformLocation(m_programID, "lights[2].cosInner");
	m_parameters[U_LIGHT2_EXPONENT] = glGetUniformLocation(m_programID, "lights[2].exponent");

	m_parameters[U_LIGHT3_POSITION] = glGetUniformLocation(m_programID, "lights[3].position_cameraspace");
	m_parameters[U_LIGHT3_COLOR] = glGetUniformLocation(m_programID, "lights[3].color");
	m_parameters[U_LIGHT3_POWER] = glGetUniformLocation(m_programID, "lights[3].power");
	m_parameters[U_LIGHT3_KC] = glGetUniformLocation(m_programID, "lights[3].kC");
	m_parameters[U_LIGHT3_KL] = glGetUniformLocation(m_programID, "lights[3].kL");
	m_parameters[U_LIGHT3_KQ] = glGetUniformLocation(m_programID, "lights[3].kQ");
	m_parameters[U_LIGHT3_TYPE] = glGetUniformLocation(m_programID, "lights[3].type");
	m_parameters[U_LIGHT3_SPOTDIRECTION] = glGetUniformLocation(m_programID, "lights[3].spotDirection");
	m_parameters[U_LIGHT3_COSCUTOFF] = glGetUniformLocation(m_programID, "lights[3].cosCutoff");
	m_parameters[U_LIGHT3_COSINNER] = glGetUniformLocation(m_programID, "lights[3].cosInner");
	m_parameters[U_LIGHT3_EXPONENT] = glGetUniformLocation(m_programID, "lights[3].exponent");

	m_parameters[U_LIGHT4_POSITION] = glGetUniformLocation(m_programID, "lights[4].position_cameraspace");
	m_parameters[U_LIGHT4_COLOR] = glGetUniformLocation(m_programID, "lights[4].color");
	m_parameters[U_LIGHT4_POWER] = glGetUniformLocation(m_programID, "lights[4].power");
	m_parameters[U_LIGHT4_KC] = glGetUniformLocation(m_programID, "lights[4].kC");
	m_parameters[U_LIGHT4_KL] = glGetUniformLocation(m_programID, "lights[4].kL");
	m_parameters[U_LIGHT4_KQ] = glGetUniformLocation(m_programID, "lights[4].kQ");
	m_parameters[U_LIGHT4_TYPE] = glGetUniformLocation(m_programID, "lights[4].type");
	m_parameters[U_LIGHT4_SPOTDIRECTION] = glGetUniformLocation(m_programID, "lights[4].spotDirection");
	m_parameters[U_LIGHT4_COSCUTOFF] = glGetUniformLocation(m_programID, "lights[4].cosCutoff");
	m_parameters[U_LIGHT4_COSINNER] = glGetUniformLocation(m_programID, "lights[4].cosInner");
	m_parameters[U_LIGHT4_EXPONENT] = glGetUniformLocation(m_programID, "lights[4].exponent");

	m_parameters[U_LIGHT5_POSITION] = glGetUniformLocation(m_programID, "lights[5].position_cameraspace");
	m_parameters[U_LIGHT5_COLOR] = glGetUniformLocation(m_programID, "lights[5].color");
	m_parameters[U_LIGHT5_POWER] = glGetUniformLocation(m_programID, "lights[5].power");
	m_parameters[U_LIGHT5_KC] = glGetUniformLocation(m_programID, "lights[5].kC");
	m_parameters[U_LIGHT5_KL] = glGetUniformLocation(m_programID, "lights[5].kL");
	m_parameters[U_LIGHT5_KQ] = glGetUniformLocation(m_programID, "lights[5].kQ");
	m_parameters[U_LIGHT5_TYPE] = glGetUniformLocation(m_programID, "lights[5].type");
	m_parameters[U_LIGHT5_SPOTDIRECTION] = glGetUniformLocation(m_programID, "lights[5].spotDirection");
	m_parameters[U_LIGHT5_COSCUTOFF] = glGetUniformLocation(m_programID, "lights[5].cosCutoff");
	m_parameters[U_LIGHT5_COSINNER] = glGetUniformLocation(m_programID, "lights[5].cosInner");
	m_parameters[U_LIGHT5_EXPONENT] = glGetUniformLocation(m_programID, "lights[5].exponent");

	m_parameters[U_LIGHT6_POSITION] = glGetUniformLocation(m_programID, "lights[6].position_cameraspace");
	m_parameters[U_LIGHT6_COLOR] = glGetUniformLocation(m_programID, "lights[6].color");
	m_parameters[U_LIGHT6_POWER] = glGetUniformLocation(m_programID, "lights[6].power");
	m_parameters[U_LIGHT6_KC] = glGetUniformLocation(m_programID, "lights[6].kC");
	m_parameters[U_LIGHT6_KL] = glGetUniformLocation(m_programID, "lights[6].kL");
	m_parameters[U_LIGHT6_KQ] = glGetUniformLocation(m_programID, "lights[6].kQ");
	m_parameters[U_LIGHT6_TYPE] = glGetUniformLocation(m_programID, "lights[6].type");
	m_parameters[U_LIGHT6_SPOTDIRECTION] = glGetUniformLocation(m_programID, "lights[6].spotDirection");
	m_parameters[U_LIGHT6_COSCUTOFF] = glGetUniformLocation(m_programID, "lights[6].cosCutoff");
	m_parameters[U_LIGHT6_COSINNER] = glGetUniformLocation(m_programID, "lights[6].cosInner");
	m_parameters[U_LIGHT6_EXPONENT] = glGetUniformLocation(m_programID, "lights[6].exponent");

	m_parameters[U_LIGHT7_POSITION] = glGetUniformLocation(m_programID, "lights[7].position_cameraspace");
	m_parameters[U_LIGHT7_COLOR] = glGetUniformLocation(m_programID, "lights[7].color");
	m_parameters[U_LIGHT7_POWER] = glGetUniformLocation(m_programID, "lights[7].power");
	m_parameters[U_LIGHT7_KC] = glGetUniformLocation(m_programID, "lights[7].kC");
	m_parameters[U_LIGHT7_KL] = glGetUniformLocation(m_programID, "lights[7].kL");
	m_parameters[U_LIGHT7_KQ] = glGetUniformLocation(m_programID, "lights[7].kQ");
	m_parameters[U_LIGHT7_TYPE] = glGetUniformLocation(m_programID, "lights[7].type");
	m_parameters[U_LIGHT7_SPOTDIRECTION] = glGetUniformLocation(m_programID, "lights[7].spotDirection");
	m_parameters[U_LIGHT7_COSCUTOFF] = glGetUniformLocation(m_programID, "lights[7].cosCutoff");
	m_parameters[U_LIGHT7_COSINNER] = glGetUniformLocation(m_programID, "lights[7].cosInner");
	m_parameters[U_LIGHT7_EXPONENT] = glGetUniformLocation(m_programID, "lights[7].exponent");

	/*m_parameters[U_LIGHT8_POSITION] = glGetUniformLocation(m_programID, "lights[8].position_cameraspace");
	m_parameters[U_LIGHT8_COLOR] = glGetUniformLocation(m_programID, "lights[8].color");
	m_parameters[U_LIGHT8_POWER] = glGetUniformLocation(m_programID, "lights[8].power");
	m_parameters[U_LIGHT8_KC] = glGetUniformLocation(m_programID, "lights[8].kC");
	m_parameters[U_LIGHT8_KL] = glGetUniformLocation(m_programID, "lights[8].kL");
	m_parameters[U_LIGHT8_KQ] = glGetUniformLocation(m_programID, "lights[8].kQ");
	m_parameters[U_LIGHT8_TYPE] = glGetUniformLocation(m_programID, "lights[8].type");
	m_parameters[U_LIGHT8_SPOTDIRECTION] = glGetUniformLocation(m_programID, "lights[8].spotDirection");
	m_parameters[U_LIGHT8_COSCUTOFF] = glGetUniformLocation(m_programID, "lights[8].cosCutoff");
	m_parameters[U_LIGHT8_COSINNER] = glGetUniformLocation(m_programID, "lights[8].cosInner");
	m_parameters[U_LIGHT8_EXPONENT] = glGetUniformLocation(m_programID, "lights[8].exponent");*/

	//Get a handle for our "colorTexture" uniform
	m_parameters[U_COLOR_TEXTURE_ENABLED] = glGetUniformLocation(m_programID, "colorTextureEnabled");
	m_parameters[U_COLOR_TEXTURE] = glGetUniformLocation(m_programID, "colorTexture");
	// Get a handle for our "textColor" uniform
	m_parameters[U_TEXT_ENABLED] = glGetUniformLocation(m_programID, "textEnabled");
	m_parameters[U_TEXT_COLOR] = glGetUniformLocation(m_programID, "textColor");


	glUseProgram(m_programID);
	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	
		light[0].type = Light::LIGHT_SPOT;
		light[0].position.Set(30, 30, -30);
		light[0].color.Set(0.5f, 0.5f, 0.5f);
		light[0].power = 1.f;
		light[0].kC = 1.f;
		light[0].kL = 0.01f;
		light[0].kQ = 0.001f;
		light[0].cosCutoff = cos(Math::DegreeToRadian(45));
		light[0].cosInner = cos(Math::DegreeToRadian(30));
		light[0].exponent = 3.f;
		light[0].spotDirection.Set(0.f, 1.f, 0.f);

		light[1].type = Light::LIGHT_SPOT;
		light[1].position.Set(30, 30, 0);
		light[1].color.Set(0.5f, 0.5f, 0.5f);
		light[1].power = 1.f;
		light[1].kC = 1.f;
		light[1].kL = 0.01f;
		light[1].kQ = 0.001f;
		light[1].cosCutoff = cos(Math::DegreeToRadian(45));
		light[1].cosInner = cos(Math::DegreeToRadian(30));
		light[1].exponent = 3.f;
		light[1].spotDirection.Set(0.f, 1.f, 0.f);

		light[2].type = Light::LIGHT_SPOT;
		light[2].position.Set(30, 30, 30);
		light[2].color.Set(0.5f, 0.5f, 0.5f);
		light[2].power = 1.f;
		light[2].kC = 1.f;
		light[2].kL = 0.01f;
		light[2].kQ = 0.001f;
		light[2].cosCutoff = cos(Math::DegreeToRadian(45));
		light[2].cosInner = cos(Math::DegreeToRadian(30));
		light[2].exponent = 3.f;
		light[2].spotDirection.Set(0.f, 1.f, 0.f);

		light[3].type = Light::LIGHT_SPOT;
		light[3].position.Set(0, 30, -30);
		light[3].color.Set(0.5f, 0.5f, 0.5f);
		light[3].power = 1.f;
		light[3].kC = 1.f;
		light[3].kL = 0.01f;
		light[3].kQ = 0.001f;
		light[3].cosCutoff = cos(Math::DegreeToRadian(45));
		light[3].cosInner = cos(Math::DegreeToRadian(30));
		light[3].exponent = 3.f;
		light[3].spotDirection.Set(0.f, 1.f, 0.f);

		light[4].type = Light::LIGHT_SPOT;
		light[4].position.Set(0, 30, 0);
		light[4].color.Set(0.5f, 0.5f, 0.5f);
		light[4].power = 1.f;
		light[4].kC = 1.f;
		light[4].kL = 0.01f;
		light[4].kQ = 0.001f;
		light[4].cosCutoff = cos(Math::DegreeToRadian(45));
		light[4].cosInner = cos(Math::DegreeToRadian(30));
		light[4].exponent = 3.f;
		light[4].spotDirection.Set(0.f, 1.f, 0.f);

		light[5].type = Light::LIGHT_SPOT;
		light[5].position.Set(0, 30, 30);
		light[5].color.Set(0.5f, 0.5f, 0.5f);
		light[5].power = 1.f;
		light[5].kC = 1.f;
		light[5].kL = 0.01f;
		light[5].kQ = 0.001f;
		light[5].cosCutoff = cos(Math::DegreeToRadian(45));
		light[5].cosInner = cos(Math::DegreeToRadian(30));
		light[5].exponent = 3.f;
		light[5].spotDirection.Set(0.f, 1.f, 0.f);

		light[6].type = Light::LIGHT_SPOT;
		light[6].position.Set(-30, 30, -30);
		light[6].color.Set(0.5f, 0.5f, 0.5f);
		light[6].power = 1.f;
		light[6].kC = 1.f;
		light[6].kL = 0.01f;
		light[6].kQ = 0.001f;
		light[6].cosCutoff = cos(Math::DegreeToRadian(45));
		light[6].cosInner = cos(Math::DegreeToRadian(30));
		light[6].exponent = 3.f;
		light[6].spotDirection.Set(0.f, 1.f, 0.f);

		light[7].type = Light::LIGHT_SPOT;
		light[7].position.Set(-30, 30, 0);
		light[7].color.Set(0.5f, 0.5f, 0.5f);
		light[7].power = 1.f;
		light[7].kC = 1.f;
		light[7].kL = 0.01f;
		light[7].kQ = 0.001f;
		light[7].cosCutoff = cos(Math::DegreeToRadian(45));
		light[7].cosInner = cos(Math::DegreeToRadian(30));
		light[7].exponent = 3.f;
		light[7].spotDirection.Set(0.f, 1.f, 0.f);

		/*light[8].type = Light::LIGHT_SPOT;
		light[8].position.Set(-20, 30, 20);
		light[8].color.Set(0.5f, 0.5f, 0.5f);
		light[8].power = 1.f;
		light[8].kC = 1.f;
		light[8].kL = 0.01f;
		light[8].kQ = 0.001f;
		light[8].cosCutoff = cos(Math::DegreeToRadian(45));
		light[8].cosInner = cos(Math::DegreeToRadian(30));
		light[8].exponent = 3.f;
		light[8].spotDirection.Set(0.f, 1.f, 0.f);*/

		glUniform3fv(m_parameters[U_LIGHT0_COLOR], 1, &light[0].color.r);
		glUniform1f(m_parameters[U_LIGHT0_POWER], light[0].power);
		glUniform1f(m_parameters[U_LIGHT0_KC], light[0].kC);
		glUniform1f(m_parameters[U_LIGHT0_KL], light[0].kL);
		glUniform1f(m_parameters[U_LIGHT0_KQ], light[0].kQ);
		glUniform1i(m_parameters[U_LIGHT0_TYPE], light[0].type);
		glUniform3fv(m_parameters[U_LIGHT0_SPOTDIRECTION], 1, &light[0].spotDirection.x);
		glUniform1f(m_parameters[U_LIGHT0_COSCUTOFF], light[0].cosCutoff);
		glUniform1f(m_parameters[U_LIGHT0_COSINNER], light[0].cosInner);
		glUniform1f(m_parameters[U_LIGHT0_EXPONENT], light[0].exponent);

		glUniform3fv(m_parameters[U_LIGHT1_COLOR], 1, &light[1].color.r);
		glUniform1f(m_parameters[U_LIGHT1_POWER], light[1].power);
		glUniform1f(m_parameters[U_LIGHT1_KC], light[1].kC);
		glUniform1f(m_parameters[U_LIGHT1_KL], light[1].kL);
		glUniform1f(m_parameters[U_LIGHT1_KQ], light[1].kQ);
		glUniform1i(m_parameters[U_LIGHT1_TYPE], light[1].type);
		glUniform3fv(m_parameters[U_LIGHT1_SPOTDIRECTION], 1, &light[1].spotDirection.x);
		glUniform1f(m_parameters[U_LIGHT1_COSCUTOFF], light[1].cosCutoff);
		glUniform1f(m_parameters[U_LIGHT1_COSINNER], light[1].cosInner);
		glUniform1f(m_parameters[U_LIGHT1_EXPONENT], light[1].exponent);

		glUniform3fv(m_parameters[U_LIGHT2_COLOR], 1, &light[2].color.r);
		glUniform1f(m_parameters[U_LIGHT2_POWER], light[2].power);
		glUniform1f(m_parameters[U_LIGHT2_KC], light[2].kC);
		glUniform1f(m_parameters[U_LIGHT2_KL], light[2].kL);
		glUniform1f(m_parameters[U_LIGHT2_KQ], light[2].kQ);
		glUniform1i(m_parameters[U_LIGHT2_TYPE], light[2].type);
		glUniform3fv(m_parameters[U_LIGHT2_SPOTDIRECTION], 1, &light[2].spotDirection.x);
		glUniform1f(m_parameters[U_LIGHT2_COSCUTOFF], light[2].cosCutoff);
		glUniform1f(m_parameters[U_LIGHT2_COSINNER], light[2].cosInner);
		glUniform1f(m_parameters[U_LIGHT2_EXPONENT], light[2].exponent);

		glUniform3fv(m_parameters[U_LIGHT3_COLOR], 1, &light[3].color.r);
		glUniform1f(m_parameters[U_LIGHT3_POWER], light[3].power);
		glUniform1f(m_parameters[U_LIGHT3_KC], light[3].kC);
		glUniform1f(m_parameters[U_LIGHT3_KL], light[3].kL);
		glUniform1f(m_parameters[U_LIGHT3_KQ], light[3].kQ);
		glUniform1i(m_parameters[U_LIGHT3_TYPE], light[3].type);
		glUniform3fv(m_parameters[U_LIGHT3_SPOTDIRECTION], 1, &light[3].spotDirection.x);
		glUniform1f(m_parameters[U_LIGHT3_COSCUTOFF], light[3].cosCutoff);
		glUniform1f(m_parameters[U_LIGHT3_COSINNER], light[3].cosInner);
		glUniform1f(m_parameters[U_LIGHT3_EXPONENT], light[3].exponent);

		glUniform3fv(m_parameters[U_LIGHT4_COLOR], 1, &light[4].color.r);
		glUniform1f(m_parameters[U_LIGHT4_POWER], light[4].power);
		glUniform1f(m_parameters[U_LIGHT4_KC], light[4].kC);
		glUniform1f(m_parameters[U_LIGHT4_KL], light[4].kL);
		glUniform1f(m_parameters[U_LIGHT4_KQ], light[4].kQ);
		glUniform1i(m_parameters[U_LIGHT4_TYPE], light[4].type);
		glUniform3fv(m_parameters[U_LIGHT4_SPOTDIRECTION], 1, &light[4].spotDirection.x);
		glUniform1f(m_parameters[U_LIGHT4_COSCUTOFF], light[4].cosCutoff);
		glUniform1f(m_parameters[U_LIGHT4_COSINNER], light[4].cosInner);
		glUniform1f(m_parameters[U_LIGHT4_EXPONENT], light[4].exponent);

		glUniform3fv(m_parameters[U_LIGHT5_COLOR], 1, &light[5].color.r);
		glUniform1f(m_parameters[U_LIGHT5_POWER], light[5].power);
		glUniform1f(m_parameters[U_LIGHT5_KC], light[5].kC);
		glUniform1f(m_parameters[U_LIGHT5_KL], light[5].kL);
		glUniform1f(m_parameters[U_LIGHT5_KQ], light[5].kQ);
		glUniform1i(m_parameters[U_LIGHT5_TYPE], light[5].type);
		glUniform3fv(m_parameters[U_LIGHT5_SPOTDIRECTION], 1, &light[5].spotDirection.x);
		glUniform1f(m_parameters[U_LIGHT5_COSCUTOFF], light[5].cosCutoff);
		glUniform1f(m_parameters[U_LIGHT5_COSINNER], light[5].cosInner);
		glUniform1f(m_parameters[U_LIGHT5_EXPONENT], light[5].exponent);

		glUniform3fv(m_parameters[U_LIGHT6_COLOR], 1, &light[6].color.r);
		glUniform1f(m_parameters[U_LIGHT6_POWER], light[6].power);
		glUniform1f(m_parameters[U_LIGHT6_KC], light[6].kC);
		glUniform1f(m_parameters[U_LIGHT6_KL], light[6].kL);
		glUniform1f(m_parameters[U_LIGHT6_KQ], light[6].kQ);
		glUniform1i(m_parameters[U_LIGHT6_TYPE], light[6].type);
		glUniform3fv(m_parameters[U_LIGHT6_SPOTDIRECTION], 1, &light[6].spotDirection.x);
		glUniform1f(m_parameters[U_LIGHT6_COSCUTOFF], light[6].cosCutoff);
		glUniform1f(m_parameters[U_LIGHT6_COSINNER], light[6].cosInner);
		glUniform1f(m_parameters[U_LIGHT6_EXPONENT], light[6].exponent);

		glUniform3fv(m_parameters[U_LIGHT7_COLOR], 1, &light[7].color.r);
		glUniform1f(m_parameters[U_LIGHT7_POWER], light[7].power);
		glUniform1f(m_parameters[U_LIGHT7_KC], light[7].kC);
		glUniform1f(m_parameters[U_LIGHT7_KL], light[7].kL);
		glUniform1f(m_parameters[U_LIGHT7_KQ], light[7].kQ);
		glUniform1i(m_parameters[U_LIGHT7_TYPE], light[7].type);
		glUniform3fv(m_parameters[U_LIGHT7_SPOTDIRECTION], 1, &light[7].spotDirection.x);
		glUniform1f(m_parameters[U_LIGHT7_COSCUTOFF], light[7].cosCutoff);
		glUniform1f(m_parameters[U_LIGHT7_COSINNER], light[7].cosInner);
		glUniform1f(m_parameters[U_LIGHT7_EXPONENT], light[7].exponent);
		//Having more than 8 lights breaks the system somehow
		/*glUniform3fv(m_parameters[U_LIGHT8_COLOR], 1, &light[8].color.r);
		glUniform1f(m_parameters[U_LIGHT8_POWER], light[8].power);
		glUniform1f(m_parameters[U_LIGHT8_KC], light[8].kC);
		glUniform1f(m_parameters[U_LIGHT8_KL], light[8].kL);
		glUniform1f(m_parameters[U_LIGHT8_KQ], light[8].kQ);
		glUniform1i(m_parameters[U_LIGHT8_TYPE], light[8].type);
		glUniform3fv(m_parameters[U_LIGHT8_SPOTDIRECTION], 1, &light[8].spotDirection.x);
		glUniform1f(m_parameters[U_LIGHT8_COSCUTOFF], light[8].cosCutoff);
		glUniform1f(m_parameters[U_LIGHT8_COSINNER], light[8].cosInner);
		glUniform1f(m_parameters[U_LIGHT8_EXPONENT], light[8].exponent);*/

	glUniform1i(m_parameters[U_NUMLIGHTS], 8);

	dialogueTime = 0;
	elapsedTime = 0;
	debounceTime = 0;
	dayData->NextDay();

	if (HasCars == false) {
		Selection->AddCar("Car1", 1000);
		Selection->AddCar("Car2", 2000);
		Selection->AddCar("Car3", 3000);
		Selection->AddCar("Car4", 4000);
		Selection->AddCar("Car5", 5000);
		Selection->AddCar("Car6", 6000);
		Selection->AddCar("Car7", 7000);
		Selection->AddCar("Car8", 8000);
		Selection->AddCar("Car9", 9000);
		Selection->AddCar("Car10", 10000);
		Selection->AddCar("Car11", 11000);
		Selection->AddCar("Car12", 12000);
		HasCars = true;
	}

	meshList[GEO_LEFT] = MeshBuilder::GenerateQuad("left", Color(1, 1, 1), 1.f, 1.f);
	meshList[GEO_LEFT]->textureID = LoadTGA("Image//left.tga");

	meshList[GEO_RIGHT] = MeshBuilder::GenerateQuad("right", Color(1, 1, 1), 1.f, 1.f);
	meshList[GEO_RIGHT]->textureID = LoadTGA("Image//right.tga");

	meshList[GEO_TOP] = MeshBuilder::GenerateQuad("top", Color(1, 1, 1), 1.f, 1.f);
	meshList[GEO_TOP]->textureID = LoadTGA("Image//top.tga");

	meshList[GEO_BOTTOM] = MeshBuilder::GenerateQuad("bottom", Color(1, 1, 1), 1.f, 1.f);
	meshList[GEO_BOTTOM]->textureID = LoadTGA("Image//bottom.tga");

	meshList[GEO_FRONT] = MeshBuilder::GenerateQuad("front", Color(1, 1, 1), 1.f, 1.f);
	meshList[GEO_FRONT]->textureID = LoadTGA("Image//front.tga");

	meshList[GEO_BACK] = MeshBuilder::GenerateQuad("back", Color(1, 1, 1), 1.f, 1.f);
	meshList[GEO_BACK]->textureID = LoadTGA("Image//back.tga");

	meshList[GEO_CHAR] = MeshBuilder::GenerateQuad("char", Color(1, 1, 1), 1.f, 1.f);
	meshList[GEO_CHAR]->textureID = LoadTGA("Image//char.tga");

	meshList[GEO_DICE] = MeshBuilder::GenerateOBJ("Dice","OBJ//doorman.obj");
	meshList[GEO_DICE]->textureID = LoadTGA("Image//doorman.tga");

	meshList[GEO_LIGHTSPHERE] = MeshBuilder::GenerateSphere("lightBall", Color(1.f, 1.f, 1.f), 9, 36, 1.f);

	meshList[GEO_BANKMENU] = MeshBuilder::GenerateQuad("BankMenu", Color(0, 1, 0), 1, 1);
	meshList[GEO_BANKMENU]->textureID = LoadTGA("Image//UI.tga");

	meshList[GEO_TEXT] = MeshBuilder::GenerateText("text", 16, 16);
	meshList[GEO_TEXT]->textureID = LoadTGA("Image//calibri.tga");
	
	meshList[GEO_TEXTBOX] = MeshBuilder::GenerateQuad("textbox", Color(1, 1, 1), 7.f, 2.5f);
	meshList[GEO_TEXTBOX]->textureID = LoadTGA("Image//NPCtextbox.tga");
	
	meshList[GEO_ACHIEVEMENTS] = MeshBuilder::GenerateText("text", 16, 16);
	meshList[GEO_ACHIEVEMENTS]->textureID = LoadTGA("Image//calibri.tga");
	
	meshList[GEO_ACHIEVEMENTSBG] = MeshBuilder::GenerateQuad("Achievementsbg", Color(1, 1, 1), 1.6f, 1.4f);
	meshList[GEO_ACHIEVEMENTSBG]->textureID = LoadTGA("Image//AchievementScreen.tga");

	meshList[GEO_MECHANIC_GAME] = MeshBuilder::GenerateQuad("Mechanic_Game", Color(0.5f, 1, 1), 1.f, 1.f);

	meshList[GEO_FRAME] = MeshBuilder::GenerateQuad("Frame", Color(0, 1, 0), 7.f, 1.5f);
	meshList[GEO_FRAME]->textureID = LoadTGA("Image//NPCtextbox2.tga");

	meshList[GEO_FRAME2] = MeshBuilder::GenerateQuad("Frame2", Color(0, 1, 0), 7.f, 2.4f);
	meshList[GEO_FRAME2]->textureID = LoadTGA("Image//customersalesbackground.tga");

	meshList[GEO_FRAME3] = MeshBuilder::GenerateQuad("Frame3", Color(0, 1, 0), 7.5f, 1.5f);
	meshList[GEO_FRAME3]->textureID = LoadTGA("Image//whiteframe.tga");

	meshList[GEO_CUSTOMERLOGO] = MeshBuilder::GenerateQuad("CustomerLogo", Color(0, 0, 0), 0.8f, 0.8f);
	meshList[GEO_CUSTOMERLOGO]->textureID = LoadTGA("Image//customerlogo.tga");

	meshList[GEO_LITTER] = MeshBuilder::GenerateOBJ("Litter", "OBJ//Tissue.obj");
	meshList[GEO_LITTER]->textureID = LoadTGA("Image//Tissue.tga");

	// Cleaner Robot
	meshList[CLEANER_TOP] = MeshBuilder::GenerateOBJ("CleanerTopBody", "OBJ//CleanerBot//CleanerTopBody.obj");
	meshList[CLEANER_TOP]->textureID = LoadTGA("Image//CleanerRobot.tga");
	
	meshList[CLEANER_BOTTOM] = MeshBuilder::GenerateOBJ("CleanerBottomBody", "OBJ//CleanerBot//CleanerBottomBody.obj");
	meshList[CLEANER_BOTTOM]->textureID = LoadTGA("Image//CleanerRobot.tga");
	
	meshList[CLEANER_WHEEL] = MeshBuilder::GenerateOBJ("CleanerWheel", "OBJ//CleanerBot//CleanerWheel.obj");
	meshList[CLEANER_WHEEL]->textureID = LoadTGA("Image//CleanerRobot.tga");
	
	meshList[CLEANER_WHEELJOINT] = MeshBuilder::GenerateOBJ("CleanerWheelJoint", "OBJ//CleanerBot//CleanerWheelJoint.obj");
	meshList[CLEANER_WHEELJOINT]->textureID = LoadTGA("Image//CleanerRobot.tga");
	
	// Player
	meshList[PLAYER_BODY] = MeshBuilder::GenerateOBJ("PlayerBody", "OBJ//Customer//CustomerBody.obj");
	meshList[PLAYER_BODY]->textureID = LoadTGA("Image//Customer.tga");
			 
	meshList[PLAYER_HEAD] = MeshBuilder::GenerateOBJ("PlayerHead", "OBJ//Customer//CustomerHead.obj");
	meshList[PLAYER_HEAD]->textureID = LoadTGA("Image//Customer.tga");
			 
	meshList[PLAYER_SHOULDER] = MeshBuilder::GenerateOBJ("PlayerShoulder", "OBJ//Customer//CustomerShoulder.obj");
	meshList[PLAYER_SHOULDER]->textureID = LoadTGA("Image//Customer.tga");
			
	meshList[PLAYER_ARM] = MeshBuilder::GenerateOBJ("PlayerArm", "OBJ//Customer//CustomerArm.obj");
	meshList[PLAYER_ARM]->textureID = LoadTGA("Image//Customer.tga");
			
	meshList[PLAYER_HAND] = MeshBuilder::GenerateOBJ("PlayerHand", "OBJ//Customer//CustomerHand.obj");
	meshList[PLAYER_HAND]->textureID = LoadTGA("Image//Customer.tga");
			
	meshList[PLAYER_LEG] = MeshBuilder::GenerateOBJ("PlayerLeg", "OBJ//Customer//CustomerLeg.obj");
	meshList[PLAYER_LEG]->textureID = LoadTGA("Image//Customer.tga");

	// Customer
	meshList[CUSTOMER_BODY] = MeshBuilder::GenerateOBJ("CustomerBody", "OBJ//Customer//CustomerBody.obj");
	meshList[CUSTOMER_BODY]->textureID = LoadTGA("Image//Customer.tga");
	
	meshList[CUSTOMER_HEAD] = MeshBuilder::GenerateOBJ("CustomerHead", "OBJ//Customer//CustomerHead.obj");
	meshList[CUSTOMER_HEAD]->textureID = LoadTGA("Image//Customer.tga");
	
	meshList[CUSTOMER_SHOULDER] = MeshBuilder::GenerateOBJ("CustomerShoulder", "OBJ//Customer//CustomerShoulder.obj");
	meshList[CUSTOMER_SHOULDER]->textureID = LoadTGA("Image//Customer.tga");
	
	meshList[CUSTOMER_ARM] = MeshBuilder::GenerateOBJ("CustomerArm", "OBJ//Customer//CustomerArm.obj");
	meshList[CUSTOMER_ARM]->textureID = LoadTGA("Image//Customer.tga");
	
	meshList[CUSTOMER_HAND] = MeshBuilder::GenerateOBJ("CustomerHand", "OBJ//Customer//CustomerHand.obj");
	meshList[CUSTOMER_HAND]->textureID = LoadTGA("Image//Customer.tga");
	
	meshList[CUSTOMER_LEG] = MeshBuilder::GenerateOBJ("CustomerLeg", "OBJ//Customer//CustomerLeg.obj");
	meshList[CUSTOMER_LEG]->textureID = LoadTGA("Image//Customer.tga");
	
	// Customer2
	meshList[CUSTOMER2_BODY] = MeshBuilder::GenerateOBJ("CustomerBody", "OBJ//Customer//CustomerBody.obj");
	meshList[CUSTOMER2_BODY]->textureID = LoadTGA("Image//CustomerAlt.tga");
	
	meshList[CUSTOMER2_HEAD] = MeshBuilder::GenerateOBJ("CustomerHead", "OBJ//Customer//CustomerHead.obj");
	meshList[CUSTOMER2_HEAD]->textureID = LoadTGA("Image//CustomerAlt.tga");
	
	meshList[CUSTOMER2_SHOULDER] = MeshBuilder::GenerateOBJ("CustomerShoulder", "OBJ//Customer//CustomerShoulder.obj");
	meshList[CUSTOMER2_SHOULDER]->textureID = LoadTGA("Image//CustomerAlt.tga");
	
	meshList[CUSTOMER2_ARM] = MeshBuilder::GenerateOBJ("CustomerArm", "OBJ//Customer//CustomerArm.obj");
	meshList[CUSTOMER2_ARM]->textureID = LoadTGA("Image//CustomerAlt.tga");
	
	meshList[CUSTOMER2_HAND] = MeshBuilder::GenerateOBJ("CustomerHand", "OBJ//Customer//CustomerHand.obj");
	meshList[CUSTOMER2_HAND]->textureID = LoadTGA("Image//CustomerAlt.tga");
	
	meshList[CUSTOMER2_LEG] = MeshBuilder::GenerateOBJ("CustomerLeg", "OBJ//Customer//CustomerLeg.obj");
	meshList[CUSTOMER2_LEG]->textureID = LoadTGA("Image//CustomerAlt.tga");
	
	//suspect 1
	meshList[SUSPECT1_BODY] = MeshBuilder::GenerateOBJ("CustomerBody", "OBJ//Customer//CustomerBody.obj");
	meshList[SUSPECT1_BODY]->textureID = LoadTGA("Image//CustomerAlt2.tga");

	meshList[SUSPECT1_HEAD] = MeshBuilder::GenerateOBJ("CustomerHead", "OBJ//Customer//CustomerHead.obj");
	meshList[SUSPECT1_HEAD]->textureID = LoadTGA("Image//CustomerAlt2.tga");

	meshList[SUSPECT1_SHOULDER] = MeshBuilder::GenerateOBJ("CustomerShoulder", "OBJ//Customer//CustomerShoulder.obj");
	meshList[SUSPECT1_SHOULDER]->textureID = LoadTGA("Image//CustomerAlt2.tga");

	meshList[SUSPECT1_ARM] = MeshBuilder::GenerateOBJ("CustomerArm", "OBJ//Customer//CustomerArm.obj");
	meshList[SUSPECT1_ARM]->textureID = LoadTGA("Image//CustomerAlt2.tga");

	meshList[SUSPECT1_HAND] = MeshBuilder::GenerateOBJ("CustomerHand", "OBJ//Customer//CustomerHand.obj");
	meshList[SUSPECT1_HAND]->textureID = LoadTGA("Image//CustomerAlt2.tga");

	meshList[SUSPECT1_LEG] = MeshBuilder::GenerateOBJ("CustomerLeg", "OBJ//Customer//CustomerLeg.obj");
	meshList[SUSPECT1_LEG]->textureID = LoadTGA("Image//CustomerAlt2.tga");
    
	//suspect 2
	meshList[SUSPECT2_BODY] = MeshBuilder::GenerateOBJ("CustomerBody", "OBJ//Customer//CustomerBody.obj");
	meshList[SUSPECT2_BODY]->textureID = LoadTGA("Image//CustomerAlt3.tga");

	meshList[SUSPECT2_HEAD] = MeshBuilder::GenerateOBJ("CustomerHead", "OBJ//Customer//CustomerHead.obj");
	meshList[SUSPECT2_HEAD]->textureID = LoadTGA("Image//CustomerAlt3.tga");

	meshList[SUSPECT2_SHOULDER] = MeshBuilder::GenerateOBJ("CustomerShoulder", "OBJ//Customer//CustomerShoulder.obj");
	meshList[SUSPECT2_SHOULDER]->textureID = LoadTGA("Image//CustomerAlt3.tga");

	meshList[SUSPECT2_ARM] = MeshBuilder::GenerateOBJ("CustomerArm", "OBJ//Customer//CustomerArm.obj");
	meshList[SUSPECT2_ARM]->textureID = LoadTGA("Image//CustomerAlt3.tga");

	meshList[SUSPECT2_HAND] = MeshBuilder::GenerateOBJ("CustomerHand", "OBJ//Customer//CustomerHand.obj");
	meshList[SUSPECT2_HAND]->textureID = LoadTGA("Image//CustomerAlt3.tga");

	meshList[SUSPECT2_LEG] = MeshBuilder::GenerateOBJ("CustomerLeg", "OBJ//Customer//CustomerLeg.obj");
	meshList[SUSPECT2_LEG]->textureID = LoadTGA("Image//CustomerAlt3.tga");

	// Guard Bot
	meshList[GUARD_BODY] = MeshBuilder::GenerateOBJ("GuardBody", "OBJ//GuardBot//GuardBody.obj");
	meshList[GUARD_BODY]->textureID = LoadTGA("Image//GuardBot.tga");
	
	meshList[GUARD_HEAD] = MeshBuilder::GenerateOBJ("GuardHead", "OBJ//GuardBot//GuardHead.obj");
	meshList[GUARD_HEAD]->textureID = LoadTGA("Image//GuardBot.tga");
	
	meshList[GUARD_LEFTARM] = MeshBuilder::GenerateOBJ("GuardLeftArm", "OBJ//GuardBot//GuardLeftArm.obj");
	meshList[GUARD_LEFTARM]->textureID = LoadTGA("Image//GuardBot.tga");
	
	meshList[GUARD_RIGHTARM] = MeshBuilder::GenerateOBJ("GuardRightArm", "OBJ//GuardBot//GuardRightArm.obj");
	meshList[GUARD_RIGHTARM]->textureID = LoadTGA("Image//GuardBot.tga");
	
	meshList[GUARD_LEG] = MeshBuilder::GenerateOBJ("GuardLeg", "OBJ//GuardBot//GuardLeg.obj");
	meshList[GUARD_LEG]->textureID = LoadTGA("Image//GuardBot.tga");

	// Sale Bot
	meshList[SALE_BOTTOM] = MeshBuilder::GenerateOBJ("SaleBody", "OBJ//SaleBot//SaleBottom.obj");
	meshList[SALE_BOTTOM]->textureID = LoadTGA("Image//Salesbot.tga");
	
	meshList[SALE_TOP] = MeshBuilder::GenerateOBJ("SaleTop", "OBJ//SaleBot//SaleTop.obj");
	meshList[SALE_TOP]->textureID = LoadTGA("Image//Salesbot.tga");
	
	meshList[SALE_NECK] = MeshBuilder::GenerateOBJ("SaleNeck", "OBJ//SaleBot//SaleNeck.obj");
	meshList[SALE_NECK]->textureID = LoadTGA("Image//Salesbot.tga");
	
	meshList[SALE_HEAD] = MeshBuilder::GenerateOBJ("SaleHead", "OBJ//SaleBot//SaleHead.obj");
	meshList[SALE_HEAD]->textureID = LoadTGA("Image//Salesbot.tga");
	
	meshList[SALE_LEFTARM] = MeshBuilder::GenerateOBJ("SaleLeftArm", "OBJ//SaleBot//SaleLeftArm.obj");
	meshList[SALE_LEFTARM]->textureID = LoadTGA("Image//Salesbot.tga");
	
	meshList[SALE_RIGHTARM] = MeshBuilder::GenerateOBJ("SaleRightArm", "OBJ//SaleBot//SaleRightArm.obj");
	meshList[SALE_RIGHTARM]->textureID = LoadTGA("Image//Salesbot.tga");
	
	meshList[SALE_WHEEL] = MeshBuilder::GenerateOBJ("SaleWheel", "OBJ//SaleBot//SaleWheel.obj");
	meshList[SALE_WHEEL]->textureID = LoadTGA("Image//Salesbot.tga");

	//Background Assets
	meshList[ITEM_INFORMATION_STAND] = MeshBuilder::GenerateOBJ("Information Stand", "OBJ//Background_Items//Infostand.obj");
	meshList[ITEM_INFORMATION_STAND]->textureID = LoadTGA("Image//background_items//Infostand.tga");
	meshList[ITEM_INFORMATION_COUNTER] = MeshBuilder::GenerateOBJ("Information Stand", "OBJ//Background_Items//InfoCounter.obj");
	meshList[ITEM_INFORMATION_COUNTER]->textureID = LoadTGA("Image//background_items//InfoCounter.tga");
	meshList[ITEM_BATTERY] = MeshBuilder::GenerateOBJ("Battery","OBJ//Background_Items//Battery.obj");
	meshList[ITEM_BATTERY]->textureID = LoadTGA("Image//background_items//Battery.tga");
	meshList[ITEM_DISPLAY_STAND] = MeshBuilder::GenerateOBJ("Display Stand","OBJ//Background_Items//Display_Stand.obj");
	meshList[ITEM_DISPLAY_STAND]->textureID = LoadTGA("Image//background_items//DisplayStand.tga");
	meshList[ITEM_LIGHTBULB] = MeshBuilder::GenerateOBJ("Lightbulb","OBJ//Background_Items//Lightbulb.obj");
	meshList[ITEM_LIGHTBULB]->textureID = LoadTGA("Image//background_items//Light.tga");
	meshList[ITEM_BOOTH] = MeshBuilder::GenerateOBJ("Booth", "OBJ//Background_Items//Booth.obj");
	meshList[ITEM_BOOTH]->textureID = LoadTGA("Image//background_items//Booth.tga");
	meshList[ITEM_FAE_BOOTH] = MeshBuilder::GenerateOBJ("Fae Booth", "OBJ//Background_Items//Booth.obj");
	meshList[ITEM_FAE_BOOTH]->textureID = LoadTGA("Image//background_items//Fae_Booth.tga");
	meshList[ITEM_BOOTH_3] = MeshBuilder::GenerateOBJ("Booth", "OBJ//Background_Items//Booth.obj");
	meshList[ITEM_BOOTH_3]->textureID = LoadTGA("Image//background_items//Checkout_Booth.tga");
	meshList[ITEM_BOOTH_4] = MeshBuilder::GenerateOBJ("Booth", "OBJ//Background_Items//Booth.obj");
	meshList[ITEM_BOOTH_4]->textureID = LoadTGA("Image//background_items//Driving_Test_Booth.tga");
	meshList[ITEM_BOOTH_5] = MeshBuilder::GenerateOBJ("Booth", "OBJ//Background_Items//Booth.obj");
	meshList[ITEM_BOOTH_5]->textureID = LoadTGA("Image//background_items//Tour_Booth.tga");
	meshList[ITEM_BENCH] = MeshBuilder::GenerateOBJ("Bench", "OBJ//Background_Items//Bench.obj");
	meshList[ITEM_BENCH]->textureID = LoadTGA("Image//background_items//Bench.tga");
	meshList[ITEM_FLOOR] = MeshBuilder::GenerateCuboid("Floor", Color(1, 1, 1), 1.f, 1.f, 1.f);

	// Cars
	meshList[CAR1_BODY] = MeshBuilder::GenerateOBJ("Car1Body", "OBJ//Cars//Car1//Car1Body.obj");
	meshList[CAR1_BODY]->textureID = LoadTGA("Image//CarsTexture//Car1.tga");
	meshList[CAR1_STEERINGWHEEL] = MeshBuilder::GenerateOBJ("Car1SteeringWheel", "OBJ//Cars//Car1//Car1SteeringWheel.obj");
	meshList[CAR1_STEERINGWHEEL]->textureID = LoadTGA("Image//CarsTexture//Car1.tga");
	meshList[CAR1_WHEEL] = MeshBuilder::GenerateOBJ("Car1Wheel", "OBJ//Cars//Car1//Car1Wheel.obj");
	meshList[CAR1_WHEEL]->textureID = LoadTGA("Image//CarsTexture//Car1.tga");

	meshList[CAR2_BODY] = MeshBuilder::GenerateOBJ("Car2Body", "OBJ//Cars//Car2//Car2body.obj");
	meshList[CAR2_BODY]->textureID = LoadTGA("Image//CarsTexture//Car2.tga");
	
	meshList[CAR3_BODY] = MeshBuilder::GenerateOBJ("Car3Body", "OBJ//Cars//Car3//Car3Body.obj");
	meshList[CAR3_BODY]->textureID = LoadTGA("Image//CarsTexture//Car3.tga");

	meshList[CAR4_BODY] = MeshBuilder::GenerateOBJ("Car4Body", "OBJ//Cars//Car4//Car4Body.obj");
	meshList[CAR4_BODY]->textureID = LoadTGA("Image//CarsTexture//Car4.tga");
	meshList[CAR4_LEFTWHEEL] = MeshBuilder::GenerateOBJ("Car4LeftWheel", "OBJ//Cars//Car4//Car4LeftWheel.obj");
	meshList[CAR4_LEFTWHEEL]->textureID = LoadTGA("Image//CarsTexture//Car4.tga");
	meshList[CAR4_RIGHTWHEEL] = MeshBuilder::GenerateOBJ("Car4RightWheel", "OBJ//Cars//Car4//Car4RightWheel.obj");
	meshList[CAR4_RIGHTWHEEL]->textureID = LoadTGA("Image//CarsTexture//Car4.tga");
	
	meshList[CAR5_BODY] = MeshBuilder::GenerateOBJ("Car5Body", "OBJ//Cars//Car5//Car5Body.obj");
	meshList[CAR5_BODY]->textureID = LoadTGA("Image//CarsTexture//Car5.tga");
	meshList[CAR5_WHEEL] = MeshBuilder::GenerateOBJ("Car5Wheel", "OBJ//Cars//Car5//Car5Wheel.obj");
	meshList[CAR5_WHEEL ]->textureID = LoadTGA("Image//CarsTexture//Car5.tga");

	meshList[CAR6_BODY] = MeshBuilder::GenerateOBJ("Car6Body", "OBJ//Cars//Car6//Car6Body.obj");
	meshList[CAR6_BODY]->textureID = LoadTGA("Image//CarsTexture//Car6.tga");
	meshList[CAR6_LEFTWHEEL] = MeshBuilder::GenerateOBJ("Car6LeftWheel", "OBJ//Cars//Car6//Car6LeftWheel.obj");
	meshList[CAR6_LEFTWHEEL]->textureID = LoadTGA("Image//CarsTexture//Car6.tga");
	meshList[CAR6_RIGHTWHEEL] = MeshBuilder::GenerateOBJ("Car6RightWheel", "OBJ//Cars//Car6//Car6RightWheel.obj");
	meshList[CAR6_RIGHTWHEEL]->textureID = LoadTGA("Image//CarsTexture//Car6.tga");

	meshList[CAR7_BODY] = MeshBuilder::GenerateOBJ("Car7Body", "OBJ//Cars//Car7//Car7Body.obj");
	meshList[CAR7_BODY]->textureID = LoadTGA("Image//CarsTexture//Car7.tga");
	meshList[CAR7_STEERINGWHEEL] = MeshBuilder::GenerateOBJ("Car7SteeringWheel", "OBJ//Cars//Car7//Car7SteeringWheel.obj");
	meshList[CAR7_STEERINGWHEEL]->textureID = LoadTGA("Image//CarsTexture//Car7.tga");
	meshList[CAR7_FRONTWHEEL] = MeshBuilder::GenerateOBJ("Car7FrontWheel", "OBJ//Cars//Car7//Car7FrontWheel.obj");
	meshList[CAR7_FRONTWHEEL]->textureID = LoadTGA("Image//CarsTexture//Car7.tga");
	meshList[CAR7_BACKWHEEL] = MeshBuilder::GenerateOBJ("Car7RightWheel", "OBJ//Cars//Car7//Car7BackWheel.obj");
	meshList[CAR7_BACKWHEEL]->textureID = LoadTGA("Image//CarsTexture//Car7.tga");

	//meshList[CAR8_BODY] = MeshBuilder::GenerateOBJ("Car8Body", "OBJ//Cars//Car8//Car8Body.obj");
	//meshList[CAR8_BODY]->textureID = LoadTGA("Image//CarsTexture//Car8.tga");
	//meshList[CAR8_STEERINGWHEEL] = MeshBuilder::GenerateOBJ("Car8SteeringWheel", "OBJ//Cars//Car8//Car8SteeringWheel.obj");
	//meshList[CAR8_STEERINGWHEEL]->textureID = LoadTGA("Image//CarsTexture//Car8.tga");
	//meshList[CAR8_WHEEL] = MeshBuilder::GenerateOBJ("Car8Wheel", "OBJ//Cars//Car8//Car8Wheel.obj");
	//meshList[CAR8_WHEEL]->textureID = LoadTGA("Image//CarsTexture//Car8.tga");

	meshList[CAR9_BODY] = MeshBuilder::GenerateOBJ("Car9Body", "OBJ//Cars//Car9//Car9Body.obj");
	meshList[CAR9_BODY]->textureID = LoadTGA("Image//CarsTexture//Car9.tga");
	meshList[CAR9_WHEEL] = MeshBuilder::GenerateOBJ("Car9Wheel", "OBJ//Cars//Car9//Car9Wheel.obj");
	meshList[CAR9_WHEEL]->textureID = LoadTGA("Image//CarsTexture//Car9.tga");

	meshList[CAR10_BODY] = MeshBuilder::GenerateOBJ("GalaxyCar", "OBJ//Cars//Car10//GalaxyCar.obj");
	meshList[CAR10_BODY]->textureID = LoadTGA("Image//CarsTexture//Car10//GalaxyCar.tga");
	//meshList[CAR10_WHEEL] = MeshBuilder::GenerateOBJ("Car10Wheel", "OBJ//Cars//Car10//GalaxyWheel.obj");
	//meshList[CAR10_WHEEL]->textureID = LoadTGA("Image//CarsTexture//GalaxyWheel.tga");

	meshList[CAR11_BODY] = MeshBuilder::GenerateOBJ("Car11Body", "OBJ//Cars//Car11//PointyCar.obj");
	meshList[CAR11_BODY]->textureID = LoadTGA("Image//CarsTexture//Car11//PointyCarTexture.tga");
	//meshList[CAR11_WHEEL] = MeshBuilder::GenerateOBJ("Car11Wheel", "OBJ//Cars//Car11//PointyWheel.obj");
	//meshList[CAR11_BODY]->textureID = LoadTGA("Image//CarsTexture//Car11//PointyCarTexture.tga");
	

	cameraLockToPlayer = true;
	cameraY = 10;
	cameraZ = -20;
}

void SceneText::Update(double dt)
{
	// Animations for models
	//rotateCleanerTop += (float)(RSPEED * dt);
	rotateCleanerWheels -= (float)(RSPEED * dt);
	rotateCustomerRightArm += (float)(10.f * dt);
	rotateCustomerLeftArm -= (float)(10.f * dt);
	rotateCustomerHead += (float)(0.5f * dt);
	rotateCustomerLeftLeg += (float)(10.f * dt);
	rotateCustomerRightLeg -= (float)(10.f * dt);
	rotatecar += (float)(10.f * dt);
	

	if (!FreezeMovement)
	{
		if (cameraLockToPlayer);
		{
			Vector3 playerPos = { movePlayerX-2.5f, 5, movePlayerZ };
			Mtx44 rotation;
			rotation.SetToRotation(rotatePlayer, 0.f, 1.f, 0.f);
			Vector3 translation = { cameraZ, cameraY, 0 };
			translation = rotation * translation;
			camera.target = playerPos;
			camera.position = playerPos + translation;
		}
		if (Application::IsKeyPressed('W'))
		{
			Mtx44 rotation;
			rotation.SetToRotation(rotatePlayer, 0.f, 1.f, 0.f);
			Vector3 Movement = { (float)(10.f * dt), 0, 0 };
			Movement = rotation * Movement;
			Vector3 playerPos = { movePlayerX, 0, movePlayerZ };
			if (CollisionCheck::TrueCollisionCheck(playerPos + Movement, 1, 1))
			{
				movePlayerX += Movement.x;
				movePlayerZ += Movement.z;
			}
				rotatePlayerLeftArm += (float)(10.f * dt);
				rotatePlayerRightArm -= (float)(10.f * dt);
				rotatePlayerLeftLeg -= (float)(10.f * dt);
				rotatePlayerRightLeg += (float)(10.f * dt);
		
		}
		if (Application::IsKeyPressed('S'))
		{
			Mtx44 rotation;
			rotation.SetToRotation(rotatePlayer, 0.f, 1.f, 0.f);
			Vector3 Movement = { (float)(10.f * dt), 0, 0 };
			Movement = rotation * Movement;
			Vector3 playerPos = { movePlayerX, 0, movePlayerZ };
			if (CollisionCheck::TrueCollisionCheck(playerPos - Movement, 1, 1))
			{
				movePlayerX -= Movement.x;
				movePlayerZ -= Movement.z;
			}
				rotatePlayerLeftArm += (float)(10.f * dt);
				rotatePlayerRightArm -= (float)(10.f * dt);
				rotatePlayerLeftLeg -= (float)(10.f * dt);
				rotatePlayerRightLeg += (float)(10.f * dt);

		}
		if (Application::IsKeyPressed('A'))
		{
			Mtx44 rotation;
			rotation.SetToRotation(rotatePlayer, 0.f, 1.f, 0.f);
			Vector3 Movement = { 0, 0, (float)(10.f * dt) };
			Movement = rotation * Movement;
			Vector3 playerPos = { movePlayerX, 0, movePlayerZ };
			if (CollisionCheck::TrueCollisionCheck(playerPos - Movement, 1, 1))
			{
				movePlayerX -= Movement.x;
				movePlayerZ -= Movement.z;
			}
				rotatePlayerLeftArm += (float)(10.f * dt);
				rotatePlayerRightArm -= (float)(10.f * dt);
				rotatePlayerLeftLeg -= (float)(10.f * dt);
				rotatePlayerRightLeg += (float)(10.f * dt);
	

		}
		if (Application::IsKeyPressed('D'))
		{
			Mtx44 rotation;
			rotation.SetToRotation(rotatePlayer, 0.f, 1.f, 0.f);
			Vector3 Movement = { 0, 0, (float)(10.f * dt) };
			Movement = rotation * Movement;
			Vector3 playerPos = { movePlayerX, 0, movePlayerZ };
			if (CollisionCheck::TrueCollisionCheck(playerPos + Movement, 1, 1))
			{
				
				movePlayerX += Movement.x;
				movePlayerZ += Movement.z;
			}
			rotatePlayerLeftArm += (float)(10.f * dt);
			rotatePlayerRightArm -= (float)(10.f * dt);
			rotatePlayerLeftLeg -= (float)(10.f * dt);
			rotatePlayerRightLeg += (float)(10.f * dt);
		}
		if (Application::IsKeyPressed(VK_LEFT))
		{
			rotatePlayer += (float)(RSPEED * dt);
		}
		if (Application::IsKeyPressed(VK_RIGHT))
		{
			rotatePlayer -= (float)(RSPEED * dt);
		}
		if (Application::IsKeyPressed(VK_UP))
		{
			cameraY += (float)(RSPEED / 4 * dt);
		}
		if (Application::IsKeyPressed(VK_DOWN))
		{
			cameraY -= (float)(RSPEED / 4 * dt);
		}
		if (Application::IsKeyPressed('N'))
		{
			if (cameraZ != 0)
				cameraZ += (float)(RSPEED / 4 * dt);
		}
		if (Application::IsKeyPressed('M'))
		{
			cameraZ -= (float)(RSPEED / 4 * dt);
		}
	}

	//for bouncer
	if (escapeanimation == true)
	{
		
		elapsedTime += dt;
	}
	if (secondescpaeanimation == true)
	{
		passedTime += dt;
	}

	if (thirdescapeanimation == true)
	{
		timeDisappeared += dt;
	}

	if (MechanicGame == true) {
		// to make the keypress game
		elapsed += dt;
		if (hasreset == false) {
			for (int i = 0; i < 10; i++) {
				game[i] = '-';
			}
			hasreset = true;
		}
		if (Application::IsKeyPressed('W') && game[2] == 'W')
		{
			game[2] = '-';
			MechanicGameScore->AddPoints();
		}
		else if (Application::IsKeyPressed('A') && game[2] == 'A')
		{
			game[2] = '-';
			MechanicGameScore->AddPoints();
		}
		else if (Application::IsKeyPressed('S') && game[2] == 'S')
		{
			game[2] = '-';
			MechanicGameScore->AddPoints();
		}
		else if (Application::IsKeyPressed('D') && game[2] == 'D')
		{
			game[2] = '-';
			MechanicGameScore->AddPoints();
		}

		if (elapsed - debounce > 0.5f)
		{
			count++;

			random = rand() % 6 + 1;
			debounce = elapsed;
			for (int i = 1; i < 10; i++)
			{
				//Moves characters forward
				game[i - 1] = game[i];
				
			}
			if (game[1] != '-' && hasmissed == false) {
				MechanicGameScore->AddStrike();
				hasmissed = true;

			}
			hasmissed = false;
			game[9] = '-';
			if (count >= random)
			{
				count = 0;
				int random2 = rand() % 4;
				if (random2 == 0)
					game[9] = 'W';
				else if (random2 == 1)
					game[9] = 'A';
				else if (random2 == 2)
					game[9] = 'S';
				else game[9] = 'D';
			}
		}
	}
	// end for keypress game

	if (CleanerGame == true)
	{
		camera.position.x = 50;
		camera.position.y = 10;
		if (Application::IsKeyPressed(VK_LEFT))
			rotateCleanerTop += (float)(RSPEED * dt);
		if (Application::IsKeyPressed(VK_RIGHT))
			rotateCleanerTop -= (float)(RSPEED * dt);
		if (Application::IsKeyPressed('W'))
		{
			rotateCleanerWheelsForward += (float)(RSPEED * dt);
			moveX -= (float)(15.f * dt);
		}
		if (Application::IsKeyPressed('S'))
		{
			rotateCleanerWheelsForward -= (float)(RSPEED * dt);
			moveX += (float)(15.f * dt);
		}
		rotateCleanerWheelsForward -= (float)(RSPEED * dt);
		if (Application::IsKeyPressed('A'))
		{
			if (rotateCleanerWheelsY < 25.f)
				rotateCleanerWheelsY += (float)(RSPEED * dt);

		}
		if (Application::IsKeyPressed('D'))
		{
			if (rotateCleanerWheelsY > -25.f)
				rotateCleanerWheelsY -= (float(RSPEED * dt));
		}

		for (int i = 0; i < Day1Litter.size(); i++)
		{
			LitterX = Day1Litter[i].x;
			LitterY = Day1Litter[i].y;
			LitterZ = Day1Litter[i].z;
			float positionX = camera.position.x - 10;
			float vacuum = 2.5;
			if (positionX - vacuum > Day1Litter[i].x - 2 && positionX - vacuum < Day1Litter[i].x + 2
				&& camera.position.z + vacuum > Day1Litter[i].z - 2 && camera.position.z + vacuum < Day1Litter[i].z + 2)
			{

				Day1Litter[i].x = 55;
				Day1Litter[i].y = 0;
				Day1Litter[i].z = 55;
				//LitterX = positionX;
				//LitterY = 3; // inside the cleaner bot
				//LitterZ = camera.position.z;
				//CleanerTask->Addscore(Day1);
				std::cout << "PICKED UP";
			}
			std::cout << LitterX << " " << LitterZ << std::endl;
			std::cout << positionX << " " << camera.position.z << std::endl;

			//code for updating task lisk
			/*Tasklist* temp;
			BossOpinion->AddGoodwill(5);
			if (dayData->getDay() == 1) {
				temp = new Cleanertask(Day1);
				Day1 = temp->Addscore(Day1);
			}
			else if (dayData->getDay() == 2) {
				temp = new Cleanertask(Day2);
				Day2 = temp->Addscore(Day2);
			}
			else if (dayData->getDay() == 3) {
				temp = new Cleanertask(Day3);
				Day3 = temp->Addscore(Day3);
			}
			delete temp;*/
		}
	}
	
	for (int i = 0; i < 10; i++)
	{
		Vector3 characterPosition = { movePlayerX, 0, movePlayerZ };
		if (CollisionCheck::DistanceCheck(characterPosition, customerLocations[i].position) < 30 && salesCustomer == nullptr && !FreezeMovement && (customerLocations[i].isCurious == true || Vocation::isCustomerCurious() == true))
		{
			
			customerLocations[i].isCurious = true;
			customerLocations[i].inRange = true;
			if (Application::IsKeyPressed('I'))
			{
				FreezeMovement = true;
				customerLocations[i].selected = true;
				salesCustomer = new CSalesCustomer;
			}

		}
		else
		{
			customerLocations[i].inRange = false;
		}
	}

	if (salesCustomer != nullptr)
	{
		salesCustomer->CustomerUpdate(dt);
		if (salesCustomer->isCompleted() != 0)
		{
			if (salesCustomer->isCompleted() == 1)
			{
				Tasklist* temp = nullptr;
				BossOpinion->AddGoodwill(5);
				if (dayData->getDay() == 1) {
					temp = new Salesmantask(Day1);
					Day1 = temp->Addscore(Day1);
				}
				else if (dayData->getDay() == 2) {
					temp = new Salesmantask(Day2);
					Day2 = temp->Addscore(Day2);
				}
				else if (dayData->getDay() == 3) {
					temp = new Salesmantask(Day3);
					Day3 = temp->Addscore(Day3);
				}
				delete temp;
			}
			else {
				BossOpinion->LoseGoodwill(5);
			}
			delete salesCustomer;
			salesCustomer = nullptr;
			FreezeMovement = false;
			for (int i = 0; i < 10; i++)
			{
				if (customerLocations[i].selected == true)
				{
					customerLocations[i].selected = false; // customer minigame has ended
					customerLocations[i].isCurious = false; // customer is no longer curious since player has attended to him
				}
			}
		}
	}

	if (Application::IsKeyPressed(0x31))
	{
		glDisable(GL_CULL_FACE);
	}
	else if (Application::IsKeyPressed(0x32))
	{
		glEnable(GL_CULL_FACE);
	}
	else if (Application::IsKeyPressed(0x33))
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	else if (Application::IsKeyPressed(0x34))
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	/*if (Application::IsKeyPressed('I'))
		light[0].position.z -= (float)(LSPEED * dt);
	if (Application::IsKeyPressed('K'))
		light[0].position.z += (float)(LSPEED * dt);
	if (Application::IsKeyPressed('J'))
		light[0].position.x -= (float)(LSPEED * dt);
	if (Application::IsKeyPressed('L'))
		light[0].position.x += (float)(LSPEED * dt);
	if (Application::IsKeyPressed('O'))
		light[0].position.y -= (float)(LSPEED * dt);
	if (Application::IsKeyPressed('P'))
		light[0].position.y += (float)(LSPEED * dt);*/
	if (Application::IsKeyPressed('I')) {
		//Universal interaction key
		if ((distancecalculator(Vector3(movePlayerX, 0, movePlayerZ), Vector3(45, 0.5, 45)) < 10)) {
			MechanicGame = true;
			FreezeMovement = true;
		}
	}
	/*if (Application::IsKeyPressed('Q'))
	{
		//to do: switch light type to POINT and pass the information to
		light[0].type = Light::LIGHT_POINT;
	}
	else if (Application::IsKeyPressed('W'))
	{
		//to do: switch light type to DIRECTIONAL and pass the
		light[0].type = Light::LIGHT_DIRECTIONAL;
	}
	else if (Application::IsKeyPressed('E'))
	{
		//to do: switch light type to SPOT and pass the information to
		light[0].type = Light::LIGHT_SPOT;
	}*/
	if (MechanicGame == false && salesCustomer == nullptr) {
		if (AchievementScene == false && GameScene == true && DayEnds == false)
		{
			if (Application::IsKeyPressed(VK_RETURN))
			{
				AchievementScene = true;
				GameScene = false;

			}
		}
	}
	if (Application::IsKeyPressed(VK_RETURN) && MechanicGame == true)
	{
	}
	if (AchievementScene == true && GameScene == false)
	{
		if (Application::IsKeyPressed(VK_DELETE))
		{
			AchievementScene = false;
			GameScene = true;
		}
	}

	if (AchievementScene == false)
	{
			if (dayData->getDay() == 3)
		{
			if (Application::IsKeyPressed('X')) // change to I
			{
				escapeanimation = true;
			}
		}
		
		if (dayData->getDay() == 2)
		{
			if (Application::IsKeyPressed('Z')) //change to I
			{
				secondescpaeanimation = true;
			}
		}
		
		if (dayData->getDay() == 1)
		{
			if (Application::IsKeyPressed('C')) //change to I
			{
				thirdescapeanimation = true;
			}
		}
	}
	if (AchievementScene == false && GameScene == true && DayEnds == false)
	{
		timeData->setinGameTime(std::stof(timeData->getinGameTime()) - dt);

	  if (std::stof(timeData->getinGameTime()) < 0)
	  {
		DayEnds = true;
		GameScene = false;
	  } 
	}
	else if (AchievementScene == true)
	{
		
	}
	if (DayEnds == true && GameScene == false && AchievementScene == false)
	{
		if (Application::IsKeyPressed(VK_LBUTTON))
		{
			DayEnds = false;
			GameScene = true;
			dayData->NextDay();
			timeData->setinGameTime(600);

			
		}
	}
	if (FreezeMovement == false) {
		camera.Update(dt);
	}

	debounceTime += dt;
	if (BankOpen == true)
	{
		
		BankOpen = true;
		IsReserved = true;
		if (Application::IsKeyPressed(VK_LEFT) && debounceTime > 0.2f) {
			debounceTime = 0;
			Selection->Up();
			if (no != 1)
				no--;
		}
		if (Application::IsKeyPressed(VK_RIGHT) && debounceTime > 0.2f) {
			debounceTime = 0;
			Selection->Down();
			if (no != 6 )
				no++;
		}
		if (Application::IsKeyPressed(VK_RETURN) && debounceTime > 0.2f) {
			debounceTime = 0;
			;
			if (timeData->owncar(no))
			{
				int i = 3;
				// maybe something happens if the player owns the car
				
			}
			else if (timeData->Deposit(Selection))
			{
				timeData->buycar(no);
			}
			else
			{
				IsReserved = false;
			}
				
		}
	}
	CalculateFrameRate();
	if (BossOpinion->GetGoodwill() < 1) {
		//Your Fired
		FreezeMovement = true;
		gameover = true;
	}
	
		if (movePlayerX - distanceCheckX < 5 )
	{
		RenderTextOnScreen(meshList[GEO_TEXT], "press V", Color(1, 0, 0), 5, 10, 5);

		if (Application::IsKeyPressed('V'))
		{
			dialogueTime += 10 * dt;
			if (std::stoi(timeData->getinGameTime()) == 0)
			{
				dialogueTime = 0;
			}
		}
	}

	//Code for updating task list for Bouncer game
	/*Tasklist* temp;
		BossOpinion->AddGoodwill(5);
		if (dayData->getDay() == 1) {
			temp = new Bouncertask(Day1);
			Day1 = temp->Addscore(Day1);
		}
		else if (dayData->getDay() == 2) {
			temp = new Bouncertask(Day2);
			Day2 = temp->Addscore(Day2);
		}
		else if (dayData->getDay() == 3) {
			temp = new Bouncertask(Day3);
			Day3 = temp->Addscore(Day3);
		}
	delete temp;*/
}

void SceneText::Render()
{
	//Clear color & depth buffer every frame
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	viewStack.LoadIdentity();
	viewStack.LookAt(camera.position.x, camera.position.y, camera.position.z, camera.target.x, camera.target.y, camera.target.z, camera.up.x, camera.up.y, camera.up.z);
	modelStack.LoadIdentity();	
	if (light[0].type == Light::LIGHT_DIRECTIONAL)
	{
		Vector3 lightDir(light[0].position.x, light[0].position.y, light[0].position.z);
		Vector3 lightDirection_cameraspace = viewStack.Top() * lightDir;
		glUniform3fv(m_parameters[U_LIGHT0_POSITION], 1, &lightDirection_cameraspace.x);
	}
	else if (light[0].type == Light::LIGHT_SPOT)
	{
		Position lightPosition_cameraspace = viewStack.Top() * light[0].position;
		glUniform3fv(m_parameters[U_LIGHT0_POSITION], 1, &lightPosition_cameraspace.x);
		Vector3 spotDirection_cameraspace = viewStack.Top() * light[0].spotDirection;
		glUniform3fv(m_parameters[U_LIGHT0_SPOTDIRECTION], 1, &spotDirection_cameraspace.x);
	}
	else if (light[0].type == Light::LIGHT_POINT)
	{
		Position lightPosition_cameraspace = viewStack.Top() * light[0].position;
		glUniform3fv(m_parameters[U_LIGHT0_POSITION], 1, &lightPosition_cameraspace.x);
	}

	if (light[1].type == Light::LIGHT_DIRECTIONAL)
	{
		Vector3 lightDir(light[1].position.x, light[1].position.y, light[1].position.z);
		Vector3 lightDirection_cameraspace = viewStack.Top() * lightDir;
		glUniform3fv(m_parameters[U_LIGHT1_POSITION], 1, &lightDirection_cameraspace.x);
	}
	else if (light[1].type == Light::LIGHT_SPOT)
	{
		Position lightPosition_cameraspace = viewStack.Top() * light[1].position;
		glUniform3fv(m_parameters[U_LIGHT1_POSITION], 1, &lightPosition_cameraspace.x);
		Vector3 spotDirection_cameraspace = viewStack.Top() * light[1].spotDirection;
		glUniform3fv(m_parameters[U_LIGHT1_SPOTDIRECTION], 1, &spotDirection_cameraspace.x);
	}
	else
	{
		Position lightPosition_cameraspace = viewStack.Top() * light[1].position;
		glUniform3fv(m_parameters[U_LIGHT1_POSITION], 1, &lightPosition_cameraspace.x);
	}

	if (light[2].type == Light::LIGHT_DIRECTIONAL)
	{
		Vector3 lightDir(light[2].position.x, light[2].position.y, light[2].position.z);
		Vector3 lightDirection_cameraspace = viewStack.Top() * lightDir;
		glUniform3fv(m_parameters[U_LIGHT2_POSITION], 1, &lightDirection_cameraspace.x);
	}
	else if (light[2].type == Light::LIGHT_SPOT)
	{
		Position lightPosition_cameraspace = viewStack.Top() * light[2].position;
		glUniform3fv(m_parameters[U_LIGHT2_POSITION], 1, &lightPosition_cameraspace.x);
		Vector3 spotDirection_cameraspace = viewStack.Top() * light[2].spotDirection;
		glUniform3fv(m_parameters[U_LIGHT2_SPOTDIRECTION], 1, &spotDirection_cameraspace.x);
	}
	else
	{
		Position lightPosition_cameraspace = viewStack.Top() * light[2].position;
		glUniform3fv(m_parameters[U_LIGHT2_POSITION], 1, &lightPosition_cameraspace.x);
	}

	if (light[3].type == Light::LIGHT_DIRECTIONAL)
	{
		Vector3 lightDir(light[3].position.x, light[3].position.y, light[3].position.z);
		Vector3 lightDirection_cameraspace = viewStack.Top() * lightDir;
		glUniform3fv(m_parameters[U_LIGHT3_POSITION], 1, &lightDirection_cameraspace.x);
	}
	else if (light[3].type == Light::LIGHT_SPOT)
	{
		Position lightPosition_cameraspace = viewStack.Top() * light[3].position;
		glUniform3fv(m_parameters[U_LIGHT3_POSITION], 1, &lightPosition_cameraspace.x);
		Vector3 spotDirection_cameraspace = viewStack.Top() * light[3].spotDirection;
		glUniform3fv(m_parameters[U_LIGHT3_SPOTDIRECTION], 1, &spotDirection_cameraspace.x);
	}
	else
	{
		Position lightPosition_cameraspace = viewStack.Top() * light[3].position;
		glUniform3fv(m_parameters[U_LIGHT3_POSITION], 1, &lightPosition_cameraspace.x);
	}

	if (light[4].type == Light::LIGHT_DIRECTIONAL)
	{
		Vector3 lightDir(light[4].position.x, light[4].position.y, light[4].position.z);
		Vector3 lightDirection_cameraspace = viewStack.Top() * lightDir;
		glUniform3fv(m_parameters[U_LIGHT4_POSITION], 1, &lightDirection_cameraspace.x);
	}
	else if (light[4].type == Light::LIGHT_SPOT)
	{
		Position lightPosition_cameraspace = viewStack.Top() * light[4].position;
		glUniform3fv(m_parameters[U_LIGHT4_POSITION], 1, &lightPosition_cameraspace.x);
		Vector3 spotDirection_cameraspace = viewStack.Top() * light[4].spotDirection;
		glUniform3fv(m_parameters[U_LIGHT4_SPOTDIRECTION], 1, &spotDirection_cameraspace.x);
	}
	else
	{
		Position lightPosition_cameraspace = viewStack.Top() * light[4].position;
		glUniform3fv(m_parameters[U_LIGHT4_POSITION], 1, &lightPosition_cameraspace.x);
	}
	if (light[5].type == Light::LIGHT_DIRECTIONAL)
	{
		Vector3 lightDir(light[5].position.x, light[5].position.y, light[5].position.z);
		Vector3 lightDirection_cameraspace = viewStack.Top() * lightDir;
		glUniform3fv(m_parameters[U_LIGHT5_POSITION], 1, &lightDirection_cameraspace.x);
	}
	else if (light[5].type == Light::LIGHT_SPOT)
	{
		Position lightPosition_cameraspace = viewStack.Top() * light[5].position;
		glUniform3fv(m_parameters[U_LIGHT5_POSITION], 1, &lightPosition_cameraspace.x);
		Vector3 spotDirection_cameraspace = viewStack.Top() * light[5].spotDirection;
		glUniform3fv(m_parameters[U_LIGHT5_SPOTDIRECTION], 1, &spotDirection_cameraspace.x);
	}
	else
	{
		Position lightPosition_cameraspace = viewStack.Top() * light[5].position;
		glUniform3fv(m_parameters[U_LIGHT5_POSITION], 1, &lightPosition_cameraspace.x);
	}
	if (light[6].type == Light::LIGHT_DIRECTIONAL)
	{
		Vector3 lightDir(light[6].position.x, light[6].position.y, light[6].position.z);
		Vector3 lightDirection_cameraspace = viewStack.Top() * lightDir;
		glUniform3fv(m_parameters[U_LIGHT6_POSITION], 1, &lightDirection_cameraspace.x);
	}
	else if (light[6].type == Light::LIGHT_SPOT)
	{
		Position lightPosition_cameraspace = viewStack.Top() * light[6].position;
		glUniform3fv(m_parameters[U_LIGHT6_POSITION], 1, &lightPosition_cameraspace.x);
		Vector3 spotDirection_cameraspace = viewStack.Top() * light[6].spotDirection;
		glUniform3fv(m_parameters[U_LIGHT6_SPOTDIRECTION], 1, &spotDirection_cameraspace.x);
	}
	else
	{
		Position lightPosition_cameraspace = viewStack.Top() * light[6].position;
		glUniform3fv(m_parameters[U_LIGHT6_POSITION], 1, &lightPosition_cameraspace.x);
	}

	if (light[7].type == Light::LIGHT_DIRECTIONAL)
	{
		Vector3 lightDir(light[7].position.x, light[7].position.y, light[7].position.z);
		Vector3 lightDirection_cameraspace = viewStack.Top() * lightDir;
		glUniform3fv(m_parameters[U_LIGHT7_POSITION], 1, &lightDirection_cameraspace.x);
	} 
	else if (light[7].type == Light::LIGHT_SPOT)
	{
		Position lightPosition_cameraspace = viewStack.Top() * light[7].position;
		glUniform3fv(m_parameters[U_LIGHT7_POSITION], 1, &lightPosition_cameraspace.x);
		Vector3 spotDirection_cameraspace = viewStack.Top() * light[7].spotDirection;
		glUniform3fv(m_parameters[U_LIGHT7_SPOTDIRECTION], 1, &spotDirection_cameraspace.x);
	}
	else
	{
		Position lightPosition_cameraspace = viewStack.Top() * light[7].position;
		glUniform3fv(m_parameters[U_LIGHT7_POSITION], 1, &lightPosition_cameraspace.x);
	}

	//if (light[8].type == Light::LIGHT_DIRECTIONAL)
	//{
	//	Vector3 lightDir(light[8].position.x, light[8].position.y, light[8].position.z);
	//	Vector3 lightDirection_cameraspace = viewStack.Top() * lightDir;
	//	glUniform3fv(m_parameters[U_LIGHT8_POSITION], 1, &lightDirection_cameraspace.x);
	//}
	//// if it is spot light, pass in position and direction 
	//else if (light[8].type == Light::LIGHT_SPOT)
	//{
	//	Position lightPosition_cameraspace = viewStack.Top() * light[8].position;
	//	glUniform3fv(m_parameters[U_LIGHT8_POSITION], 1, &lightPosition_cameraspace.x);
	//	Vector3 spotDirection_cameraspace = viewStack.Top() * light[8].spotDirection;
	//	glUniform3fv(m_parameters[U_LIGHT8_SPOTDIRECTION], 1, &spotDirection_cameraspace.x);
	//}
	//else
	//{
	//	// default is point light (only position since point light is 380 degrees)
	//	Position lightPosition_cameraspace = viewStack.Top() * light[8].position;
	//	glUniform3fv(m_parameters[U_LIGHT8_POSITION], 1, &lightPosition_cameraspace.x);
	//}
	if (AchievementScene == true)
	{

		TotalBouncerTask = Day1[2].maxNumber + Day2[2].maxNumber + Day2[2].maxNumber; //algo for this needs to be tested once the minigame is done
		CurrentBouncerTask = Day1[2].currentNumber + Day2[2].currentNumber + Day2[2].currentNumber;

		CurrentSalespersonTask = Day1[0].currentNumber + Day2[0].currentNumber + Day3[0].currentNumber; //just leave like this for now, same reason as bouncer
		
		std::string CarAchievements;
		Achievements* Cartext = new Achievements();
		CarAchievements = Cartext->carAchievements(Price);
		
		if (CarAchievements == "CheapSkate Achieved!" && CarAchievement2 == false && CarAchievement3 == false)
		{
			CarAchievement1 = true;
			CarAchievement2 = false;
			CarAchievement3 = false;
		}
		else if (CarAchievements == "Playing it Safe Achieved!" && CarAchievement1 == false && CarAchievement3 == false)
		{
			CarAchievement2 = true;
			CarAchievement1 = false;
			CarAchievement3 = false;
		}
		else if (CarAchievements == "Crazy Rich!" && CarAchievement1 == false && CarAchievement2 == false)
		{
			CarAchievement3 = true;
			CarAchievement1 = false;
			CarAchievement2 = false;
		}

		std::string BouncerAchievements;
		Achievements* Bouncertext = new Achievements();
		BouncerAchievements = Bouncertext->bouncerAchievements(CurrentBouncerTask, TotalBouncerTask);
		
		std::string SalesAchievements;
		Achievements* Salestext = new Achievements();
		SalesAchievements = Salestext->salespersonAchievements(CurrentSalespersonTask);
		
		RenderObjectOnScreen(meshList[GEO_ACHIEVEMENTSBG], 50, 0.8, 0.5);

		RenderTextOnScreen(meshList[GEO_ACHIEVEMENTS], CarAchievements, Color(1, 0, 0), 3.f, 3.5f, 10);
		RenderTextOnScreen(meshList[GEO_ACHIEVEMENTS], BouncerAchievements, Color(1, 0, 0), 3, 3.5f, 11);
		RenderTextOnScreen(meshList[GEO_ACHIEVEMENTS], SalesAchievements, Color(1, 0, 0), 3, 3.5f, 12);
	
	}
	else if (gameover == true) {
		RenderObjectOnScreen(meshList[GEO_ACHIEVEMENTSBG], 50, 0.8, 0.5);
		RenderTextOnScreen(meshList[GEO_TEXT], "Your Fired!", Color(1, 0, 0), 6, 3, 9);
		RenderTextOnScreen(meshList[GEO_TEXT], "Due to a shocking display of gross", Color(1, 0, 0), 3, 2, 16);
		RenderTextOnScreen(meshList[GEO_TEXT], "negligence and incompetence during", Color(1, 0, 0), 3, 2, 15);
		RenderTextOnScreen(meshList[GEO_TEXT], "work hours, the management has seen ", Color(1, 0, 0), 3, 2, 14);
		RenderTextOnScreen(meshList[GEO_TEXT], "fit to terminate you contract", Color(1, 0, 0), 3, 2, 13);
		RenderTextOnScreen(meshList[GEO_TEXT], "effective immediately.", Color(1, 0, 0), 3, 2, 12);
		RenderTextOnScreen(meshList[GEO_TEXT], "Press Esc to vacate the building", Color(1, 0, 0), 3, 2, 10);
	}
	else if (MechanicGame == true) {
		std::string print = "";
		RenderObjectOnScreen(meshList[GEO_ACHIEVEMENTSBG], 50, 0.8, 0.5);
		//RenderObjectOnScreen(meshList[GEO_MECHANIC_GAME], 50, 0.8, 0.5);
		for (int i = 0; i < 10; i++)
		{
			if (i != 2)
			{
				print.push_back(game[i]);
			}
			else
			{
				print.push_back(' ');
			}
		}
		RenderTextOnScreen(meshList[GEO_TEXT], print, Color(0, 1, 0), 5, 2, 1);
		print = "  ";
		print = "Points: ";
		print.append(std::to_string(MechanicGameScore->GetPoints()));
		print.append("/");
		print.append(std::to_string(MECHANIC_GAME_MAX_SCORE));
		RenderTextOnScreen(meshList[GEO_TEXT], print, Color(1, 0, 0), 5, 2, 3);
		print = "  ";
		print = "Strikes ";
		print.append(std::to_string(MechanicGameScore->GetStrike()));
		print.append("/");
		print.append(std::to_string(MECHANIC_GAME_MAX_LIVES));
		RenderTextOnScreen(meshList[GEO_TEXT], print, Color(1, 0, 0), 5, 2, 2);
		print = "  ";
		print.push_back(game[2]);
		RenderTextOnScreen(meshList[GEO_TEXT], print, Color(1, 0, 0), 5, 2, 1);
		RenderTextOnScreen(meshList[GEO_TEXT], "^", Color(0, 1, 0), 4, 3.9, 0);


		if (MechanicGameScore->GetStrike() > MECHANIC_GAME_MAX_LIVES) {
			//You Lose
			FreezeMovement = false;
			MechanicGame = false;
			BossOpinion->LoseGoodwill(5);
			delete MechanicGameScore;
			MechanicGameScore = new Mechanictask();
			hasreset = false;
		}

		if (MechanicGameScore->GetPoints() == MECHANIC_GAME_MAX_SCORE) {
			Tasklist* Temp;
			Temp = new Mechanictask();
			BossOpinion->AddGoodwill(5);
			if (dayData->getDay() == 1) {
				Day1 = Temp->Addscore(Day1);
			}
			else if (dayData->getDay() == 2) {
				Day2 = Temp->Addscore(Day2);
			}
			else if (dayData->getDay() == 3) {
				Day3 = Temp->Addscore(Day3);
			}
			else {
				std::cout << "Error Updating Score" ;
			}
			delete Temp;
			FreezeMovement = false;
			MechanicGame = false;
			timeData->setMoney(MechanicSalary* BossOpinion->getmodifier());
			std::cout << timeData->getMoney();
			delete MechanicGameScore;
			MechanicGameScore = new Mechanictask();
			hasreset = false;
		}
	}
	
	else if(GameScene == true)
	{
		
		RenderSkybox();
		Renderlevel();
		RenderPlayer();
		
		//RenderCar1();
		//RenderCar2();
		//RenderCar3();
		//RenderCar4();
		//RenderCar5();
		//RenderCar6();
		//RenderCar7();
		//RenderCar8(); // unable to show
		//RenderCar9();
		//RenderCar10(); // need wheels
		//RenderCar11(); // need wheels

		
		/*modelStack.PushMatrix();
		modelStack.Translate(light[0].position.x, light[0].position.y, light[0].position.z);
		RenderMesh(meshList[GEO_LIGHTSPHERE], false);
		modelStack.PopMatrix();*/

		modelStack.PushMatrix();
		modelStack.Translate(30, 30, -30);
		RenderMesh(meshList[ITEM_LIGHTBULB], false);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(30, 30, 0);
		RenderMesh(meshList[ITEM_LIGHTBULB], false);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(30, 30, 30);
		RenderMesh(meshList[ITEM_LIGHTBULB], false);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(0, 30, -30);
		RenderMesh(meshList[ITEM_LIGHTBULB], false);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(0, 30, 0);
		RenderMesh(meshList[ITEM_LIGHTBULB], false);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(0, 30, 30);
		RenderMesh(meshList[ITEM_LIGHTBULB], false);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(-30, 30, -30);
		RenderMesh(meshList[ITEM_LIGHTBULB], false);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(-30, 30, 0);
		RenderMesh(meshList[ITEM_LIGHTBULB], false);
		modelStack.PopMatrix();

		/*modelStack.PushMatrix();
		modelStack.Translate(-20, 30, 20);
		RenderMesh(meshList[ITEM_LIGHTBULB], false);
		modelStack.PopMatrix();*/

		//modelStack.PushMatrix();
		//modelStack.Translate(0, -3, 0);
		//RenderMesh(meshList[GEO_DICE], true);
		//modelStack.PopMatrix();
		
		modelStack.PushMatrix();
		modelStack.Translate(-45, 0, -27);
		modelStack.Rotate(90.f, 0, 1, 0);
		RenderGuardBot();
		modelStack.PopMatrix();
		
		// Render Customer
		for (int i = 0; i < 10; i++)
		{
			modelStack.PushMatrix();
			modelStack.Translate(customerLocations[i].position.x, customerLocations[i].position.y, customerLocations[i].position.z);
			modelStack.PushMatrix();
			modelStack.Rotate(customerLocations[i].angle, 0.f, 1.f, 0.f);
			if (customerLocations[i].customerType == 0)
			{
				RenderCustomer();
			}
			else
			{
				RenderCustomer2();
			}
			if (customerLocations[i].inRange == true)
			{
				modelStack.Translate(2.f, 5, 0);
				modelStack.Scale(1, 1, 1);

				modelStack.Rotate(-CollisionCheck::angleBetween2Coords(camera.target, camera.position) + 90 - customerLocations[i].angle, 0.f, 1.f, 0.f);
				modelStack.Translate(-9.5, 0, 0);
				RenderText(meshList[GEO_TEXT], "Press I to interact", Color(0, 1, 0));
			}

			modelStack.PopMatrix();
			modelStack.PopMatrix();
		}


		if (dayData->getDay() == 1)
		{
		
			if (Day1[2].currentNumber == 0)
			{
				if (thirdescapeanimation == false) //Day1
				{
					RenderSuspect3();
				}

				if (timeDisappeared <= 2 && thirdescapeanimation == true)
				{
					startingPosition = 15;

					modelStack.PushMatrix();
					modelStack.Translate(-20, 1, startingPosition + timeDisappeared * 15);
					RenderSuspect3();
					modelStack.PopMatrix();
				}
				else if (timeDisappeared > 2 && timeDisappeared <= 4 && thirdescapeanimation == true)
				{
					middlePosition = -20;

					modelStack.PushMatrix();
					modelStack.Translate(middlePosition + (timeDisappeared - 2) * 15 / 2, 1, 45);
					RenderSuspect3();
					modelStack.PopMatrix();
				}
				else if (timeDisappeared > 4 && timeDisappeared <= 6 && thirdescapeanimation == true)
				{
					finalPosition = 38;

					modelStack.PushMatrix();
					modelStack.Translate(-5, 1, finalPosition + (timeDisappeared - 3) * 15 / 2);
					RenderSuspect3();
					modelStack.PopMatrix();
				}
			}

			std::string TrackedTask;
			Vocation::getVocation();
			Tasklist* Task = new Salesmantask(Day1);
			TrackedTask = Task->Taskstatus(Day1);
			RenderTextOnScreen(meshList[GEO_TEXT], TrackedTask, Color(0, 1, 0), 2, 0, 0);
			delete Task;

			Tasklist* BouncerTask = new Bouncertask(Day1);
			TrackedTask = BouncerTask->Taskstatus(Day1);
			RenderTextOnScreen(meshList[GEO_TEXT], TrackedTask, Color(0, 1, 0), 2, 0, 2);
			delete BouncerTask;

			Tasklist* CleanerTask = new Cleanertask(Day1);
			TrackedTask = CleanerTask->Taskstatus(Day1);
			RenderTextOnScreen(meshList[GEO_TEXT], TrackedTask, Color(0, 1, 0), 2, 0, 1);
			delete CleanerTask;
			
			if (dialogueTime > 1 && dialogueTime <= 3)
			{

				RenderObjectOnScreen(meshList[GEO_TEXTBOX], 9, 3.f, 1.f);
				RenderTextOnScreenWithNewLine(meshList[GEO_TEXT], "Hey new guy look out for any baddies kay? Heard one of them is a human!", Color(1, 0, 0), 3, 1, 4, 25, false);

			}
		}

		else if(dayData->getDay() == 2)
		{
			if (secondescpaeanimation == false) //Day2
			{
				RenderSuspect2();
			}

			if (passedTime <= 3 && secondescpaeanimation == true)
			{
				startingPosition = 0;

				modelStack.PushMatrix();
				modelStack.Translate(15, 1, startingPosition + passedTime * 15);
				RenderSuspect2();
				modelStack.PopMatrix();
			}
			else if (passedTime > 3 && passedTime <= 6 && secondescpaeanimation == true)
			{
				middlePosition = 15;

				modelStack.PushMatrix();
				modelStack.Translate(middlePosition - (passedTime - 3) * 15 / 3, 1, 45);
				RenderSuspect2();
				modelStack.PopMatrix();
			}
			else if (passedTime > 6 && passedTime <= 10 && secondescpaeanimation == true)
			{
				finalPosition = 30;

				modelStack.PushMatrix();
				modelStack.Translate(0, 1, finalPosition + (passedTime - 2) * 15 / 4);
				RenderSuspect2();
				modelStack.PopMatrix();
			}


			

			//Day2 
			std::string TrackedTask2;
			Vocation::getVocation();
			Tasklist* BouncerTask2 = new Bouncertask(Day2);
			TrackedTask2 = BouncerTask2->Taskstatus(Day2);
			RenderTextOnScreen(meshList[GEO_TEXT], TrackedTask2, Color(0, 1, 0), 2, 0, 3);
			
			
                        if (dialogueTime > 1 && dialogueTime <= 3)
			{

				RenderObjectOnScreen(meshList[GEO_TEXTBOX], 9, 3.f, 1.f);
				RenderTextOnScreenWithNewLine(meshList[GEO_TEXT], "I heard that some rogue robot is around. Watch out", Color(1, 0, 0), 3, 1, 4, 25, false);

			}

			
		}
		else if (dayData->getDay() == 3)
		{
		if (escapeanimation == false)
		{
			RenderSuspect();
		}

		if (elapsedTime <= 1 && escapeanimation == true) //Day3
		{
			startingPosition = 30;

			modelStack.PushMatrix();
			modelStack.Translate(15, 1, startingPosition + elapsedTime * 15);
			RenderSuspect();
			modelStack.PopMatrix();
		}
		else if (elapsedTime > 1 && elapsedTime <= 3 && escapeanimation == true) // move foward
		{
			middlePosition = 15;

			modelStack.PushMatrix();
			modelStack.Translate(middlePosition - (elapsedTime - 1) * 15 / 3, 1, 45); //moving to the right
			RenderSuspect();
			modelStack.PopMatrix();

		}
		else if (elapsedTime > 3 && elapsedTime <= 6 && escapeanimation == true)
		{
			finalPosition = 45;

			modelStack.PushMatrix();
			modelStack.Translate(5, 1, finalPosition + (elapsedTime - 3) * 15 / 3); // moving foward
			RenderSuspect();
			modelStack.PopMatrix();
		}


		//Day3
		std::string TrackedTask3;
		Vocation::getVocation();
		Tasklist* BouncerTask3 = new Bouncertask(Day3);
		TrackedTask3 = BouncerTask3->Taskstatus(Day3);
		RenderTextOnScreen(meshList[GEO_TEXT], TrackedTask3, Color(0, 1, 0), 2, 0, 4);
			
			if (dialogueTime > 1 && dialogueTime <= 3)
		{

			RenderObjectOnScreen(meshList[GEO_TEXTBOX], 9, 3.f, 1.f);
			RenderTextOnScreenWithNewLine(meshList[GEO_TEXT], "This is bad, I heard from the boss that this dude disguised himself! I can't tell where he is", Color(1, 0, 0), 3, 1, 4, 25, false);

		}
        }

		modelStack.PushMatrix();
		//scale, translate, rotate
		RenderText(meshList[GEO_TEXT], "HELLO WORLD", Color(0, 1, 0));
		modelStack.PopMatrix();
		
		if (elapsedTime > 5 && elapsedTime <= 9 || passedTime > 7 && passedTime <= 10 || timeDisappeared > 6 && timeDisappeared <= 9) //if within 15 seconds
		{
				RenderBouncerTextBox();

				if (dayData->getDay() == 1)
				{
					Tasklist* temp;
					temp = new Bouncertask(Day1);
					Day1 = temp->Addscore(Day1);
					delete temp;
			    }
				else if (dayData->getDay() == 2)
				{
					Tasklist* temp;
					temp = new Bouncertask(Day2);
					Day2 = temp->Addscore(Day2);
					delete temp;
				}
				else if (dayData->getDay() == 3)
				{
					Tasklist* temp;
					temp = new Bouncertask(Day3);
					Day3 = temp->Addscore(Day3);
					delete temp;
				}

				
		}
		std::string time = std::to_string((int)std::stof(timeData->getinGameTime()));

		RenderTextOnScreen(meshList[GEO_TEXT], time , Color(1, 0, 0), 2, 0, 29);
		
		std::string day = std::to_string(dayData->getDay());
		RenderTextOnScreen(meshList[GEO_TEXT], day, Color(1, 0, 0), 2, 3, 29);
		if (CleanerGame == true)
		{
			modelStack.PushMatrix();
			modelStack.Translate(camera.position.x - 10, camera.position.y -0.5, camera.position.z);
			RenderCleanerRobot();
			modelStack.PopMatrix();
			for (int i = 0; i < Day1Litter.size(); ++i)
			{
				modelStack.Translate(Day1Litter[i].x, Day1Litter[i].y, Day1Litter[i].z);
				RenderLitter();
			}
		}
	}
	else if (DayEnds == true)
	{
		RenderObjectOnScreen(meshList[GEO_ACHIEVEMENTSBG], 50, 0.8, 0.5);
		RenderTextOnScreen(meshList[GEO_TEXT], "Day " + std::to_string(dayData->getDay()) + " completed", Color(0, 1, 0), 3, 9, 10);
    }


	if (salesCustomer != nullptr && salesCustomer->answered == false)
	{
		if (salesCustomer->count <= salesCustomer->getQuestion().length())
			salesCustomer->count++;
		std::string toPrint = (salesCustomer->getQuestion()).substr(0, salesCustomer->count);

		RenderObjectOnScreen(meshList[GEO_FRAME], 10, 4, 3.5);
		RenderTextOnScreenWithNewLine(meshList[GEO_TEXT], toPrint, Color(0, 0, 0), 3, 4, 12.5, 28, false);
		RenderTextOnScreenWithNewLine(meshList[GEO_TEXT], "[Q: Up | E: Down | Enter: Select]", Color(0, 0, 0), 2, 8.5, 15, 50, false);
		
		RenderObjectOnScreen(meshList[GEO_FRAME3], 10, 4, 5.1);
		RenderObjectOnScreen(meshList[GEO_CUSTOMERLOGO], 15, 1, 3.4);
		RenderTextOnScreen(meshList[GEO_TEXT], salesCustomer->getName(), Color(0,0,1), 3, 8, 18.25);
		RenderTextOnScreen(meshList[GEO_TEXT], "Satisfaction:", Color(0,2,0), 3, 8, 17.25);

		RenderObjectOnScreen(meshList[GEO_FRAME2], 10, 4, 1.3);
		if (salesCustomer->count > salesCustomer->getQuestion().length())
		{
			RenderTextOnScreenWithNewLine(meshList[GEO_TEXT], " > " + salesCustomer->getPlayerChoice()->getPrevious()->getPrevious()->getText(), Color(0, 0, 0), 2.25, 5, 10, 40, true);
			RenderTextOnScreenWithNewLine(meshList[GEO_TEXT], " > " + salesCustomer->getPlayerChoice()->getPrevious()->getText(), Color(0, 0, 0), 2.25, 3.5, 8, 40, true);
			RenderTextOnScreenWithNewLine(meshList[GEO_TEXT], " > " + salesCustomer->getPlayerChoice()->getText(), Color(0,0,0), 2.25, 2.2, 6, 40, true);
			RenderTextOnScreenWithNewLine(meshList[GEO_TEXT], " > " + salesCustomer->getPlayerChoice()->getNext()->getText(), Color(0, 0, 0), 2.25, 3.5, 4, 40, true);
			RenderTextOnScreenWithNewLine(meshList[GEO_TEXT], " > " + salesCustomer->getPlayerChoice()->getNext()->getNext()->getText(), Color(0, 0, 0), 2.25, 5, 2, 40, true);
		}
		if (salesCustomer->getAppeaseRate() < 0)
			RenderTextOnScreen(meshList[GEO_TEXT], std::to_string(salesCustomer->getAppeaseRate()), Color(3, 0, 0), 3, 17, 17.25);
		else
			RenderTextOnScreen(meshList[GEO_TEXT], std::to_string(salesCustomer->getAppeaseRate()), Color(0, 2, 0), 3, 17, 17.25);
		RenderTextOnScreen(meshList[GEO_TEXT], "/" + std::to_string(salesCustomer->getSatisfactionRate()), Color(0, 2, 0), 3, 18.5, 17.25);
		RenderTextOnScreenWithNewLine(meshList[GEO_TEXT], "Reach satisfaction(" + std::to_string(salesCustomer->getSatisfactionRate() - 4) + ") and you will face the penalty!", Color(3, 0, 0), 2, 12, 24.37, 30, false);
	}
	else if (salesCustomer != nullptr && salesCustomer->answered == true)
	{
		if (salesCustomer->count <= salesCustomer->getPlaceHolderText().length() + 100)
			salesCustomer->count++;
		std::string toPrint = salesCustomer->getPlaceHolderText().substr(0, salesCustomer->count);
		modelStack.PushMatrix();
		RenderObjectOnScreen(meshList[GEO_FRAME], 10, 4, 3.5);
		RenderTextOnScreenWithNewLine(meshList[GEO_TEXT], toPrint, Color(0, 0, 0), 3, 4, 12.5, 28, false);
		modelStack.PopMatrix();
		modelStack.PushMatrix();
		RenderObjectOnScreen(meshList[GEO_FRAME2], 10, 4, 1.3);
		if (salesCustomer->count >= salesCustomer->getPlaceHolderText().length() + 100)
		{
			salesCustomer->answered = false;
			salesCustomer->count = 0;
			salesCustomer->isAppeased(); // will check if customer is appeased, if not get another question
		}
		RenderObjectOnScreen(meshList[GEO_FRAME3], 10, 4, 5.1);
		RenderObjectOnScreen(meshList[GEO_CUSTOMERLOGO], 15, 1, 3.4);
		RenderTextOnScreen(meshList[GEO_TEXT], salesCustomer->getName(), Color(0, 0, 1), 3, 8, 18.25);
		RenderTextOnScreen(meshList[GEO_TEXT], "Satisfaction:", Color(0, 2, 0), 3, 8, 17.25);
		if (salesCustomer->getAppeaseRate() < 0)
			RenderTextOnScreen(meshList[GEO_TEXT], std::to_string(salesCustomer->getAppeaseRate()), Color(3, 0, 0), 3, 17, 17.25);
		else
			RenderTextOnScreen(meshList[GEO_TEXT], std::to_string(salesCustomer->getAppeaseRate()), Color(0, 2, 0), 3, 17, 17.25);
		RenderTextOnScreen(meshList[GEO_TEXT], "/" + std::to_string(salesCustomer->getSatisfactionRate()), Color(0, 2, 0), 3, 18.5, 17.25);
		RenderTextOnScreenWithNewLine(meshList[GEO_TEXT], "Reach satisfaction(" + std::to_string(salesCustomer->getSatisfactionRate() - 4) + ") and you will face the penalty!", Color(3, 0, 0), 2, 12, 24.37, 30, false);
	}
	if (BankOpen)
	{
		RenderObjectOnScreen(meshList[GEO_BANKMENU], 0.5, 0.4, 80);
		RenderTextOnScreen(meshList[GEO_TEXT], "BANK: $" + std::to_string(timeData->getMoney()), Color(0, 1, 1), 2.5, 15.5, 17.5);
		RenderTextOnScreen(meshList[GEO_TEXT], "RESERVE A CAR", Color(0, 1, 1), 3, 7, 16);
		int position = 0;
		while (true) {
			RenderTextOnScreen(meshList[GEO_TEXT], Selection->PrintName(position), Color(0, 1, 1), 2.5, 4, 16 - position);
			RenderTextOnScreen(meshList[GEO_TEXT], "$" + Selection->PrintPrice(position), Color(0, 1, 1), 2.5, 21, 16 - position);
			position++;
			if (position > 11) {
				break;
			}
		}
		position = 0;
		if (IsReserved)
			RenderTextOnScreen(meshList[GEO_TEXT], "Reserve " + Selection->PrintSelection(Selection) + "?", Color(0, 1, 1), 2.5, 3, 3.5);
		if (!IsReserved && Application::IsKeyPressed(VK_RETURN))
			RenderTextOnScreen(meshList[GEO_TEXT], "Not enough money", Color(0, 1, 1), 2.5, 3, 3.5);

		//Selection->Printing(); // for checking

	}
	
}

void SceneText::Exit()
{
	// Cleanup here
	for (int i = 0; i < NUM_GEOMETRY; ++i)
	{
		if (meshList[i] != NULL)
			delete meshList[i];
	}
	Vocation::ConnectQuest(1, Day1);
	Vocation::ConnectQuest(2, Day2);
	Vocation::ConnectQuest(3, Day3);
	timeData->saveGame();
	Vocation::ClearJob(true);
	// Cleanup VBO here
	glDeleteVertexArrays(1, &m_vertexArrayID);
	glDeleteProgram(m_programID);

}

void SceneText::RenderMesh(Mesh* mesh, bool enableLight)
{
	Mtx44 MVP, modelView, modelView_inverse_transpose;

	MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top();
	glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);

	modelView = viewStack.Top() * modelStack.Top();
	glUniformMatrix4fv(m_parameters[U_MODELVIEW], 1, GL_FALSE, &modelView.a[0]);


	if (enableLight)
	{
		glUniform1i(m_parameters[U_LIGHTENABLED], 1);
		modelView_inverse_transpose = modelView.GetInverse().GetTranspose();
		glUniformMatrix4fv(m_parameters[U_MODELVIEW_INVERSE_TRANSPOSE], 1, GL_FALSE, &modelView_inverse_transpose.a[0]);

		//load material
		glUniform3fv(m_parameters[U_MATERIAL_AMBIENT], 1, &mesh->material.kAmbient.r);
		glUniform3fv(m_parameters[U_MATERIAL_DIFFUSE], 1, &mesh->material.kDiffuse.r);
		glUniform3fv(m_parameters[U_MATERIAL_SPECULAR], 1, &mesh->material.kSpecular.r);
		glUniform1f(m_parameters[U_MATERIAL_SHININESS], mesh->material.kShininess);
	}
	else
	{
		glUniform1i(m_parameters[U_LIGHTENABLED], 0);
	}

	if(mesh->textureID > 0){ 
		glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mesh->textureID);
		glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);} 
	else { 
		glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 0);
	} 
	mesh->Render(); //this line should only be called once in the whole function

	if(mesh->textureID > 0) glBindTexture(GL_TEXTURE_2D, 0);
}

void SceneText::RenderSkybox()
{
	modelStack.PushMatrix();
		///scale, translate, rotate 
		modelStack.Translate(-50.f, 0.f, 0.f);
		modelStack.Scale(100.f, 100.f, 100.f);
		modelStack.Rotate(90.f, 0.f, 1.f, 0.f);
		RenderMesh(meshList[GEO_LEFT], false);
	modelStack.PopMatrix();
	modelStack.PushMatrix();
		///scale, translate, rotate 
		modelStack.Translate(50.f, 0.f, 0.f);
		modelStack.Scale(100.f, 100.f, 100.f);
		modelStack.Rotate(-90.f, 0.f, 1.f, 0.f);
		RenderMesh(meshList[GEO_RIGHT], false);
	modelStack.PopMatrix();
	modelStack.PushMatrix();
		///scale, translate, rotate 
		modelStack.Translate(0.f, 50.f, 0.f);
		modelStack.Scale(100.f, 100.f, 100.f);
		modelStack.Rotate(90.f, 1.f, 0.f, 0.f);
		modelStack.PushMatrix();
			modelStack.Rotate(90.f, 0.f, 0.f, 1.f);
			RenderMesh(meshList[GEO_TOP], false);
		modelStack.PopMatrix();
	modelStack.PopMatrix();
	modelStack.PushMatrix();
		///scale, translate, rotate 
		modelStack.Translate(0.f, -50.f, 0.f);
		modelStack.Scale(100.f, 100.f, 100.f);
		modelStack.Rotate(-90.f, 1.f, 0.f, 0.f);
		modelStack.PushMatrix();
		modelStack.Rotate(90.f, 0.f, 0.f, 1.f);
		RenderMesh(meshList[GEO_BOTTOM], false);
		modelStack.PopMatrix();
		modelStack.PopMatrix();
	modelStack.PushMatrix();
		///scale, translate, rotate 
		modelStack.Translate(0.f, 0.f, -50.f);
		modelStack.Scale(100.f, 100.f, 100.f);
		RenderMesh(meshList[GEO_FRONT], false);
	modelStack.PopMatrix();
	modelStack.PushMatrix();
		///scale, translate, rotate 
		modelStack.Translate(0.f, 0.f, 50.f);
		modelStack.Scale(100.f, 100.f, 100.f);
		modelStack.Rotate(180.f, 0.f, 1.f, 0.f);
		RenderMesh(meshList[GEO_BACK], false);
	modelStack.PopMatrix();
}

void SceneText::RenderCleanerRobot() // Facing x-axis
{
	modelStack.PushMatrix();
	modelStack.Rotate(180, 0, 1, 0);
	modelStack.Translate(0, 0.4, 0);
	RenderMesh(meshList[CLEANER_BOTTOM], true);
	
	modelStack.PushMatrix();
	modelStack.Rotate(rotateCleanerTop, 0, 1, 0);
	modelStack.Translate(0, 0.55, 0);
	RenderMesh(meshList[CLEANER_TOP], true);
	modelStack.PopMatrix();
	
	modelStack.PushMatrix();
	modelStack.Translate(0.65, 0.1, -0.8);
	modelStack.Rotate(rotateCleanerWheelsY, 0, 1, 0);
	modelStack.Rotate(rotateCleanerWheels, 0, 0, 1);
	RenderMesh(meshList[CLEANER_WHEELJOINT], true); // Front Left
	
	modelStack.PushMatrix();
	RenderMesh(meshList[CLEANER_WHEEL], true);
	modelStack.PopMatrix();
	
	modelStack.PopMatrix();
	
	modelStack.PushMatrix();
	modelStack.Translate(0.65, 0.1, 0.8);
	modelStack.Rotate(rotateCleanerWheelsY, 0, 1, 0);
	modelStack.Rotate(rotateCleanerWheels, 0, 0, 1);
	RenderMesh(meshList[CLEANER_WHEELJOINT], true); // Front Right
	
	modelStack.PushMatrix();
	RenderMesh(meshList[CLEANER_WHEEL], true);
	modelStack.PopMatrix();
	
	modelStack.PopMatrix();
	
	modelStack.PushMatrix();
	modelStack.Translate(-0.8, 0.1, -0.8);
	modelStack.Rotate(rotateCleanerWheelsY, 0, 1, 0);
	modelStack.Rotate(rotateCleanerWheels, 0, 0, 1);
	RenderMesh(meshList[CLEANER_WHEELJOINT], true); // Back Left
	
	modelStack.PushMatrix();
	RenderMesh(meshList[CLEANER_WHEEL], true);
	modelStack.PopMatrix();
	
	modelStack.PopMatrix();
	
	modelStack.PushMatrix();
	modelStack.Translate(-0.8, 0.1, 0.8);
	modelStack.Rotate(rotateCleanerWheelsY, 0, 1, 0);
	modelStack.Rotate(rotateCleanerWheels, 0, 0, 1);
	RenderMesh(meshList[CLEANER_WHEELJOINT], true); // Back Right
	
	modelStack.PushMatrix();
	RenderMesh(meshList[CLEANER_WHEEL], true);
	modelStack.PopMatrix();
	
	modelStack.PopMatrix();
	
	modelStack.PopMatrix();
}

void SceneText::RenderPlayer() // Facing x-axis
{
	std::cout << "X: " << movePlayerX << "\nY: " << movePlayerZ << "\n";
	modelStack.PushMatrix();
	modelStack.Translate(movePlayerX-2.5, -0.5f, movePlayerZ);
	modelStack.Scale(0.5, 0.4, 0.5);
	modelStack.Translate(5, 3.8, 0);
	modelStack.Rotate(rotatePlayer, 0, 1, 0);
	RenderMesh(meshList[PLAYER_BODY], true);

	modelStack.PushMatrix();
	modelStack.Translate(0, 6.5, 0);
	modelStack.Rotate(sin(rotatePlayerHead) * 20, 0, 1, 0);
	RenderMesh(meshList[PLAYER_HEAD], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0, 5.2, -1.5);
	modelStack.Rotate(15, 1, 0, 0);
	modelStack.Rotate(sin(rotatePlayerLeftArm) * 20, 0, 0, 1);
	RenderMesh(meshList[PLAYER_SHOULDER], true); // Left Arm

	modelStack.PushMatrix();
	modelStack.Translate(0, LeftArmY, LeftArmZ);
	RenderMesh(meshList[PLAYER_ARM], true);

	modelStack.PushMatrix();
	modelStack.Translate(0, -2.2, 0);
	RenderMesh(meshList[PLAYER_HAND], true);
	modelStack.PopMatrix();

	modelStack.PopMatrix();

	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0, 5.2, 1.5);
	modelStack.Rotate(-15, 1, 0, 0);
	modelStack.Rotate(sin(rotatePlayerRightArm) * 20, 0, 0, 1);
	RenderMesh(meshList[PLAYER_SHOULDER], true); // Right Arm

	modelStack.PushMatrix();
	modelStack.Translate(0, -1.8, 0);
	RenderMesh(meshList[PLAYER_ARM], true);

	modelStack.PushMatrix();
	modelStack.Translate(0, -2.2, 0);
	RenderMesh(meshList[PLAYER_HAND], true);

	modelStack.PopMatrix();

	modelStack.PopMatrix();

	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0, 0, -0.5);
	modelStack.Rotate(sin(rotatePlayerLeftLeg) * 20, 0, 0, 1);
	RenderMesh(meshList[PLAYER_LEG], true); // Left Leg
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0, 0, 0.5);
	modelStack.Rotate(sin(rotatePlayerRightLeg) * 20, 0, 0, 1);
	RenderMesh(meshList[PLAYER_LEG], true); // Right Leg
	modelStack.PopMatrix();

	modelStack.PopMatrix();
}

void SceneText::RenderCustomer() // Facing x-axis
{
	modelStack.PushMatrix();
	modelStack.Scale(0.5, 0.4, 0.5);
	modelStack.Translate(5, 3.8, 0);
	RenderMesh(meshList[CUSTOMER_BODY], true);
	
	modelStack.PushMatrix();
	modelStack.Translate(0, 6.5, 0);
	modelStack.Rotate(sin(rotateCustomerHead) * 20, 0, 1, 0);
	RenderMesh(meshList[CUSTOMER_HEAD], true);
	modelStack.PopMatrix();
	
	modelStack.PushMatrix();
	modelStack.Translate(0, 5.2, -1.5);
	modelStack.Rotate(15, 1, 0, 0);
	modelStack.Rotate(sin(rotateCustomerLeftArm) * 20, 0, 0, 1);
	RenderMesh(meshList[CUSTOMER_SHOULDER], true); // Left Arm
	
	modelStack.PushMatrix();
	modelStack.Translate(0, LeftArmY, LeftArmZ);
	RenderMesh(meshList[CUSTOMER_ARM], true);
	
	modelStack.PushMatrix();
	modelStack.Translate(0, -2.2, 0);
	RenderMesh(meshList[CUSTOMER_HAND], true);
	modelStack.PopMatrix();
	
	modelStack.PopMatrix();
	
	modelStack.PopMatrix();
	
	modelStack.PushMatrix();
	modelStack.Translate(0, 5.2, 1.5);
	modelStack.Rotate(-15, 1, 0, 0);
	modelStack.Rotate(sin(rotateCustomerRightArm) * 20, 0, 0, 1);
	RenderMesh(meshList[CUSTOMER_SHOULDER], true); // Right Arm
	
	modelStack.PushMatrix();
	modelStack.Translate(0, -1.8, 0);
	RenderMesh(meshList[CUSTOMER_ARM], true);
	
	modelStack.PushMatrix();
	modelStack.Translate(0, -2.2, 0);
	RenderMesh(meshList[CUSTOMER_HAND], true);
	
	modelStack.PopMatrix();
	
	modelStack.PopMatrix();
	
	modelStack.PopMatrix();
	
	modelStack.PushMatrix();
	modelStack.Translate(0, 0, -0.5);
	modelStack.Rotate(sin(rotateCustomerLeftLeg) * 20, 0, 0, 1);
	RenderMesh(meshList[CUSTOMER_LEG], true); // Left Leg
	modelStack.PopMatrix();
	
	modelStack.PushMatrix();
	modelStack.Translate(0, 0, 0.5);
	modelStack.Rotate(sin(rotateCustomerRightLeg) * 20, 0, 0, 1);
	RenderMesh(meshList[CUSTOMER_LEG], true); // Right Leg
	modelStack.PopMatrix();
	
	modelStack.PopMatrix();
}

void SceneText::RenderCustomer2() // Facing x-axis
{
	modelStack.PushMatrix();
	modelStack.Scale(0.5, 0.4, 0.5);
	modelStack.Translate(5, 3.8, 0);
	RenderMesh(meshList[CUSTOMER2_BODY], true);
	
	modelStack.PushMatrix();
	modelStack.Translate(0, 6.5, 0);
	modelStack.Rotate(sin(rotateCustomerHead) * 20, 0, 1, 0);
	RenderMesh(meshList[CUSTOMER2_HEAD], true);
	modelStack.PopMatrix();
	
	modelStack.PushMatrix();
	modelStack.Translate(0, 5.2, -1.5);
	modelStack.Rotate(15, 1, 0, 0);
	modelStack.Rotate(sin(rotateCustomerLeftArm) * 20, 0, 0, 1);
	RenderMesh(meshList[CUSTOMER2_SHOULDER], true); // Left Arm
	
	modelStack.PushMatrix();
	modelStack.Translate(0, LeftArmY, LeftArmZ);
	RenderMesh(meshList[CUSTOMER2_ARM], true);
	
	modelStack.PushMatrix();
	modelStack.Translate(0, -2.2, 0);
	RenderMesh(meshList[CUSTOMER2_HAND], true);
	modelStack.PopMatrix();
	
	modelStack.PopMatrix();
	
	modelStack.PopMatrix();
	
	modelStack.PushMatrix();
	modelStack.Translate(0, 5.2, 1.5);
	modelStack.Rotate(-15, 1, 0, 0);
	modelStack.Rotate(sin(rotateCustomerRightArm) * 20, 0, 0, 1);
	RenderMesh(meshList[CUSTOMER2_SHOULDER], true); // Right Arm
	
	modelStack.PushMatrix();
	modelStack.Translate(0, -1.8, 0);
	RenderMesh(meshList[CUSTOMER2_ARM], true);
	modelStack.PushMatrix();
	modelStack.Translate(0, -2.2, 0);
	RenderMesh(meshList[CUSTOMER2_HAND], true);
	modelStack.PopMatrix();
	
	modelStack.PopMatrix();
	
	modelStack.PopMatrix();
	
	modelStack.PushMatrix();
	modelStack.Translate(0, 0, -0.5);
	modelStack.Rotate(sin(rotateCustomerLeftLeg) * 20, 0, 0, 1);
	RenderMesh(meshList[CUSTOMER2_LEG], true); // Left Leg
	modelStack.PopMatrix();
	
	modelStack.PushMatrix();
	modelStack.Translate(0, 0, 0.5);
	modelStack.Rotate(sin(rotateCustomerRightLeg) * 20, 0, 0, 1);
	RenderMesh(meshList[CUSTOMER2_LEG], true); // Right Leg
	modelStack.PopMatrix();
	
	modelStack.PopMatrix();
}

void SceneText::RenderSuspect()
{
	modelStack.PushMatrix();
	
	if (escapeanimation == false)
	{
		modelStack.Translate(15, 1, 30);
	}
	modelStack.Scale(0.5, 0.4, 0.5);
	RenderMesh(meshList[CUSTOMER_BODY], true);

	modelStack.PushMatrix();
	modelStack.Translate(0, 6.5, 0);
	modelStack.Rotate(sin(rotateCustomerHead) * 20, 0, 1, 0);
	RenderMesh(meshList[CUSTOMER_HEAD], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0, 5.2, -1.5);
	modelStack.Rotate(15, 1, 0, 0);
	modelStack.Rotate(sin(rotateCustomerLeftArm) * 20, 0, 0, 1);
	RenderMesh(meshList[CUSTOMER_SHOULDER], true); // Left Arm

	modelStack.PushMatrix();
	modelStack.Translate(0, -1.8, 0);
	RenderMesh(meshList[CUSTOMER_ARM], true);

	modelStack.PushMatrix();
	modelStack.Translate(0, -2.2, 0);
	RenderMesh(meshList[CUSTOMER_HAND], true);
	modelStack.PopMatrix();

	modelStack.PopMatrix();

	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0, 5.2, 1.5);
	modelStack.Rotate(-15, 1, 0, 0);
	modelStack.Rotate(sin(rotateCustomerRightArm) * 20, 0, 0, 1);
	RenderMesh(meshList[CUSTOMER_SHOULDER], true); // Right Arm

	modelStack.PushMatrix();
	modelStack.Translate(0, -1.8, 0);
	RenderMesh(meshList[CUSTOMER_ARM], true);

	modelStack.PushMatrix();
	modelStack.Translate(0, -2.2, 0);
	RenderMesh(meshList[CUSTOMER_HAND], true);
	modelStack.PopMatrix();

	modelStack.PopMatrix();
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0, 0, -0.5);
	modelStack.Rotate(sin(rotateCustomerLeftLeg) * 20, 0, 0, 1);
	RenderMesh(meshList[CUSTOMER_LEG], true); // Left Leg
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0, 0, 0.5);
	modelStack.Rotate(sin(rotateCustomerRightLeg) * 20, 0, 0, 1);
	RenderMesh(meshList[CUSTOMER_LEG], true); // Right Leg
	modelStack.PopMatrix();

	modelStack.PopMatrix();
}

void SceneText::RenderSuspect2()
{
	modelStack.PushMatrix();

	if (secondescpaeanimation == false)
	{
		modelStack.Translate(15, 1, 0);
	}
	modelStack.Scale(0.5, 0.4, 0.5);
	RenderMesh(meshList[SUSPECT1_BODY], true);

	modelStack.PushMatrix();
	modelStack.Translate(0, 6.5, 0);
	modelStack.Rotate(sin(rotateCustomerHead) * 20, 0, 1, 0);
	RenderMesh(meshList[SUSPECT1_HEAD], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0, 5.2, -1.5);
	modelStack.Rotate(15, 1, 0, 0);
	modelStack.Rotate(sin(rotateCustomerLeftArm) * 20, 0, 0, 1);
	RenderMesh(meshList[SUSPECT1_SHOULDER], true); // Left Arm

	modelStack.PushMatrix();
	modelStack.Translate(0, LeftArmY, LeftArmZ);
	RenderMesh(meshList[SUSPECT1_ARM], true);

	modelStack.PushMatrix();
	modelStack.Translate(0, -2.2, 0);
	RenderMesh(meshList[SUSPECT1_HAND], true);
	modelStack.PopMatrix();

	modelStack.PopMatrix();

	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0, 5.2, 1.5);
	modelStack.Rotate(-15, 1, 0, 0);
	modelStack.Rotate(sin(rotateCustomerRightArm) * 20, 0, 0, 1);
	RenderMesh(meshList[SUSPECT1_SHOULDER], true); // Right Arm

	modelStack.PushMatrix();
	modelStack.Translate(0, -1.8, 0);
	RenderMesh(meshList[SUSPECT1_ARM], true);
	modelStack.PushMatrix();
	modelStack.Translate(0, -2.2, 0);
	RenderMesh(meshList[SUSPECT1_HAND], true);
	modelStack.PopMatrix();

	modelStack.PopMatrix();

	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0, 0, -0.5);
	modelStack.Rotate(sin(rotateCustomerLeftLeg) * 20, 0, 0, 1);
	RenderMesh(meshList[SUSPECT1_LEG], true); // Left Leg
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0, 0, 0.5);
	modelStack.Rotate(sin(rotateCustomerRightLeg) * 20, 0, 0, 1);
	RenderMesh(meshList[SUSPECT1_LEG], true); // Right Leg
	modelStack.PopMatrix();

	modelStack.PopMatrix();
}

void SceneText::RenderSuspect3()
{
	modelStack.PushMatrix();
	
	if (thirdescapeanimation == false)
	{
		modelStack.Translate(-20, 1, 15);
	}
	modelStack.Scale(0.5, 0.4, 0.5);
	RenderMesh(meshList[SUSPECT2_BODY], true);

	modelStack.PushMatrix();
	modelStack.Translate(0, 6.5, 0);
	modelStack.Rotate(sin(rotateCustomerHead) * 20, 0, 1, 0);
	RenderMesh(meshList[SUSPECT2_HEAD], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0, 5.2, -1.5);
	modelStack.Rotate(15, 1, 0, 0);
	modelStack.Rotate(sin(rotateCustomerLeftArm) * 20, 0, 0, 1);
	RenderMesh(meshList[SUSPECT2_SHOULDER], true); // Left Arm

	modelStack.PushMatrix();
	modelStack.Translate(0, LeftArmY, LeftArmZ);
	RenderMesh(meshList[SUSPECT2_ARM], true);

	modelStack.PushMatrix();
	modelStack.Translate(0, -2.2, 0);
	RenderMesh(meshList[SUSPECT2_HAND], true);
	modelStack.PopMatrix();

	modelStack.PopMatrix();

	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0, 5.2, 1.5);
	modelStack.Rotate(-15, 1, 0, 0);
	modelStack.Rotate(sin(rotateCustomerRightArm) * 20, 0, 0, 1);
	RenderMesh(meshList[SUSPECT2_SHOULDER], true); // Right Arm

	modelStack.PushMatrix();
	modelStack.Translate(0, -1.8, 0);
	RenderMesh(meshList[SUSPECT2_ARM], true);
	modelStack.PushMatrix();
	modelStack.Translate(0, -2.2, 0);
	RenderMesh(meshList[SUSPECT2_HAND], true);
	modelStack.PopMatrix();

	modelStack.PopMatrix();

	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0, 0, -0.5);
	modelStack.Rotate(sin(rotateCustomerLeftLeg) * 20, 0, 0, 1);
	RenderMesh(meshList[SUSPECT2_LEG], true); // Left Leg
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0, 0, 0.5);
	modelStack.Rotate(sin(rotateCustomerRightLeg) * 20, 0, 0, 1);
	RenderMesh(meshList[SUSPECT2_LEG], true); // Right Leg
	modelStack.PopMatrix();
	
	modelStack.PopMatrix();
}

void SceneText::RenderGuardBot() // Facing z-axis
{
	modelStack.PushMatrix();
	modelStack.Translate(15, 9, 0);
	RenderMesh(meshList[GUARD_BODY], true);
	
	modelStack.PushMatrix();
	modelStack.Translate(0, 3, 0);
	RenderMesh(meshList[GUARD_HEAD], true);
	modelStack.PopMatrix();
	
	modelStack.PushMatrix();
	modelStack.Translate(4.5, 2.5, 0);
	RenderMesh(meshList[GUARD_LEFTARM], true);
	modelStack.PopMatrix();
	
	modelStack.PushMatrix();
	modelStack.Translate(-4.5, 2.5, 0);
	RenderMesh(meshList[GUARD_RIGHTARM], true);
	modelStack.PopMatrix();
	
	modelStack.PushMatrix();
	modelStack.Translate(1.3, -3, 0);
	RenderMesh(meshList[GUARD_LEG], true); // Left Leg
	modelStack.PopMatrix();
	
	modelStack.PushMatrix();
	modelStack.Translate(-1.3, -3, 0);
	RenderMesh(meshList[GUARD_LEG], true); // Right Leg
	modelStack.PopMatrix();
	
	modelStack.PopMatrix();
}

void SceneText::RenderSaleBot() // Facing negative z-axis
{
	modelStack.PushMatrix();
	modelStack.Translate(-7, 0.7, 0);
	RenderMesh(meshList[SALE_BOTTOM], true);
	
	modelStack.PushMatrix();
	modelStack.Translate(0, 3, 0);
	RenderMesh(meshList[SALE_TOP], true);
	
	modelStack.PushMatrix();
	modelStack.Translate(0, 4.5, 0);
	RenderMesh(meshList[SALE_NECK], true);
	
	modelStack.PushMatrix();
	modelStack.Translate(0, 0.5, 0);
	RenderMesh(meshList[SALE_HEAD], true);
	modelStack.PopMatrix();
	
	modelStack.PopMatrix();
	
	modelStack.PushMatrix();
	modelStack.Translate(-1.5, 3, 0);
	modelStack.Rotate(-15, 0, 0, 1);
	RenderMesh(meshList[SALE_LEFTARM], true);
	modelStack.PopMatrix();
	
	modelStack.PushMatrix();
	modelStack.Translate(1.5, 3, 0);
	modelStack.Rotate(15, 0, 0, 1);
	RenderMesh(meshList[SALE_RIGHTARM], true);
	modelStack.PopMatrix();
	
	modelStack.PopMatrix();
	
	modelStack.PushMatrix();
	modelStack.Translate(0, -0.2, -1.1);
	RenderMesh(meshList[SALE_WHEEL], true); // First Horizontal Wheel
	modelStack.PopMatrix();
	
	modelStack.PushMatrix();
	modelStack.Translate(0, -0.2, 1.1);
	RenderMesh(meshList[SALE_WHEEL], true); // Second Horizontal Wheel
	modelStack.PopMatrix();
	
	modelStack.PushMatrix();
	modelStack.Rotate(90, 0, 1, 0);
	modelStack.Translate(0, -0.2, -1);
	RenderMesh(meshList[SALE_WHEEL], true); // First Vertical Wheel
	modelStack.PopMatrix();
	
	modelStack.PushMatrix();
	modelStack.Rotate(90, 0, 1, 0);
	modelStack.Translate(0, -0.2, 0);
	RenderMesh(meshList[SALE_WHEEL], true); // Second Vertical Wheel
	modelStack.PopMatrix();
	
	modelStack.PushMatrix();
	modelStack.Rotate(90, 0, 1, 0);
	modelStack.Translate(0, -0.2, 1);
	RenderMesh(meshList[SALE_WHEEL], true); // Third Vertical Wheel
	modelStack.PopMatrix();
	
	modelStack.PopMatrix();
}

void SceneText::Renderlevel()
{
	modelStack.PushMatrix();
	modelStack.Translate(5, 0, 30);
	modelStack.Scale(4, 4, 4);
	modelStack.Rotate(rotatecar, 0, 1, 0);
	RenderMesh(meshList[ITEM_DISPLAY_STAND], true);
	modelStack.PushMatrix();
	modelStack.Translate(-0.2, 0.1, 0);
	modelStack.Scale(0.1, 0.1, 0.1);
	RenderCar10();
	modelStack.PopMatrix();
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(1, 0.5, 35);
	modelStack.Rotate(90, 0, 1, 0);
	RenderMesh(meshList[ITEM_INFORMATION_STAND], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-40, 1, 10);
	modelStack.Scale(3, 3, 3);
	modelStack.Rotate(90, 0, 1, 0);
	RenderMesh(meshList[ITEM_BOOTH], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-40, 1, 0);
	modelStack.Scale(3, 3, 3);
	modelStack.Rotate(90, 0, 1, 0);
	RenderMesh(meshList[ITEM_FAE_BOOTH], true);
	modelStack.PopMatrix();
	
	modelStack.PushMatrix();
	modelStack.Translate(-40, 1, 30);
	modelStack.Scale(3, 3, 3);
	modelStack.Rotate(90, 0, 1, 0);
	RenderMesh(meshList[ITEM_BOOTH_3], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-40, 1, 20);
	modelStack.Scale(3, 3, 3);
	modelStack.Rotate(90, 0, 1, 0);
	RenderMesh(meshList[ITEM_BOOTH_4], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-40, 1, -10);
	modelStack.Scale(3, 3, 3);
	modelStack.Rotate(90, 0, 1, 0);
	RenderMesh(meshList[ITEM_BOOTH_5], true);
	modelStack.PopMatrix();
	
	modelStack.PushMatrix();
	modelStack.Translate(-10, 0, 30);
	modelStack.Scale(4, 8, 4);
	RenderMesh(meshList[ITEM_INFORMATION_COUNTER], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(40, 1, 20);
	RenderMesh(meshList[ITEM_BENCH], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(40, 1, 0);
	RenderMesh(meshList[ITEM_BENCH], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(40, 1, -20);
	RenderMesh(meshList[ITEM_BENCH], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(40, 1, -40);
	RenderMesh(meshList[ITEM_BENCH], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0, -1, 0);
	modelStack.Scale(100, 1, 100);
	RenderMesh(meshList[ITEM_FLOOR], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(5, 0.5, 20);
	modelStack.Rotate(60, 0, 1, 0);
	RenderMesh(meshList[ITEM_INFORMATION_STAND], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-5, 0.5, 20);
	modelStack.Rotate(150, 0, 1, 0);
	RenderMesh(meshList[ITEM_INFORMATION_STAND], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(5, 0.5, 10);
	modelStack.Rotate(60, 0, 1, 0);
	RenderMesh(meshList[ITEM_INFORMATION_STAND], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-5, 0.5, 10);
	modelStack.Rotate(150, 0, 1, 0);
	RenderMesh(meshList[ITEM_INFORMATION_STAND], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(5, 0.5, 0);
	modelStack.Rotate(60, 0, 1, 0);
	RenderMesh(meshList[ITEM_INFORMATION_STAND], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-5, 0.5, 0);
	modelStack.Rotate(150, 0, 1, 0);
	RenderMesh(meshList[ITEM_INFORMATION_STAND], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(5, 0.5, -10);
	modelStack.Rotate(60, 0, 1, 0);
	RenderMesh(meshList[ITEM_INFORMATION_STAND], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-5, 0.5, -10);
	modelStack.Rotate(150, 0, 1, 0);
	RenderMesh(meshList[ITEM_INFORMATION_STAND], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(5, 0.5, -20);
	modelStack.Rotate(60, 0, 1, 0);
	RenderMesh(meshList[ITEM_INFORMATION_STAND], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-5, 0.5, -20);
	modelStack.Rotate(150, 0, 1, 0);
	RenderMesh(meshList[ITEM_INFORMATION_STAND], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(5, 0.5, -30);
	modelStack.Rotate(60, 0, 1, 0);
	RenderMesh(meshList[ITEM_INFORMATION_STAND], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-5, 0.5, -30);
	modelStack.Rotate(150, 0, 1, 0);
	RenderMesh(meshList[ITEM_INFORMATION_STAND], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(45, 0.5, 45);
	//modelStack.Rotate(150, 0, 1, 0);
	RenderMesh(meshList[ITEM_INFORMATION_STAND], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	//Scale, Translate, Rotate
	modelStack.Translate(45, 5, 40);
	modelStack.Rotate(-90, 0, 1, 0);
	RenderText(meshList[GEO_TEXT], "Mechanic Bay", Color(0, 1, 0));
	modelStack.PopMatrix();

	if (dayData->getDay() == 1) {
		for (int i = 0; i < Day1[3].maxNumber - Day1[3].currentNumber; i++) {
			modelStack.PushMatrix();
			modelStack.Translate(44, 0, 40 - (i * 2));
			RenderCleanerRobot();
			modelStack.PopMatrix();
		}
	}
	else if (dayData->getDay() == 2) {
		for (int i = 0; i < Day2[3].maxNumber - Day2[3].currentNumber; i++) {
			modelStack.PushMatrix();
			modelStack.Translate(44, 0, 40 - (i * 2));
			RenderCleanerRobot();
			modelStack.PopMatrix();
		}
	}
	else if (dayData->getDay() == 3) {
		for (int i = 0; i < Day3[3].maxNumber - Day3[3].currentNumber; i++) {
			modelStack.PushMatrix();
			modelStack.Translate(44, 0, 40 - (i * 2));
			RenderCleanerRobot();
			modelStack.PopMatrix();
		}
	}

	if ((distancecalculator(Vector3(movePlayerX, 0, movePlayerZ), Vector3(45, 0.5, 45)) < 10)) {
		modelStack.PushMatrix();
		//Scale, Translate, Rotate
		modelStack.Translate(45, 3, 38);
		modelStack.Rotate(-90, 0, 1, 0);
		RenderText(meshList[GEO_TEXT], "Press I to play", Color(0, 1, 0));
		modelStack.PopMatrix();
		modelStack.PushMatrix();
		//Scale, Translate, Rotate
		modelStack.Translate(45, 2, 38);
		modelStack.Rotate(-90, 0, 1, 0);
		RenderText(meshList[GEO_TEXT], "the Mechanic Game", Color(0, 1, 0));
		modelStack.PopMatrix();
	}

	modelStack.PushMatrix();
	modelStack.Translate(-7, 0.5, 20);
	modelStack.Rotate(330, 0, 1, 0);
	RenderCar1();
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(12, 0, 20);
	modelStack.Rotate(60, 0, 1, 0);
	RenderCar2();
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-11, 0, 8);
	modelStack.Rotate(330, 0, 1, 0);
	RenderCar3();
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(13, -2, 4);
	modelStack.Rotate(60, 0, 1, 0);
	RenderCar4();
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-11, 2, 0);
	modelStack.Rotate(330, 0, 1, 0);
	modelStack.Scale(1.5, 1.5, 1.5);
	RenderCar5();
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(11, 0.5, -11);
	modelStack.Rotate(60, 0, 1, 0);
	modelStack.Scale(1.2, 1.2, 1.2);
	RenderCar6();
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-9, 0, -11);
	modelStack.Rotate(330, 0, 1, 0);
	RenderCar7();
	modelStack.PopMatrix();

	/*modelStack.PushMatrix();
	modelStack.Translate(11, 0.5, -21);
	modelStack.Rotate(60, 0, 1, 0);
	modelStack.Scale(1.2, 1.2, 1.2);
	RenderCar8();
	modelStack.PopMatrix();*/

	modelStack.PushMatrix();
	modelStack.Translate(-15, -0.5, -23);
	modelStack.Rotate(150, 0, 1, 0);
	RenderCar9();
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-12, 0, -33);
	modelStack.Rotate(60, 0, 1, 0);
	RenderCar11();
	modelStack.PopMatrix();
}

void SceneText::RenderLitter()
{
	modelStack.PushMatrix();;
	RenderMesh(meshList[GEO_LITTER], true);
	modelStack.PopMatrix();

}

void SceneText::RenderCar1()
{
	modelStack.PushMatrix();
	modelStack.Translate(-5, 0, 0);
	RenderMesh(meshList[CAR1_BODY], true);
	
		modelStack.PushMatrix();
		modelStack.Translate(2, 1, 0);
		modelStack.Rotate(-20, 0, 0, 1);
		RenderMesh(meshList[CAR1_STEERINGWHEEL], true);
		modelStack.PopMatrix();
	
		modelStack.PushMatrix();	// Right Front Wheel
		modelStack.Translate(3.85, -0.5, 3);
		RenderMesh(meshList[CAR1_WHEEL], true);
		modelStack.PopMatrix();
	
		modelStack.PushMatrix();	// Left Front Wheel
		modelStack.Translate(3.85, -0.5, -3);
		RenderMesh(meshList[CAR1_WHEEL], true);
		modelStack.PopMatrix();
	
		modelStack.PushMatrix();	// Right Back Wheel
		modelStack.Translate(-4, -0.5, 3);
		RenderMesh(meshList[CAR1_WHEEL], true);
		modelStack.PopMatrix();
	
		modelStack.PushMatrix();	// Left Back Wheel
		modelStack.Translate(-4, -0.5, -3);
		RenderMesh(meshList[CAR1_WHEEL], true);
		modelStack.PopMatrix();
	
	modelStack.PopMatrix();
}

void SceneText::RenderCar2()
{
	modelStack.PushMatrix();
	RenderMesh(meshList[CAR2_BODY], true);
	modelStack.PopMatrix();
}

void SceneText::RenderCar3()
{
	modelStack.PushMatrix();
	RenderMesh(meshList[CAR3_BODY], true);
	modelStack.PopMatrix();
}

void SceneText::RenderCar4()
{
	modelStack.PushMatrix();
	RenderMesh(meshList[CAR4_BODY], true);
	
		modelStack.PushMatrix();
		modelStack.Translate(-4, 2.5, 5);
		RenderMesh(meshList[CAR4_LEFTWHEEL], true); // Front Wheel
		modelStack.PopMatrix();	
	
		modelStack.PushMatrix();
		modelStack.Translate(-4, 2.5, -4);
		RenderMesh(meshList[CAR4_RIGHTWHEEL], true); // Front Wheel
		modelStack.PopMatrix();
	
		modelStack.PushMatrix();
		modelStack.Translate(3.5, 2.5, 4.5);
		RenderMesh(meshList[CAR4_LEFTWHEEL], true); // Back Wheel
		modelStack.PopMatrix();
	
		modelStack.PushMatrix();
		modelStack.Translate(3.5, 2.5, -4.5);
		RenderMesh(meshList[CAR4_RIGHTWHEEL], true); // Back Wheel
		modelStack.PopMatrix();
	
	modelStack.PopMatrix();
}

void SceneText::RenderCar5()
{
	modelStack.PushMatrix();
	RenderMesh(meshList[CAR5_BODY], true);
	
		modelStack.PushMatrix();
		modelStack.Translate(0.1, -0.7, 2);
		RenderMesh(meshList[CAR5_WHEEL], true); // Left Front Wheel
		modelStack.PopMatrix();
	
		modelStack.PushMatrix();
		modelStack.Translate(0.1, -0.7, -2);
		RenderMesh(meshList[CAR5_WHEEL], true); // Right Front Wheel
		modelStack.PopMatrix();
	
		modelStack.PushMatrix();
		modelStack.Translate(3.8, -0.7, 2);
		RenderMesh(meshList[CAR5_WHEEL], true); // Left Back Wheel
		modelStack.PopMatrix();
	
		modelStack.PushMatrix();
		modelStack.Translate(3.8, -0.7, -2);
		RenderMesh(meshList[CAR5_WHEEL], true); // Right Back Wheel
		modelStack.PopMatrix();
	
	modelStack.PopMatrix();
}

void SceneText::RenderCar6()
{
	modelStack.PushMatrix();
	modelStack.Translate(-5, 0, 0);
	RenderMesh(meshList[CAR6_BODY], true);
	
		modelStack.PushMatrix();
		modelStack.Translate(-1.2, 0, 2);
		RenderMesh(meshList[CAR6_LEFTWHEEL], true); // Front Wheel
		modelStack.PopMatrix();
	
		modelStack.PushMatrix();
		modelStack.Translate(-1.2, 0, -2);
		RenderMesh(meshList[CAR6_RIGHTWHEEL], true);
		modelStack.PopMatrix();
	
		modelStack.PushMatrix();
		modelStack.Translate(3.7, 0, 2);
		RenderMesh(meshList[CAR6_LEFTWHEEL], true); // Back Wheel
		modelStack.PopMatrix();
	
		modelStack.PushMatrix();
		modelStack.Translate(3.7, 0, -2);
		RenderMesh(meshList[CAR6_RIGHTWHEEL], true);
		modelStack.PopMatrix();
	
	modelStack.PopMatrix();
}

void SceneText::RenderCar7()
{
	modelStack.PushMatrix();
	modelStack.Translate(-5, 0, 0);
	RenderMesh(meshList[CAR7_BODY], true);
	
		modelStack.PushMatrix();
		modelStack.Rotate(15, 0, 0, 1);
		modelStack.Translate(3, 2.7, 0);
		RenderMesh(meshList[CAR7_STEERINGWHEEL], true);
		modelStack.PopMatrix();
	
		modelStack.PushMatrix();
		modelStack.Rotate(15, 0, 0, 1);
		modelStack.Translate(3.2, 0, 0);
		RenderMesh(meshList[CAR7_FRONTWHEEL], true);
		modelStack.PopMatrix();
	
		modelStack.PushMatrix();
		modelStack.Rotate(-35, 0, 0, 1);
		modelStack.Translate(-2.7, -0.7, 0);
		RenderMesh(meshList[CAR7_BACKWHEEL], true);
		modelStack.PopMatrix();
	
	modelStack.PopMatrix();
}

void SceneText::RenderCar8()
{
	modelStack.PushMatrix();
	RenderMesh(meshList[CAR8_BODY], true);
		modelStack.PushMatrix();
		RenderMesh(meshList[CAR8_STEERINGWHEEL], true);
		modelStack.PopMatrix();
	
		modelStack.PushMatrix();
		RenderMesh(meshList[CAR8_WHEEL], true); // Left Front Wheel
		modelStack.PopMatrix();
	
		modelStack.PushMatrix();
		RenderMesh(meshList[CAR8_WHEEL], true); // Right Front Wheel
		modelStack.PopMatrix();
	
		modelStack.PushMatrix();
		RenderMesh(meshList[CAR8_WHEEL], true); // Left Back Wheel
		modelStack.PopMatrix();
	
		modelStack.PushMatrix();
		RenderMesh(meshList[CAR8_WHEEL], true); // Right Back Wheel
		modelStack.PopMatrix();
	
	modelStack.PopMatrix();
}

void SceneText::RenderCar9()
{
	modelStack.PushMatrix();
	RenderMesh(meshList[CAR9_BODY], true);
		modelStack.PushMatrix();
		modelStack.Translate(0, 1, 4);
		RenderMesh(meshList[CAR9_WHEEL], true); // Left Front Wheel
		modelStack.PopMatrix();
	
		modelStack.PushMatrix();
		modelStack.Translate(0, 1, -4);
		RenderMesh(meshList[CAR9_WHEEL], true); // Right Front Wheel
		modelStack.PopMatrix();
	
		modelStack.PushMatrix();
		modelStack.Translate(5.5, 1, 4);
		RenderMesh(meshList[CAR9_WHEEL], true); // Left Back Wheel
		modelStack.PopMatrix();
	
		modelStack.PushMatrix();
		modelStack.Translate(5.5, 1, -4);
		RenderMesh(meshList[CAR9_WHEEL], true); // Right Back Wheel
		modelStack.PopMatrix();
	
	modelStack.PopMatrix();
}

void SceneText::RenderCar10()
{
	modelStack.PushMatrix();
	RenderMesh(meshList[CAR10_BODY], true);
	modelStack.PopMatrix();
}

void SceneText::RenderCar11()
{
	modelStack.PushMatrix();
	RenderMesh(meshList[CAR11_BODY], true);
	modelStack.PopMatrix();
}

void SceneText::RenderBouncerTextBox()
{
	RenderObjectOnScreen(meshList[GEO_TEXTBOX], 10, 3.3f, 1.f);
	RenderTextOnScreen(meshList[GEO_TEXT], "I left okay geez!", Color(1, 0, 0), 3.5f, 1.f, 3);
}

void SceneText::RenderText(Mesh* mesh, std::string text, Color color)
{
	if (!mesh || mesh->textureID <= 0) //Proper error check
		return;

	glDisable(GL_DEPTH_TEST);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 1);
	glUniform3fv(m_parameters[U_TEXT_COLOR], 1, &color.r);
	glUniform1i(m_parameters[U_LIGHTENABLED], 0);
	glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mesh->textureID);
	glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);
	for (unsigned i = 0; i < text.length(); ++i)
	{
		Mtx44 characterSpacing;
		characterSpacing.SetToTranslation(i * 1.f, 0, 0); //1.0f is the spacing of each character, you may change this value
		Mtx44 MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top() * characterSpacing;
		glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);

		mesh->Render((unsigned)text[i] * 6, 6);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 0);
	glEnable(GL_DEPTH_TEST);

}

void SceneText::RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y)
{
	if (!mesh || mesh->textureID <= 0) //Proper error check
		return;

	glDisable(GL_DEPTH_TEST);

	Mtx44 ortho;
	ortho.SetToOrtho(0, 80, 0, 60, -10, 10); //size of screen UI
	projectionStack.PushMatrix();
	projectionStack.LoadMatrix(ortho);
	viewStack.PushMatrix();
	viewStack.LoadIdentity(); //No need camera for ortho mode
	modelStack.PushMatrix();
	modelStack.LoadIdentity(); //Reset modelStack
	modelStack.Scale(size, size, size);
	modelStack.Translate(x, y, 0);

	glUniform1i(m_parameters[U_TEXT_ENABLED], 1);
	glUniform3fv(m_parameters[U_TEXT_COLOR], 1, &color.r);
	glUniform1i(m_parameters[U_LIGHTENABLED], 0);
	glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mesh->textureID);
	glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);
	for (unsigned i = 0; i < text.length(); ++i)
	{
		Mtx44 characterSpacing;
		characterSpacing.SetToTranslation(i * 0.7f, 0, 0); //1.0f is the spacing of each character, you may change this value
		Mtx44 MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top() * characterSpacing;
		glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);

		mesh->Render((unsigned)text[i] * 6, 6);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 0);
	projectionStack.PopMatrix();
	viewStack.PopMatrix();
	modelStack.PopMatrix();

	glEnable(GL_DEPTH_TEST);
}

void SceneText::RenderTextOnScreenWithNewLine(Mesh* mesh, std::string text, Color color, float size, float x, float y, int character, bool spacing)
{
	std::string additionalSpacing = "";
	if (spacing)
		additionalSpacing = "   ";
	std::string toPrint = text;
	int lastSpacing = 0;
	int length = toPrint.length();
	for (int i = 0; length > 0; i++)
	{
		if (toPrint[0] == ' ' && toPrint[1] != ' ' && i != 0) // toPrint[1] != ' ' is to check if the spacing was intended
			toPrint.erase(0, 1);			// remove the spacing if it's the first char of line
		length = toPrint.length();
		if (length < character)
		{
			if (i != 0)
				RenderTextOnScreen(meshList[GEO_TEXT], additionalSpacing + toPrint.substr(0, length), color, size, x, y - i);
			else
				RenderTextOnScreen(meshList[GEO_TEXT], toPrint.substr(0, length), color, size, x, y - i);
			break;
		}
		else if (toPrint[character - 1] != ' ' && toPrint[character] != ' ') // means the word may be cut off if nothing is done
		{
			for (int j = 0; j <= character; j++)
			{
				if (toPrint[j] == ' ')
					lastSpacing = j;
			}
			if (i != 0)
				RenderTextOnScreen(meshList[GEO_TEXT], additionalSpacing + toPrint.substr(0, lastSpacing), color, size, x, y - i);
			else
				RenderTextOnScreen(meshList[GEO_TEXT], toPrint.substr(0, lastSpacing), color, size, x, y - i);
			toPrint.erase(0, lastSpacing);
		}
		else
		{
			if (i != 0)
				RenderTextOnScreen(meshList[GEO_TEXT], additionalSpacing + toPrint.substr(0, character), color, size, x, y - i);
			else
				RenderTextOnScreen(meshList[GEO_TEXT], toPrint.substr(0, character), color, size, x, y - i);
			toPrint.erase(0, character);
		}
	}
}

void SceneText::RenderObjectOnScreen(Mesh* mesh, float size, float x, float y)
{
	if (!mesh || mesh->textureID <= 0) //Proper error check
		return;

	glDisable(GL_DEPTH_TEST);

	Mtx44 ortho;
	ortho.SetToOrtho(0, 80, 0, 60, -10, 10); //size of screen UI
	projectionStack.PushMatrix();
	projectionStack.LoadMatrix(ortho);
	viewStack.PushMatrix();
	viewStack.LoadIdentity(); //No need camera for ortho mode
	modelStack.PushMatrix();
	modelStack.LoadIdentity(); //Reset modelStack
	modelStack.Scale(size, size, size);
	modelStack.Translate(x, y, 0);
	glUniform1i(m_parameters[U_LIGHTENABLED], 0);
	glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mesh->textureID);
	glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);

	Mtx44 MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top();
	glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);
	mesh->Render();
	glBindTexture(GL_TEXTURE_2D, 0);
	projectionStack.PopMatrix();
	viewStack.PopMatrix();
	modelStack.PopMatrix();


	glEnable(GL_DEPTH_TEST);
}

void SceneText::CalculateFrameRate()
{
	static float framesPerSecond = 0.0f;
	static int fps;
	static float lastTime = 0.0f;
	float currentTime = GetTickCount() * 0.001f;
	++framesPerSecond;
	printf("Current Frames Per Second: %d\n\n", fps);
	if (currentTime - lastTime > 1.0f)
	{
		lastTime = currentTime;
		fps = (int)framesPerSecond;
		framesPerSecond = 0;
	}
}
