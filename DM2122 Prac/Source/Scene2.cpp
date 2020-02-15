#include "Scene2.h"
#include "GL\glew.h"
#include "Application.h"

#include "shader.hpp"


Scene2::Scene2()
{

}

Scene2::~Scene2()
{
}

void Scene2::Init()
{
	// Set background color to dark blue
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
	// Enable depth test
	glEnable(GL_DEPTH_TEST);

	// Init VBO here
	// Generate a default VAO for now
	glGenVertexArrays(1, &m_vertexArrayID);
	glBindVertexArray(m_vertexArrayID);

	// Generate buffers
	glGenBuffers(SCENE2_NUM_GEOMETRY, &m_vertexBuffer[0]);
	glGenBuffers(SCENE2_NUM_GEOMETRY, &m_colorBuffer[0]);

	//Creating Triangle 1 (SCENE2_GEO_TRIANGLE_1)
	static const GLfloat vertex_buffer_data[] = {
		-0.5f,  0.0f, 0.0f,
		0.0f, 0.5f, 0.0f,
		0.5f, 0.0f, 0.0f,
	};
	static const GLfloat color_buffer_data[] = {
		1.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 0.0f, 1.0f,
	};

	// Set the current active buffer
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer[SCENE2_GEO_TRIANGLE_1]);
	// Transfer vertices to OpenGL
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data), vertex_buffer_data,
		GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, m_colorBuffer[SCENE2_GEO_TRIANGLE_1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(color_buffer_data), color_buffer_data,
		GL_STATIC_DRAW);

	//Creating Triangle 1 (SCENE2_GEO_TRIANGLE_1)
	static const GLfloat vertex_buffer_data2[] = {
		-0.5f,  0.0f, -2.0f,
		0.5f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.0f,
	};
	static const GLfloat color_buffer_data2[] = {
		1.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 0.0f, 1.0f,
	};

	// Set the current active buffer
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer[SCENE2_GEO_TRIANGLE_2]);
	// Transfer vertices to OpenGL
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data2), vertex_buffer_data2,
		GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, m_colorBuffer[SCENE2_GEO_TRIANGLE_2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(color_buffer_data2), color_buffer_data2,
		GL_STATIC_DRAW);

	static const GLuint index_buffer_data[] = {
		-0.5f, -0.5f,  0.5f, //front
		 0.5f, -0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
	};
	glGenBuffers(1, &m_indexBuffer); //generate index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer); //bind index buffer
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_buffer_data), index_buffer_data, GL_STATIC_DRAW);

	//Load vertex and fragment shaders
	m_programID = LoadShaders("Shader//TransformVertexShader.vertexshader", "Shader//SimpleFragmentShader.fragmentshader");
	// Use our shader
	glUseProgram(m_programID);

	m_parameters[UNIFORM_TYPE::U_MVP] = glGetUniformLocation(m_programID, "MVP");

	for (int i = 0; i < SCENE2_NUM_GEOMETRY; ++i)
	{ 
		translate[i].SetToIdentity();
		rotate[i].SetToIdentity();
		scale[i].SetToIdentity();
	}

	model.SetToIdentity();
	view.SetToIdentity();
	projection.SetToPerspective(45.0f, 4.0f / 3.0f, 0.1f, 1000.0f);
	camera.Init(Vector3(4, 3, 3), Vector3(0, 0, 0), Vector3(0, 1, 0));
	m_fAngle = 0.f;
}

void Scene2::Update(double dt)
{
	if (Application::IsKeyPressed(VK_SPACE))
	{
		for (int i = 0; i < SCENE2_NUM_GEOMETRY; ++i)
		{
			scale[i].SetToScale(1.f, 2.f, 1.f);
			rotate[i].SetToRotation(m_fAngle, 0.f, 0.f, 1.f);
			translate[i].SetToTranslation(0.f, -0.5f, 0.f);
			m_fAngle *= -1;
		}
	m_fAngle = abs(m_fAngle);
	m_fAngle += dt * 10;
	}
	else if (Application::IsKeyPressed(0x31))
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

}

void Scene2::Render()
{
	//Clear color & depth buffer every frame
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	view.SetToLookAt(
		camera.position.x, camera.position.y, camera.position.z,
		camera.target.x, camera.target.y, camera.target.z,
		camera.up.x, camera.up.y, camera.up.z
	);
	
	for (int i = 0; i < SCENE2_NUM_GEOMETRY; ++i)
		RenderTriangle((SCENE2_GEOMETRY_TYPE)i);
}

void Scene2::RenderTriangle(SCENE2_GEOMETRY_TYPE eGeoType)
{
	model = translate[eGeoType] * rotate[eGeoType] * scale[eGeoType];
	MVP = projection * view * model;
	glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);
	
	glEnableVertexAttribArray(0); // 1st attribute buffer : vertices
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer[eGeoType]);
	glVertexAttribPointer(
		0,			// attribute 0. Must match the layout in the shader. Usually 0 is for vertex
		3,              // size
		GL_FLOAT,    // type
		GL_FALSE,    // normalized?
		0,           // stride
		0            // array buffer offset
	);

	glEnableVertexAttribArray(1); // 2nd attribute buffer : colors
	glBindBuffer(GL_ARRAY_BUFFER, m_colorBuffer[eGeoType]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glDrawArrays(GL_TRIANGLES, 0, 3); // Starting from vertex 0; 3 vertices = 1 triangle

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}

void Scene2::Exit()
{
	// Cleanup VBO here
	glDeleteBuffers(SCENE2_NUM_GEOMETRY, &m_vertexBuffer[0]);
	glDeleteBuffers(SCENE2_NUM_GEOMETRY, &m_colorBuffer[0]);
	glDeleteVertexArrays(1, &m_vertexArrayID);

	glDeleteProgram(m_programID);

}
