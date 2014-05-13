#pragma once

#include "d3dApp.h"
#include "d3dUtil.h"
#include "Vertex.h"
#include "GameObject.h"
#include "constants.h"

enum rotationAxis { X, Y, Z, YZ, ZY };

struct Layer {
	rotationAxis axis;
	int radius;
	Matrix rotations[NUM_WALLS];
	Matrix spin, translate, diagonal;
	GameObject walls[NUM_WALLS];
	float thetas[NUM_WALLS];
	float phis[NUM_WALLS];
	float startingTheta[NUM_WALLS];
	float startingPhi[NUM_WALLS];
	float regenTime[NUM_WALLS];
	int wallHealth[NUM_WALLS];
	
	Layer::Layer(rotationAxis a, int r);
	Layer::Layer() {}

	void init(ID3D10Device* device, float scale);
	void reinit(rotationAxis a, int r);
	void draw();

	void updateMatrices(float spinAmount);

private:
	DWORD mNumVertices;
	DWORD mNumFaces;

	ID3D10Device* md3dDevice;
	ID3D10Buffer* mVB;
	ID3D10Buffer* mIB;
};