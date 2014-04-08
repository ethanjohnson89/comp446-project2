//=======================================================================================
// Box.h by Frank Luna (C) 2008 All Rights Reserved.
//=======================================================================================

#ifndef SPLASHSCREEN_H
#define SPLASHSCREEN_H

#include "d3dUtil.h"

class Splashscreen
{
public:

	Splashscreen();
	~Splashscreen();

	void init(ID3D10Device* device, float scale);
	void init(ID3D10Device* device, float scale, ID3D10ShaderResourceView* mDiffuseMapRV, ID3D10ShaderResourceView* mSpecMapRV, ID3D10EffectTechnique* mTech);
	ID3D10ShaderResourceView* getDiffuseMapRV() {return mDiffuseMapRV;}
	ID3D10ShaderResourceView* getSpecMapRV() {return mSpecMapRV;}
	void draw();

private:
	DWORD mNumVertices;
	DWORD mNumFaces;

	ID3D10Device* md3dDevice;
	ID3D10Buffer* mVB;
	ID3D10Buffer* mIB;
	ID3D10ShaderResourceView* mDiffuseMapRV;
	ID3D10ShaderResourceView* mSpecMapRV;
	ID3D10EffectTechnique* mTech;
};

#endif // Splashscreen_H
