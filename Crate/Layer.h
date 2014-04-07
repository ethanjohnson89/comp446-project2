#include "d3dApp.h"
#include "GameObject.h"

#define NUM_WALLS 16
#define NUM_LAYERS 5

enum rotationAxis { X, Y, Z, YZ, ZY };

struct Layer {
	rotationAxis axis;
	int radius;
	Matrix rotations[NUM_WALLS];
	Matrix spin, translate, diagonal;
	GameObject walls[NUM_WALLS];
	
	Layer::Layer(rotationAxis a, int r) {radius = r; axis = a;}
	Layer::Layer() {}

	void updateMatrices(float spinAmount)
	{
		if(axis==Y)
		{
			Translate(&translate,radius,0,0); //CAN PUT SOMETHING LIKE radius*5*spinAmount/36.0f TO HAVE IT GROW IN AND OUT
			RotateY(&spin, ToRadian(spinAmount*50));
			for(int i=0; i<NUM_WALLS; i++)
				RotateY(&rotations[i], i*2*PI/NUM_WALLS);
		}
		else if(axis==Z)
		{
			Translate(&translate,0,radius,0);
			RotateZ(&spin, ToRadian(spinAmount*50));
			for(int i=0; i<NUM_WALLS; i++)
				RotateZ(&rotations[i], i*2*PI/NUM_WALLS);
		}
		else if(axis==X)
		{
			Translate(&translate,0,0,radius);
			RotateX(&spin, ToRadian(spinAmount*50));
			for(int i=0; i<NUM_WALLS; i++)
				RotateX(&rotations[i], i*2*PI/NUM_WALLS);
		}
		else if(axis==YZ)
		{
			Translate(&translate,0,0,radius);
			RotateX(&spin, ToRadian(spinAmount*50));
			RotateY(&diagonal, PI/4.0f);
			for(int i=0; i<NUM_WALLS; i++)
				RotateX(&rotations[i], i*2*PI/NUM_WALLS);
		}
		else if(axis==ZY)
		{
			Translate(&translate,0,0,radius);
			RotateX(&spin, ToRadian(spinAmount*50));
			RotateY(&diagonal, -PI/4.0f);
			for(int i=0; i<NUM_WALLS; i++)
				RotateX(&rotations[i], i*2*PI/NUM_WALLS);
		}
	}
};