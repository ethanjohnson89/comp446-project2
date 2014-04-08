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
#include <sstream>

enum rotationAxis { X, Y, Z, YZ, ZY };

struct Layer {
	rotationAxis axis;
	int radius;
	Matrix rotations[NUM_WALLS];
	Matrix spin, translate, diagonal;
	GameObject walls[NUM_WALLS];
	float thetas[NUM_WALLS];
	float phis[NUM_WALLS];
	float startingTheta[NUM_WALLS];
	float startingPhi[NUM_WALLS];
	float regenTime[NUM_WALLS];
	
	Layer::Layer(rotationAxis a, int r) {
		radius = r; 
		axis = a;
		for(int i=0; i<NUM_WALLS; i++)
			regenTime[i] = 0;

		if(axis==Y)
		{
			for(int i=0; i<NUM_WALLS; i++)
			{
				startingTheta[i] = i*2*PI/NUM_WALLS - (PI/2); // THAT PI/2 MAKES UP FOR DIFFERENCE IN STARTING THETAS FOR WALLS AND CAMERA
				startingPhi[i] = PI/2;
			}
		}
		if(axis==Z)
		{
			for(int i=0; i<NUM_WALLS; i++)
			{
				//startingTheta[i] = i*2*PI/NUM_WALLS; 
				startingPhi[i] = i*2*PI/NUM_WALLS;
			}
		}
		if(axis==X || axis==YZ || axis==ZY)
		{
			for(int i=0; i<NUM_WALLS; i++)
			{
				//startingTheta[i] = i*2*PI/NUM_WALLS + PI; 
				//startingPhi[i] = i*2*PI/NUM_WALLS + PI/2; 
				float t = i*2*PI/NUM_WALLS + PI/2;
				int temp = static_cast<int>(t/(2*PI));
				startingPhi[i] = t - static_cast<float>(temp)*(2*PI);
				//phis[i] = startingPhi[i];
			}
		}
		/*if(axis==YZ || axis==ZY)
		{
			for(int i=0; i<NUM_WALLS; i++)
			{
				startingPhi[i] = i*2*PI/NUM_WALLS;
			}
		}
		*/
	}
	Layer::Layer() {}

	void updateMatrices(float spinAmount)
	{
		if(axis==Y)
		{
			Translate(&translate,radius,0,0); //CAN PUT SOMETHING LIKE radius*5*spinAmount/36.0f TO HAVE IT GROW IN AND OUT
			RotateY(&spin, ToRadian(spinAmount*50));
			for(int i=0; i<NUM_WALLS; i++)
			{
				RotateY(&rotations[i], i*2*PI/NUM_WALLS);
				//thetas[i] = 2*PI*spinAmount/(6*36);
				float t = ToRadian(spinAmount*50) + startingTheta[i];
				int temp = static_cast<int>(t/(2*PI));
				thetas[i] = t - static_cast<float>(temp)*(2*PI);
				phis[i] = startingPhi[i];
				//if(thetas[i] > 2*PI) thetas[i] = 0;
			}
		}
		if(axis==Z)
		{
			Translate(&translate,0,radius,0);
			RotateZ(&spin, ToRadian(spinAmount*50));
			for(int i=0; i<NUM_WALLS; i++)
			{
				RotateZ(&rotations[i], i*2*PI/NUM_WALLS);
				float t = ToRadian(spinAmount*50) + startingPhi[i];
				int temp = static_cast<int>(t/(2*PI));
				float tempPhi = t - static_cast<float>(temp)*(2*PI);
				if(tempPhi > PI)
				{
					phis[i] = 2*PI - tempPhi; //IF ON RIGHT SIDE GOING BACK UP
					thetas[i] = 3*PI/2;
				}
				else 
				{
					phis[i] = tempPhi;
					thetas[i] = PI/2;
				}
				//phis[i] = startingPhi[i];
			}
		}
		if(axis==X || axis==ZY || axis==YZ)
		{
			Translate(&translate,0,0,radius);
			RotateX(&spin, ToRadian(spinAmount*50));
			for(int i=0; i<NUM_WALLS; i++)
			{
				RotateX(&rotations[i], i*2*PI/NUM_WALLS);
				float t = ToRadian(spinAmount*50) + startingPhi[i];
				int temp = static_cast<int>(t/(2*PI));
				float tempPhi = t - static_cast<float>(temp)*(2*PI);
				if(tempPhi > PI)
				{
					phis[i] = 2*PI - tempPhi; 
					thetas[i] = 0;
				}
				else 
				{
					phis[i] = tempPhi;
					thetas[i] = PI;
				}
			}
		}
		if(axis==YZ)
		{
			RotateY(&diagonal, PI/4.0f);
			for(int i=0; i<NUM_WALLS; i++)
			{
				thetas[i] += PI/4;
			}

		}
		else if(axis==ZY)
		{
			RotateY(&diagonal, -PI/4.0f);
			for(int i=0; i<NUM_WALLS; i++)
			{
				
				float t = thetas[i] + 7*PI/4;
				int temp = static_cast<int>(t/(2*PI));
				thetas[i] = t - static_cast<float>(temp)*(2*PI);
			}
		}
	}
};

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

private:
	void buildFX();
	void buildVertexLayouts();
 
private:

	Box mCrateMesh;
	Box target1, bullet;
	GameObject boss;
	GameObject health[3];

	GameObject laser;
	Layer layers[NUM_LAYERS];

	Bullet bulletObject;

	Light mParallelLight;

	ID3D10Effect* mFX;
	ID3D10EffectTechnique* mTech;
	ID3D10InputLayout* mVertexLayout;
	ID3D10ShaderResourceView* mDiffuseMapRV;
	ID3D10ShaderResourceView* mSpecMapRV;

	ID3D10EffectMatrixVariable* mfxWVPVar;
	ID3D10EffectMatrixVariable* mfxWorldVar;
	ID3D10EffectVariable* mfxEyePosVar;
	ID3D10EffectVariable* mfxLightVar;
	ID3D10EffectShaderResourceVariable* mfxDiffuseMapVar;
	ID3D10EffectShaderResourceVariable* mfxSpecMapVar;
	ID3D10EffectMatrixVariable* mfxTexMtxVar;

	D3DXMATRIX mCrateWorld;

	D3DXMATRIX mView;
	D3DXMATRIX mProj;
	D3DXMATRIX mWVP;

	D3DXVECTOR3 mEyePos;
	float mRadius;
	float mTheta;
	float mPhi;

	float laserTheta;
	float laserPhi;
	
	float spinAmount;

	std::wstring stats;

	bool fireLaser;
	float laserTimer;

	int bossHealth;

	bool spacePressedLastFrame;

	int level;
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
  mRadius(25.0f), mTheta(0.0f), mPhi(PI/2), spinAmount(0), fireLaser(false), laserTimer(0), bossHealth(3),
  spacePressedLastFrame(false), level(1)
{
	D3DXMatrixIdentity(&mCrateWorld);
	D3DXMatrixIdentity(&mView);
	D3DXMatrixIdentity(&mProj);
	D3DXMatrixIdentity(&mWVP); 

	laserTheta = mTheta;
	laserPhi = mPhi;
}

CrateApp::~CrateApp()
{
	if( md3dDevice )
		md3dDevice->ClearState();

	ReleaseCOM(mFX);
	ReleaseCOM(mVertexLayout);
	ReleaseCOM(mDiffuseMapRV);
	ReleaseCOM(mSpecMapRV);
}

void CrateApp::initApp()
{
	D3DApp::initApp();

	mClearColor = D3DXCOLOR(0.9f, 0.9f, 0.9f, 1.0f);
	
	buildFX();
	buildVertexLayouts();
	
	mCrateMesh.init(md3dDevice, 1.0f);

	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
		L"murray.jpg", 0, 0, &mDiffuseMapRV, 0 ));

	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
		L"defaultspec.dds", 0, 0, &mSpecMapRV, 0 ));


	mParallelLight.dir      = D3DXVECTOR3(0.57735f, -0.57735f, 0.57735f);
	mParallelLight.ambient  = D3DXCOLOR(0.4f, 0.4f, 0.4f, 1.0f);
	mParallelLight.diffuse  = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	mParallelLight.specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);


	bullet.init(md3dDevice, 1.0f);

	boss.init(&bullet, sqrt(2.0f), D3DXVECTOR3(0,0,0), D3DXVECTOR3(0,0,0), 10,1);
	health[0].init(&bullet, sqrt(2.0f), D3DXVECTOR3(0,0,0), D3DXVECTOR3(-10,0,10), 10,1);
	laser.init(&bullet, sqrt(2.0f), D3DXVECTOR3(0,0,0), D3DXVECTOR3(0,0,0), 10,.05,.05,mRadius*2);
	laser.setInActive();

	//LAYERS:
	//SPECIFY ROTATION AXIS AND RADIUS IN CONSTRUCTOR
	layers[0] = Layer(rotationAxis::Y, 5);
	layers[1] = Layer(rotationAxis::Z, 5);
	layers[2] = Layer(rotationAxis::X, 5);
	layers[3] = Layer(rotationAxis::YZ, 5);
	layers[4] = Layer(rotationAxis::ZY, 5);
	for(int i=0; i<NUM_WALLS; i++)
	{
		layers[0].walls[i].init(&bullet, sqrt(2.0f), D3DXVECTOR3(0,0,0), D3DXVECTOR3(0,0,0), 10,.1,1.7,1.7);
		layers[1].walls[i].init(&bullet, sqrt(2.0f), D3DXVECTOR3(0,0,0), D3DXVECTOR3(0,0,0), 10,1.7,.1,1.7);
		layers[2].walls[i].init(&bullet, sqrt(2.0f), D3DXVECTOR3(0,0,0), D3DXVECTOR3(0,0,0), 10,1.7,1.7,.1);
		layers[3].walls[i].init(&bullet, sqrt(2.0f), D3DXVECTOR3(0,0,0), D3DXVECTOR3(0,0,0), 10,1.7,1.7,.1);
		layers[4].walls[i].init(&bullet, sqrt(2.0f), D3DXVECTOR3(0,0,0), D3DXVECTOR3(0,0,0), 10,1.7,1.7,.1);
	/*	layers[0].walls[i].init(&bullet, sqrt(2.0f), D3DXVECTOR3(0,0,0), D3DXVECTOR3(0,0,0), 10,.1,1.5,1.5);
		layers[1].walls[i].init(&bullet, sqrt(2.0f), D3DXVECTOR3(0,0,0), D3DXVECTOR3(0,0,0), 10,1.5,.1,1.5);
		layers[2].walls[i].init(&bullet, sqrt(2.0f), D3DXVECTOR3(0,0,0), D3DXVECTOR3(0,0,0), 10,1.5,1.5,.1);
		layers[3].walls[i].init(&bullet, sqrt(2.0f), D3DXVECTOR3(0,0,0), D3DXVECTOR3(0,0,0), 10,1.5,1.5,.1);
		layers[4].walls[i].init(&bullet, sqrt(2.0f), D3DXVECTOR3(0,0,0), D3DXVECTOR3(0,0,0), 10,1.5,1.5,.1);*/
	}
	

	//bullet1[0].init(&bullet, 1, D3DXVECTOR3(7,1,7), D3DXVECTOR3(0,0,0), 10,.5,.05,.05);
	//bullet1[1].init(&bullet, 1, D3DXVECTOR3(7,1,7), D3DXVECTOR3(0,0,0), 10,.05,.5,.05);
	//bullet1[2].init(&bullet, 1, D3DXVECTOR3(7,1,7), D3DXVECTOR3(0,0,0), 10,.05,.05,.5);
	//for(int i=0; i<3; i++)
	//	bullet1[i].setInActive();
	bulletObject.init(&bullet, 1, D3DXVECTOR3(7,1,7), D3DXVECTOR3(0,0,0), 10, 1); // initialized as inactive	
}

void CrateApp::onResize()
{
	D3DApp::onResize();

	float aspect = (float)mClientWidth/mClientHeight;
	D3DXMatrixPerspectiveFovLH(&mProj, 0.25f*PI, aspect, 1.0f, 1000.0f);
}

void CrateApp::updateScene(float dt)
{
	D3DApp::updateScene(dt);

	// Update angles based on input to orbit camera around scene.
	if(GetAsyncKeyState('A') & 0x8000)	mTheta += 3.0f*dt;
	if(GetAsyncKeyState('D') & 0x8000)	mTheta -= 3.0f*dt;
	if(GetAsyncKeyState('W') & 0x8000)	mPhi -= 3.0f*dt;
	if(GetAsyncKeyState('S') & 0x8000)	mPhi += 3.0f*dt;
	if(GetAsyncKeyState('Z') & 0x8000)	mRadius -= 15.0f*dt;
	if(GetAsyncKeyState('X') & 0x8000)	mRadius += 15.0f*dt;

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
	if(bulletObject.collided(&boss))
	{
		bossHealth--;
		if(bossHealth == 0) boss.setInActive();
		//for(int i=0; i<3; i++)
		//	bullet1[i].setInActive();
		bulletObject.setInActive();
	}


	//BULLET COLLISION ON WALLS
	if(bulletObject.getActiveState())
	{
		for(int j=0; j<NUM_LAYERS; j++)
		{
			for(int i=0; i<NUM_WALLS; i++)
			{
				if(layers[j].walls[i].getActiveState() && ((abs(bulletObject.getTheta() - layers[j].thetas[i]) < .3) || (abs(bulletObject.getTheta() - 2*PI - layers[j].thetas[i]) < .3)) && (abs(bulletObject.getPhi() - layers[j].phis[i]) < .3) && (abs(bulletObject.getDistanceToOrigin() - layers[j].radius) < .5))
				{
					layers[j].walls[i].setInActive();
					bulletObject.setInActive();
				}
			}
		}
	}


	//LASER COLLISION ON WALLS
	if(fireLaser)
	{
		for(int j=0; j<NUM_LAYERS; j++)
		{
			for(int i=0; i<NUM_WALLS; i++)
			{
				if(layers[j].walls[i].getActiveState())
				{
					//if laserTheta is close to either 0 or 2PI, ignore theta comparisons
					if(laserPhi < .3 || laserPhi > 2.7)
					{
						if(abs(laserPhi - layers[j].phis[i]) < .3)
						{
							layers[j].walls[i].setInActive();
						}
					}
					else
					{
						if((abs(laserTheta - layers[j].thetas[i]) < .3) && (abs(laserPhi - layers[j].phis[i]) < .3))
						//if(layers[j].walls[i].getActiveState() && ((layers[j].thetas[i] > laserTheta) && (laserTheta + .3 > layers[j].thetas[i])) || ((layers[j].thetas[i] < laserTheta) && (layers[j].thetas[i] + .3 > laserTheta)))
						{
							layers[j].walls[i].setInActive();
						}
					}
				}
			}
		}
	}

	
	regenerateWalls(dt);

	//CONTROLS WHEN LASER DISPLAYS
	if(!fireLaser)
	{
		if(rand()%4000==0)
		{
			fireLaser = true;
			laser.setActive();
			laserTheta = (int)(mTheta+PI)%6;
		}
	}
	else
	{
		laser.update(dt);
		laserTimer += dt;
		//at end of timer,set timer back to zero, set laser inactive
		if(laserTimer > 100)
		{
			fireLaser = false;
			laserTimer = 0;
			laser.setInActive();
		}
	}


	
	// Restrict the angle mPhi.
	if( mPhi < 0.1f )	mPhi = 0.1f;
	if( mPhi > PI-0.1f)	mPhi = PI-0.1f;

	if( mTheta < 0.01f )	mTheta = 2*PI-.02f;
	if( mTheta > 2*PI-0.01f)	mTheta = .02f;

	if( laserTheta < 0.01f )	laserTheta = 2*PI-.02f;
	if( laserTheta > 2*PI-0.01f)	laserTheta = .02f;


	// Convert Spherical to Cartesian coordinates: mPhi measured from +y
	// and mTheta measured counterclockwise from -z.
	mEyePos.x =  mRadius*sinf(mPhi)*sinf(-mTheta);
	mEyePos.z = -mRadius*sinf(mPhi)*cosf(-mTheta);
	mEyePos.y =  mRadius*cosf(mPhi);
	
	// Build the view matrix.
	D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
	D3DXMatrixLookAtLH(&mView, &mEyePos, &target, &up);
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
				if(layers[i].regenTime[j] > REGEN_TIME)
				{
					layers[i].walls[j].setActive();
					layers[i].regenTime[j] = 0;
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
	mWVP = mCrateWorld*mView*mProj;
	mfxWVPVar->SetMatrix((float*)&mWVP);
	mfxWorldVar->SetMatrix((float*)&mCrateWorld);
	mfxDiffuseMapVar->SetResource(mDiffuseMapRV);
	mfxSpecMapVar->SetResource(mSpecMapRV);
 
	// Don't transform texture coordinates, so just use identity transformation.
	D3DXMATRIX texMtx;
	D3DXMatrixIdentity(&texMtx);
	mfxTexMtxVar->SetMatrix((float*)&texMtx);

    D3D10_TECHNIQUE_DESC techDesc;
    mTech->GetDesc( &techDesc );
    for(UINT p = 0; p < techDesc.Passes; ++p)
    {
        mTech->GetPassByIndex( p )->Apply(0);
        
		//mCrateMesh.draw();
    }

	//BOSS
	mWVP = boss.getWorldMatrix() *mView*mProj;
	mfxWVPVar->SetMatrix((float*)&mWVP);
	boss.setMTech(mTech);
	boss.draw();

	//Matrix s;
	//RotateY(&s, PI/4);



	//HEALTH BAR:
	Matrix rphi, rtheta;
	RotateX(&rphi, -mPhi);
	RotateY(&rtheta, -mTheta);

	/*for(int i=0; i<1; i++)
	{
		mWVP = health[i].getWorldMatrix() * rphi * rtheta * mView*mProj;
		mfxWVPVar->SetMatrix((float*)&mWVP);
		health[i].setMTech(mTech);
		health[i].draw();
	}*/


	//LAYERS:
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
	
	//laser
	//TRACKING:
	if(abs(laserTheta-mTheta)>.01)
	{
		if((laserTheta-mTheta > -PI && laserTheta-mTheta < 0) || (laserTheta-mTheta > PI)) laserTheta += .0015f;
		else laserTheta -= .0015f;
	}
	if(abs(laserPhi-mPhi)>.01)
	{
		if(laserPhi > mPhi) laserPhi -= .0015f;
		else laserPhi += .0015f;
	}
	Matrix translateOut;
	Matrix rotatePhi, rotateTheta;
	RotateX(&rotatePhi, -laserPhi+PI/2);
	RotateY(&rotateTheta, laserTheta);
	Translate(&translateOut, 0, 0, -mRadius*2);
	mWVP = laser.getWorldMatrix() *translateOut * rotatePhi * rotateTheta * mView*mProj;
	mfxWVPVar->SetMatrix((float*)&mWVP);
	laser.setMTech(mTech);
	laser.draw();

	std::wostringstream outs;   
	outs.precision(2);
	//outs << L"Phi: " <<  mPhi << "\nTheta: " << mTheta << "\n\n laser phi: " << laserPhi << "\nlaser theta: " << laserTheta
	outs << L"Phi: " <<  mPhi << "\nTheta: " << mTheta << "\n\n laser phi: " << laserPhi << "\nlaser theta: " << laserTheta << "\nwall theta: " << layers[3].thetas[0] << "\nwall phi: " << layers[3].phis[0];;
	stats = outs.str();
	// We specify DT_NOCLIP, so we do not care about width/height of the rect.
	RECT R = {5, 5, 0, 0};
	mFont->DrawText(0, stats.c_str(), -1, &R, DT_NOCLIP, BLACK);

	mSwapChain->Present(0, 0);
}

void CrateApp::buildFX()
{
	DWORD shaderFlags = D3D10_SHADER_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
    shaderFlags |= D3D10_SHADER_DEBUG;
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
	mfxDiffuseMapVar = mFX->GetVariableByName("gDiffuseMap")->AsShaderResource();
	mfxSpecMapVar    = mFX->GetVariableByName("gSpecMap")->AsShaderResource();
	mfxTexMtxVar     = mFX->GetVariableByName("gTexMtx")->AsMatrix();
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

 