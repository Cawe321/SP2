#ifndef SCENE_2_H
#define SCENE_2_H

#include "Scene.h"
#include <Mtx44.h>
#include "Camera.h"

enum SCENE2_GEOMETRY_TYPE
{
	SCENE2_GEO_TRIANGLE_1 = 0,
	SCENE2_GEO_TRIANGLE_2,
	SCENE2_NUM_GEOMETRY,
};

enum UNIFORM_TYPE
{
	U_MVP = 0,
	U_TOTAL,
};

class Scene2 : public Scene
{
private:
	unsigned m_vertexArrayID;
	unsigned m_vertexBuffer[SCENE2_NUM_GEOMETRY];
	unsigned m_colorBuffer[SCENE2_NUM_GEOMETRY];
	unsigned m_programID;
	unsigned m_indexBuffer;

	unsigned m_parameters[U_TOTAL];

	Mtx44 translate[SCENE2_NUM_GEOMETRY];
	Mtx44 rotate[SCENE2_NUM_GEOMETRY];
	Mtx44 scale[SCENE2_NUM_GEOMETRY];
	float m_fAngle;

	Mtx44 model;
	Mtx44 view;
	Mtx44 projection;
	Mtx44 MVP;

	Camera camera;
	
	void RenderTriangle(SCENE2_GEOMETRY_TYPE);


public:
	Scene2();
	~Scene2();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();
};

#endif