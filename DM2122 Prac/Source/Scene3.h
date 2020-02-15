#ifndef SCENE_3_H
#define SCENE_3_H

#include "Scene.h"
#include <Mtx44.h>
#include "Camera.h"
#include "Mesh.h"

class Scene3 : public Scene
{
	enum SCENE3_GEOMETRY_LIST
	{
		GEO_AXES,
		GEO_QUAD,
		GEO_CUBOID,
		NUM_GEOMETRY,
	};

	enum SCENE3_UNIFORM_TYPE
	{
		SCENE3_U_MVP = 0,
		SCENE3_U_TOTAL,
	};

private:
	unsigned m_vertexArrayID;
	//unsigned m_vertexBuffer[SCENE3_NUM_GEOMETRY];
	//unsigned m_colorBuffer[SCENE3_NUM_GEOMETRY];
	unsigned m_programID;
	/*unsigned m_indexBuffer;*/
	Mesh* meshList[NUM_GEOMETRY];

	unsigned m_parameters[SCENE3_U_TOTAL];

	Mtx44 translate;
	Mtx44 rotate;
	Mtx44 scale;

	Mtx44 model;
	Mtx44 view;
	Mtx44 projection;
	Mtx44 MVP;

	Camera camera;

public:
	Scene3();
	~Scene3();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();
};

#endif