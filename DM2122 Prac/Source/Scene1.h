#ifndef SCENE_1_H
#define SCENE_1_H

#include "Scene.h"

enum GEOMETRY_TYPE
{
	GEO_TRIANGLE_1 = 0,
	NUM_GEOMETRY,
};


class Scene1 : public Scene
{
private:
	unsigned m_vertexArrayID;
	unsigned m_vertexBuffer[NUM_GEOMETRY];
	unsigned m_colorBuffer[NUM_GEOMETRY];
	unsigned m_programID;

public:
	Scene1();
	~Scene1();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();
};

#endif