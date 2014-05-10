#pragma once

#include "d3dApp.h"
#include "d3dUtil.h"
#include "Vertex.h"
#include "GameObject.h"
#include "constants.h"
#include <vector>
#include "Mesh.h"

class Sentry : public Mesh
{
public:
	void update(float dt)
	{
		//position += velocity*dt;
		Identity(&world);

		Matrix center;
		Translate(&center, -1, 0, 0); // to center the sphere at 0,0,0 before applying the world matrix (kind of a hack)

		/*Matrix rotXM, rotYM, rotZM, transM, scaleM;
		RotateX(&rotXM, rotX);
		RotateY(&rotYM, rotY);
		RotateZ(&rotZM, rotZ); 
		Scale(&scaleM, scaleX, scaleY, scaleZ);
		Translate(&transM, position.x, position.y, position.z);
		world = scaleM * rotXM * rotYM * rotZM * transM * center;*/

		if( theta < 0.01f )	theta = 2*PI-.02f;
		if( theta > 2*PI-0.01f)	theta = .02f;
		/*if(phi < 0.01f) phi = PI-.02f;
		if(phi > PI-.01f) phi=.02f;*/

		Matrix translateOut;
		Matrix rotatePhi, rotateTheta;
		RotateX(&rotatePhi, -phi+PI/2);
		RotateY(&rotateTheta, theta);
		Translate(&translateOut, position.x, position.y, position.z);
		world = center * translateOut * rotatePhi * rotateTheta;
	}

	float getPhi() {return phi;}
	void setPhi(float p) {phi = p;}
	float getTheta() {return theta;}
	void setTheta(float t) {theta = t;}

private:

};