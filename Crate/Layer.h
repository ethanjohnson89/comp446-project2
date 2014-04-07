#pragma once

#include "d3dApp.h"
#include "d3dUtil.h"
#include "Vertex.h"
#include "GameObject.h"
#include "constants.h"

#define NUM_WALLS 16
#define NUM_LAYERS 5

enum rotationAxis { X, Y, Z, YZ, ZY };

struct Layer {
	rotationAxis axis;
	int radius;
	Matrix rotations[NUM_WALLS];
	Matrix spin, translate, diagonal;
	GameObject walls[NUM_WALLS];
	
	Layer::Layer(rotationAxis a, int r) : radius(r), axis(a) {}
	Layer::Layer() {}

	void init(ID3D10Device* device, float scale);
	void draw();

	void updateMatrices(float spinAmount);

private:
	DWORD mNumVertices;
	DWORD mNumFaces;

	ID3D10Device* md3dDevice;
	ID3D10Buffer* mVB;
	ID3D10Buffer* mIB;
};