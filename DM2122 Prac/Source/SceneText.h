#ifndef SCENETEXT_H
#define SCENETEXT_H

#include "Scene.h"
#include <MatrixStack.h>
#include "Camera2.h"
#include "Mesh.h"
#include "Light.h"
#include "Vocation.h"
#include "Mechanictask.h"
#include "Boss.h"
//#include "StarterScene.h"
#include "CSalesCustomer.h"
#include "Cleanertask.h"

const int MECHANIC_GAME_MAX_SCORE = 10;
const int MECHANIC_GAME_MAX_LIVES = 3;
class SceneText : public Scene
{
	enum GEOMETRY_TYPE
	{
		GEO_AXES = 0,
		GEO_LEFT, 
		GEO_RIGHT, 
		GEO_TOP, 
		GEO_BOTTOM, 
		GEO_FRONT, 
		GEO_BACK,
		GEO_CHAR,
		GEO_DICE,
		GEO_LIGHTSPHERE,
		GEO_ACHIEVEMENTS,
		GEO_ACHIEVEMENTSBG,
		GEO_MECHANIC_GAME,
		GEO_FRAME,
		GEO_FRAME2,
		GEO_FRAME3,
		GEO_CUSTOMERLOGO,
		GEO_LITTER,
		GEO_TEXT,
		GEO_TEXTBOX,

		CLEANER_TOP,
		CLEANER_BOTTOM,
		CLEANER_WHEEL,
		CLEANER_WHEELJOINT,

		CUSTOMER_BODY,
		CUSTOMER_HEAD,
		CUSTOMER_SHOULDER,
		CUSTOMER_ARM,
		CUSTOMER_HAND,
		CUSTOMER_LEG,

		CUSTOMER2_BODY,
		CUSTOMER2_HEAD,
		CUSTOMER2_SHOULDER,
		CUSTOMER2_ARM,
		CUSTOMER2_HAND,
		CUSTOMER2_LEG,

		GUARD_BODY,
		GUARD_HEAD,
		GUARD_LEFTARM,
		GUARD_RIGHTARM,
		GUARD_LEG,

		SALE_BOTTOM,
		SALE_TOP,
		SALE_NECK,
		SALE_HEAD,
		SALE_LEFTARM,
		SALE_RIGHTARM,
		SALE_WHEEL,

		ITEM_INFORMATION_STAND,
		ITEM_INFORMATION_COUNTER,
		ITEM_BATTERY,
		ITEM_DISPLAY_STAND,
		ITEM_LIGHTBULB,
		ITEM_BOOTH,
		ITEM_FAE_BOOTH,
		ITEM_BENCH,

		CAR1_BODY,
		CAR1_STEERINGWHEEL,
		CAR1_WHEEL,

		CAR2_BODY,

		CAR3_BODY,

		CAR4_BODY,
		CAR4_LEFTWHEEL,
		CAR4_RIGHTWHEEL,

		CAR5_BODY,
		CAR5_WHEEL,

		CAR6_BODY,
		CAR6_LEFTWHEEL,
		CAR6_RIGHTWHEEL,

		CAR7_BODY,
		CAR7_STEERINGWHEEL,
		CAR7_FRONTWHEEL,
		CAR7_BACKWHEEL,

		CAR8_BODY,
		CAR8_STEERINGWHEEL,
		CAR8_WHEEL,

		CAR9_BODY,
		CAR9_WHEEL,

		CAR10_BODY,
		CAR10_WHEEL,

		CAR11_BODY,
		CAR11_WHEEL,

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
	
    int TotalBouncerTask;
	int CurrentBouncerTask;

	int CurrentSalespersonTask;

	int Price;
	
	float RSPEED = 45.f;
	
	float rotateCustomerHead = 0.0f;
	float rotateCustomerLeftArm = 0.0f;
	float rotateCustomerRightArm = 0.0f;
	float rotateCustomerLeftLeg = 0.0f;
	float rotateCustomerRightLeg = 0.0f;
	float LeftArmY = -1.8f;
	float LeftArmZ = 0.0f;
	
	float elapsedTime;
	float dialogueTime;
	float finalPosition;
	float middlePosition;
	float startingPosition;
	bool escapeanimation;
	bool textbox;

	bool AchievementScene;


	unsigned m_parameters[U_TOTAL];

	MS modelStack, viewStack, projectionStack;
	Light light[1];

	Camera2 camera;

	std::vector<Vocation::Quest> Day1;
	std::vector<Vocation::Quest> Day2;
	std::vector<Vocation::Quest> Day3;
	std::vector<Vector3> LitterLocations;

	Mechanictask* MechanicGameScore;

	Boss* BossOpinion;
	
	char game[10];

	float debounce;
	float elapsed;
	int count;
	int random;

	bool MechanicGame;
	bool FreezeMovement;
	bool hasmissed;
 
	float rotateCleanerWheelsForward = 0.0f;
	float rotateCleanerWheelsY = 0.0f; 
	float rotateCleanerTop = 0.0f;
	float rotateCleanerWheels = 0.0f;
	int randomLitter;
	bool CleanerGame;
	Cleanertask* CleanerScore;

	CSalesCustomer* salesCustomer;

	void RenderMesh(Mesh* mesh, bool enableLight);
	void RenderSkybox();
	void RenderCleanerRobot();
	void RenderCustomer();
	void RenderCustomer2();
	void RenderSuspect();
	void RenderGuardBot();
	void RenderSaleBot();
	void Renderlevel();
	void RenderLitter();

	void RenderCar1();
	void RenderCar2();
	void RenderCar3();
	void RenderCar4();
	void RenderCar5();
	void RenderCar6();
	void RenderCar7();
	void RenderCar8();
	void RenderCar9();
	void RenderCar10();
	void RenderCar11();

	void RenderBouncerTextBox();

	void RenderText(Mesh* mesh, std::string text, Color color);
	void RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y);
	void RenderTextOnScreenWithNewLine(Mesh* mesh, std::string text, Color color, float size, float x, float y, int characters, bool spacing);
	void RenderObjectOnScreen(Mesh* mesh, float size, float x, float y);
	void CalculateFrameRate();

public:
	SceneText();
	~SceneText();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();
};

#endif
