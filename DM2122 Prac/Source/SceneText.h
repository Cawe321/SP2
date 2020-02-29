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
#include "globalData.h"
#include "CarSelection.h"
#include <string>

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
		GEO_BANKMENU,
		GEO_TEXT,
		GEO_TEXTBOX,

		CLEANER_TOP,
		CLEANER_BOTTOM,
		CLEANER_WHEEL,
		CLEANER_WHEELJOINT,
		
		PLAYER_BODY,
		PLAYER_HEAD,
		PLAYER_SHOULDER,
		PLAYER_ARM,
		PLAYER_HAND,
		PLAYER_LEG,
		
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
		
		SUSPECT1_BODY,
		SUSPECT1_HEAD,
		SUSPECT1_SHOULDER,
		SUSPECT1_ARM,
		SUSPECT1_HAND,
		SUSPECT1_LEG,

		SUSPECT2_BODY,
		SUSPECT2_HEAD,
		SUSPECT2_SHOULDER,
		SUSPECT2_ARM,
		SUSPECT2_HAND,
		SUSPECT2_LEG,

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
		ITEM_INFORMATION_STAND_2,
		ITEM_INFORMATION_STAND_3,
		ITEM_INFORMATION_STAND_4,
		ITEM_INFORMATION_STAND_5,
		ITEM_INFORMATION_STAND_6,
		ITEM_INFORMATION_STAND_7,
		ITEM_INFORMATION_STAND_8,
		ITEM_INFORMATION_STAND_9,
		ITEM_INFORMATION_STAND_10,
		ITEM_INFORMATION_STAND_11,
		ITEM_INFORMATION_STAND_12,
		ITEM_INFORMATION_STAND_13,
		ITEM_INFORMATION_COUNTER,
		ITEM_BATTERY,
		ITEM_DISPLAY_STAND,
		ITEM_LIGHTBULB,
		ITEM_BOOTH,
		ITEM_FAE_BOOTH,
		ITEM_BOOTH_3,
		ITEM_BOOTH_4,
		ITEM_BOOTH_5,
		ITEM_BENCH,
		ITEM_BENCH_2,
		ITEM_BENCH_3,
		ITEM_BENCH_4,
		ITEM_FLOOR,
		GEO_ENTRANCE,

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
		U_LIGHTENABLED,
		U_NUMLIGHTS,
		//add these enum in UNIFORM_TYPE before U_TOTAL

		U_LIGHT0_POSITION,
		U_LIGHT0_COLOR,
		U_LIGHT0_POWER,
		U_LIGHT0_KC,
		U_LIGHT0_KL,
		U_LIGHT0_KQ,
		U_LIGHT0_TYPE,
		U_LIGHT0_SPOTDIRECTION,
		U_LIGHT0_COSCUTOFF,
		U_LIGHT0_COSINNER,
		U_LIGHT0_EXPONENT,

		U_LIGHT1_POSITION,
		U_LIGHT1_COLOR,
		U_LIGHT1_POWER,
		U_LIGHT1_KC,
		U_LIGHT1_KL,
		U_LIGHT1_KQ,
		U_LIGHT1_TYPE,
		U_LIGHT1_SPOTDIRECTION,
		U_LIGHT1_COSCUTOFF,
		U_LIGHT1_COSINNER,
		U_LIGHT1_EXPONENT,

		U_LIGHT2_POSITION,
		U_LIGHT2_COLOR,
		U_LIGHT2_POWER,
		U_LIGHT2_KC,
		U_LIGHT2_KL,
		U_LIGHT2_KQ,
		U_LIGHT2_TYPE,
		U_LIGHT2_SPOTDIRECTION,
		U_LIGHT2_COSCUTOFF,
		U_LIGHT2_COSINNER,
		U_LIGHT2_EXPONENT,

		U_LIGHT3_POSITION,
		U_LIGHT3_COLOR,
		U_LIGHT3_POWER,
		U_LIGHT3_KC,
		U_LIGHT3_KL,
		U_LIGHT3_KQ,
		U_LIGHT3_TYPE,
		U_LIGHT3_SPOTDIRECTION,
		U_LIGHT3_COSCUTOFF,
		U_LIGHT3_COSINNER,
		U_LIGHT3_EXPONENT,

		U_LIGHT4_POSITION,
		U_LIGHT4_COLOR,
		U_LIGHT4_POWER,
		U_LIGHT4_KC,
		U_LIGHT4_KL,
		U_LIGHT4_KQ,
		U_LIGHT4_TYPE,
		U_LIGHT4_SPOTDIRECTION,
		U_LIGHT4_COSCUTOFF,
		U_LIGHT4_COSINNER,
		U_LIGHT4_EXPONENT,

		U_LIGHT5_POSITION,
		U_LIGHT5_COLOR,
		U_LIGHT5_POWER,
		U_LIGHT5_KC,
		U_LIGHT5_KL,
		U_LIGHT5_KQ,
		U_LIGHT5_TYPE,
		U_LIGHT5_SPOTDIRECTION,
		U_LIGHT5_COSCUTOFF,
		U_LIGHT5_COSINNER,
		U_LIGHT5_EXPONENT,

		U_LIGHT6_POSITION,
		U_LIGHT6_COLOR,
		U_LIGHT6_POWER,
		U_LIGHT6_KC,
		U_LIGHT6_KL,
		U_LIGHT6_KQ,
		U_LIGHT6_TYPE,
		U_LIGHT6_SPOTDIRECTION,
		U_LIGHT6_COSCUTOFF,
		U_LIGHT6_COSINNER,
		U_LIGHT6_EXPONENT,

		U_LIGHT7_POSITION,
		U_LIGHT7_COLOR,
		U_LIGHT7_POWER,
		U_LIGHT7_KC,
		U_LIGHT7_KL,
		U_LIGHT7_KQ,
		U_LIGHT7_TYPE,
		U_LIGHT7_SPOTDIRECTION,
		U_LIGHT7_COSCUTOFF,
		U_LIGHT7_COSINNER,
		U_LIGHT7_EXPONENT,

		/*U_LIGHT8_POSITION,
		U_LIGHT8_COLOR,
		U_LIGHT8_POWER,
		U_LIGHT8_KC,
		U_LIGHT8_KL,
		U_LIGHT8_KQ,
		U_LIGHT8_TYPE,
		U_LIGHT8_SPOTDIRECTION,
		U_LIGHT8_COSCUTOFF,
		U_LIGHT8_COSINNER,
		U_LIGHT8_EXPONENT,*/

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
	
	irrklang::ISoundEngine* soundEngine = irrklang::createIrrKlangDevice();
	
	irrklang::ISound* LoadingMusic;
	irrklang::ISound* CleanerSound;
	irrklang::ISound* MechanicSound;
	float CleanerDebounce;
	bool DayEndedSound;

    int TotalBouncerTask;
	int CurrentBouncerTask;

	int CurrentSalespersonTask;

	int Price;
	int no;
	
	bool BankOpen;
	bool NotEnough;
	bool HasCars;
	bool IsReserved;
	bool IsOwned;
	CarSelection* Selection;

	float RSPEED = 45.f;
	
	float rotateCustomerHead = 0.0f;
	float rotateCustomerLeftArm = 0.0f;
	float rotateCustomerRightArm = 0.0f;
	float rotateCustomerLeftLeg = 0.0f;
	float rotateCustomerRightLeg = 0.0f;
	float LeftArmY = -1.8f;
	float LeftArmZ = 0.0f;
	
	bool cameraLockToPlayer;
	float cameraY;
	float cameraZ;
	float rotatePlayerHead = 0.0f;
	float rotatePlayerLeftArm = 0.0f;
	float rotatePlayerRightArm = 0.0f;
	float rotatePlayerLeftLeg = 0.0f;
	float rotatePlayerRightLeg = 0.0f;
	float movePlayerX = 0.0f;
	float movePlayerZ = 0.0f;
	float rotatePlayer = 0.0f;
	
	float rotatecar = 0.0f;
	
	float passedTime = 0.0f;
	float timeDisappeared = 0.0f;

	float elapsedTime;
	float dialogueTime;
	float finalPosition;
	float middlePosition;
	float startingPosition;
	float debounceTime;
	bool escapeanimation;
	bool secondescpaeanimation;
	bool thirdescapeanimation;
	bool textbox;
	
	int SalesPersonSalary;
	int CleanerSalary;
	int BouncerSalary;
	int MechanicSalary;
	
	bool GuardBotInteraction;
	

	bool AchievementScene;
	bool GameScene;
	bool DayEnds;
	
	bool CarAchievement1;
	bool CarAchievement2;
	bool CarAchievement3;
        
	globalData* timeData = globalData::instance();
	globalData* dayData = globalData::instance();

	unsigned m_parameters[U_TOTAL];

	MS modelStack, viewStack, projectionStack;
	Light light[8];

	Camera2 camera;

	std::vector<Vocation::Quest> Day1;
	std::vector<Vocation::Quest> Day2;
	std::vector<Vocation::Quest> Day3;

	std::vector<Vector3> LitterLocations;
	std::vector<Vector3> Day1Litter;
	std::vector<Vector3> Day2Litter;
	std::vector<Vector3> Day3Litter;
	float LitterX, LitterY, LitterZ;

	Mechanictask* MechanicGameScore;

	Boss* BossOpinion;
	
	char game[10];

	float debounce;
	float elapsed;
	int count;
	int random;

	bool MechanicGame;
	bool FreezeMovement;
	bool hasreset;
	bool gameover;
	bool hasmissed;
 
	float rotateCleanerWheelsForward = 0.0f;
	float rotateCleanerWheelsY = 0.0f; 
	float rotateCleanerTop = 0.0f;
	float rotateCleanerWheels = 0.0f;
	float rotateCleaner = 0.0f;
	int randomLitter;
	float moveX = 0.0f;
	float moveZ = 0.0f;
	bool CleanerGame;
	Cleanertask* CleanerScore;

	CSalesCustomer* salesCustomer;
	struct customerPositioning
	{
		Vector3 position;
		int customerType;
		float angle;
		bool inRange;
		bool isCurious;
		bool selected;
	};
	customerPositioning customerLocations[10];

	void RenderMesh(Mesh* mesh, bool enableLight);
	void RenderSkybox();
	void RenderCleanerRobot();
	void RenderCleanerRobotWithoutRotation();
	void RenderPlayer();
	void RenderCustomer();
	void RenderCustomer2();
	void RenderSuspect();
	void RenderSuspect2();
	void RenderSuspect3();
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
