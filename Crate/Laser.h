#ifndef LASER_H
#define LASER_H

#include "d3dUtil.h"
#include "GameObject.h"
#include "Particle.h"
#include <vector>
#include <random>

class Laser : public GameObject
{
public:
	Laser();
	~Laser();
	
	//this might not actually be used?
	void init(Box *b, float r, Vector3 pos, Vector3 vel, float sp, float sx, float sy, float sz, float trans);

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
	
	void setActive() {active = true; laser.setActive();}
	void setInActive() {active = false; laser.setInActive();}
	bool getActiveState() {return laser.getActiveState();}

	void setScaleZ(float s) {laser.setScaleZ(s);}

	void setPulsing(bool p) {pulsing=p;}
	bool getPulsing() {return pulsing;}
	
	void setPulseOnTime(float p) {pulseOnTime=p;}
	float getPulseOnTime() {return pulseOnTime;}
	void setPulseOffTime(float p) {pulseOffTime=p;}
	float getPulseOffTime() {return pulseOffTime;}
	void setStartingPhi(float p) {startingPhi=p;}
	float getStartingPhi() {return startingPhi;}
	void setPhiSpeed(float p) {phiSpeed=p;}
	float getPhiSpeed() {return phiSpeed;}
	void setThetaSpeed(float p) {thetaSpeed=p;}
	float getThetaSpeed() {return thetaSpeed;}
	
	void setMTech(ID3D10EffectTechnique* m)
	{
		mTech = m;
		laser.setMTech(m);
		for(int i = 0; i < MAX_PARTICLES; i++)
			particles[i].setMTech(m);
	}
	
	void setColor(D3DXCOLOR c)
	{
		laser.setColor(c);
		for(int i = 0; i < MAX_PARTICLES; i++)
			particles[i].setColor(c);
	}
    D3DXCOLOR getColor() { return laser.getColor(); }
    void setOverrideColorVar(ID3D10EffectVariable *v)
	{
		laser.setOverrideColorVar(v);
		for(int i = 0; i < MAX_PARTICLES; i++)
			particles[i].setOverrideColorVar(v);
	} 
    void setObjectColorVar(ID3D10EffectVectorVariable *v)
	{
		laser.setObjectColorVar(v);
		for(int i = 0; i < MAX_PARTICLES; i++)
			particles[i].setObjectColorVar(v);
	}

	void setAmbientOnly(bool ambOnly)
	{
		laser.setAmbientOnly(ambOnly);
		for(int i = 0; i < MAX_PARTICLES; i++)
			particles[i].setAmbientOnly(ambOnly);
	}
	void setAmbientOnlyFlagVar(ID3D10EffectVariable *v)
	{
		laser.setAmbientOnlyFlagVar(v);
		for(int i = 0; i < MAX_PARTICLES; i++)
			particles[i].setAmbientOnlyFlagVar(v);
	}

private:
	GameObject laser;
	float speed;
	bool active;
	//Matrix world;
	float scaleX, scaleY, scaleZ;
	ID3D10EffectTechnique* mTech;
	//float rotX, rotY, rotZ;

	float phi, theta;

	bool pulsing, laserOn;
	float pulseOnTime, pulseOffTime, timer;
	float startingPhi, phiSpeed, thetaSpeed;
	float translate;

	std::vector<Particle> particles; // initialized to size MAX_PARTICLES in constructor
	int oldestParticleIndex;

	Vector3 getRandomParticleVelocity()
	{
		float x = getRandomSign() * (rand() % 10) / 20.0f;
		float y = getRandomSign() * (rand() % 10) / 20.0f;
		float z = getRandomSign() * (rand() % 10) / 20.0f;

		return Vector3(x,y,z);
	}
	int getRandomSign() // to randomly choose whether a number is positive or negative
	{
		if(rand() % 2)
			return 1;
		else
			return -1;
	}
	float getRandomLifetime() // returns a random float in the range [0, MAX_PARTICLE_LIFETIME]
	{
		static std::uniform_real_distribution<float> dist(0, MAX_PARTICLE_LIFETIME);
		return dist(randEngine);
	}

	std::default_random_engine randEngine;

	/*bool overrideColor;
    D3DXCOLOR color;
    ID3D10EffectVariable *mfxOverrideColorVar;
    ID3D10EffectVectorVariable *mfxObjectColorVar;*/

};

#endif // LASER_H