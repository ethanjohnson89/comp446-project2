#ifndef LAYER_H
#define LAYER_H

#include "d3dUtil.h"
#include "GameObject.h"
#include <vector>

#include "constants.h"

class Layer
{
public:

	Layer();
	~Layer();

	//void init(ID3D10Device* device, float scale);
	void draw();
	void addShell();
	//removeShell();
	void update(float dt);
	//collided();
	Matrix getRotateMatrix() {return rotation;}

private:
	//vector<GameObject> shells;
	Matrix rotation;
	float spinAmount;

	//ID3D10Device* md3dDevice;
	//ID3D10Buffer* mVB;
	//ID3D10Buffer* mIB;
};

#endif // LAYER_H
