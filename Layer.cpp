
#include "Layer.h"

Layer::Layer()
{
	spinAmount = 0;
	Identity(&rotation);

}

Layer::~Layer()
{
	
}

void Layer::draw()
{
	//for(int i=0; i<shells.size(); i++)
		//shells[i].draw();
}


//void Layer::init(Box *b, float r, Vector3 pos, Vector3 vel, float sp, float s)
//{
//	box = b;
//	radius = r;
//	radius *= 1.01; //fudge factor
//	position = pos;
//	velocity = vel;
//	speed = sp;
//	scale = s;
//	radiusSquared = radius * radius;
//	collidedLastFrame = false;
//	hit = false;
//}

void Layer::update(float dt)
{
//	for(int i=0; i<shells.size(); i++)
	//	shells[i].update(dt);


	RotateY(&rotation, ToRadian(spinAmount*50));
	
}

void Layer::addShell()
{

}

//bool GameObject::collided(GameObject *gameObject)
//{
//	if(!gameObject->getActiveState()) return false; //no need to calculate if inactive
//
//	Vector3 diff = position - gameObject->getPosition();
//	float length = D3DXVec3LengthSq(&diff);
//	float radii = radiusSquared + gameObject->getRadiusSquare();
//	if (length <= radii) {
//		//this->collisionVelocity(gameObject);
//		return true;
//	}
//	return false; 
//}

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

//
//void Layer::randomizeVelocity()
//{
//
//	int x = rand();
//	if(rand()%2 == 0) x *= -1;
//	int y = rand();
//	if(rand()%2 == 0) y *= -1;
//	int z = rand();
//	if(rand()%2 == 0) z *= -1;
//	Vector3 temp(x,y,z);
//	Normalize(&temp, &temp);
//	velocity = temp;
//}