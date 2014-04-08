
#include "Bullet.h"

Bullet::Bullet()
{
	radius = 0;
	speed = 0;
	active = false;
	Identity(&world);

	rotX = rotY = rotZ = 0;

	spinAmountX = spinAmountY = spinAmountZ = 0;
	spinXSpeed = spinYSpeed = spinZSpeed = 0;

	srand(time(0));

	overrideColor = false;

	phi = theta = distanceToOrigin = 0;
}

Bullet::~Bullet()
{
}

void Bullet::draw(ID3D10EffectMatrixVariable *mfxWVPVar, D3DXMATRIX mViewProj)
{
	for(int i=0; i<3; i++)
	{
		D3DXMATRIX pieceMatrix = pieces[i].getWorldMatrix() * getWorldMatrix() * mViewProj;
		mfxWVPVar->SetMatrix((float*)&pieceMatrix);
		// mTech has already been set for all pieces by Bullet::setMTech()
		pieces[i].draw();
	}
}


void Bullet::init(Box *b, float r, Vector3 pos, Vector3 vel, float sp, float sX, float sY, float sZ)
{
	pieces[0].init(b, r, pos, vel, sp, sX*.5,  sY*.05,  sZ*.05);
	pieces[1].init(b, r, pos, vel, sp, sX*.05, sY*.5,  sZ*.05);
	pieces[2].init(b, r, pos, vel, sp, sX*.05, sY*.05, sZ*.5);
	for(int i=0; i<3;i++)
		pieces[i].setInActive();

	box = b;
	radius = r;
	radius *= 1.01; //fudge factor
	position = pos;
	velocity = vel;
	speed = sp;
	scaleX = sX;
	scaleY = sY;
	scaleZ = sZ;
	scale = sX;  //DON'T KNOW WHAT I SHOULD MAKE THIS
	radiusSquared = radius * radius;
	collidedLastFrame = false;
	hit = false;
}

void Bullet::init(Box *b, float r, Vector3 pos, Vector3 vel, float sp, float s)
{
	pieces[0].init(b, r, pos, vel, sp, s*.5,  s*.05, s*.05);
	pieces[1].init(b, r, pos, vel, sp, s*.05, s*.5,  s*.05);
	pieces[2].init(b, r, pos, vel, sp, s*.05, s*.05, s*.5);
	for(int i=0; i<3;i++)
		pieces[i].setInActive();

	box = b;
	radius = r;
	radius *= 1.01; //fudge factor
	position = pos;
	velocity = vel;
	speed = sp;
	scale=s;
	scaleX = s;
	scaleY = s;
	scaleZ = s;
	radiusSquared = radius * radius;
	collidedLastFrame = false;
	hit = false;
}

void Bullet::update(float dt)
{
	for(int i=0; i<3; i++)
		pieces[i].update(dt);

	position += velocity*dt;
	Identity(&world);

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

	if(active)
	{
		distanceToOrigin = D3DXVec3Length(&position);
		if(distanceToOrigin > 25) 
		{
			active = false;
			for(int i=0; i<3;i++)
				pieces[i].setInActive();
		}
	}


	Matrix rotXM, rotYM, rotZM, transM, scaleM;
	RotateX(&rotXM, rotX);
	RotateY(&rotYM, rotY);
	RotateZ(&rotZM, rotZ); 
	Scale(&scaleM, scaleX, scaleY, scaleZ);
	Translate(&transM, position.x, position.y, position.z);
	world = scaleM * rotXM * rotYM * rotZM * transM;
}

void Bullet::shoot(Vector3 pos, Vector3 vel, float mTheta, float mPhi) {
	setActive();
	setPosition(pos);
	setVelocity(vel);

	theta = mTheta;
	phi = mPhi;
	distanceToOrigin = Radius;
}

bool Bullet::collided(GameObject *gameObject)
{
	for(int i=0; i<3; i++) {
		if(pieces[i].collided(gameObject)) {
			return true;
		}
	}

	return false;
}

bool Bullet::collided(Mesh *mesh)
{
	for(int i=0; i<3; i++) {
		//if(pieces[i].collided(gameObject)) {
		if(mesh->collided(&pieces[i])) {
			return true;
		}
	}

	return false;
}

void Bullet::randomizeVelocity()
{
	int x = rand();
	if(rand()%2 == 0) x *= -1;
	int y = rand();
	if(rand()%2 == 0) y *= -1;
	int z = rand();
	if(rand()%2 == 0) z *= -1;
	Vector3 temp(x,y,z);
	Normalize(&temp, &temp);
	velocity = temp;

	for(int i=0; i<3; i++) {
		pieces[i].setVelocity(velocity);
	}
}