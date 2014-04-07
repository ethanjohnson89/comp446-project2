
#include "GameObject.h"

GameObject::GameObject()
{
	radius = 0;
	speed = 0;
	active = true;
	Identity(&world);

	rotX = rotY = rotZ = 0;

	spinAmountX = spinAmountY = spinAmountZ = 0;
	spinXSpeed = spinYSpeed = spinZSpeed = 0;

	srand(time(0));

	overrideColor = false;
}

GameObject::~GameObject()
{
	box = NULL;
}

void GameObject::draw()
{
	if (!active)
		return;
	D3D10_TECHNIQUE_DESC techDesc;
	mTech->GetDesc( &techDesc );

	int overrideColorFlag = 0;
	if(overrideColor)
	{
		overrideColorFlag = 1;
		mfxOverrideColorVar->SetRawValue(&overrideColorFlag, 0, sizeof(int));
		mfxObjectColorVar->SetFloatVector(color);
	}

	for(UINT p = 0; p < techDesc.Passes; ++p)
	{
		mTech->GetPassByIndex( p )->Apply(0);
		box->draw();
	}

	// Set overrideColorFlag back to 0 so other, non-overridden stuff doesn't draw with this color
	if(overrideColor)
	{
		overrideColorFlag = 0;
		mfxOverrideColorVar->SetRawValue(&overrideColorFlag, 0, sizeof(int));
	}
}


void GameObject::init(Box *b, float r, Vector3 pos, Vector3 vel, float sp, float sX, float sY, float sZ)
{
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

void GameObject::init(Box *b, float r, Vector3 pos, Vector3 vel, float sp, float s)
{
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

void GameObject::update(float dt)
{
	position += velocity*dt;
	Identity(&world);

	spinAmountY += (dt * spinYSpeed);
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
	rotZ = spinAmountZ;


	Matrix rotXM, rotYM, rotZM, transM, scaleM;
	RotateX(&rotXM, rotX);
	RotateY(&rotYM, rotY);
	RotateZ(&rotZM, rotZ); 
	Scale(&scaleM, scaleX, scaleY, scaleZ);
	Translate(&transM, position.x, position.y, position.z);
	world = scaleM * rotXM * rotYM * rotZM * transM;
}

bool GameObject::collided(GameObject *gameObject)
{
	if(!gameObject->getActiveState() || !active) return false; //no need to calculate if inactive

	Vector3 diff = position - gameObject->getPosition();
	float length = D3DXVec3LengthSq(&diff);
	float radii = radiusSquared + gameObject->getRadiusSquare();
	if (length <= radii) {
		//this->collisionVelocity(gameObject);
		return true;
	}
	return false; 
}

//bool GameObject::collided(QuadObject *quadObject)
//{
//	/*if(!quadObject->getActiveState())
//		return false;*/
//	
//	Vector3 qoPos = quadObject->getPosition();
//	if(qoPos.x > 0 && position.x + radius > qoPos.x) {
//		velocity.x = -velocity.x;
//		return true;
//	}
//	else if(qoPos.x < 0 && position.x - radius < qoPos.x) {
//		velocity.x = -velocity.x;
//		return true;
//	}
//	if(qoPos.y > 0 && position.y + radius > qoPos.y) {
//		velocity.y = -velocity.y;
//		return true;
//	}
//	else if(qoPos.y < 0 && position.y - radius < qoPos.y) {
//		velocity.y = -velocity.y;
//		return true;
//	}
//	if(qoPos.z > 0 && position.z + radius > qoPos.z) {
//		velocity.z = -velocity.z;
//		return true;
//	}
//	else if(qoPos.z < 0 && position.z - radius < qoPos.z) {
//		velocity.z = -velocity.z;
//		return true;
//	}
//
//	return false;
//}


void GameObject::randomizeVelocity()
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
}