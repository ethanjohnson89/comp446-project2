#pragma once

#include "d3dApp.h"
#include "d3dUtil.h"
#include "Vertex.h"
#include "GameObject.h"
#include "constants.h"
#include <vector>

class Mesh
{
public:
	struct Face // triangular face structure
	{
		Vertex p0, p1, p2; // the three points of the triangle
	};

	Mesh() : initialized(false)
	{
		radius = 0;
		speed = 0;
		active = true;
		Identity(&world);

		rotX = rotY = rotZ = 0;

		spinAmountX = spinAmountY = spinAmountZ = 0;
		spinXSpeed = spinYSpeed = spinZSpeed = 0;

		overrideColor = false;
	}

	~Mesh()
	{
		ReleaseCOM(mVB);
	}

	void addFace(Face face)
	{
		vertices.push_back(face.p0);
		vertices.push_back(face.p1);
		vertices.push_back(face.p2);
	}

	void init(ID3D10Device* device, float r, Vector3 pos, Vector3 vel, float sp, float s)
	{
		md3dDevice = device;

		radius = r * 1.01;
		position = pos;
		velocity = vel;
		speed = sp;
		scale = scaleX = scaleY = scaleZ = s;
		radiusSquared = radius * radius;
		collidedLastFrame = false;
		hit = false;

		// Send the vertex buffer to the GPU
		D3D10_BUFFER_DESC vbd;
		vbd.Usage = D3D10_USAGE_IMMUTABLE;
		vbd.ByteWidth = sizeof(Vertex) * vertices.size();
		vbd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
		vbd.CPUAccessFlags = 0;
		vbd.MiscFlags = 0;
		D3D10_SUBRESOURCE_DATA vinitData;
		vinitData.pSysMem = vertices.data();
		HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mVB));
	}

	void init(ID3D10Device* device, float r, Vector3 pos, Vector3 vel, float sp, float sX, float sY, float sZ)
	{
		md3dDevice = device;

		radius = r * 1.01;
		position = pos;
		velocity = vel;
		speed = sp;
		scaleX = sX;
		scaleY = sY;
		scaleZ = sZ;
		scale = sX; // ??? (I don't think this is being used any more internally)
		radiusSquared = radius * radius;
		collidedLastFrame = false;
		hit = false;

		// Send the vertex buffer to the GPU
		D3D10_BUFFER_DESC vbd;
		vbd.Usage = D3D10_USAGE_IMMUTABLE;
		vbd.ByteWidth = sizeof(Vertex) * vertices.size();
		vbd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
		vbd.CPUAccessFlags = 0;
		vbd.MiscFlags = 0;
		D3D10_SUBRESOURCE_DATA vinitData;
		vinitData.pSysMem = vertices.data();
		HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mVB));
	}

	void update(float dt)
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

	void draw()
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
			
			// Draw the mesh
			UINT stride = sizeof(Vertex);
			UINT offset = 0;
			md3dDevice->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);
			md3dDevice->Draw(vertices.size(), 0);
		}

		// Set overrideColorFlag back to 0 so other, non-overridden stuff doesn't draw with this color
		if(overrideColor)
		{
			overrideColorFlag = 0;
			mfxOverrideColorVar->SetRawValue(&overrideColorFlag, 0, sizeof(int));
		}
	}

	void setPosition (Vector3 pos) {position = pos;}
	Vector3 getPosition() {return position;}
	void setVelocity (Vector3 vel) {velocity = vel;}
	Vector3 getVelocity() {return velocity;}
	void setSpeed(float s) {speed = s;}
	float getSpeed() {return speed;}
	//void setRadius(float r) {radius = r; radiusSquared = (scale*r)*(scale*r);}
	float getRadiusSquare() {return radiusSquared;}
	float getRadius() {return radius;}
	Matrix getWorldMatrix() {return world;}
	void setScale(float s) {scaleX = s; scaleY=s; scaleZ=s; radiusSquared = (s*radius)*(s*radius);}
	float getScale() {return scale;}
	void setScaleX(float s) {scaleX=s;}
	void setScaleY(float s) {scaleY=s;}
	void setScaleZ(float s) {scaleZ=s;}
	void setActive() {active = true;}
	void setInActive() {active = false;}
	bool getActiveState() {return active;}
	void setMTech(ID3D10EffectTechnique* m){ mTech = m;}
	//bool collided(GameObject *gameObject);
	//void collisionVelocity(GameObject *gameObject);
	//bool collided(QuadObject *quadObject);
	
	//used for good collision detection:
	Vector3 getOldPosition() {return oldPosition;}
	void setOldPosition(Vector3 pos) {oldPosition = pos;}

	void randomizeVelocity();

	void setRotXSpeed(float r) {spinXSpeed = r;}
	void setRotYSpeed(float r) {spinYSpeed = r;}
	void setRotZSpeed(float r) {spinZSpeed = r;}
	float getRotXSpeed() {return spinXSpeed;}
	float getRotYSpeed() {return spinYSpeed;}
	float getRotZSpeed() {return spinZSpeed;}
	float getRotX() {return rotX;}
	float getRotY() {return rotY;}
	float getRotZ() {return rotZ;}
	void setRotX(float r) {rotX = r;}
	void setRotY(float r) {rotY = r;}
	void setRotZ(float r) {rotZ = r;}

	float spinAmountX, spinAmountY, spinAmountZ;
	float spinXSpeed, spinYSpeed, spinZSpeed;

	bool collidedLastFrame;
	bool getCollided() {return collidedLastFrame;}
	void setCollided(bool c) {collidedLastFrame = c;}

	bool hit; //used for its color and updating velocity

	void setColor(D3DXCOLOR c) { overrideColor = true; color = c; }
    D3DXCOLOR getColor() { return color; }
    void setOverrideColorVar(ID3D10EffectVariable *v) { mfxOverrideColorVar = v; } // both of these must be set if overriding color
    void setObjectColorVar(ID3D10EffectVectorVariable *v) { mfxObjectColorVar = v; }

//private: // DEBUG - commented
	std::vector<Vertex> vertices; // includes both positions and normals
	// Note: we're not using an index buffer for this class at the moment, since given the way it's
	// currently structured it'd be pointless (merely sequential). It'd be more efficient to store
	// a proper face list and build an index buffer from this when we buffer the vertices, but
	// since we have limited time (and not all that many vertices, relative to what our GPUs can
	// handle) it's not really worth the trouble right now.

	bool initialized;

	ID3D10Device *md3dDevice;
	ID3D10Buffer *mVB;

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

	bool overrideColor;
    D3DXCOLOR color;
    ID3D10EffectVariable *mfxOverrideColorVar;
    ID3D10EffectVectorVariable *mfxObjectColorVar;
};