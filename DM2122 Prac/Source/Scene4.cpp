#include "Scene4.h"
#include "GL\glew.h"
#include "Application.h"
#include <Mtx44.h>
#include "shader.hpp"
#include "MeshBuilder.h"

Scene4::Scene4()
{
}

Scene4::~Scene4()
{
}

void Scene4::Init()
{
	// Set background color to dark blue
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
	// Enable depth test
	glEnable(GL_DEPTH_TEST);

	// Init VBO here
	// Generate a default VAO for now
	glGenVertexArrays(1, &m_vertexArrayID);
	glBindVertexArray(m_vertexArrayID);

	//Load vertex and fragment shaders
	m_programID = LoadShaders("Shader//TransformVertexShader.vertexshader", "Shader//SimpleFragmentShader.fragmentshader");
	// Use our shader
	glUseProgram(m_programID);

	m_parameters[Scene4_U_MVP] = glGetUniformLocation(m_programID, "MVP");

	Mtx44 projection;
	projection.SetToPerspective(45.f, 4.f / 3.f, 0.1f, 1000.f);
	projectionStack.LoadMatrix(projection);

	camera.Init(Vector3(40, 30, 30), Vector3(0, 0, 0), Vector3(0, 1, 0));

	meshList[GEO_AXES] = MeshBuilder::GenerateAxes("reference", 1000, 1000, 1000);
	meshList[GEO_SUN] = MeshBuilder::GenerateSphere("sun", Color(1, 1, 0), 18, 36, 1.f);
	meshList[GEO_PLANET1] = MeshBuilder::GenerateSphere("planet1", Color(0, 1, 0), 18, 36, 1.f);
	meshList[GEO_RING1] = MeshBuilder::GenerateRing("planet1Ring", Color(0, 1, 1), 18, 2, 1.5);
	meshList[GEO_MOON2] = MeshBuilder::GenerateSphere("planet1Moon", Color(0, 0, 1), 18, 36, 0.5);
}

void Scene4::Update(double dt)
{
	//scale.SetToScale(1.f, 1.f, 1.f);
	//rotate.SetToRotation(0, 0.f, 0.f, 1.f);
	//translate.SetToTranslation(0.f, 0.f, 0.f);

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

	rotateAngle += (float)(10 * dt);

	camera.Update(dt);
}

void Scene4::Render()
{
	//Clear color & depth buffer every frame
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	viewStack.LoadIdentity();
	viewStack.LookAt(camera.position.x, camera.position.y, camera.position.z,
		camera.target.x, camera.target.y, camera.target.z, camera.up.x, camera.up.y,
		camera.up.z);
	modelStack.LoadIdentity();

	Mtx44 MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top();

	//view.SetToLookAt(
	//	camera.position.x, camera.position.y, camera.position.z,
	//	camera.target.x, camera.target.y, camera.target.z,
	//	camera.up.x, camera.up.y, camera.up.z
	//);

	//model = translate * rotate * scale;
	//MVP = projection * view * model;
	glUniformMatrix4fv(m_parameters[Scene4_U_MVP], 1, GL_FALSE, &MVP.a[0]);

	meshList[GEO_AXES]->Render();

	modelStack.PushMatrix(); //sun
		modelStack.Rotate(rotateAngle, 0, 1, 0);
		modelStack.Scale(10, 10, 10);

		MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top();
		glUniformMatrix4fv(m_parameters[Scene4_U_MVP], 1, GL_FALSE, &MVP.a[0]);

		meshList[GEO_SUN]->Render();
	modelStack.PopMatrix(); //sun

	modelStack.PushMatrix(); //planet1
		modelStack.Rotate(3 * rotateAngle, 0, 1, 0);
		modelStack.Translate(15, 0, 0);
		modelStack.Scale(2, 2, 2);

		MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top();
		glUniformMatrix4fv(m_parameters[Scene4_U_MVP], 1, GL_FALSE, &MVP.a[0]);
		meshList[GEO_PLANET1]->Render();
		modelStack.PushMatrix(); //Planet 1 ring
			modelStack.Rotate(0, 0, 1, 0);
			modelStack.Translate(0, 0, 0);
			modelStack.Scale(1, 1, 1);
			MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top();
			glUniformMatrix4fv(m_parameters[Scene4_U_MVP], 1, GL_FALSE, &MVP.a[0]);
			meshList[GEO_RING1]->Render();
		modelStack.PopMatrix(); //Planet 1 ring

	modelStack.PopMatrix(); //planet1

}

void Scene4::Exit()
{
	// Cleanup VBO here
	glDeleteVertexArrays(1, &m_vertexArrayID);

	glDeleteProgram(m_programID);

}
