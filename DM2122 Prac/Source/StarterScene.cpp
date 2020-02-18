#include "StarterScene.h"
#include "GL\glew.h"
#include "Application.h"
#include <Mtx44.h>
#include "shader.hpp"
#include "MeshBuilder.h"
#include "Utility.h"
#include "LoadTGA.h"
#include "Cleanertask.h"


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

	SoundEngine->play2D("audio//creeper.mp3", true, false, true);

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

	// set to 0, to track debounce time, and also get the global/elapsed time
	globalTime = 0;
	debounceTime = 0;

	// Setting the parts of this Scene in order
	IntroScene = true;
	VocationScene = false;
	EntranceScene = false;

	// Initializing the Scene's parts
	meshList[GEO_SCREEN] = MeshBuilder::GenerateQuad("Screen", Color(0, 1, 0), 1, 1);
	SalesPersonTexture = LoadTGA("Image//salespersonlogo.tga");
	CleanerTexture = LoadTGA("Image//cleanerlogo.tga");
	BouncerTexture = LoadTGA("Image//bouncerlogo.tga");
	MechanicTexture = LoadTGA("Image//mechaniclogo.tga");
	JobSelection = Vocation::SALES;
	meshList[GEO_SCREEN]->textureID = SalesPersonTexture;

	meshList[GEO_BACKGROUND] = MeshBuilder::GenerateQuad("Background", Color(0, 1, 0), 1.6, 1.4);
	meshList[GEO_BACKGROUND]->textureID = LoadTGA("Image//motorshowbackground2.tga");
	background2 = LoadTGA("Image//motorshowbackground.tga");

	meshList[GEO_TEXT] = MeshBuilder::GenerateText("text", 16, 16);
	meshList[GEO_TEXT]->textureID = LoadTGA("Image//calibri.tga");

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
		if (Application::IsKeyPressed(VK_RETURN))
		{
			IntroScene = false;
			VocationScene = true;
			debounceTime = globalTime;
			animationTime = globalTime;
		}
	}
	else if (VocationScene)
	{
		if (Application::IsKeyPressed(VK_RETURN) && Vocation::getVocation() == Vocation::NONE && globalTime - debounceTime > 0.2f) // enter key
		{
			Vocation::setVocation(JobSelection);
			SoundEngine->stopAllSounds();
			
			//std::vector<Vocation::Quest> Day1 = Vocation::getMainQuest(1);
			Application::Isonlevel = true;
			
			/*Vocation::Quest temp = name[0];
			temp.currentNumber = 0;
			temp.job == Vocation::BOUNCER;*/
			
			//Vocation::getMainQuest(1)[JobSelection].currentNumber = 1;

		}
		if (Vocation::getVocation() != Vocation::NONE)
		{
			auto lol = Vocation::getVocation();
			std::cout << "\n" + lol;
		}
		if (Application::IsKeyPressed('E') && globalTime - debounceTime > 0.2f)
		{
			debounceTime = globalTime; // to check the time taken after this keypress to press another key
			if (JobSelection == Vocation::SALES)
			{
				JobSelection = Vocation::CLEANER;
			}
			else if (JobSelection == Vocation::CLEANER)
			{
				JobSelection = Vocation::BOUNCER;
			}
			else if (JobSelection == Vocation::BOUNCER)
			{
				JobSelection = Vocation::MECHANIC;
			}
			else if (JobSelection == Vocation::MECHANIC)
			{
				JobSelection = Vocation::SALES;
			}
		}
		if (Application::IsKeyPressed('Q') && globalTime - debounceTime > 0.2f)
		{
			debounceTime = globalTime; // to check the time taken after this keypress to press another key
			if (JobSelection == Vocation::SALES)
			{
				JobSelection = Vocation::MECHANIC;
			}
			else if (JobSelection == Vocation::CLEANER)
			{
				JobSelection = Vocation::SALES;
			}
			else if (JobSelection == Vocation::BOUNCER)
			{
				JobSelection = Vocation::CLEANER;
			}
			else if (JobSelection == Vocation::MECHANIC)
			{
				JobSelection = Vocation::BOUNCER;
			}
		}
		// Assign the correct texture to the chosen vocation
		if (JobSelection == Vocation::SALES)
			meshList[GEO_SCREEN]->textureID = SalesPersonTexture;
		else if (JobSelection == Vocation::CLEANER)
			meshList[GEO_SCREEN]->textureID = CleanerTexture;
		else if (JobSelection == Vocation::BOUNCER)
			meshList[GEO_SCREEN]->textureID = BouncerTexture;
		else if (JobSelection == Vocation::MECHANIC)
			meshList[GEO_SCREEN]->textureID = MechanicTexture;
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
		RenderObjectOnScreen(meshList[GEO_SCREEN], 2, 1.5, 20);
		if (globalTime - animationTime < 1.f)
		{
			CustomRenderTextOnScreen(meshList[GEO_TEXT], "CHOOSE", Color(0, 1, 1), 3, 10.5, 17.5, 180 + (globalTime - animationTime) * 180, 0.f, 1.f, 0.f);
			CustomRenderTextOnScreen(meshList[GEO_TEXT], "YOUR", Color(0, 1, 1), 3, 11.5, 16.5, 180 + (globalTime - animationTime) * 180, 0.f, 1.f, 0.f);
			CustomRenderTextOnScreen(meshList[GEO_TEXT], "VOCATION", Color(0, 1, 1), 3, 9.5, 15.25, 180 + (globalTime - animationTime) * 180, 0.f, 1.f, 0.f);

			CustomRenderTextOnScreen(meshList[GEO_TEXT], "<ENTER>", Color(0, 1, 1), 2.5, 12.75, 5, 180 + (globalTime - animationTime) * 180, 1.f, 0.f, 0.f);
			CustomRenderTextOnScreen(meshList[GEO_TEXT], "To", Color(0, 1, 1), 2.5, 15.25, 3.75, 180 + (globalTime - animationTime) * 180, 1.f, 0.f, 0.f);
			CustomRenderTextOnScreen(meshList[GEO_TEXT], "Confirm", Color(0, 1, 1), 2.5, 12.66, 2.5, 180 + (globalTime - animationTime) * 180, 1.f, 0.f, 0.f);
		}
		else
		{
			RenderTextOnScreen(meshList[GEO_TEXT], "CHOOSE", Color(0, 1, 1), 3, 10.5, 17.5);
			RenderTextOnScreen(meshList[GEO_TEXT], "YOUR", Color(0, 1, 1), 3, 11.5, 16.5);
			RenderTextOnScreen(meshList[GEO_TEXT], "VOCATION", Color(0, 1, 1), 3, 9.5, 15.25);

			RenderTextOnScreen(meshList[GEO_TEXT], "<ENTER>", Color(0, 1, 1), 2.5, 12.75, 5);
			RenderTextOnScreen(meshList[GEO_TEXT], "To", Color(0, 1, 1), 2.5, 15.25, 3.75);
			RenderTextOnScreen(meshList[GEO_TEXT], "Confirm", Color(0, 1, 1), 2.5, 12.66, 2.5);
		}


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