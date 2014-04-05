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
#include "Layer.h"
#include <sstream>

class CrateApp : public D3DApp
{
public:
	CrateApp(HINSTANCE hInstance);
	~CrateApp();

	void initApp();
	void onResize();
	void updateScene(float dt);
	void drawScene(); 

private:
	void buildFX();
	void buildVertexLayouts();
 
private:

	Box mCrateMesh;
	Box target1, bullet;
	GameObject gameObject6;

	GameObject layer1[3];
	GameObject layer2[3];
	GameObject laser;
	//Layer layers[3];
	Layer layer;

	//bullet construct
	GameObject bullet1[3];

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
  mRadius(25.0f), mTheta(0.0f), mPhi(PI*0.4f), spinAmount(0)
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

	gameObject6.init(&bullet, sqrt(2.0f*1.4), D3DXVECTOR3(0,0,0), D3DXVECTOR3(0,0,0), 10,1);
	laser.init(&bullet, sqrt(2.0f), D3DXVECTOR3(0,0,0), D3DXVECTOR3(0,0,0), 10,.05,.05,mRadius);


	layer1[0].init(&bullet, sqrt(2.0f), D3DXVECTOR3(3,0,3), D3DXVECTOR3(0,0,0), 10,1);
	layer1[1].init(&bullet, sqrt(2.0f), D3DXVECTOR3(-1,0,-3), D3DXVECTOR3(0,0,0), 10,1);
	layer1[2].init(&bullet, sqrt(2.0f), D3DXVECTOR3(-3,0,3), D3DXVECTOR3(0,0,0), 10,1);

	layer2[0].init(&bullet, sqrt(2.0f), D3DXVECTOR3(4,-1,0), D3DXVECTOR3(0,0,0), 10,1);
	layer2[1].init(&bullet, sqrt(2.0f), D3DXVECTOR3(0,5,0), D3DXVECTOR3(0,0,0), 10,1);
	layer2[2].init(&bullet, sqrt(2.0f), D3DXVECTOR3(-2,-4, 0), D3DXVECTOR3(0,0,0), 10,1);

	bullet1[0].init(&bullet, sqrt(2.0f), D3DXVECTOR3(7,1,7), D3DXVECTOR3(0,0,0), 10,.5,.05,.05);
	bullet1[1].init(&bullet, sqrt(2.0f), D3DXVECTOR3(7,1,7), D3DXVECTOR3(0,0,0), 10,.05,.5,.05);
	bullet1[2].init(&bullet, sqrt(2.0f), D3DXVECTOR3(7,1,7), D3DXVECTOR3(0,0,0), 10,.05,.05,.5);
	
	for(int i=0; i<3; i++)
		bullet1[i].setInActive();
			
	
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
	if(GetAsyncKeyState('A') & 0x8000)	mTheta -= 3.0f*dt;
	if(GetAsyncKeyState('D') & 0x8000)	mTheta += 3.0f*dt;
	if(GetAsyncKeyState('W') & 0x8000)	mPhi -= 3.0f*dt;
	if(GetAsyncKeyState('S') & 0x8000)	mPhi += 3.0f*dt;
	if(GetAsyncKeyState('Z') & 0x8000)	mRadius -= 15.0f*dt;
	if(GetAsyncKeyState('X') & 0x8000)	mRadius += 15.0f*dt;

	if(GetAsyncKeyState(' ') & 0x8000)
	{
		//if(!bullet1[0].getActiveState())
		//{
			for(int i=0; i<3; i++)
			{
				bullet1[i].setActive();
				bullet1[i].setPosition(mEyePos);
				//calculate velocity vector
				Vector3 t(-mEyePos);
				bullet1[i].setVelocity(t*2);
			}
	//	}
	}
	
	spinAmount += 1*dt;
	if (spinAmount*10 > 360)
		spinAmount = 0;


	for(int i =0; i<3; i++)
	{
		layer1[i].update(dt);
		layer2[i].update(dt);
	}

	laser.update(dt);

	for(int i=0; i<3; i++)
		bullet1[i].update(dt);
	
	// Restrict the angle mPhi.
	if( mPhi < 0.1f )	mPhi = 0.1f;
	if( mPhi > PI-0.1f)	mPhi = PI-0.1f;

	if( mTheta < 0.01f )	mTheta = 2*PI-.02f;
	if( mTheta > 2*PI-0.01f)	mTheta = .02f;

	if( laserTheta < 0.01f )	laserTheta = 2*PI-.02f;
	if( laserTheta > 2*PI-0.01f)	laserTheta = .02f;

	//needed?
	//if( laserPhi < 0.1f )	laserPhi = 0.1f;
	//if( laserPhi > PI-0.1f)	laserPhi = PI-0.1f;

	// Convert Spherical to Cartesian coordinates: mPhi measured from +y
	// and mTheta measured counterclockwise from -z.
	mEyePos.x =  mRadius*sinf(mPhi)*sinf(mTheta);
	mEyePos.z = -mRadius*sinf(mPhi)*cosf(mTheta);
	mEyePos.y =  mRadius*cosf(mPhi);
	
	// Build the view matrix.
	D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
	D3DXMatrixLookAtLH(&mView, &mEyePos, &target, &up);
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
        
		mCrateMesh.draw();
    }

	//BOSS
	mWVP = gameObject6.getWorldMatrix() *mView*mProj;
	mfxWVPVar->SetMatrix((float*)&mWVP);
	gameObject6.setMTech(mTech);
	gameObject6.draw();

	Matrix spinY, spinZ;
	RotateY(&spinY, ToRadian(spinAmount*50));
	RotateZ(&spinZ, ToRadian(spinAmount*50));

	for(int i=0; i<3; i++)
	{
		mWVP = layer1[i].getWorldMatrix()  * spinY *mView*mProj;
		mfxWVPVar->SetMatrix((float*)&mWVP);
		layer1[i].setMTech(mTech);
		layer1[i].draw();

		mWVP = layer2[i].getWorldMatrix()  * spinZ *mView*mProj;
		mfxWVPVar->SetMatrix((float*)&mWVP);
		layer2[i].setMTech(mTech);
		layer2[i].draw();
	}

	//bullet
	for(int i=0; i<3; i++)
	{
		mWVP = bullet1[i].getWorldMatrix() *mView*mProj;
		mfxWVPVar->SetMatrix((float*)&mWVP);
		bullet1[i].setMTech(mTech);
		bullet1[i].draw();
	}
	
	//laser
	//TRACKING:
	if(abs(laserTheta-mTheta)>.01)
	{
		//if(laserTheta > mTheta) laserTheta -= .0003f;
		//else laserTheta += .0003f;
		if((laserTheta-mTheta > -PI && laserTheta-mTheta < 0) || (laserTheta-mTheta > PI)) laserTheta += .0003f;
		else laserTheta -= .0003f;
	}
	if(abs(laserPhi-mPhi)>.01)
	{
		if(laserPhi > mPhi) laserPhi -= .0003f;
		else laserPhi += .0003f;
	}
	Matrix translateOut;
	Matrix rotatePhi, rotateTheta;
	RotateX(&rotatePhi, -laserPhi+.5);
	RotateY(&rotateTheta, -laserTheta);
	Translate(&translateOut, 0, 0, -mRadius);
	mWVP = laser.getWorldMatrix() *translateOut * rotatePhi * rotateTheta * mView*mProj;
	mfxWVPVar->SetMatrix((float*)&mWVP);
	laser.setMTech(mTech);
	laser.draw();

	std::wostringstream outs;   
	outs.precision(2);
	outs << L"Phi: " <<  mPhi << "\nTheta: " << mTheta << "\n\n laser phi: " << laserPhi << "\nlaser theta: " << laserTheta;
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

 