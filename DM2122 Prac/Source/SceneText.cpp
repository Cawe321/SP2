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


#define ROT_LIMIT 45.f;
#define SCALE_LIMIT 5.f;
#define LSPEED 10.f

SceneText::SceneText()
{
	for (int i = 0; i < NUM_GEOMETRY; ++i)
	{
		meshList[i] = NULL;
	}
	Day1 = Vocation::getMainQuest(1);
	Day2 = Vocation::getMainQuest(2);
	Day3 = Vocation::getMainQuest(3);
	BossOpinion = new Boss();
	MechanicGameScore = new Mechanictask();
	MechanicGame = false;
	FreezeMovement = false;
	hasmissed = false;

	CleanerGame = true;
	srand(20);
	randomLitter = -20 + rand() % 21;

	for (int i = 0; i < 10; i++) {
		game[i] = '-';
	}
	
	Price = 0; //for me to render out the achievements
	
	escapeanimation = false;

	AchievementScene = false;

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
	m_parameters[U_LIGHT0_POSITION] = glGetUniformLocation(m_programID, "lights[0].position_cameraspace");
	m_parameters[U_LIGHT0_COLOR] = glGetUniformLocation(m_programID, "lights[0].color");
	m_parameters[U_LIGHT0_POWER] = glGetUniformLocation(m_programID, "lights[0].power");
	m_parameters[U_LIGHT0_KC] = glGetUniformLocation(m_programID, "lights[0].kC");
	m_parameters[U_LIGHT0_KL] = glGetUniformLocation(m_programID, "lights[0].kL");
	m_parameters[U_LIGHT0_KQ] = glGetUniformLocation(m_programID, "lights[0].kQ");
	m_parameters[U_LIGHTENABLED] = glGetUniformLocation(m_programID, "lightEnabled");
	m_parameters[U_NUMLIGHTS] = glGetUniformLocation(m_programID, "numLights");
	m_parameters[U_LIGHT0_TYPE] = glGetUniformLocation(m_programID, "lights[0].type");
	m_parameters[U_LIGHT0_SPOTDIRECTION] = glGetUniformLocation(m_programID, "lights[0].spotDirection");
	m_parameters[U_LIGHT0_COSCUTOFF] = glGetUniformLocation(m_programID, "lights[0].cosCutoff");
	m_parameters[U_LIGHT0_COSINNER] = glGetUniformLocation(m_programID, "lights[0].cosInner");
	m_parameters[U_LIGHT0_EXPONENT] = glGetUniformLocation(m_programID, "lights[0].exponent");

	//Get a handle for our "colorTexture" uniform
	m_parameters[U_COLOR_TEXTURE_ENABLED] = glGetUniformLocation(m_programID, "colorTextureEnabled");
	m_parameters[U_COLOR_TEXTURE] = glGetUniformLocation(m_programID, "colorTexture");
	// Get a handle for our "textColor" uniform
	m_parameters[U_TEXT_ENABLED] = glGetUniformLocation(m_programID, "textEnabled");
	m_parameters[U_TEXT_COLOR] = glGetUniformLocation(m_programID, "textColor");


	glUseProgram(m_programID);
	// Enable depth test
	glEnable(GL_DEPTH_TEST);

	light[0].type = Light::LIGHT_POINT;
	light[0].position.Set(0, 5, 0);
	light[0].color.Set(0.5f, 0.5f, 0.5f);
	light[0].power = 1;
	light[0].kC = 1.f;
	light[0].kL = 0.01f;
	light[0].kQ = 0.001f;
	light[0].cosCutoff = cos(Math::DegreeToRadian(45));
	light[0].cosInner = cos(Math::DegreeToRadian(30));
	light[0].exponent = 3.f;
	light[0].spotDirection.Set(0.f, 1.f, 0.f);

	glUniform1i(m_parameters[U_LIGHT0_TYPE], light[0].type);
	glUniform3fv(m_parameters[U_LIGHT0_COLOR], 1, &light[0].color.r);
	glUniform1f(m_parameters[U_LIGHT0_POWER], light[0].power);
	glUniform1f(m_parameters[U_LIGHT0_KC], light[0].kC);
	glUniform1f(m_parameters[U_LIGHT0_KL], light[0].kL);
	glUniform1f(m_parameters[U_LIGHT0_KQ], light[0].kQ);
	glUniform3fv(m_parameters[U_LIGHT0_SPOTDIRECTION], 1, &light[0].spotDirection.x);
	glUniform1f(m_parameters[U_LIGHT0_COSCUTOFF], light[0].cosCutoff);
	glUniform1f(m_parameters[U_LIGHT0_COSINNER], light[0].cosInner);
	glUniform1f(m_parameters[U_LIGHT0_EXPONENT], light[0].exponent);
	glUniform1i(m_parameters[U_NUMLIGHTS], 1); 
	
	
	dialogueTime = 0;
	elapsedTime = 0;

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
	
	meshList[ITEM_BENCH] = MeshBuilder::GenerateOBJ("Bench", "OBJ//Background_Items//Bench.obj");
	meshList[ITEM_BENCH]->textureID = LoadTGA("Image//background_items//Bench.tga");

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
	
	// init values
	salesCustomer =  nullptr; //new CSalesCustomer(Vector3(0,0,0))
}

void SceneText::Update(double dt)
{
	// Animations for models
	if (rotateCleanerTop > -30.f)
		rotateCleanerTop -= (float)(RSPEED * dt);
	if (rotateCleanerTop < -30.f)					// does not work
		rotateCleanerTop += (float)(RSPEED * dt);

	rotateCleanerTop += (float)(RSPEED * dt);
	rotateCleanerWheels -= (float)(RSPEED * dt);
	rotateCustomerRightArm -= (float)(RSPEED * dt);
	if (LeftArmY < 5.0f && LeftArmZ < 2.0f)
		rotateCustomerLeftArm += (float)(RSPEED * dt);

	rotateCustomerHead = 45;
	rotateCustomerLeftLeg = 25;
	rotateCustomerRightLeg = -25;
	
	//for bouncer
	if (escapeanimation == true)
	{
		
		elapsedTime += dt;
	}

	if (MechanicGame == true) {
		// to make the keypress game
		elapsed += dt;
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
		if (Application::IsKeyPressed(VK_LEFT))
			rotateCleanerTop += (float)(RSPEED * dt);
		if (Application::IsKeyPressed(VK_RIGHT))
			rotateCleanerTop -= (float)(RSPEED * dt);
		if (Application::IsKeyPressed('W'))
			rotateCleanerWheelsForward += (float)(RSPEED * dt);
		if (Application::IsKeyPressed('S'))
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

		//LitterLocations = ;
		/*if (camera.position.x < randomLitter - 1 && camera.position.x >randomLitter + 1)
			CleanerScore->Addscore(Day1);*/
	}

	if (salesCustomer != nullptr)
	{
		salesCustomer->CustomerUpdate(dt);
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
	if (Application::IsKeyPressed('I'))
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
		light[0].position.y += (float)(LSPEED * dt);

	if (Application::IsKeyPressed('Q'))
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
	}
	if (Application::IsKeyPressed(VK_RETURN))
	{
		if (AchievementScene == false)
		{
			AchievementScene = true;
		}
		
	}
	if (Application::IsKeyPressed(VK_DELETE))
	{
	 if (AchievementScene == true)
	 {
		AchievementScene = false;
	 }
	}
	if (AchievementScene == false)
	{
		if (Application::IsKeyPressed('X'))
		{
			escapeanimation = true;
		}
		
	}
	if (FreezeMovement == false) {
		camera.Update(dt);
	}
	
	CalculateFrameRate();
}

void SceneText::Render()
{
	//Clear color & depth buffer every frame
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	viewStack.LoadIdentity();
	viewStack.LookAt(camera.position.x, camera.position.y, camera.position.z, camera.target.x, camera.target.y, camera.target.z, camera.up.x, camera.up.y, camera.up.z);
	modelStack.LoadIdentity();

	// passing the light direction if it is a direction light	
	if (light[0].type == Light::LIGHT_DIRECTIONAL)
	{
		Vector3 lightDir(light[0].position.x, light[0].position.y, light[0].position.z);
		Vector3 lightDirection_cameraspace = viewStack.Top() * lightDir;
		glUniform3fv(m_parameters[U_LIGHT0_POSITION], 1, &lightDirection_cameraspace.x);
	}
	// if it is spot light, pass in position and direction 
	else if (light[0].type == Light::LIGHT_SPOT)
	{
		Position lightPosition_cameraspace = viewStack.Top() * light[0].position;
		glUniform3fv(m_parameters[U_LIGHT0_POSITION], 1, &lightPosition_cameraspace.x);
		Vector3 spotDirection_cameraspace = viewStack.Top() * light[0].spotDirection;
		glUniform3fv(m_parameters[U_LIGHT0_SPOTDIRECTION], 1, &spotDirection_cameraspace.x);
	}
	else
	{
		// default is point light (only position since point light is 360 degrees)
		Position lightPosition_cameraspace = viewStack.Top() * light[0].position;
		glUniform3fv(m_parameters[U_LIGHT0_POSITION], 1, &lightPosition_cameraspace.x);
	}

	 
	

	//modelStack.PushMatrix();
	//modelStack.Translate(0, -3, 0);
	//RenderMesh(meshList[GEO_DICE], true);
	//modelStack.PopMatrix();
	
	if (AchievementScene == true)
	{

		TotalBouncerTask = Day1[2].maxNumber + Day2[2].maxNumber + Day2[2].maxNumber; //algo for this needs to be tested once the minigame is done
		CurrentBouncerTask = Day1[2].currentNumber + Day2[2].currentNumber + Day2[2].currentNumber;

		CurrentSalespersonTask = Day1[0].currentNumber + Day2[0].currentNumber + Day3[0].currentNumber; //just leave like this for now, same reason as bouncer
		
		std::string CarAchievements;
		Achievements* Cartext = new Achievements();
		CarAchievements = Cartext->carAchievements(Price);

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
		RenderTextOnScreen(meshList[GEO_TEXT], "^", Color(0, 1, 0), 5, 3.9, 0);


		if (MechanicGameScore->GetStrike() > MECHANIC_GAME_MAX_LIVES) {
			//You Lose
			FreezeMovement = false;
			MechanicGame = false;
		}

		if (MechanicGameScore->GetPoints() == MECHANIC_GAME_MAX_SCORE) {
			Tasklist* Temp;
			Temp = new Mechanictask();
			Day1 = Temp->Addscore(Day1);
			delete Temp;
			FreezeMovement = false;
			MechanicGame = false;
		}
	}
	else
	{
		RenderSkybox();
		RenderCustomer();
		//RenderCustomer2();
		Renderlevel();

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

		modelStack.PushMatrix();
		modelStack.Translate(light[0].position.x, light[0].position.y, light[0].position.z);
		RenderMesh(meshList[GEO_LIGHTSPHERE], false);
		modelStack.PopMatrix();

		//modelStack.PushMatrix();
		//modelStack.Translate(0, -3, 0);
		//RenderMesh(meshList[GEO_DICE], true);
		//modelStack.PopMatrix();
		if (escapeanimation == false)
		{
			RenderSuspect();
		}

		if (elapsedTime <= 5 && escapeanimation == true) //move foward a bit
		{
			startingPosition = 0;

			modelStack.PushMatrix();
			modelStack.Translate(startingPosition + elapsedTime * 15 / 5, 0, 30);
			RenderSuspect();
			modelStack.PopMatrix();
		}
		else if (elapsedTime > 5 && elapsedTime <= 10 && escapeanimation == true) // move to the left 
		{
			middlePosition = 30;
			
			modelStack.PushMatrix();
			modelStack.Translate(15, 0, middlePosition - (elapsedTime - 5) * 15 / 5); //moving to the right
			RenderSuspect();
			modelStack.PopMatrix();

		}
		else if (elapsedTime > 10 && elapsedTime <= 25 && escapeanimation == true) 
		{
			finalPosition = 0;

			modelStack.PushMatrix();
			modelStack.Translate(finalPosition + (elapsedTime - 5) * 15/5 , 0, 15); // moving foward
			RenderSuspect();
			modelStack.PopMatrix();
		}

	if (CleanerGame == true)
	{
		modelStack.PushMatrix();
		modelStack.Translate(camera.position.x, camera.position.y - 2, camera.position.z);
		RenderCleanerRobot();
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(randomLitter, 0, randomLitter);
		RenderMesh(meshList[GEO_LITTER], true);
		modelStack.PopMatrix();

		for(int i = 0; i < 10 ; ++i)
			RenderMesh(meshList[GEO_LITTER], true);
	}


		std::string TrackedTask;
		Vocation::getVocation();
		Tasklist* Task = new Salesmantask(Day1);
		TrackedTask = Task->Taskstatus(Day1);
		RenderTextOnScreen(meshList[GEO_TEXT], TrackedTask, Color(0, 1, 0), 2, 0, 0);


		Tasklist* BouncerTask = new Bouncertask(Day1);
		TrackedTask = BouncerTask->Taskstatus(Day1);
		RenderTextOnScreen(meshList[GEO_TEXT], TrackedTask, Color(0, 1, 0), 2, 0, 2);

		Tasklist* CleanerTask = new Cleanertask(Day1);
		TrackedTask = CleanerTask->Taskstatus(Day1);
		RenderTextOnScreen(meshList[GEO_TEXT], TrackedTask, Color(0, 1, 0), 2, 0, 1);


		//Day2 
		std::string TrackedTask2;
		Vocation::getVocation();
		Tasklist* BouncerTask2 = new Bouncertask(Day2);
		TrackedTask2 = BouncerTask2->Taskstatus(Day2);
		RenderTextOnScreen(meshList[GEO_TEXT], TrackedTask2, Color(0, 1, 0), 2, 0, 3);


		//Day3
		std::string TrackedTask3;
		Vocation::getVocation();
		Tasklist* BouncerTask3 = new Bouncertask(Day3);
		TrackedTask3 = BouncerTask3->Taskstatus(Day3);
		RenderTextOnScreen(meshList[GEO_TEXT], TrackedTask3, Color(0, 1, 0), 2, 0, 4);

		modelStack.PushMatrix();
		//scale, translate, rotate
		RenderText(meshList[GEO_TEXT], "HELLO WORLD", Color(0, 1, 0));
		modelStack.PopMatrix();
		
		if (elapsedTime > 25 && elapsedTime <= 30) //if within 15 seconds
			{
				RenderBouncerTextBox();
		//for bouncer command text box to appear
		//if (walkingSide < -4 && walkingSide > -21 && escapeanimation == false) //replace this with distance checker
		//{
		//	RenderCommandTextBox("Press 'X' to tell him to get out" , 2.7f, 1.f, 3);
		//}
				Tasklist* temp;
				temp = new Bouncertask(Day1);
				Day1 = temp->Addscore(Day1);
				delete temp;
			}
	}

	//No transform needed
	//RenderTextOnScreen(meshList[GEO_TEXT], "Hello World", Color(0, 1, 0), 2, 0, 0);

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
	
	
}

void SceneText::Exit()
{
	// Cleanup here
	for (int i = 0; i < NUM_GEOMETRY; ++i)
	{
		if (meshList[i] != NULL)
			delete meshList[i];
	}
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

void SceneText::RenderCustomer() // Facing x-axis
{
	modelStack.PushMatrix();
	modelStack.Translate(5, 3.8, 0);
	RenderMesh(meshList[CUSTOMER_BODY], true);
	
	modelStack.PushMatrix();
	modelStack.Translate(0, 6.5, 0);
	modelStack.Rotate(rotateCustomerHead, 0, 1, 0);
	RenderMesh(meshList[CUSTOMER_HEAD], true);
	modelStack.PopMatrix();
	
	modelStack.PushMatrix();
	modelStack.Translate(0, 5.2, -1.5);
	modelStack.Rotate(15, 1, 0, 0);
	modelStack.Rotate(rotateCustomerLeftArm, 0, 0, 1);
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
	modelStack.Rotate(rotateCustomerRightArm, 0, 0, 1);
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
	modelStack.Rotate(rotateCustomerLeftLeg, 0, 0, 1);
	RenderMesh(meshList[CUSTOMER_LEG], true); // Left Leg
	modelStack.PopMatrix();
	
	modelStack.PushMatrix();
	modelStack.Translate(0, 0, 0.5);
	modelStack.Rotate(rotateCustomerRightLeg, 0, 0, 1);
	RenderMesh(meshList[CUSTOMER_LEG], true); // Right Leg
	modelStack.PopMatrix();
	
	modelStack.PopMatrix();
}

void SceneText::RenderCustomer2() // Facing x-axis
{
	modelStack.PushMatrix();
	modelStack.Translate(5, 3.8, 0);
	RenderMesh(meshList[CUSTOMER2_BODY], true);
	
	modelStack.PushMatrix();
	modelStack.Translate(0, 6.5, 0);
	modelStack.Rotate(rotateCustomerHead, 0, 1, 0);
	RenderMesh(meshList[CUSTOMER2_HEAD], true);
	modelStack.PopMatrix();
	
	modelStack.PushMatrix();
	modelStack.Translate(0, 5.2, -1.5);
	modelStack.Rotate(15, 1, 0, 0);
	modelStack.Rotate(rotateCustomerLeftArm, 0, 0, 1);
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
	modelStack.Rotate(rotateCustomerRightArm, 0, 0, 1);
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
	modelStack.Rotate(rotateCustomerLeftLeg, 0, 0, 1);
	RenderMesh(meshList[CUSTOMER2_LEG], true); // Left Leg
	modelStack.PopMatrix();
	
	modelStack.PushMatrix();
	modelStack.Translate(0, 0, 0.5);
	modelStack.Rotate(rotateCustomerRightLeg, 0, 0, 1);
	RenderMesh(meshList[CUSTOMER2_LEG], true); // Right Leg
	modelStack.PopMatrix();
	
	modelStack.PopMatrix();
}

void SceneText::RenderSuspect()
{
		modelStack.PushMatrix();
	
	if (escapeanimation == false)
	{
		modelStack.Translate(0, 0, 30);
	}
	

	RenderMesh(meshList[CUSTOMER_BODY], true);

	modelStack.PushMatrix();
	modelStack.Translate(0, 6.5, 0);
	modelStack.Rotate(rotateCustomerHead, 0, 1, 0);
	RenderMesh(meshList[CUSTOMER_HEAD], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0, 5.2, -1.5);
	modelStack.Rotate(15, 1, 0, 0);
	modelStack.Rotate(rotateCustomerLeftArm, 0, 0, 1);
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
	modelStack.Rotate(rotateCustomerRightArm, 0, 0, 1);
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
	modelStack.Rotate(rotateCustomerLeftLeg, 0, 0, 1);
	RenderMesh(meshList[CUSTOMER_LEG], true); // Left Leg
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0, 0, 0.5);
	modelStack.Rotate(rotateCustomerRightLeg, 0, 0, 1);
	RenderMesh(meshList[CUSTOMER_LEG], true); // Right Leg
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
	modelStack.Translate(5, 0, 0);
	modelStack.Scale(4, 4, 4);
	RenderMesh(meshList[ITEM_DISPLAY_STAND], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(1, 0, 5);
	modelStack.Rotate(90, 0, 1, 0);
	RenderMesh(meshList[ITEM_INFORMATION_STAND], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-40, 0, 10);
	modelStack.Scale(3, 3, 3);
	modelStack.Rotate(90, 0, 1, 0);
	RenderMesh(meshList[ITEM_BOOTH], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-40, 0, 0);
	modelStack.Scale(3, 3, 3);
	modelStack.Rotate(90, 0, 1, 0);
	RenderMesh(meshList[ITEM_FAE_BOOTH], true);
	modelStack.PopMatrix();
	
	modelStack.PushMatrix();
	modelStack.Translate(-10, 0, 1);
	modelStack.Scale(4, 4, 4);
	RenderMesh(meshList[ITEM_INFORMATION_COUNTER], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(40, 0, 0);
	//modelStack.Scale(4, 4, 4);
	RenderMesh(meshList[ITEM_BENCH], true);
	modelStack.PopMatrix();

}

void SceneText::RenderLitter()
{
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
		modelStack.Translate(-4, 2.5, -5);
		RenderMesh(meshList[CAR4_RIGHTWHEEL], true); // Front Wheel
		modelStack.PopMatrix();
	
		modelStack.PushMatrix();
		modelStack.Translate(3.5, 2.5, 5);
		RenderMesh(meshList[CAR4_LEFTWHEEL], true); // Back Wheel
		modelStack.PopMatrix();
	
		modelStack.PushMatrix();
		modelStack.Translate(3.5, 2.5, -5);
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
		characterSpacing.SetToTranslation(i * 1.0f, 0, 0); //1.0f is the spacing of each character, you may change this value
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
