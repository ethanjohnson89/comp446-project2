#include "Laser.h"


void Laser::init(Box *b, float r, Vector3 pos, Vector3 vel, float sp, float sX, float sY, float sZ, float trans)
{
	laser.init(b, r, pos, vel, sp, sX,  sY,  sZ);

	translate = trans;

	active = false;

	pulsing = false;
	laserOn = true;

	timer = 0;

	srand((unsigned)time(0));

	// Initialize all particles to inactive, zero speed, placed at the origin (we'll be changing speed and location when they're
	// activated later)
	for(int i = 0; i < MAX_PARTICLES; i++)
	{
		particles[i].init(b, r, Vector3(0,0,0), Vector3(0,0,0), 0, .01f,.01f,.01f);
		particles[i].setInActive();
	}
	oldestParticleIndex = 0;

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
	particles.resize(MAX_PARTICLES);
}

Laser::~Laser()
{
}

void Laser::draw(ID3D10EffectMatrixVariable *mfxWVPVar, D3DXMATRIX mViewProj)
{
	D3DXMATRIX pieceMatrix = laser.getWorldMatrix() * getWorldMatrix() * mViewProj;
	mfxWVPVar->SetMatrix((float*)&pieceMatrix);
	// mTech has already been set for the "laser" GameObject by Laser::setMTech()
	laser.draw();

	for(int i = 0; i < MAX_PARTICLES; i++)
	{
		D3DXMATRIX particleMatrix = particles[i].getWorldMatrix() * mViewProj;
		mfxWVPVar->SetMatrix((float*)&particleMatrix);
		// mTech has already been set for all particles by Laser::setMTech()
		particles[i].draw();
	}
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
	Translate(&translateOut, 0, 0, translate);
	laser.setWorldMatrix(laser.getWorldMatrix() *translateOut * rotatePhi * rotateTheta);

	////// PARTICLES

	// Increment active particles' timers by dt, and set them to inactive if they've exceeded their lifetime
	for(int i = 0; i < MAX_PARTICLES; i++)
	{
		if(particles[i].getActiveState()) // no need to do anything if the particle isn't actually active
		{
			particles[i].timeActive += dt;
			if(particles[i].timeActive > particles[i].lifetime)
			{
				particles[i].setInActive();
				particles[i].timeActive = 0;
			}
		}
	}

	// Create new particles to "trail" the laser
	if(laser.getActiveState())
	{
		Vector3 laserStartPos = laser.getPosition();
		Matrix laserMiddleTrans = rotatePhi * rotateTheta;
		for(float d = 0; d < PLAYER_RADIUS*2; d += 0.5f) // generate particles at points along the laser beam
		{
			if(rand() % 2 == 0) // to make it look a bit more "natural"
			{
				Vector3 laserMiddlePos = laserStartPos + Vector3(0,0,-d);
				TransformCoord(&laserMiddlePos, &laserMiddlePos, &laserMiddleTrans);
				particles[oldestParticleIndex].setPosition(laserMiddlePos); // start the particle at the middle of the laser beam
				particles[oldestParticleIndex].setVelocity(getRandomParticleVelocity());
				particles[oldestParticleIndex].setActive();
				particles[oldestParticleIndex].timeActive = 0.0f;
				particles[oldestParticleIndex].lifetime = getRandomLifetime();
				oldestParticleIndex = (oldestParticleIndex + 1) % MAX_PARTICLES;

				// DEBUG - breakpoint to check if particles are being recycled while still active (i.e. MAX_PARTICLES is too low)
				if(particles[oldestParticleIndex].getActiveState())
					oldestParticleIndex = (oldestParticleIndex - 1) + 1;
			}
		}
	}

	// Update the particle objects themselves
	// (NOTE: move this before the "create new particles" section to get a potentially interesting "flicker" effect at the particles'
	// old positions as they're re-used.)
	for(int i = 0; i < MAX_PARTICLES; i++)
		particles[i].update(dt);

	//pulsing:
	if(active && pulsing)
	{
		if(laserOn)
		{
			timer += dt;
			if(timer >= pulseOnTime)
			{
				timer = 0;
				laserOn = false;
				laser.setInActive();
			}
		}
		else
		{
			timer += dt;
			if(timer >= pulseOffTime)
			{
				timer = 0;
				laserOn = true;
				laser.setActive();
			}
		}
	}
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