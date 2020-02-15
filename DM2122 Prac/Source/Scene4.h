#ifndef SCENE_4_H
#define SCENE_4_H

#include "Scene.h"
#include <MatrixStack.h>
#include "Camera.h"
#include "Mesh.h"

class Scene4 : public Scene
{
	enum GEOMETRY_TYPE
	{
		GEO_AXES,
		GEO_SUN,
		GEO_PLANET1,
		//more planets
		GEO_RING1,
		//more rings
		GEO_MOON2,
		//more moons
		NUM_GEOMETRY,
	};

	enum Scene4_UNIFORM_TYPE
	{
		Scene4_U_MVP = 0,
		Scene4_U_TOTAL,
	};

private:
	unsigned m_vertexArrayID;
	//unsigned m_vertexBuffer[Scene4_NUM_GEOMETRY];
	//unsigned m_colorBuffer[Scene4_NUM_GEOMETRY];
	unsigned m_programID;
	/*unsigned m_indexBuffer;*/
	Mesh* meshList[NUM_GEOMETRY];

	unsigned m_parameters[Scene4_U_TOTAL];

	//Mtx44 translate;
	//Mtx44 rotate;
	//Mtx44 scale;

	//Mtx44 model;
	//Mtx44 view;
	//Mtx44 projection;
	//Mtx44 MVP;

	MS modelStack, viewStack, projectionStack;

	float rotateAngle;

	Camera camera;

public:
	Scene4();
	~Scene4();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();
};

#endif