#ifndef STARTERSCENE_H
#define STARTERSCENE_H

#include "Scene.h"
#include <MatrixStack.h>
#include "Camera2.h"
#include "Mesh.h"
#include "Light.h"
#include "Vocation.h"
#include "VocationJob.h"


class StarterScene : public Scene
{
	enum GEOMETRY_TYPE
	{
		GEO_AXES = 0,
		GEO_TEXT,
		GEO_BACKGROUND,
		GEO_SCREEN,
		GEO_ENTRANCE,
		GEO_ENTRANCELEFT,
		GEO_ENTRANCERIGHT,
		GEO_CARLOGO,
		GEO_SAVESCENE,
		GEO_LOADSCREEN,


		GEO_LEFT,
		GEO_RIGHT,
		GEO_TOP,
		GEO_BOTTOM,
		GEO_FRONT,
		GEO_BACK,
		NUM_GEOMETRY,
	};

	enum Scene5_UNIFORM_TYPE
	{
		U_MVP = 0,
		U_MODELVIEW,
		U_MODELVIEW_INVERSE_TRANSPOSE,
		U_MATERIAL_AMBIENT,
		U_MATERIAL_DIFFUSE,
		U_MATERIAL_SPECULAR,
		U_MATERIAL_SHININESS,
		U_LIGHT0_POSITION,
		U_LIGHT0_COLOR,
		U_LIGHT0_POWER,
		U_LIGHT0_KC,
		U_LIGHT0_KL,
		U_LIGHT0_KQ,
		U_LIGHTENABLED,
		//add these enum in UNIFORM_TYPE before U_TOTAL
		U_LIGHT0_TYPE,
		U_LIGHT0_SPOTDIRECTION,
		U_LIGHT0_COSCUTOFF,
		U_LIGHT0_COSINNER,
		U_LIGHT0_EXPONENT,
		U_NUMLIGHTS,
		// add these enum for texture
		U_COLOR_TEXTURE_ENABLED,
		U_COLOR_TEXTURE,
		U_TEXT_ENABLED,
		U_TEXT_COLOR,
		U_TOTAL,
	};

private:
	unsigned m_vertexArrayID;
	unsigned m_programID;
	/*unsigned m_indexBuffer;*/
	Mesh* meshList[NUM_GEOMETRY];

	unsigned m_parameters[U_TOTAL];

	MS modelStack, viewStack, projectionStack;
	Light light[1];

	Camera2 camera;
	
	globalData* GlobalData;

	irrklang::ISoundEngine* SoundEngine = irrklang::createIrrKlangDevice();
	bool zoomPlaying;
	bool vroomPlaying;

	// cooldown
	float globalTime;
	float debounceTime;
	float animationTime; // only used on Vocation Selection or Entrance Anim
	
	// Scenes of Scene
	bool IntroScene;
	bool VocationScene;
	bool EntranceScene;
	bool SaveScene;

	void RenderMesh(Mesh* mesh, bool enableLight);
	void RenderSkybox();
	
	void RenderText(Mesh* mesh, std::string text, Color color);
	void RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y);
	void CustomRenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y, float angle, float angleX = 0.f, float angleY = 0.f, float angleZ = 1.f);
	void RenderObjectOnScreen(Mesh* mesh, float x, float y, float size);
	void CustomRenderObjectOnScreen(Mesh* mesh, float x, float y, float size, float angle, float angleX = 0.f, float angleY = 0.f, float angleZ = 1.f);
	void CalculateFrameRate();

	//Background Texture
	int background2;
	// Vocation Stuff
	VocationJob* JobSelection;
	int SalesPersonTexture;
	int CleanerTexture;
	int BouncerTexture;
	int MechanicTexture;
public:
	//std::vector<Vocation::Quest> Day1 = Vocation::getMainQuest(1);

	StarterScene();
	~StarterScene();
	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();
};

#endif
