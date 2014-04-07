
#ifndef Bullet_H
#define Bullet_H

#include "GameObject.h"
#include "d3dUtil.h"
//#include "C:\Users\KAPPELKR1\Desktop\Games 2\games code\Common\d3dUtil.h"
//#include "Box.h"
#include "constants.h"
#include <ctime>
//#include "QuadObject.h"



class Bullet
{
public:
	Bullet();
	~Bullet();

	void init(Box *b, float r, Vector3 pos, Vector3 vel, float sp, float sx, float sy, float sz);
	void init(Box *b, float r, Vector3 pos, Vector3 vel, float sp, float s);
	void draw();
	void update(float dt);

	void shoot(Vector3 pos, Vector3 vel);
	void setPosition (Vector3 pos) {
		position = pos;
		for(int i=0; i<3;i++)
			pieces[i].setPosition(pos); }
	Vector3 getPosition() {return position;}
	void setVelocity (Vector3 vel) {
		velocity = vel;
		for(int i=0; i<3;i++)
			pieces[i].setVelocity(vel);}
	Vector3 getVelocity() {return velocity;}
	void setSpeed(float s) {
		speed = s;
		for(int i=0; i<3;i++)
			pieces[i].setSpeed(s);}
	float getSpeed() {return speed;}
	//void setRadius(float r) {radius = r; radiusSquared = (scale*r)*(scale*r);}
	float getRadiusSquare() {return radiusSquared;}
	float getRadius() {return radius;}
	Matrix getWorldMatrix() {return world;}
	void setScale(float s) {
		scaleX = s; scaleY=s; scaleZ=s; radiusSquared = (s*radius)*(s*radius);
		for(int i=0; i<3;i++)
			pieces[i].setScale(s);}
	float getScale() {return scale;}
	void setScaleX(float s) {
		scaleX=s;
		for(int i=0; i<3;i++)
			pieces[i].setScaleX(s);}
	void setScaleY(float s) {
		scaleY=s;
		for(int i=0; i<3;i++)
			pieces[i].setScaleY(s);}
	void setScaleZ(float s) {
		scaleZ=s;
		for(int i=0; i<3;i++)
			pieces[i].setScaleZ(s);}
	void setActive() {
		active = true;
		for(int i=0; i<3;i++)
			pieces[i].setActive();}
	void setInActive() {
		active = false;
		for(int i=0; i<3;i++)
			pieces[i].setInActive();}
	bool getActiveState() {return active;}
	void setMTech(ID3D10EffectTechnique* m){ 
		mTech = m;
		for(int i=0; i<3;i++)
			pieces[i].setMTech(m);}
	bool collided(GameObject *gameObject);
	void collisionVelocity(GameObject *gameObject);

	Vector3 getOldPosition() {return oldPosition;}
	void setOldPosition(Vector3 pos) {
		oldPosition = pos;
		for(int i=0; i<3;i++)
			pieces[i].setOldPosition(pos);}

	void randomizeVelocity();

	void setRotXSpeed(float r) {
		spinXSpeed = r;
		for(int i=0; i<3;i++)
			pieces[i].setRotXSpeed(r);}
	void setRotYSpeed(float r) {
		spinYSpeed = r;
		for(int i=0; i<3;i++)
			pieces[i].setRotYSpeed(r);}
	void setRotZSpeed(float r) {
		spinZSpeed = r;
		for(int i=0; i<3;i++)
			pieces[i].setRotZSpeed(r);}
	float getRotXSpeed() {return spinXSpeed;}
	float getRotYSpeed() {return spinYSpeed;}
	float getRotZSpeed() {return spinZSpeed;}
	float getRotX() {return rotX;}
	float getRotY() {return rotY;}
	float getRotZ() {return rotZ;}
	//float getRotX() {return rotX;}
	//float getRotY() {return rotY;}
	//float getRotZ() {return rotZ;}
	void setRotX(float r) {
		rotX = r;
		for(int i=0; i<3;i++)
			pieces[i].setRotX(r);}
	void setRotY(float r) {
		rotX = r;
		for(int i=0; i<3;i++)
			pieces[i].setRotY(r);}
	void setRotZ(float r) {
		rotX = r;
		for(int i=0; i<3;i++)
			pieces[i].setRotZ(r);}

	float spinAmountX, spinAmountY, spinAmountZ;
	float spinXSpeed, spinYSpeed, spinZSpeed;

	bool collidedLastFrame;
	bool getCollided() {return collidedLastFrame;}
	void setCollided(bool c) {
		collidedLastFrame = c;
		for(int i=0; i<3; i++)
			pieces[i].setCollided(c);}

	bool hit; //used for its color and updating velocity

	void setColor(D3DXCOLOR c) { 
		overrideColor = true; color = c; 
		for(int i=0; i<3; i++)
			pieces[i].setColor(c);}
    D3DXCOLOR getColor() { return color; }
    void setOverrideColorVar(ID3D10EffectVariable *v) { 
		mfxOverrideColorVar = v; 
		for(int i=0; i<3; i++)
			pieces[i].setOverrideColorVar(v);} // both of these must be set if overriding color
    void setObjectColorVar(ID3D10EffectVectorVariable *v) { 
		mfxObjectColorVar = v; 
		for(int i=0; i<3; i++)
			pieces[i].setObjectColorVar(v);}

private:
	Box* box;
	GameObject pieces[3]; 
	Vector3 position;
	Vector3 oldPosition;
	Vector3 velocity;
	float speed;
	float radius;
	float radiusSquared;
	bool active;
	Matrix world;
	float scaleX, scaleY, scaleZ, scale;
	ID3D10EffectTechnique* mTech;
	float rotX, rotY, rotZ;

	//Matrix r;

	bool overrideColor;
    D3DXCOLOR color;
    ID3D10EffectVariable *mfxOverrideColorVar;
    ID3D10EffectVectorVariable *mfxObjectColorVar;

};


#endif
