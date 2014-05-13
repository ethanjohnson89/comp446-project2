#include "Layer.h"

Layer::Layer(rotationAxis a, int r) {
	radius = r; 
	axis = a;
	for(int i=0; i<NUM_WALLS; i++)
	{
		regenTime[i] = 0;
		wallHealth[i] = 2;
	}

	if(axis==Y)
	{
		for(int i=0; i<NUM_WALLS; i++)
		{
			startingTheta[i] = i*2*PI/NUM_WALLS - (PI/2); // THAT PI/2 MAKES UP FOR DIFFERENCE IN STARTING THETAS FOR WALLS AND CAMERA
			startingPhi[i] = PI/2;
		}
	}
	if(axis==Z)
	{
		for(int i=0; i<NUM_WALLS; i++)
		{
			//startingTheta[i] = i*2*PI/NUM_WALLS; 
			startingPhi[i] = i*2*PI/NUM_WALLS;
		}
	}
	if(axis==X || axis==YZ || axis==ZY)
	{
		for(int i=0; i<NUM_WALLS; i++)
		{
			//startingTheta[i] = i*2*PI/NUM_WALLS + PI; 
			//startingPhi[i] = i*2*PI/NUM_WALLS + PI/2; 
			float t = i*2*PI/NUM_WALLS + PI/2;
			int temp = static_cast<int>(t/(2*PI));
			startingPhi[i] = t - static_cast<float>(temp)*(2*PI);
			//phis[i] = startingPhi[i];
		}
	}
	/*if(axis==YZ || axis==ZY)
	{
		for(int i=0; i<NUM_WALLS; i++)
		{
			startingPhi[i] = i*2*PI/NUM_WALLS;
		}
	}
	*/
}

void Layer::init(ID3D10Device* device, float scale)
{
	md3dDevice = device;
 
	mNumVertices = 24;
	mNumFaces    = 12; // 2 per quad

	// Create vertex buffer
    Vertex v[24];

	// ***** TODO: replace Box vertex data with vertices for portion of spherical shell

	// Fill in the front face vertex data.
 	v[0] = Vertex(-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 2.0f);
	v[1] = Vertex(-1.0f,  1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f); //CHANGE THESE LAST TWO FOR DIFFERENT SIZE/NUMBER OF TEXTURE IMAGES
	v[2] = Vertex( 1.0f,  1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 2.0f, 0.0f);
	v[3] = Vertex( 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 2.0f, 2.0f);

	// Fill in the back face vertex data.
	v[4] = Vertex(-1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
	v[5] = Vertex( 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
	v[6] = Vertex( 1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
	v[7] = Vertex(-1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);

	// Fill in the top face vertex data.
	v[8]  = Vertex(-1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
	v[9]  = Vertex(-1.0f, 1.0f,  1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
	v[10] = Vertex( 1.0f, 1.0f,  1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);
	v[11] = Vertex( 1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f);

	// Fill in the bottom face vertex data.
	v[12] = Vertex(-1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f);
	v[13] = Vertex( 1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f);
	v[14] = Vertex( 1.0f, -1.0f,  1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f);
	v[15] = Vertex(-1.0f, -1.0f,  1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f);

	// Fill in the left face vertex data.
	v[16] = Vertex(-1.0f, -1.0f,  1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[17] = Vertex(-1.0f,  1.0f,  1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[18] = Vertex(-1.0f,  1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	v[19] = Vertex(-1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

	// Fill in the right face vertex data.
	v[20] = Vertex( 1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[21] = Vertex( 1.0f,  1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[22] = Vertex( 1.0f,  1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	v[23] = Vertex( 1.0f, -1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

	// Scale the box.
	for(DWORD i = 0; i < mNumVertices; ++i)
		v[i].pos *= scale;


    D3D10_BUFFER_DESC vbd;
    vbd.Usage = D3D10_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof(Vertex) * mNumVertices;
    vbd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    D3D10_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = v;
    HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mVB));


	// Create the index buffer

	DWORD i[36];

	// Fill in the front face index data
	i[0] = 0; i[1] = 1; i[2] = 2;
	i[3] = 0; i[4] = 2; i[5] = 3;

	// Fill in the back face index data
	i[6] = 4; i[7]  = 5; i[8]  = 6;
	i[9] = 4; i[10] = 6; i[11] = 7;

	// Fill in the top face index data
	i[12] = 8; i[13] =  9; i[14] = 10;
	i[15] = 8; i[16] = 10; i[17] = 11;

	// Fill in the bottom face index data
	i[18] = 12; i[19] = 13; i[20] = 14;
	i[21] = 12; i[22] = 14; i[23] = 15;

	// Fill in the left face index data
	i[24] = 16; i[25] = 17; i[26] = 18;
	i[27] = 16; i[28] = 18; i[29] = 19;

	// Fill in the right face index data
	i[30] = 20; i[31] = 21; i[32] = 22;
	i[33] = 20; i[34] = 22; i[35] = 23;

	D3D10_BUFFER_DESC ibd;
    ibd.Usage = D3D10_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(DWORD) * mNumFaces*3;
    ibd.BindFlags = D3D10_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    D3D10_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = i;
    HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mIB));
}

void Layer::draw()
{
	UINT stride = sizeof(Vertex);
    UINT offset = 0;
    md3dDevice->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);
	md3dDevice->IASetIndexBuffer(mIB, DXGI_FORMAT_R32_UINT, 0);
	md3dDevice->DrawIndexed(mNumFaces*3, 0, 0);
}

void Layer::updateMatrices(float spinAmount)
{
	if(axis==Y)
	{
		Translate(&translate,radius,0,0); //CAN PUT SOMETHING LIKE radius*5*spinAmount/36.0f TO HAVE IT GROW IN AND OUT
		RotateY(&spin, ToRadian(spinAmount*50));
		for(int i=0; i<NUM_WALLS; i++)
		{
			RotateY(&rotations[i], i*2*PI/NUM_WALLS);
			//thetas[i] = 2*PI*spinAmount/(6*36);
			float t = ToRadian(spinAmount*50) + startingTheta[i];
			int temp = static_cast<int>(t/(2*PI));
			thetas[i] = t - static_cast<float>(temp)*(2*PI);
			phis[i] = startingPhi[i];
			//if(thetas[i] > 2*PI) thetas[i] = 0;
		}
	}
	if(axis==Z)
	{
		Translate(&translate,0,radius,0);
		RotateZ(&spin, ToRadian(spinAmount*50));
		for(int i=0; i<NUM_WALLS; i++)
		{
			RotateZ(&rotations[i], i*2*PI/NUM_WALLS);
			float t = ToRadian(spinAmount*50) + startingPhi[i];
			int temp = static_cast<int>(t/(2*PI));
			float tempPhi = t - static_cast<float>(temp)*(2*PI);
			if(tempPhi > PI)
			{
				phis[i] = 2*PI - tempPhi; //IF ON RIGHT SIDE GOING BACK UP
				thetas[i] = 3*PI/2;
			}
			else 
			{
				phis[i] = tempPhi;
				thetas[i] = PI/2;
			}
			//phis[i] = startingPhi[i];
		}
	}
	if(axis==X || axis==ZY || axis==YZ)
	{
		Translate(&translate,0,0,radius);
		RotateX(&spin, ToRadian(spinAmount*50));
		for(int i=0; i<NUM_WALLS; i++)
		{
			RotateX(&rotations[i], i*2*PI/NUM_WALLS);
			float t = ToRadian(spinAmount*50) + startingPhi[i];
			int temp = static_cast<int>(t/(2*PI));
			float tempPhi = t - static_cast<float>(temp)*(2*PI);
			if(tempPhi > PI)
			{
				phis[i] = 2*PI - tempPhi; 
				thetas[i] = 0;
			}
			else 
			{
				phis[i] = tempPhi;
				thetas[i] = PI;
			}
		}
	}
	if(axis==YZ)
	{
		RotateY(&diagonal, PI/4.0f);
		for(int i=0; i<NUM_WALLS; i++)
		{
			thetas[i] += PI/4;
		}

	}
	else if(axis==ZY)
	{
		RotateY(&diagonal, -PI/4.0f);
		for(int i=0; i<NUM_WALLS; i++)
		{
				
			float t = thetas[i] + 7*PI/4;
			int temp = static_cast<int>(t/(2*PI));
			thetas[i] = t - static_cast<float>(temp)*(2*PI);
		}
	}
}