#ifndef LASER_H
#define LASER_H

#include "d3dUtil.h"
#include "GameObject.h"

class Laser : public GameObject
{
public:
	Laser();
	~Laser();
	
	//this might not actually be used?
	void init(Box *b, float r, Vector3 pos, Vector3 vel, float sp, float sx, float sy, float sz);

	//ALSO HAVE ONE THAT SETS INITIAL THETA AND PHIS
	void init(Box *b, float r, Vector3 pos, Vector3 vel, float sp, float sx, float sy, float sz, float p, float t); //phi and theta

	void draw(ID3D10EffectMatrixVariable *mfxWVPVar, D3DXMATRIX mViewProj);
	void update(float dt);

	float getPhi() {return phi;}
	void setPhi(float p) {phi = p;}
	float getTheta() {return theta;}
	void setTheta(float t) {theta = t;}

	void setSpeed(float s) {speed = s;}

	void trackPlayer(float mTheta, float mPhi);

	/*void setPosition (Vector3 pos) {
		position = pos;
		for(int i=0; i<3;i++)
			pieces[i].setPosition(pos); }
	Vector3 getPosition() {return position;}*/
	/*void setVelocity (Vector3 vel) {
		velocity = vel;
		for(int i=0; i<3;i++)
			pieces[i].setVelocity(vel);}
	Vector3 getVelocity() {return velocity;}*/
	//void setSpeed(float s) {speed = s;}
	//float getSpeed() {return speed;}
	//Matrix getWorldMatrix() {return world;}
	
	void setActive() {active = true;}
	void setInActive() {active = false;}
	bool getActiveState() {return active;}
	
	
	void setMTech(ID3D10EffectTechnique* m){mTech = m; laser.setMTech(m);}


	/*void setRotXSpeed(float r) {
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
			pieces[i].setRotZSpeed(r);}*/
	/*float getRotXSpeed() {return spinXSpeed;}
	float getRotYSpeed() {return spinYSpeed;}
	float getRotZSpeed() {return spinZSpeed;}
	float getRotX() {return rotX;}
	float getRotY() {return rotY;}
	float getRotZ() {return rotZ;}*/
	//float getRotX() {return rotX;}
	//float getRotY() {return rotY;}
	//float getRotZ() {return rotZ;}
	/*void setRotX(float r) {
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
			pieces[i].setRotZ(r);}*/

	
	


	void setColor(D3DXCOLOR c) {laser.setColor(c);}
    D3DXCOLOR getColor() { return laser.getColor(); }
    void setOverrideColorVar(ID3D10EffectVariable *v) {laser.setOverrideColorVar(v);} 
    void setObjectColorVar(ID3D10EffectVectorVariable *v) {laser.setObjectColorVar(v);}

private:
	GameObject laser; 
	float speed;
	bool active;
	//Matrix world;
	float scaleX, scaleY, scaleZ;
	ID3D10EffectTechnique* mTech;
	//float rotX, rotY, rotZ;

	float phi, theta;

	//float spinAmountX, spinAmountY, spinAmountZ;
	//float spinXSpeed, spinYSpeed, spinZSpeed;

	//Matrix r;

	/*bool overrideColor;
    D3DXCOLOR color;
    ID3D10EffectVariable *mfxOverrideColorVar;
    ID3D10EffectVectorVariable *mfxObjectColorVar;*/

};

#endif // LASER_H