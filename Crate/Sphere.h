/*
Laura Wendlandt, 3/24/2014
Modified code from http://blog.andreaskahler.com/2009/06/creating-icosphere-mesh-in-code.html
*/

#ifndef __SPHERE_H
#define __SPHERE_H

#include "d3dUtil.h"
//#include "constants_MCK_TAL_WEN.h"
#include "Vertex.h"
#include <map>
using std::map;
#include <utility>
using std::pair;
#include <vector>
using std::vector;

#define Vector3 D3DXVECTOR3
#define Matrix D3DXMATRIX
#define Identity D3DXMatrixIdentity
#define Translate D3DXMatrixTranslation
#define Scale D3DXMatrixScaling
#define RotateX D3DXMatrixRotationX
#define RotateY D3DXMatrixRotationY
#define RotateZ D3DXMatrixRotationZ
#define VectorZero Vector3(0,0,0)
#define Color D3DXCOLOR

class Sphere
{
public:

	Sphere();
	~Sphere();

	void init(ID3D10Device* device, float scale, Color c, int recursionLevel); //greater recursion level = more tessellation = smoother sphere
	void draw(Matrix* mView, Matrix* mProj, ID3D10Effect* mFX, ID3D10EffectTechnique* mTech);
	void update(float dt);

	//from GameObject
	void setPosition (Vector3 pos) {position = pos;}
	void setPositionX(float x) { position.x = x; }
	void setPositionY(float y) { position.y = y; }
	void setPositionZ(float z) { position.z = z; }
	Vector3 getPosition() {return position;}
	void setVelocity (Vector3 vel) {velocity = vel;}
	Vector3 getVelocity() {return velocity;}
	void setSpeed(float s) {speed = s;}
	float getSpeed() {return speed;}
	Matrix getWorldMatrix() {return world;}
	void setActive() {active = true;}
	void setInActive() {active = false;}
	bool getActiveState() {return active;}

	//RST - from GameObject
	void RST(Vector3 rot, Vector3 scale, Vector3 trans) { setRotate(rot); setScale(scale); setTranslate(trans); }
	void setTranslate(Vector3 trans) { this->trans = trans; }
	void setTranslate(float x, float y, float z) { trans = Vector3(x,y,z); }
	void addToTranslate(float x, float y, float z) { trans.x += x; trans.y += y; trans.z += z; }
	Vector3 getTranslate() { return trans; }
	void setRotate(Vector3 rot) { this->rot = rot; } //in radians
	void setRotate(float x, float y, float z) { rot = Vector3(x,y,z); } //in radians
	void setRotateX(float x) { rot.x = x; }
	void setRotateY(float y) { rot.y = y; }
	void setRotateZ(float z) { rot.z = z; }
	void modRotate() { while(rot.x > 2*PI) rot.x -= 2*PI; while(rot.y > 2*PI) rot.y -= 2*PI; while(rot.z > 2*PI) rot.z -= 2*PI; }
	Vector3 getRotation() { return rot; }
	void setScale(Vector3 scale) { this->scale = scale; }
	void setScale(float x, float y, float z) { scale = Vector3(x,y,z); }

private:
	DWORD mNumVertices;
	DWORD mNumFaces;

	ID3D10Device* md3dDevice;
	ID3D10Buffer* mVB;
	ID3D10Buffer* mIB;

	Color color;

	//sphere creation variables
	int index;
	vector<Vertex> vertices;
	vector<DWORD> indices;
	map<pair<int,int>,int> middlePointIndexCache; //pair(smaller index, larger index)

	//sphere creation functions
	int addVertex(Vector3 p); //add vertex to array, fix position to be on unit sphere, return index
	int getMiddlePoint(int p1, int p2); //return index of point in the middle of p1 and p2
	void create(int recursionLevel, float scale);
	void addIndices(vector<DWORD>& arr, int a, int b, int c); 
	Vertex setVertex(float x, float y, float z, Color c);

	bool active;
	Matrix world;
	ID3D10Effect* mFX;
	ID3D10EffectTechnique* mTech;
	ID3D10EffectMatrixVariable* mfxWVPVar;
	Matrix mWVP;

	Vector3 position;
	Vector3 velocity;
	float speed;

	//RST
	Vector3 trans, rot, scale;
	Matrix rotXM, rotYM, rotZM, transM, scaleM;
};

#endif