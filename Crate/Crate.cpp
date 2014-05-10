//=============================================================================
// Create.cpp by Frank Luna (C) 2008 All Rights Reserved.
//
// Shows how to texture a box to make it look like a crate.
//
// Controls:
//		'A'/'D'/'W'/'S' - Rotate 
//              'Z'/'X' - Zoom
//
//=============================================================================

#include "d3dApp.h"
#include "Light.h"
#include "Box.h"
#include "GameObject.h"
#include "Bullet.h"
#include "Layer.h"
#include "Mesh.h"
#include "constants.h"
#include "Splashscreen.h"
#include <sstream>
#include <string>
#include <vector>
#include <fstream>
#include "audio.h"
#include "Laser.h"
#include "Sphere.h"
#include "Sentry.h"

// Utility functions for generating meshes
std::vector<Vector3> generateSurfrev2D(float degreesY); // generates a x-z cross-sectional "slice" (of specified degrees around the y-axis) of a unit spherical shell (thickness 0.1)
void generateSurfrev3D(std::vector<Vector3> polygon, float degreesZ, Mesh &mesh); // rotates a polygon in the x-y plane to create a 3-D mesh

class CrateApp : public D3DApp
{
public:
	CrateApp(HINSTANCE hInstance);
	~CrateApp();

	void initApp();
	void onResize();
	void updateScene(float dt);
	void drawScene(); 

	void regenerateWalls(float dt);
	void reinitialize();
	void bulletWallCollision();
	void laserWallCollision();
	void bulletSentryCollision();

private:
	void buildFX();
	void buildVertexLayouts();

private:
	Audio *audio;

	Box mCrateMesh;
	Box target1, bullet;
	GameObject boss;	
	GameObject health[3];

	//GameObject laser;
	Laser laser;
	//Laser wallOfLasers[8];
	

	Layer layers[NUM_LAYERS];

	//Sphere sentry;

	GameObject background[6];

	Splashscreen introIntroSplashscreen;
	Splashscreen introSplashscreen;
	Splashscreen restartSplashscreen;
	Splashscreen nextLevelSplashscreen;
	Splashscreen endSplashscreen;

	//ID3D10ShaderResourceView* mDiffuseMapRV_IntroScreen;

	ID3D10ShaderResourceView* mBackgroundRV;
	ID3D10ShaderResourceView* mIntroIntroRV;
	ID3D10ShaderResourceView* mIntroRV;
	ID3D10ShaderResourceView* mRestartRV;
	ID3D10ShaderResourceView* mNextLevelRV;
	ID3D10ShaderResourceView* mEndRV;

	Bullet bulletObject;

	Light mParallelLight;
	Light mBossEmissive;
	Light mSpot;

	ID3D10Effect* mFX;
	ID3D10EffectTechnique* mTech;
	ID3D10InputLayout* mVertexLayout;
	ID3D10ShaderResourceView* mDiffuseMapRV;
	ID3D10ShaderResourceView* mSpecMapRV;

	ID3D10EffectMatrixVariable* mfxWVPVar;
	ID3D10EffectMatrixVariable* mfxWorldVar;
	ID3D10EffectVariable* mfxEyePosVar;
	ID3D10EffectVariable* mfxLightVar; // parallel light
	ID3D10EffectVariable *mfxBossEmissiveLightVar;
	ID3D10EffectVariable *mfxSpotLightVar;
	ID3D10EffectShaderResourceVariable* mfxDiffuseMapVar;
	ID3D10EffectShaderResourceVariable* mfxSpecMapVar;
	ID3D10EffectMatrixVariable* mfxTexMtxVar;
	// additional effect variables for color changing
	ID3D10EffectVariable *mfxOverrideColorFlag;
	ID3D10EffectVectorVariable *mfxObjectColor;
	ID3D10EffectVariable *mfxAmbientOnlyFlag;
	ID3D10EffectVectorVariable *mfxTintOffset;

	D3DXMATRIX mCrateWorld;

	D3DXMATRIX mView;
	D3DXMATRIX mProj;
	D3DXMATRIX mWVP;

	D3DXVECTOR3 mEyePos;
	float mRadius;
	float mTheta;
	float mPhi;

	//float laserTheta;
	//float laserPhi;

	float spinAmount;

	std::wstring stats;

	//bool fireLaser;
	//float laserTimer;

	int bossHealth;
	float playerHealth;

	bool spacePressedLastFrame;
	bool enterPressedLastFrame;

	int level;

	State state;

	float bossDyingTimer;
	bool bossDead;

	float introMusicTimer;

	// BOSS AND SENTRIES
	Mesh mesh;
	Sentry sentries[NUM_SENTRIES_LVL3];
	Laser sentryLasers[NUM_SENTRIES_LVL3];
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
	PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif


	CrateApp theApp(hInstance);

	theApp.initApp();

	return theApp.run();
}

CrateApp::CrateApp(HINSTANCE hInstance)
	: D3DApp(hInstance), mFX(0), mTech(0), mfxWVPVar(0), mfxWorldVar(0), mfxEyePosVar(0),
	mfxLightVar(0), mfxDiffuseMapVar(0), mfxSpecMapVar(0), mfxTexMtxVar(0), 
	mVertexLayout(0), mDiffuseMapRV(0), mSpecMapRV(0), mEyePos(0.0f, 0.0f, 0.0f), 
	mRadius(PLAYER_RADIUS), mTheta(0.0f), mPhi(PI/2), spinAmount(0), bossHealth(3),
	spacePressedLastFrame(false), level(1), bossDyingTimer(0), bossDead(false), enterPressedLastFrame(false),
	introMusicTimer(0), playerHealth(100)
{
	D3DXMatrixIdentity(&mCrateWorld);
	D3DXMatrixIdentity(&mView);
	D3DXMatrixIdentity(&mProj);
	D3DXMatrixIdentity(&mWVP); 

	//laserTheta = mTheta;
	//laserPhi = mPhi;

	state = intro;
}

CrateApp::~CrateApp()
{
	if( md3dDevice )
		md3dDevice->ClearState();

	ReleaseCOM(mFX);
	ReleaseCOM(mVertexLayout);
	ReleaseCOM(mDiffuseMapRV);
	ReleaseCOM(mSpecMapRV);
	ReleaseCOM(mBackgroundRV);

	delete audio;
}

void CrateApp::initApp()
{
	// DEBUG
	std::vector<Vector3> output = generateSurfrev2D(180);
	generateSurfrev3D(output, 360, mesh);
	for(int i=0; i<NUM_SENTRIES_LVL3; i++)
		generateSurfrev3D(output, 360, sentries[i]);

	D3DApp::initApp();

	// init sound system
    audio = new Audio();
	HRESULT hr;
    if (*WAVE_BANK != '\0' && *SOUND_BANK != '\0')  // if sound files defined
    {
        if( FAILED( hr = audio->initialize() ) )
        {
			// throwing strings since we don't have the GameError class from last semester
            if( hr == HRESULT_FROM_WIN32( ERROR_FILE_NOT_FOUND ) )
                throw(std::string("Failed to initialize sound system because media file not found."));
            else
                throw(std::string("Failed to initialize sound system."));
        }
    }

	mClearColor = D3DXCOLOR(0.9f, 0.9f, 0.9f, 1.0f);

	buildFX();
	buildVertexLayouts();

	mCrateMesh.init(md3dDevice, 1.0f);

	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice,
		L"walltexture2.jpg", 0, 0, &mDiffuseMapRV, 0 ));

	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice,
		L"defaultspec.dds", 0, 0, &mSpecMapRV, 0 ));

	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice,
		L"heic1215b.jpg", 0, 0, &mBackgroundRV, 0 ));

	//HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice,
	//	L"#selfie.jpg", 0, 0, &mDiffuseMapRV_IntroScreen, 0 ));

	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
		L"introintro.png", 0, 0, &mIntroIntroRV, 0 ));

	introIntroSplashscreen.init(md3dDevice, 7.75f, mIntroIntroRV, mSpecMapRV, mTech);

	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
		L"INTRO.png", 0, 0, &mIntroRV, 0 ));

	introSplashscreen.init(md3dDevice, 7.75f, mIntroRV, mSpecMapRV, mTech);

	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
		L"retry.png", 0, 0, &mRestartRV, 0 ));

	restartSplashscreen.init(md3dDevice, 7.75f, mRestartRV, mSpecMapRV, mTech);

	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
		L"transition.png", 0, 0, &mNextLevelRV, 0 ));

	nextLevelSplashscreen.init(md3dDevice, 7.75f, mNextLevelRV, mSpecMapRV, mTech);

	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
		L"endscreen.png", 0, 0, &mEndRV, 0 ));

	endSplashscreen.init(md3dDevice, 7.75f, mEndRV, mSpecMapRV, mTech);

	mParallelLight.dir      = D3DXVECTOR3(0.57735f, -0.57735f, 0.57735f);
	mParallelLight.ambient  = D3DXCOLOR(0.1f, 0.1f, 0.1f, 1.0f);
	mParallelLight.diffuse  = D3DXCOLOR(0.45f, 0.45f, 0.45f, 1.0f);
	mParallelLight.specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	// Note: the boss emissive light didn't work out as well as hoped, so this light has been re-purposed as another
	// parallel source, to provide more even lighting for the background.
	mBossEmissive.dir		= D3DXVECTOR3(-.57735f,.57735f,-.57735f);
	mBossEmissive.ambient	= D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
	mBossEmissive.diffuse	= D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	mBossEmissive.specular	= D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	// Spotlight - from player position, aimed at boss (position/direction updated in update())
	// *** Turned off at the moment - it didn't really seem to add much to the scene; going with brighter diffuse lighting
	// instead produced a better effect.
	mSpot.ambient  = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
	mSpot.diffuse  = D3DXCOLOR(0,0,0,1);//D3DXCOLOR(10.0f, 10.0f, 10.0f, 1.0f);
	mSpot.specular = D3DXCOLOR(1,1,1,1);//D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	mSpot.att.x    = 0.01f;
	mSpot.att.y    = 0.01f;
	mSpot.att.z    = 0.01f;
	mSpot.spotPow  = 20.0f;
	mSpot.range    = 40.0f;


	bullet.init(md3dDevice, 1.0f);

	// BOSS
	mesh.init(md3dDevice, 2.0, Vector3(1,0,0), Vector3(0,0,0), 0, 2, 0, 0); //phi and theta set to 0 -- boss won't use them
	mesh.setOverrideColorVar(mfxOverrideColorFlag);
	mesh.setObjectColorVar(mfxObjectColor);
	mesh.setColor(D3DXCOLOR(0, 1, 0, 1));


	//boss.init(&bullet, sqrt(2.0f), D3DXVECTOR3(0,0,0), D3DXVECTOR3(0,0,0), 10,1);
	health[0].init(&bullet, sqrt(2.0f), D3DXVECTOR3(0,0,0), D3DXVECTOR3(-10,0,10), 10,1);

	laser.init(&bullet, sqrt(2.0f), D3DXVECTOR3(0,0,0), D3DXVECTOR3(0,0,0), 10,.05,.05,mRadius*2, mPhi, ((int)(mTheta+PI)%6));
	laser.setOverrideColorVar(mfxOverrideColorFlag);
	laser.setObjectColorVar(mfxObjectColor);
	laser.setColor(D3DXCOLOR(1, 0, 0, 1));
	laser.setActive();
	//laser.setPhi(mPhi);
	//laser.setTheta((int)(mTheta+PI)%6);
	//laser.setInActive();

	for(int i=0; i<NUM_SENTRIES_LVL3; i++)
	{
		//SENTRY
		sentries[i].init(md3dDevice, 1.0, Vector3(1,0,-SENTRY_RADIUS), Vector3(0,0,0), 0, 1, PI/2, PI);
		sentries[i].setOverrideColorVar(mfxOverrideColorFlag);
		sentries[i].setObjectColorVar(mfxObjectColor);
		sentries[i].setColor(D3DXCOLOR(1, 1, 1, 1));
		//wallOfLasers[i].init(&bullet, sqrt(2.0f), D3DXVECTOR3(0,0,0), D3DXVECTOR3(0,0,0), 10,.05,.05,mRadius*2, i*PI/8, ((int)(mTheta+PI)%6));
		sentryLasers[i].init(&bullet, sqrt(2.0f), D3DXVECTOR3(0,0,-SENTRY_RADIUS), D3DXVECTOR3(0,0,0), 10,.05,.05,mRadius*2, PI/2, PI);
		sentryLasers[i].setOverrideColorVar(mfxOverrideColorFlag);
		sentryLasers[i].setObjectColorVar(mfxObjectColor);
		sentryLasers[i].setColor(D3DXCOLOR(221/255.0f, 0, 1, 1));
		sentryLasers[i].setActive();
	}

	//background init
	background[0].init(&bullet, 1.0f, D3DXVECTOR3(100.0f,0.0f,0.0f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0, 50);
	background[1].init(&bullet, 1.0f, D3DXVECTOR3(-100.0f,0.0f,0.0f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0, 50);
	background[2].init(&bullet, 1.0f, D3DXVECTOR3(0.0f,100.0f,0.0f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0, 50);
	background[3].init(&bullet, 1.0f, D3DXVECTOR3(0.0f,-100.0f,0.0f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0, 50);
	background[4].init(&bullet, 1.0f, D3DXVECTOR3(0.0f,0.0f,100.0f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0, 50);
	background[5].init(&bullet, 1.0f, D3DXVECTOR3(0.0f,0.0f,-100.0f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0, 50);

	//LAYERS:
	//SPECIFY ROTATION AXIS AND RADIUS IN CONSTRUCTOR
	layers[0] = Layer(rotationAxis::Y, 5);
	layers[1] = Layer(rotationAxis::Z, 5);
	layers[2] = Layer(rotationAxis::X, 5);
	layers[3] = Layer(rotationAxis::YZ, 5);
	layers[4] = Layer(rotationAxis::ZY, 5);
	for(int i=0; i<NUM_WALLS; i++)
	{
		layers[0].walls[i].init(&bullet, sqrt(2.0f), D3DXVECTOR3(0,0,0), D3DXVECTOR3(0,0,0), 10,.1,WALL_SIZE,WALL_SIZE);
		layers[1].walls[i].init(&bullet, sqrt(2.0f), D3DXVECTOR3(0,0,0), D3DXVECTOR3(0,0,0), 10,WALL_SIZE,.1,WALL_SIZE);
		layers[2].walls[i].init(&bullet, sqrt(2.0f), D3DXVECTOR3(0,0,0), D3DXVECTOR3(0,0,0), 10,WALL_SIZE,WALL_SIZE,.1);
		layers[3].walls[i].init(&bullet, sqrt(2.0f), D3DXVECTOR3(0,0,0), D3DXVECTOR3(0,0,0), 10,WALL_SIZE,WALL_SIZE,.1);
		layers[4].walls[i].init(&bullet, sqrt(2.0f), D3DXVECTOR3(0,0,0), D3DXVECTOR3(0,0,0), 10,WALL_SIZE,WALL_SIZE,.1);
		layers[0].walls[i].mfxTintOffsetVar = mfxTintOffset;
		layers[1].walls[i].mfxTintOffsetVar = mfxTintOffset;
		layers[2].walls[i].mfxTintOffsetVar = mfxTintOffset;
		layers[3].walls[i].mfxTintOffsetVar = mfxTintOffset;
		layers[4].walls[i].mfxTintOffsetVar = mfxTintOffset;
		layers[0].walls[i].tint = true;
		layers[1].walls[i].tint = true;
		layers[2].walls[i].tint = true;
		layers[3].walls[i].tint = true;
		layers[4].walls[i].tint = true;
	}

	

	//bullet1[0].init(&bullet, 1, D3DXVECTOR3(7,1,7), D3DXVECTOR3(0,0,0), 10,.5,.05,.05);
	//bullet1[1].init(&bullet, 1, D3DXVECTOR3(7,1,7), D3DXVECTOR3(0,0,0), 10,.05,.5,.05);
	//bullet1[2].init(&bullet, 1, D3DXVECTOR3(7,1,7), D3DXVECTOR3(0,0,0), 10,.05,.05,.5);
	//for(int i=0; i<3; i++)
	//	bullet1[i].setInActive();
	bulletObject.init(&bullet, 1, D3DXVECTOR3(7,1,7), D3DXVECTOR3(0,0,0), 10, 1); // initialized as inactive	


	/*sentry.init(md3dDevice, 1.0f, D3DXCOLOR(1,1,1,0), 5);
	sentry.setPosition(Vector3(0,0,15));
	sentry.setActive();*/



	//just for testing purposes -- remove later
	//laser.setInActive();
}

void CrateApp::onResize()
{
	D3DApp::onResize();

	float aspect = (float)mClientWidth/mClientHeight;
	D3DXMatrixPerspectiveFovLH(&mProj, 0.25f*PI, aspect, 1.0f, 1000.0f);
}

void CrateApp::reinitialize()
{
	spacePressedLastFrame = false;
	//enterPressedLastFrame = false;
	//reset: player health, boss health, phis and thetas of laser and cam, setactive all walls, set their timers all to zero, 
	//set laser to inactive and its timer to 0 (actually set mPhi to PI/2)

	introMusicTimer = 0;
	mPhi = PI/2;
	mTheta = 0;
	//laserTheta = 0;
	//laserPhi = 0;
	laser.setPhi(0);
	laser.setTheta((int)(mTheta+PI)%6);
	laser.setActive();
	playerHealth = 100;

	for(int i=0; i<NUM_SENTRIES_LVL3; i++)
	{
		//sentryLasers[i].setPhi(i*PI/8);
		//sentryLasers[i].setTheta((int)(mTheta+PI)%6);
		//sentryLasers[i].setActive();
	}

	//playerHealth = 
	
	for(int j=0; j<NUM_LAYERS; j++)
	{
		for(int i=0; i<NUM_WALLS; i++)
		{
			//Matrix rotations[NUM_WALLS];
			//Matrix spin, translate, diagonal;
			//GameObject walls[NUM_WALLS];
			//float thetas[NUM_WALLS];
			//float phis[NUM_WALLS];
			//float startingTheta[NUM_WALLS];
			//float startingPhi[NUM_WALLS];
			layers[j].regenTime[i] = 0;
			layers[j].walls[i].setActive();
			layers[j].wallHealth[i] = 2;

			layers[j].walls[i].tintOffset = D3DXCOLOR(0,0,0,0); // reset tint
		}
	}

	//laser.setInActive();
	//laserTimer = 0;
	

	if(level==1)
	{
		bossHealth = 3;
		laser.setSpeed(LASER_SPEED_LVL1);
	}
	else if(level==2)
	{
		bossHealth = 5;
		laser.setSpeed(LASER_SPEED_LVL2);
	}

	bossDyingTimer = 0;
	bossDead = false;
	mesh.setActive();
	bulletObject.setInActive();

}

void CrateApp::updateScene(float dt)
{
	D3DApp::updateScene(dt);
	//update once
	for(int i=0; i<6; i++) {
		background[i].update(dt);
	}

	switch(state)
	{
	case intro:
		{
			if(!enterPressedLastFrame && GetAsyncKeyState(VK_RETURN) & 0x8000) 
			{
				state = start;
				enterPressedLastFrame = true;
				//PLAY INTRO MUSIC
				//audio->playCue(INTROMUSIC);
			}
			else if(!GetAsyncKeyState(VK_RETURN)) enterPressedLastFrame = false;

			break;
		}
	case start:
		{
			if(!enterPressedLastFrame && GetAsyncKeyState(VK_RETURN) & 0x8000) 
			{
				state = game;
				enterPressedLastFrame = true;
			}
			else if(!GetAsyncKeyState(VK_RETURN)) enterPressedLastFrame = false;

			/*introMusicTimer += dt;
			if(introMusicTimer > 12.5)
			{
				audio->playCue(LASER);
				state = game;
			}*/

			break;
		}
	case game:
		{
			//REMOVE THIS LATER -- FOR TESTING PURPOSES
			laser.setInActive();


			// DEBUG
			mesh.update(dt);
			for(int i=0; i<NUM_SENTRIES_LVL3; i++)
				sentries[i].update(dt);

			for(int i=0; i<3; i++)
				health[0].update(dt);

			if(!spacePressedLastFrame && !bulletObject.getActiveState() && GetAsyncKeyState(' ') & 0x8000)
			{
				//if(!bullet1[0].getActiveState())
				//{
				//for(int i=0; i<3; i++)
				//{
				//	bullet1[i].setActive();
				//	bullet1[i].setPosition(mEyePos);
				//	//calculate velocity vector
				//	Vector3 t(-mEyePos);
				//	bullet1[i].setVelocity(t*2);
				//}
				//	}
				bulletObject.shoot(mEyePos/2,2*Vector3(-mEyePos), mTheta, mPhi);
				//audio->playCue(BULLETSHOOT);
			}

			if(GetAsyncKeyState(' ') & 0x8000) spacePressedLastFrame = true;
			else spacePressedLastFrame = false;

			spinAmount += 1*dt;
			if (spinAmount*10 > 360)
				spinAmount = 0;

			//UPDATE LAYERS:
			for(int i=0; i<5; i++)
			{
				//if(level == 1)
				//	layers[i].updateMatrices(0);
				//else if(level == 2)
				layers[i].updateMatrices(spinAmount);
			}
			for(int i=0; i<NUM_LAYERS; i++)
			{
				for(int j=0; j<NUM_WALLS; j++)
				{
					layers[i].walls[j].update(dt);
					/*if(bulletObject.collided(&layers[i].walls[j]))
					{
					layers[i].walls[j].setInActive();
					}*/
				}
			}

			//for(int i=0; i<3; i++)
			//	bullet1[i].update(dt);
			bulletObject.update(dt);
			boss.update(dt);

			//BULLET COLLISION ON BOSS

			/*if(bulletObject.collided(&boss))
			{
			bossHealth--;
			if(bossHealth == 0) boss.setInActive();*/
			if(bulletObject.collided(&mesh))
			{
				bossHealth--;
				bulletObject.setInActive();
				if(bossHealth == 0)
				{
					mesh.setInActive();
					bossDead = true;
					laser.setInActive();
					bulletObject.setInActive();
					//PLAY SOUND
					//audio->stopCue(LASER);
					//audio->playCue(BOSSDYING);
				}
			}

			//UPDATE BOSS COLOR
			if(bossHealth==5) mesh.setColor(D3DXCOLOR(1, 0.25, 0, 1)); //start at bright red and fade to black as health decreases
			else if(bossHealth==4) mesh.setColor(D3DXCOLOR(1, 0, 0, 1));
			else if(bossHealth==3) mesh.setColor(D3DXCOLOR(0.75, 0, 0, 1));
			else if(bossHealth==2) mesh.setColor(D3DXCOLOR(0.5, 0, 0, 1));
			else if(bossHealth==1) mesh.setColor(D3DXCOLOR(0.25, 0, 0, 1));
			else if(bossHealth==0) mesh.setColor(D3DXCOLOR(0, 0, 0, 1));

			//WIN 
			if(bossDead)
			{
				bossDyingTimer += dt;
				if(bossDyingTimer > 2) //CHANGE TO LENGTH OF SOUND
				{
					if(level==1) 
					{
						level = 2;
						reinitialize();
						state = nextLevel;
						
					}
					else if(level==2)
					{
						reinitialize();
						//audio->stopCue(LASER);
						//audio->playCue(ENDMUSIC);
						state = end;
					}
				}
			}

			//LOSE
			if(laser.getActiveState() && abs(laser.getTheta()-mTheta)<.1 && abs(laser.getPhi()-mPhi)<.1)
			{
				if(level==1)
					playerHealth -= dt*75;
				else playerHealth -= dt*100;
			}
			else if(playerHealth < 100) {
				if(level==1) playerHealth += dt*45;
				else playerHealth += dt*10;
			}
			if(playerHealth > 100) playerHealth = 100;
			if(playerHealth <= 0) 
			{
				state = restart;
				reinitialize();
				//audio->stopCue(LASER);
			}

			//BULLET COLLISION ON WALLS AND SENTRIES
			bulletWallCollision();
			bulletSentryCollision();

			//LASER COLLISION ON WALLS
			laserWallCollision();

			//REGENERATION OF WALLS:
			regenerateWalls(dt);



			//CONTROLS WHEN LASER DISPLAYS
			/*if(!fireLaser)
			{
				if(rand()%4000==0)
				{
					fireLaser = true;
					laser.setActive();
					laserTheta = (int)(mTheta+PI)%6;
				}
			}
			else
			{*/
				laser.update(dt);
				laser.trackPlayer(mTheta, mPhi);
				//laserTimer += dt;
				//at end of timer,set timer back to zero, set laser inactive
				/*if(laserTimer > 100)
				{
					fireLaser = false;
					laserTimer = 0;
					laser.setInActive();
				}*/
			//}



			//SENTRY ROTATION
			sentryLasers[0].setTheta(sentryLasers[0].getTheta()+LASER_SPEED_LVL1);
			//sentryLasers[0].setPhi(sentryLasers[0].getPhi()+.001);
			sentryLasers[1].setTheta(sentryLasers[1].getTheta()+LASER_SPEED_LVL1);
	
			float t = ToRadian(spinAmount*30) + PI/2; //PI/2 is "starting phi" for this sentry
			int temp = static_cast<int>(t/(2*PI));
			float tempPhi = t - static_cast<float>(temp)*(2*PI);
			if(tempPhi > PI)
				sentryLasers[1].setPhi(2*PI - tempPhi); //IF ON RIGHT SIDE GOING BACK UP
			else 
				sentryLasers[1].setPhi(tempPhi);

			//Moves the sentries and their lasers
			for(int i=0; i<NUM_SENTRIES_LVL3; i++)
			{
				sentryLasers[i].update(dt);
				sentries[i].setTheta(sentryLasers[i].getTheta());
				sentries[i].setPhi(sentryLasers[i].getPhi());
			}


			
			// Update angles based on input to orbit camera around scene.
			if(GetAsyncKeyState('A') & 0x8000)	mTheta += 3.0f*dt;
			if(GetAsyncKeyState('D') & 0x8000)	mTheta -= 3.0f*dt;
			if(GetAsyncKeyState('W') & 0x8000)	mPhi -= 3.0f*dt;
			if(GetAsyncKeyState('S') & 0x8000)	mPhi += 3.0f*dt;
			//if(GetAsyncKeyState('Z') & 0x8000)	mRadius -= 15.0f*dt;
			//if(GetAsyncKeyState('X') & 0x8000)	mRadius += 15.0f*dt;

			// Restrict the angle mPhi.
			if( mPhi < 0.1f )	mPhi = 0.1f;
			if( mPhi > PI-0.1f)	mPhi = PI-0.1f;

			if( mTheta < 0.01f )	mTheta = 2*PI-.02f;
			if( mTheta > 2*PI-0.01f)	mTheta = .02f;

			break;
		}
	case restart:
		{
			//UPDATE THE RESTART SCREEN CUBE
			if(GetAsyncKeyState(VK_RETURN) & 0x8000) 
			{
				//audio->playCue(LASER);
				state = game;
				reinitialize();
			}

			break;
		}
	case nextLevel:
		{
			if(GetAsyncKeyState(VK_RETURN) & 0x8000) 
			{
				//audio->playCue(LASER);
				state = game;
				reinitialize();
			}
			break;
		}
	case end:
		{
			//UPDATE CUBE FOR ENDING SPLASHSCREEN
			if(!enterPressedLastFrame && GetAsyncKeyState(VK_RETURN) & 0x8000) 
			{
				//audio->stopCue(ENDMUSIC);
				state = intro;
				level = 1;
				enterPressedLastFrame = true;
			}
			else if(!GetAsyncKeyState(VK_RETURN)) enterPressedLastFrame = false;
			break;
		}
	}

	// Convert Spherical to Cartesian coordinates: mPhi measured from +y
	// and mTheta measured counterclockwise from -z.
	mEyePos.x =  mRadius*sinf(mPhi)*sinf(-mTheta);
	mEyePos.z = -mRadius*sinf(mPhi)*cosf(-mTheta);
	mEyePos.y =  mRadius*cosf(mPhi);
	
	// Build the view matrix.
	D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
	D3DXMatrixLookAtLH(&mView, &mEyePos, &target, &up);

	// Update position of spotlight based on eye position
	mSpot.pos = mEyePos;
	Normalize(&mSpot.dir, &(Vector3(0,0,0)-mEyePos));
}

void CrateApp::bulletSentryCollision()
{
	if(bulletObject.getActiveState())
		{
			for(int i=0; i<NUM_SENTRIES_LVL3; i++)
			{
				if(sentries[i].getActiveState())
				{
					//if bullet Phi is close to either 0 or PI, ignore theta comparisons
					if(bulletObject.getPhi() < .3 || bulletObject.getPhi() > 2.7)
					{
						if((abs(bulletObject.getPhi() - sentryLasers[i].getPhi()) < .3) && (abs(bulletObject.getDistanceToOrigin() - SENTRY_RADIUS) < .5))
						{
							//decrease sentry health
							bulletObject.setInActive();
							//audio
							//change color of sentry?
							sentryLasers[i].setInActive();
							sentries[i].setInActive();
							return;
						}
					}
					else {
						//if((abs(bulletObject.getTheta() - layers[j].thetas[i]) < .43) && (abs(bulletObject.getPhi() - layers[j].phis[i]) < .3) && (abs(bulletObject.getDistanceToOrigin() - layers[j].radius) < .5))
						if(((abs(bulletObject.getTheta() - sentryLasers[i].getTheta()) < .43) || (abs(bulletObject.getTheta() - 2*PI - sentryLasers[i].getTheta()) < .3)) && (abs(bulletObject.getPhi() - sentryLasers[i].getPhi()) < .3) && (abs(bulletObject.getDistanceToOrigin() - SENTRY_RADIUS) < .5))
						{
							//decrease sentry health
							bulletObject.setInActive();
							//audio
							//change color of sentry?
							sentryLasers[i].setInActive();
							sentries[i].setInActive();
							return;
						}
					}
				}
			}
		}
}

void CrateApp::bulletWallCollision()
{
	if(bulletObject.getActiveState())
		{
			for(int j=0; j<NUM_LAYERS; j++)
			{
				for(int i=0; i<NUM_WALLS; i++)
				{
					if(layers[j].walls[i].getActiveState())
					{
						//if bullet Phi is close to either 0 or PI, ignore theta comparisons
						if(bulletObject.getPhi() < .3 || bulletObject.getPhi() > 2.7)
						{
							if((abs(bulletObject.getPhi() - layers[j].phis[i]) < .3) && (abs(bulletObject.getDistanceToOrigin() - layers[j].radius) < .5))
							{
								layers[j].wallHealth[i] --;
								bulletObject.setInActive();
								//audio->playCue(WALLHIT);
								layers[j].walls[i].tintOffset += D3DXCOLOR(.5,.5,.5,0);
								if(layers[j].wallHealth[i] == 0)
									layers[j].walls[i].setInActive();
								return;
							}
						}
						else {
							//if((abs(bulletObject.getTheta() - layers[j].thetas[i]) < .43) && (abs(bulletObject.getPhi() - layers[j].phis[i]) < .3) && (abs(bulletObject.getDistanceToOrigin() - layers[j].radius) < .5))
							if(((abs(bulletObject.getTheta() - layers[j].thetas[i]) < .43) || (abs(bulletObject.getTheta() - 2*PI - layers[j].thetas[i]) < .3)) && (abs(bulletObject.getPhi() - layers[j].phis[i]) < .3) && (abs(bulletObject.getDistanceToOrigin() - layers[j].radius) < .5))
							{
								layers[j].wallHealth[i] --;
								bulletObject.setInActive();
								//audio->playCue(WALLHIT);
								layers[j].walls[i].tintOffset += D3DXCOLOR(.5,.5,.5,0);
								if(layers[j].wallHealth[i] == 0)
									layers[j].walls[i].setInActive();
								return;
							}
						}
					}
				}
			}
		}
}

void CrateApp::laserWallCollision()
{
	if(laser.getActiveState())
		{
			for(int j=0; j<NUM_LAYERS; j++)
			{
				for(int i=0; i<NUM_WALLS; i++)
				{
					if(layers[j].walls[i].getActiveState())
					{
						//if laserPhi is close to either 0 or PI, ignore theta comparisons
						if(laser.getPhi() < .2 || laser.getPhi() > 2.9)
						{
							if(abs(laser.getPhi() - layers[j].phis[i]) < .3)
							{
								layers[j].walls[i].setInActive();
							}
						}
						else
						{
							if((abs(laser.getTheta() - layers[j].thetas[i]) < .2) && (abs(laser.getPhi() - layers[j].phis[i]) < .2))
								//if(layers[j].walls[i].getActiveState() && ((layers[j].thetas[i] > laserTheta) && (laserTheta + .3 > layers[j].thetas[i])) || ((layers[j].thetas[i] < laserTheta) && (layers[j].thetas[i] + .3 > laserTheta)))
							{
								layers[j].walls[i].setInActive();
							}
						}
					}
				}
			}
		}
}

void CrateApp::regenerateWalls(float dt)
{
	for(int i=0; i<NUM_LAYERS; i++)
	{
		for(int j=0; j<NUM_WALLS; j++)
		{
			if(!layers[i].walls[j].getActiveState())
			{
				layers[i].regenTime[j] += dt;
				if(level==2)
				{
					if(layers[i].regenTime[j] > REGEN_TIME)
					{
						layers[i].walls[j].setActive();
						layers[i].regenTime[j] = 0;
						layers[i].wallHealth[j] = 2;

						layers[i].walls[j].tintOffset = D3DXCOLOR(0,0,0,0); // reset tint
					}
				}
				else if(level==1)
				{
					if(layers[i].regenTime[j] > REGEN_TIME_LVL1)
					{
						layers[i].walls[j].setActive();
						layers[i].regenTime[j] = 0;
						layers[i].wallHealth[j] = 2;

						layers[i].walls[j].tintOffset = D3DXCOLOR(0,0,0,0); // reset tint
					}
				}
			}
		}
	}
}

void CrateApp::drawScene()
{
	D3DApp::drawScene();

	// Restore default states, input layout and primitive topology 
	// because mFont->DrawText changes them.  Note that we can 
	// restore the default states by passing null.
	md3dDevice->OMSetDepthStencilState(0, 0);
	float blendFactors[] = {0.0f, 0.0f, 0.0f, 0.0f};
	md3dDevice->OMSetBlendState(0, blendFactors, 0xffffffff);
	md3dDevice->IASetInputLayout(mVertexLayout);
	md3dDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// set constants
	mfxEyePosVar->SetRawValue(&mEyePos, 0, sizeof(D3DXVECTOR3));
	mfxLightVar->SetRawValue(&mParallelLight, 0, sizeof(Light));
	mfxBossEmissiveLightVar->SetRawValue(&mBossEmissive, 0, sizeof(Light));
	mfxSpotLightVar->SetRawValue(&mSpot, 0, sizeof(Light));
	mWVP = mCrateWorld*mView*mProj;
	mfxWVPVar->SetMatrix((float*)&mWVP);
	mfxWorldVar->SetMatrix((float*)&mCrateWorld);
	mfxDiffuseMapVar->SetResource(mDiffuseMapRV);
	mfxSpecMapVar->SetResource(mSpecMapRV);

	// Don't transform texture coordinates, so just use identity transformation.
	D3DXMATRIX texMtx;
	D3DXMatrixIdentity(&texMtx);
	mfxTexMtxVar->SetMatrix((float*)&texMtx);

	//  D3D10_TECHNIQUE_DESC techDesc;
	//  mTech->GetDesc( &techDesc );
	//  for(UINT p = 0; p < techDesc.Passes; ++p)
	//  {
	//      mTech->GetPassByIndex( p )->Apply(0);
	//      
	//mCrateMesh.draw();
	//  }

	// Set lighting to ambient only for the background
	int ambientOnlyFlag = 1;
	mfxAmbientOnlyFlag->SetRawValue(&ambientOnlyFlag, 0, sizeof(int));

	//BACKGROUND
	mfxDiffuseMapVar->SetResource(mBackgroundRV);
	mfxSpecMapVar->SetResource(mSpecMapRV);
	for(int i=0; i<6; i++) {
		mWVP = background[i].getWorldMatrix()*mView*mProj;
		mfxWVPVar->SetMatrix((float*)&mWVP);
		background[i].setMTech(mTech);
		background[i].draw();
	}

	// Turn regular lighting back on for the rest of the scene
	ambientOnlyFlag = 0;
	mfxAmbientOnlyFlag->SetRawValue(&ambientOnlyFlag, 0, sizeof(int));

	switch(state) {
	case intro:
		{
			// Set lighting to ambient only for the splashscreen
			int ambientOnlyFlag = 1;
			mfxAmbientOnlyFlag->SetRawValue(&ambientOnlyFlag, 0, sizeof(int));

			mWVP = mCrateWorld*mView*mProj;
			mfxWVPVar->SetMatrix((float*)&mWVP);
			mfxDiffuseMapVar->SetResource(introIntroSplashscreen.getDiffuseMapRV());
			mfxSpecMapVar->SetResource(introIntroSplashscreen.getSpecMapRV());
			introIntroSplashscreen.draw();

			// Turn regular lighting back on for the rest of the scene
			ambientOnlyFlag = 0;
			mfxAmbientOnlyFlag->SetRawValue(&ambientOnlyFlag, 0, sizeof(int));

			break;
		}
	case start:
		{
			// Set lighting to ambient only for the splashscreen
			int ambientOnlyFlag = 1;
			mfxAmbientOnlyFlag->SetRawValue(&ambientOnlyFlag, 0, sizeof(int));

			//Splashscreen
			mWVP = mCrateWorld*mView*mProj;
			mfxWVPVar->SetMatrix((float*)&mWVP);
			mfxDiffuseMapVar->SetResource(introSplashscreen.getDiffuseMapRV());
			mfxSpecMapVar->SetResource(introSplashscreen.getSpecMapRV());
			introSplashscreen.draw();

			// Turn regular lighting back on for the rest of the scene
			ambientOnlyFlag = 0;
			mfxAmbientOnlyFlag->SetRawValue(&ambientOnlyFlag, 0, sizeof(int));

			break;
		}
	case game:
		{
			//BOSS
			/*mWVP = boss.getWorldMatrix() *mView*mProj;
			mfxWVPVar->SetMatrix((float*)&mWVP);
			boss.setMTech(mTech);
			boss.draw();*/
			mWVP = mesh.getWorldMatrix() * mView * mProj;
			mfxWVPVar->SetMatrix((float*)&mWVP);
			mesh.setMTech(mTech);
			mesh.draw();

			//SENTRY
			for(int i=0; i<NUM_SENTRIES_LVL3; i++)
				{
				mWVP = sentries[i].getWorldMatrix() * mView * mProj;
				mfxWVPVar->SetMatrix((float*)&mWVP);
				sentries[i].setMTech(mTech);
				sentries[i].draw();
			}

			//Matrix s;
			//RotateY(&s, PI/4);

			//HEALTH BAR:
			//Matrix rphi, rtheta;
			//RotateX(&rphi, -mPhi);
			//RotateY(&rtheta, -mTheta);

			/*for(int i=0; i<1; i++)
			{
			mWVP = health[i].getWorldMatrix() * rphi * rtheta * mView*mProj;
			mfxWVPVar->SetMatrix((float*)&mWVP);
			health[i].setMTech(mTech);
			health[i].draw();
			}*/

			//Matrix s;
			//RotateY(&s, PI/4);

			//SENTRY DRAW:
			/*mWVP = sentry.getWorldMatrix() * mView*mProj;
			mfxWVPVar->SetMatrix((float*)&mWVP);
			sentry.draw(&mView, &mProj, mFX, mTech);*/


			//LAYERS:
			mfxDiffuseMapVar->SetResource(mDiffuseMapRV);
			mfxSpecMapVar->SetResource(mSpecMapRV);
			for(int i=0; i<3; i++)
			{
				for(int j=0; j<NUM_WALLS; j++)
				{
					mWVP = layers[i].walls[j].getWorldMatrix() * layers[i].translate  * layers[i].rotations[j] * layers[i].spin * mView*mProj;
					mfxWVPVar->SetMatrix((float*)&mWVP);
					layers[i].walls[j].setMTech(mTech);
					layers[i].walls[j].draw();
				}
			}

			//FOR DIAGONAL ROTATIONS:
			for(int i=3; i<NUM_LAYERS; i++)
			{
				for(int j=0; j<NUM_WALLS; j++)
				{
					mWVP = layers[i].walls[j].getWorldMatrix() * layers[i].translate  * layers[i].rotations[j] * layers[i].spin * layers[i].diagonal * mView*mProj;
					mfxWVPVar->SetMatrix((float*)&mWVP);
					layers[i].walls[j].setMTech(mTech);
					layers[i].walls[j].draw();
				}
			}


			//bullet
			bulletObject.setMTech(mTech);
			bulletObject.draw(mfxWVPVar, mView*mProj);

			
			
			mWVP = laser.getWorldMatrix() * mView*mProj;
			mfxWVPVar->SetMatrix((float*)&mWVP);
			laser.setMTech(mTech);
			laser.draw(mfxWVPVar, mView*mProj);

			for(int i=0; i<NUM_SENTRIES_LVL3; i++)
			{
				mWVP = sentryLasers[i].getWorldMatrix() * mView*mProj;
				mfxWVPVar->SetMatrix((float*)&mWVP);
				sentryLasers[i].setMTech(mTech);
				sentryLasers[i].draw(mfxWVPVar, mView*mProj);
			}

			std::wostringstream outs;   
			outs.precision(3);
			outs << L"Phi: " <<  mPhi << "\nTheta: " << mTheta << "\nSentry Phi: " << sentryLasers[1].getPhi() << "\nSentry Theta: " << sentryLasers[1].getTheta();
			//outs << L"Phi: " <<  mPhi << "\nTheta: " << mTheta << "\nSentry Phi: " << layers[1].phis[1] << "\nSentry Theta: " << sentryLasers[1].getTheta();
			//outs << L"Health: " << playerHealth;
			stats = outs.str();
			// We specify DT_NOCLIP, so we do not care about width/height of the rect.
			RECT R = {5, 5, 0, 0};
			mFont->DrawText(0, stats.c_str(), -1, &R, DT_NOCLIP, WHITE);

			//mSwapChain->Present(0, 0);
			//state = restart;

			break;
		}
	case restart:
		{
			// Set lighting to ambient only for the splashscreen
			int ambientOnlyFlag = 1;
			mfxAmbientOnlyFlag->SetRawValue(&ambientOnlyFlag, 0, sizeof(int));

			mWVP = mCrateWorld*mView*mProj;
			mfxWVPVar->SetMatrix((float*)&mWVP);
			mfxDiffuseMapVar->SetResource(restartSplashscreen.getDiffuseMapRV());
			mfxSpecMapVar->SetResource(restartSplashscreen.getSpecMapRV());
			restartSplashscreen.draw();

			// Turn regular lighting back on for the rest of the scene
			ambientOnlyFlag = 0;
			mfxAmbientOnlyFlag->SetRawValue(&ambientOnlyFlag, 0, sizeof(int));

			break;
		}
	case nextLevel:
		{
			// Set lighting to ambient only for the splashscreen
			int ambientOnlyFlag = 1;
			mfxAmbientOnlyFlag->SetRawValue(&ambientOnlyFlag, 0, sizeof(int));

			mWVP = mCrateWorld*mView*mProj;
			mfxWVPVar->SetMatrix((float*)&mWVP);
			mfxDiffuseMapVar->SetResource(nextLevelSplashscreen.getDiffuseMapRV());
			mfxSpecMapVar->SetResource(nextLevelSplashscreen.getSpecMapRV());
			nextLevelSplashscreen.draw();

			// Turn regular lighting back on for the rest of the scene
			ambientOnlyFlag = 0;
			mfxAmbientOnlyFlag->SetRawValue(&ambientOnlyFlag, 0, sizeof(int));

			break;
		}
	case end:
		{
			// Set lighting to ambient only for the splashscreen
			int ambientOnlyFlag = 1;
			mfxAmbientOnlyFlag->SetRawValue(&ambientOnlyFlag, 0, sizeof(int));

			mWVP = mCrateWorld*mView*mProj;
			mfxWVPVar->SetMatrix((float*)&mWVP);
			mfxDiffuseMapVar->SetResource(endSplashscreen.getDiffuseMapRV());
			mfxSpecMapVar->SetResource(endSplashscreen.getSpecMapRV());
			endSplashscreen.draw();

			// Turn regular lighting back on for the rest of the scene
			ambientOnlyFlag = 0;
			mfxAmbientOnlyFlag->SetRawValue(&ambientOnlyFlag, 0, sizeof(int));

			break;
		}
	}



	mSwapChain->Present(0, 0);
}

void CrateApp::buildFX()
{
	DWORD shaderFlags = D3D10_SHADER_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	//shaderFlags |= D3D10_SHADER_DEBUG;
	//shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif

	ID3D10Blob* compilationErrors = 0;
	HRESULT hr = 0;
	hr = D3DX10CreateEffectFromFile(L"tex.fx", 0, 0, 
		"fx_4_0", shaderFlags, 0, md3dDevice, 0, 0, &mFX, &compilationErrors, 0);
	if(FAILED(hr))
	{
		if( compilationErrors )
		{
			MessageBoxA(0, (char*)compilationErrors->GetBufferPointer(), 0, 0);
			ReleaseCOM(compilationErrors);
		}
		DXTrace(__FILE__, (DWORD)__LINE__, hr, L"D3DX10CreateEffectFromFile", true);
	} 

	mTech = mFX->GetTechniqueByName("TexTech");

	mfxWVPVar        = mFX->GetVariableByName("gWVP")->AsMatrix();
	mfxWorldVar      = mFX->GetVariableByName("gWorld")->AsMatrix();
	mfxEyePosVar     = mFX->GetVariableByName("gEyePosW");
	mfxLightVar      = mFX->GetVariableByName("gLight");
	mfxSpotLightVar	 = mFX->GetVariableByName("gSpotLight");
	mfxBossEmissiveLightVar = mFX->GetVariableByName("gBossEmissiveLight");
	mfxDiffuseMapVar = mFX->GetVariableByName("gDiffuseMap")->AsShaderResource();
	mfxSpecMapVar    = mFX->GetVariableByName("gSpecMap")->AsShaderResource();
	mfxTexMtxVar     = mFX->GetVariableByName("gTexMtx")->AsMatrix();
	mfxOverrideColorFlag = mFX->GetVariableByName("gOverrideColorFlag");
	mfxObjectColor	 = mFX->GetVariableByName("gObjectColor")->AsVector();
	mfxAmbientOnlyFlag = mFX->GetVariableByName("gAmbientOnlyFlag");
	mfxTintOffset	 = mFX->GetVariableByName("gTintOffset")->AsVector();
}

void CrateApp::buildVertexLayouts()
{
	// Create the vertex input layout.
	D3D10_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D10_INPUT_PER_VERTEX_DATA, 0},
	};

	// Create the input layout
	D3D10_PASS_DESC PassDesc;
	mTech->GetPassByIndex(0)->GetDesc(&PassDesc);
	HR(md3dDevice->CreateInputLayout(vertexDesc, 3, PassDesc.pIAInputSignature,
		PassDesc.IAInputSignatureSize, &mVertexLayout));
}

std::vector<Vector3> generateSurfrev2D(float degreesY)
{
	// Note: the number of points generated will be 2*NUM_SLICES + 2
	// (once on the inside, once on the outside, and two to cap it off at the end)
	const int NUM_SLICES = 50;

	std::vector<Vector3> points;

	// Define the "bottom" of the polygon
	points.push_back(Vector3(1,0,0));
	points.push_back(Vector3(1,0,0));

	float sliceSize = degreesY / NUM_SLICES;
	for(float theta = 0.0f; theta < degreesY; theta += sliceSize)
	{
		// If this is the last slice, make sure we don't overshoot past the specified # of degrees
		float thisSlice = (theta + sliceSize > degreesY) ? degreesY - theta : sliceSize;

		// Rotate the "bottom" points by the appropriate number of degrees for this slice
		Matrix rotMatrix;
		RotateY(&rotMatrix, ToRadian(theta + thisSlice));
		Vector3 rotatedOutsidePoint;
		Vector3 rotatedInsidePoint;
		TransformCoord(&rotatedOutsidePoint, &points[0], &rotMatrix);
		TransformCoord(&rotatedInsidePoint, &points[1], &rotMatrix);

		points.push_back(rotatedOutsidePoint);
		points.push_back(rotatedInsidePoint);
	}

	// Right now the sphere-slice "starts" at the x-axis and goes around counterclockwise from there.
	// For easier usage, we want this to be centered at the x-axis.
	// Hence, we need to rotate it down by degreesY/2.
	// (This could have been done in one pass above, but it's clearer to follow this way, and this
	// step is being done "offline" anyway so it's not time-critical. Not that it'd take long anyway...)
	std::vector<Vector3> centeredPoints;
	for(int i = 0; i < points.size(); i++)
	{
		Matrix rotMatrix;
		RotateY(&rotMatrix, ToRadian(-degreesY / 2.0f));
		Vector3 rotatedPoint;
		centeredPoints.push_back(*TransformCoord(&rotatedPoint, &points[i], &rotMatrix));
	}

	return centeredPoints;
}

void generateSurfrev3D(std::vector<Vector3> polygon, float degreesZ, Mesh &mesh)
{
	const int NUM_SLICES = 50;

	float sliceSize = degreesZ / NUM_SLICES;
	for(float theta = 0.0f; theta < degreesZ; theta += sliceSize)
	{
		// If this is the last slice, make sure we don't overshoot past the specified # of degrees
		float thisSlice = (theta + sliceSize > degreesZ) ? degreesZ - theta : sliceSize;

		// Determine the rotated points for the beginning and end of this slice
		std::vector<Vector3> begin, end;
		Matrix beginRot, endRot;
		RotateZ(&beginRot, ToRadian(theta));
		RotateZ(&endRot, ToRadian(theta + thisSlice));
		for(int i = 0; i < polygon.size(); i++)
		{
			Vector3 rotatedPoint;
			begin.push_back(*TransformCoord(&rotatedPoint, &polygon[i], &beginRot));
			end.push_back(*TransformCoord(&rotatedPoint, &polygon[i], &endRot));
		}

		// Triangulate the side of this slice and add the resultant faces to the mesh
		for(int i = 0; i < polygon.size() - 1; i++)
		{
			Mesh::Face face;
			Vector3 faceNormal, line1, line2;

			// Compute face normal for triangle 1
			line1 = end[i+1] - begin[i+1];
			line2 = begin[i] - begin[i+1];
			VectorCross(&faceNormal, &line1, &line2);
			Normalize(&faceNormal, &faceNormal);

			// **** TODO: right now the texture coordinates are just placeholders (all 0,1)
			// Define first triangle
			face.p0 = Vertex(begin[i], faceNormal, 0, 1);
			face.p1 = Vertex(begin[i+1], faceNormal, 0, 1);
			face.p2 = Vertex(end[i+1], faceNormal, 0, 1);
			mesh.addFace(face);

			// Compute face normal for triangle 2
			line1 = begin[i] - end[i];
			line2 = end[i+1] - end[i];
			VectorCross(&faceNormal, &line1, &line2);
			Normalize(&faceNormal, &faceNormal);

			// Second triangle
			face.p0 = Vertex(end[i+1], faceNormal, 0, 1);
			face.p1 = Vertex(end[i], faceNormal, 0, 1);
			face.p2 = Vertex(begin[i], faceNormal, 0, 1);
			mesh.addFace(face);
		}

		// If this is the first or last slice, triangulate the end to cap it off
		if(theta == 0.0f) // first slice
		{
			// Compute normal for the whole endcap; for this end, it's simply (1,0,0)
			Vector3 endcapNormal(1,0,0);

			for(int i = 0; i < polygon.size() - 1; i++)
			{
				Mesh::Face face;
				face.p0 = Vertex(begin[0], endcapNormal, 0, 1); // *** TODO: as above, texture coords. are placeholders
				face.p1 = Vertex(begin[i], endcapNormal, 0, 1);
				face.p2 = Vertex(begin[i+1], endcapNormal, 0, 1);
				mesh.addFace(face);
			}
		}
		else if(theta + sliceSize > degreesZ) // last slice
		{
			// Compute normal for the whole endcap: take (1,0,0) and rotate it by degreesZ
			// (note that we've already computed the matrix endRot above for this exact rotation)
			Vector3 endcapNormal(1,0,0);
			TransformCoord(&endcapNormal, &endcapNormal, &endRot);
			Normalize(&endcapNormal, &endcapNormal);

			for(int i = 0; i < polygon.size() - 1; i++)
			{
				Mesh::Face face;
				face.p0 = Vertex(begin[0], endcapNormal, 0, 1); // *** TODO: as above, texture coords. are placeholders
				face.p1 = Vertex(begin[i], endcapNormal, 0, 1);
				face.p2 = Vertex(begin[i+1], endcapNormal, 0, 1);
				mesh.addFace(face);
			}
		}
	}
}
