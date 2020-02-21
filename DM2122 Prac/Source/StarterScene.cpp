#include "StarterScene.h"
#include "GL\glew.h"
#include "Application.h"
#include <Mtx44.h>
#include "shader.hpp"
#include "MeshBuilder.h"
#include "Utility.h"
#include "LoadTGA.h"




#define ROT_LIMIT 45.f;
#define SCALE_LIMIT 5.f;
#define LSPEED 10.f

using namespace irrklang; // audio lib
StarterScene::StarterScene()
{
	for (int i = 0; i < NUM_GEOMETRY; ++i)
	{
		meshList[i] = NULL;
	}
}

StarterScene::~StarterScene()
{
}

void StarterScene::Init()
{
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);


	// Initialize Pointer
	Vocation::InitializeJob();

	// Playing bgm
	SoundEngine->play2D("audio//IntroBackground.mp3", true, false, true);

	// Initialize Sound Values
	zoomPlaying = false;
	vroomPlaying = false;

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
	light[0].position.Set(0, 35, 25);
	light[0].color.Set(1.f, 1.f, 1.f);
	light[0].power = 10;
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

	// set to 0, to track debounce time, and also get the global/elapsed time
	globalTime = 0;
	debounceTime = 0;
	animationTime = 0;

	// Setting the parts of this Scene in order
	IntroScene = true;
	VocationScene = false;
	EntranceScene = false;


	// Skybox
	meshList[GEO_LEFT] = MeshBuilder::GenerateQuad("left", Color(1, 1, 1), 1.f, 1.f);
	meshList[GEO_LEFT]->textureID = LoadTGA("Image//StarterSceneLeft.tga");

	meshList[GEO_RIGHT] = MeshBuilder::GenerateQuad("right", Color(1, 1, 1), 1.f, 1.f);
	meshList[GEO_RIGHT]->textureID = LoadTGA("Image//StarterSceneRight.tga");

	meshList[GEO_TOP] = MeshBuilder::GenerateQuad("top", Color(1, 1, 1), 1.f, 1.f);
	meshList[GEO_TOP]->textureID = LoadTGA("Image//StarterSceneTop.tga");

	meshList[GEO_BOTTOM] = MeshBuilder::GenerateQuad("bottom", Color(1, 1, 1), 1.f, 1.f);
	meshList[GEO_BOTTOM]->textureID = LoadTGA("Image//StarterSceneBottom.tga");

	meshList[GEO_FRONT] = MeshBuilder::GenerateQuad("front", Color(1, 1, 1), 1.f, 1.f);
	meshList[GEO_FRONT]->textureID = LoadTGA("Image//StarterSceneFront.tga");

	meshList[GEO_BACK] = MeshBuilder::GenerateQuad("back", Color(1, 1, 1), 1.f, 1.f);
	meshList[GEO_BACK]->textureID = LoadTGA("Image//StarterSceneBack.tga");

	
	// Initializing the Scene's parts
	meshList[GEO_SCREEN] = MeshBuilder::GenerateQuad("Screen", Color(0, 1, 0), 1, 1);
	SalesPersonTexture = LoadTGA("Image//salespersonlogo.tga");
	CleanerTexture = LoadTGA("Image//cleanerlogo.tga");
	BouncerTexture = LoadTGA("Image//bouncerlogo.tga");
	MechanicTexture = LoadTGA("Image//mechaniclogo.tga");
	JobSelection = Vocation::getVocationJobClass(VocationJob::SALES);
	meshList[GEO_SCREEN]->textureID = SalesPersonTexture;

	meshList[GEO_BACKGROUND] = MeshBuilder::GenerateQuad("Background", Color(0, 1, 0), 1.6, 1.4);
	meshList[GEO_BACKGROUND]->textureID = LoadTGA("Image//motorshowbackground2.tga");
	background2 = LoadTGA("Image//motorshowbackground.tga");

	meshList[GEO_ENTRANCE] = MeshBuilder::GenerateOBJ("Entrance", "OBJ//carshowentrance.obj");
	meshList[GEO_ENTRANCE]->textureID = LoadTGA("Image//carshowentrance.tga");

	meshList[GEO_ENTRANCELEFT] = MeshBuilder::GenerateOBJ("EntranceLeft", "OBJ//carshowentranceleftdoor.obj");
	meshList[GEO_ENTRANCERIGHT] = MeshBuilder::GenerateOBJ("EntranceRight", "OBJ//carshowentrancerightdoor.obj");
	meshList[GEO_ENTRANCELEFT]->textureID = meshList[GEO_ENTRANCERIGHT]->textureID = LoadTGA("Image//slidingdoor.tga");

	meshList[GEO_CARLOGO] = MeshBuilder::GenerateQuad("Background", Color(0, 1, 0), 1, 1);
	meshList[GEO_CARLOGO]->textureID = LoadTGA("Image//carlogo.tga");

	meshList[GEO_TEXT] = MeshBuilder::GenerateText("text", 16, 16);
	meshList[GEO_TEXT]->textureID = LoadTGA("Image//calibri.tga");
	
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
	meshList[GUARD_LEG ]->textureID = LoadTGA("Image//GuardBot.tga");
	
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
}

void StarterScene::Update(double dt)
{
	globalTime += dt;
	
	
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

	if (IntroScene)
	{
		animationTime += dt;
		if (Application::IsKeyPressed(VK_RETURN))
		{
			SoundEngine->play2D("audio//bleep.wav", false);
			IntroScene = false;
			VocationScene = true;
			debounceTime = globalTime;
			animationTime = globalTime;
		}
	}
	else if (VocationScene)
	{
		if (Application::IsKeyPressed(VK_RETURN) && Vocation::getVocation() == VocationJob::NONE && globalTime - debounceTime > 0.2f) // enter key
		{
			SoundEngine->play2D("audio//bleep.wav", false);
			camera.cameraLock = true; // lock the camera from player interaction
			VocationScene = false;
			EntranceScene = true;
			camera.position = { 0, 30, -50 };
			animationTime = globalTime;
		
			Vocation::setVocation(JobSelection);
			Vocation::ClearJob(false); //prevent memory leaks by deleting all other pointers except the chosen vocation
			//std::vector<Vocation::Quest> name = Vocation::getMainQuest(2);
			/*Vocation::Quest temp = name[0];
			temp.currentNumber = 0;
			temp.job == Vocation::BOUNCER;*/
			
			//Vocation::getMainQuest(1)[JobSelection].currentNumber = 1;

		}

		if (Application::IsKeyPressed('E') && globalTime - debounceTime > 0.2f)
		{
			SoundEngine->play2D("audio//bleep.mp3", false);
			debounceTime = globalTime; // to check the time taken after this keypress to press another key
			JobSelection = JobSelection->getNext();
		}
		if (Application::IsKeyPressed('Q') && globalTime - debounceTime > 0.2f)
		{
			SoundEngine->play2D("audio//bleep.mp3", false);
			debounceTime = globalTime; // to check the time taken after this keypress to press another key
			JobSelection = JobSelection->getPrevious();
		}
		// Assign the correct texture to the chosen vocation
		if (JobSelection->getJob() == VocationJob::SALES)
			meshList[GEO_SCREEN]->textureID = SalesPersonTexture;
		else if (JobSelection->getJob() == VocationJob::CLEANER)
			meshList[GEO_SCREEN]->textureID = CleanerTexture;
		else if (JobSelection->getJob() == VocationJob::BOUNCER)
			meshList[GEO_SCREEN]->textureID = BouncerTexture;
		else if (JobSelection->getJob() == VocationJob::MECHANIC)
			meshList[GEO_SCREEN]->textureID = MechanicTexture;
	}
	else if (EntranceScene)
	{
		if (globalTime - animationTime <= 1)
		{
			camera.position.z = -(50 - (globalTime - animationTime) * 60);
			camera.position.y = 30 - (globalTime - animationTime) * 27.5;
			camera.target = { 0, 10, 45 };
		}
		else if (globalTime- animationTime <= 2)
		{
			camera.position.z = 10; // camera must be locked for this to work
			camera.position.y = 5;
		}
		else if (globalTime - animationTime <= 3)
		{
			camera.target = { 0, 10 - (globalTime - animationTime - 2.f) * 7.5f, 45 };
		}
		else if (globalTime - animationTime <= 4)
		{
			if (!zoomPlaying)
			{
				zoomPlaying = true;
				SoundEngine->play2D("audio//soniczoom.mp3", false);
			}
			camera.position.z = 10 + (globalTime - animationTime - 3) * 28;
			camera.position.y = 5 - (globalTime - animationTime - 3) * 2.5f;
			camera.target = { 0, 2.f, 45 };
		}
		else
		{
			camera.position.z = 38;
			camera.position.y = 2.5f;
			camera.target = { 0, 2.f, 45 };
			SoundEngine->stopAllSounds();
			Application::Isonlevel = true;
		}
	}


	camera.Update(dt);
	CalculateFrameRate();
}

void StarterScene::Render()
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



	
	
	if (IntroScene)
	{
		RenderObjectOnScreen(meshList[GEO_BACKGROUND], 0.8, 0.5, 50);

		// simple movement animation
		if (animationTime > 3)
		{
			if (vroomPlaying == false)
			{
				vroomPlaying = true;
				SoundEngine->play2D("audio//vroom.mp3", false);
			}
		
			RenderObjectOnScreen(meshList[GEO_CARLOGO], -1 + (animationTime-3) * 4, 1.5, 15);
		}
		if (animationTime > 7)
		{
			vroomPlaying = false;
			animationTime = 0;
		}

		if (globalTime < 1.f)
		{
			float toMove = 16.25f - globalTime*8;
			CustomRenderTextOnScreen(meshList[GEO_TEXT], "FUTURISTIC", Color(1, 1, 0), 4.5, 4.1, toMove + 1, globalTime * 360);
			CustomRenderTextOnScreen(meshList[GEO_TEXT], "MOTOR SHOW", Color(1, 1, 0), 4.5, 4, toMove, globalTime * 360);

			CustomRenderTextOnScreen(meshList[GEO_TEXT], "<ENTER>", Color(0, 1, 1), 2.5, 12.75, 5,180 + globalTime * 180, 1.f, 0.f, 0.f);
			CustomRenderTextOnScreen(meshList[GEO_TEXT], "To", Color(0, 1, 1), 2.5, 15.25, 3.75,180 + globalTime * 180, 1.f, 0.f, 0.f);
			CustomRenderTextOnScreen(meshList[GEO_TEXT], "Confirm", Color(0, 1, 1), 2.5, 12.66, 2.5,180 + globalTime * 180, 1.f, 0.f, 0.f);
		}
		else
		{
			RenderTextOnScreen(meshList[GEO_TEXT], "FUTURISTIC", Color(1, 1, 0), 4.5, 4.1, 9.25);
			RenderTextOnScreen(meshList[GEO_TEXT], "MOTOR SHOW", Color(1, 1, 0), 4.5, 4, 8.25);

			RenderTextOnScreen(meshList[GEO_TEXT], "<ENTER>", Color(0, 1, 1), 2.5, 12.75, 5);
			RenderTextOnScreen(meshList[GEO_TEXT], "To", Color(0, 1, 1), 2.5, 15.25, 3.75);
			RenderTextOnScreen(meshList[GEO_TEXT], "Confirm", Color(0, 1, 1), 2.5, 12.66, 2.5);
		}
		

		
	}
	else if (VocationScene)
	{
		meshList[GEO_BACKGROUND]->textureID = background2;
		RenderObjectOnScreen(meshList[GEO_BACKGROUND], 0.8, 0.5, 50);
		
		if (globalTime - animationTime < 1.f)
		{
			CustomRenderObjectOnScreen(meshList[GEO_SCREEN], 2, 1.5, 20, 180 + (globalTime-animationTime) * 180, 1.f, 1.f, 1.f);
			CustomRenderTextOnScreen(meshList[GEO_TEXT], "CHOOSE", Color(0, 1, 1), 3, 10.5, 17.5, 180 + (globalTime - animationTime) * 180, 0.f, 1.f, 0.f);
			CustomRenderTextOnScreen(meshList[GEO_TEXT], "YOUR", Color(0, 1, 1), 3, 11.5, 16.5, 180 + (globalTime - animationTime) * 180, 0.f, 1.f, 0.f);
			CustomRenderTextOnScreen(meshList[GEO_TEXT], "VOCATION", Color(0, 1, 1), 3, 9.5, 15.25, 180 + (globalTime - animationTime) * 180, 0.f, 1.f, 0.f);

			CustomRenderTextOnScreen(meshList[GEO_TEXT], "<ENTER>", Color(0, 1, 1), 2.5, 12.75, 5, 180 + (globalTime - animationTime) * 180, 1.f, 0.f, 0.f);
			CustomRenderTextOnScreen(meshList[GEO_TEXT], "To", Color(0, 1, 1), 2.5, 15.25, 3.75, 180 + (globalTime - animationTime) * 180, 1.f, 0.f, 0.f);
			CustomRenderTextOnScreen(meshList[GEO_TEXT], "Confirm", Color(0, 1, 1), 2.5, 12.66, 2.5, 180 + (globalTime - animationTime) * 180, 1.f, 0.f, 0.f);
		}
		else
		{
			RenderObjectOnScreen(meshList[GEO_SCREEN], 2, 1.5, 20);
			RenderTextOnScreen(meshList[GEO_TEXT], "CHOOSE", Color(0, 1, 1), 3, 10.5, 17.5);
			RenderTextOnScreen(meshList[GEO_TEXT], "YOUR", Color(0, 1, 1), 3, 11.5, 16.5);
			RenderTextOnScreen(meshList[GEO_TEXT], "VOCATION", Color(0, 1, 1), 3, 9.5, 15.25);

			RenderTextOnScreen(meshList[GEO_TEXT], "<ENTER>", Color(0, 1, 1), 2.5, 12.75, 5);
			RenderTextOnScreen(meshList[GEO_TEXT], "To", Color(0, 1, 1), 2.5, 15.25, 3.75);
			RenderTextOnScreen(meshList[GEO_TEXT], "Confirm", Color(0, 1, 1), 2.5, 12.66, 2.5);
		}


	}
	else if (EntranceScene)
	{
		RenderSkybox();
		modelStack.PushMatrix();
			modelStack.Translate(0, 0, 40);
			modelStack.Rotate(90, 0, 1, 0);
			RenderMesh(meshList[GEO_ENTRANCE], true);
			if (globalTime - animationTime < 1)
			{
				RenderMesh(meshList[GEO_ENTRANCELEFT], false);
				RenderMesh(meshList[GEO_ENTRANCERIGHT], false);
			}
			else if (globalTime - animationTime > 1 && globalTime - animationTime <= 2)
			{
				modelStack.PushMatrix();
					modelStack.Translate(0, 0, globalTime - animationTime - 1);
					RenderMesh(meshList[GEO_ENTRANCELEFT], false);
				modelStack.PopMatrix();

				modelStack.PushMatrix();
					modelStack.Translate(0, 0, -(globalTime - animationTime - 1));
					RenderMesh(meshList[GEO_ENTRANCERIGHT], false);
				modelStack.PopMatrix();

			}
			else if (globalTime - animationTime > 2)
			{
				modelStack.PushMatrix();
					modelStack.Translate(0, 0, 1);
					RenderMesh(meshList[GEO_ENTRANCELEFT], false);
				modelStack.PopMatrix();
				
				modelStack.PushMatrix();
					modelStack.Translate(0, 0, -1);
					RenderMesh(meshList[GEO_ENTRANCERIGHT], false);
				modelStack.PopMatrix();
				
			}
			
			
		modelStack.PopMatrix();

	}



}

void StarterScene::Exit()
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

void StarterScene::RenderMesh(Mesh* mesh, bool enableLight)
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

void StarterScene::RenderSkybox()
{
	modelStack.PushMatrix();
	modelStack.Translate(0, 3.75, 0);

	modelStack.PushMatrix();
	///scale, translate, rotate 
	modelStack.Translate(19.9f, 0.f, 0.f);
	modelStack.Scale(40.f, 60.f, 82.f);
	modelStack.Rotate(-90.f, 0.f, 1.f, 0.f);
	RenderMesh(meshList[GEO_LEFT], false);
	modelStack.PopMatrix();
	modelStack.PushMatrix();
	///scale, translate, rotate 
	modelStack.Translate(-19.9f, 0.f, 0.f);
	modelStack.Scale(80.f, 60.f, 82.f);
	modelStack.Rotate(90.f, 0.f, 1.f, 0.f);
	RenderMesh(meshList[GEO_RIGHT], false);
	modelStack.PopMatrix();
	modelStack.PushMatrix();
	///scale, translate, rotate 
	modelStack.Translate(0.0f, 29.9f, 1.f);
	modelStack.Scale(40.f, 60.f, 80.9f);
	modelStack.Rotate(90.f, 1.f, 0.f, 0.f);
	modelStack.PushMatrix();
	modelStack.Rotate(180.f, 0.f, 0.f, 1.f);
	RenderMesh(meshList[GEO_TOP], false);
	modelStack.PopMatrix();
	modelStack.PopMatrix();
	modelStack.PushMatrix();
	///scale, translate, rotate 
	modelStack.Translate(0.f, -29.9f, 0.f);
	modelStack.Scale(40.f, 60.f, 82.f);
	modelStack.Rotate(-90.f, 1.f, 0.f, 0.f);
	modelStack.PushMatrix();
	modelStack.Rotate(180.f, 0.f, 0.f, 1.f);
	RenderMesh(meshList[GEO_BOTTOM], false);
	modelStack.PopMatrix();
	modelStack.PopMatrix();
	modelStack.PushMatrix();
	///scale, translate, rotate 
	modelStack.Translate(0.f, 0.f, -39.9f);
	modelStack.Scale(40.f, 60.f, 40.f);
	RenderMesh(meshList[GEO_FRONT], false);
	modelStack.PopMatrix();
	modelStack.PushMatrix();
	///scale, translate, rotate 
	modelStack.Translate(0.f, 0.f, 40.9f);
	modelStack.Scale(40.f, 60.f, 40.f);
	modelStack.Rotate(180.f, 0.f, 1.f, 0.f);
	RenderMesh(meshList[GEO_BACK], false);
	modelStack.PopMatrix();

	modelStack.PopMatrix();
}


void StarterScene::RenderCleanerRobot() // Facing x-axis
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
		modelStack.Rotate(rotateCleanerWheels, 0, 0, 1);
		RenderMesh(meshList[CLEANER_WHEELJOINT], true); // Front Left
			modelStack.PushMatrix();
			RenderMesh(meshList[CLEANER_WHEEL], true);
			modelStack.PopMatrix();
		modelStack.PopMatrix();
		modelStack.PushMatrix();
		modelStack.Translate(0.65, 0.1, 0.8);
		modelStack.Rotate(rotateCleanerWheels, 0, 0, 1);
		RenderMesh(meshList[CLEANER_WHEELJOINT], true); // Front Right
			modelStack.PushMatrix();
			RenderMesh(meshList[CLEANER_WHEEL], true);
			modelStack.PopMatrix();
		modelStack.PopMatrix();
		modelStack.PushMatrix();
		modelStack.Translate(-0.8, 0.1, -0.8);
		modelStack.Rotate(rotateCleanerWheels, 0, 0, 1);
		RenderMesh(meshList[CLEANER_WHEELJOINT], true); // Back Left
			modelStack.PushMatrix();
			RenderMesh(meshList[CLEANER_WHEEL], true);
			modelStack.PopMatrix();
		modelStack.PopMatrix();
		modelStack.PushMatrix();
		modelStack.Translate(-0.8, 0.1, 0.8);
		modelStack.Rotate(rotateCleanerWheels, 0, 0, 1);
		RenderMesh(meshList[CLEANER_WHEELJOINT], true); // Back Right
			modelStack.PushMatrix();
			RenderMesh(meshList[CLEANER_WHEEL], true);
			modelStack.PopMatrix();
		modelStack.PopMatrix();
	modelStack.PopMatrix();
}

void StarterScene::RenderCustomer() // Facing x-axis
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
			modelStack.Translate(0, LeftArmY , LeftArmZ);
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

void StarterScene::RenderGuardBot() // Facing z-axis
{
	modelStack.PushMatrix();
	modelStack.Translate(15, 9, 0);
	RenderMesh(meshList[GUARD_BODY], true);
		modelStack.PushMatrix();
		modelStack.Translate(0, 3, 0);
		RenderMesh(meshList[GUARD_HEAD], true);
		modelStack.PopMatrix();
		modelStack.PushMatrix();
		modelStack.Translate(4.5, 2.5 ,0);
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

void StarterScene::RenderSaleBot() // Facing negative z-axis
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

void StarterScene::RenderText(Mesh* mesh, std::string text, Color color)
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

void StarterScene::RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y)
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
		characterSpacing.SetToTranslation(i * 1.0f, 0, 0); //1.0f is the spacing of each character, you may change this value
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


void StarterScene::CustomRenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y, float angle, float angleX, float angleY, float angleZ) //default angleX = 1.f, angleY = 0.f, angleZ = 0.f
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
	modelStack.Translate(x + text.length()/2, y, 0);
	modelStack.Rotate(angle, angleX, angleY, angleZ);
	modelStack.Translate(-(signed)(text.length()/2), 0, 0);

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
	projectionStack.PopMatrix();
	viewStack.PopMatrix();
	modelStack.PopMatrix();

	glEnable(GL_DEPTH_TEST);
}
void StarterScene::RenderObjectOnScreen(Mesh* mesh, float x, float y, float size)
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

void StarterScene::CustomRenderObjectOnScreen(Mesh* mesh, float x, float y, float size, float angle, float angleX, float angleY, float angleZ)
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
	modelStack.Rotate(angle, angleX, angleY, angleZ);
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

void StarterScene::CalculateFrameRate()
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
