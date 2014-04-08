#ifndef VERTEX_H
#define VERTEX_H

#include "constants.h"

struct Vertex
{
	Vertex(){}
	Vertex(float x, float y, float z, 
		float nx, float ny, float nz, 
		float u, float v)
		: pos(x,y,z), normal(nx,ny,nz), texC(u,v){}
	Vertex(Vector3 pos, Vector3 normal, float u, float v)
		: pos(pos), normal(normal), texC(u,v) {}

	D3DXVECTOR3 pos;
	D3DXVECTOR3 normal;
	D3DXVECTOR2 texC;
};

#endif // VERTEX_H

