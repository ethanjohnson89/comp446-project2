#include "Laser.h"

void Laser::init(Box *b, float r, Vector3 pos, Vector3 vel, float sp, float sX, float sY, float sZ)
{
	laser.init(b, r, pos, vel, sp, sX,  sY,  sZ);

	active = false;
	
	//box = b;
	//scaleX = sX;
	//scaleY = sY;
	//scaleZ = sZ;
}
void Laser::init(Box *b, float r, Vector3 pos, Vector3 vel, float sp, float sX, float sY, float sZ, float p, float t)
{
	laser.init(b, r, pos, vel, sp, sX,  sY,  sZ);

	active = false;

	phi = p;
	theta = t;

	srand(time(0));

	

	//overrideColor = false;

	//spinAmountX = spinAmountY = spinAmountZ = 0;
	//spinXSpeed = spinYSpeed = spinZSpeed = 0;
	
	//box = b;
	//scaleX = sX;
	//scaleY = sY;
	//scaleZ = sZ;
}


Laser::Laser()
{
	speed = LASER_SPEED_LVL1;
}

Laser::~Laser()
{
}

void Laser::draw(ID3D10EffectMatrixVariable *mfxWVPVar, D3DXMATRIX mViewProj)
{
	D3DXMATRIX pieceMatrix = laser.getWorldMatrix() * getWorldMatrix() * mViewProj;
	mfxWVPVar->SetMatrix((float*)&pieceMatrix);
	// mTech has already been set for all pieces by Bullet::setMTech()
	laser.draw();
}

void Laser::update(float dt)
{
	laser.update(dt);

	if( theta < 0.01f )	theta = 2*PI-.02f;
	if( theta > 2*PI-0.01f)	theta = .02f;

	Matrix translateOut;
	Matrix rotatePhi, rotateTheta;
	RotateX(&rotatePhi, -phi+PI/2);
	RotateY(&rotateTheta, theta);
	Translate(&translateOut, 0, 0, -PLAYER_RADIUS*2);
	laser.setWorldMatrix(laser.getWorldMatrix() *translateOut * rotatePhi * rotateTheta);



	//PUT LASER TRACKING HERE
	//need to be able to differentiate whether we want this laser to be tracking or following the wall rotation pattern....


	/*spinAmountY += (dt * spinYSpeed);
	if (spinAmountY>2*PI)
		spinAmountY = 0;
	rotY = spinAmountY;
	spinAmountX += (dt * spinXSpeed);
	if (spinAmountX>2*PI)
		spinAmountX = 0;
	rotX = spinAmountX;
	spinAmountZ += (dt * spinZSpeed);
	if (spinAmountZ>2*PI)
		spinAmountZ = 0;
	rotZ = spinAmountZ;*/
	
	/*Matrix rotXM, rotYM, rotZM, transM, scaleM;
	RotateX(&rotXM, rotX);
	RotateY(&rotYM, rotY);
	RotateZ(&rotZM, rotZ); 
	Scale(&scaleM, scaleX, scaleY, scaleZ);
	Translate(&transM, position.x, position.y, position.z);
	world = scaleM * rotXM * rotYM * rotZM * transM;*/
}

void Laser::trackPlayer(float mTheta, float mPhi)
{
	if(abs(theta-mTheta)>.01)
	{
		if((theta-mTheta > -PI && theta-mTheta < 0) || (theta-mTheta > PI)) theta += speed;
		else theta -= speed;
	}
	if(abs(phi-mPhi)>.01)
	{
		if(phi > mPhi) phi -= speed;
		else phi += speed;
	}
}