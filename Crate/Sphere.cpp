#include "Sphere.h"
#include "Vertex.h"

Sphere::Sphere()
: mNumVertices(0), mNumFaces(0), md3dDevice(0), mVB(0), mIB(0), position(VectorZero), velocity(VectorZero), speed(0),
active(true), trans(VectorZero), rot(VectorZero), scale(Vector3(1,1,1))
{
}
 
Sphere::~Sphere()
{
	ReleaseCOM(mVB);
	ReleaseCOM(mIB);
}

void Sphere::init(ID3D10Device* device, float scale, Color c, int recursionLevel)
{
	md3dDevice = device;
	color = c;
	create(recursionLevel,scale); //create sphere

	mNumVertices = vertices.size();
	mNumFaces = indices.size() / 3;

	//initialize vertex buffer
    D3D10_BUFFER_DESC vbd;
    vbd.Usage = D3D10_USAGE_DYNAMIC;
    vbd.ByteWidth = sizeof(Vertex) * mNumVertices;
    vbd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
    vbd.MiscFlags = 0;
    D3D10_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = &vertices[0];
    HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mVB));

	//initialize index buffer
	D3D10_BUFFER_DESC ibd;
    ibd.Usage = D3D10_USAGE_DYNAMIC;
    ibd.ByteWidth = sizeof(DWORD) * mNumFaces*3;
    ibd.BindFlags = D3D10_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
    ibd.MiscFlags = 0;
    D3D10_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = &indices[0];
    HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mIB));
}

void Sphere::update(float dt)
{
	position += velocity*dt;

	Identity(&world);
	Identity(&transM);
	Identity(&rotXM);
	Identity(&rotYM);
	Identity(&rotZM);
	Identity(&scaleM);

	RotateX(&rotXM, rot.x);
	RotateY(&rotYM, rot.y);
	RotateZ(&rotZM, rot.z); 
	Translate(&transM, position.x + trans.x, position.y + trans.y, position.z + trans.z);
	Scale(&scaleM,scale.x,scale.y,scale.z);

	world = scaleM * rotYM * rotXM * rotZM * transM;
}

void Sphere::draw(Matrix* mView, Matrix* mProj, ID3D10Effect* mFX, ID3D10EffectTechnique* mTech)
{
	if (!active)
		return;

	this->mFX = mFX;
	this->mTech = mTech;

	mWVP = world*(*mView)*(*mProj);
	mfxWVPVar = mFX->GetVariableByName("gWVP")->AsMatrix();
	mfxWVPVar->SetMatrix((float*)&mWVP);

    D3D10_TECHNIQUE_DESC techDesc;
    mTech->GetDesc( &techDesc );
    for(UINT p = 0; p < techDesc.Passes; ++p)
    {
        mTech->GetPassByIndex( p )->Apply(0);
        UINT stride = sizeof(Vertex);
		UINT offset = 0;
		md3dDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		md3dDevice->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);
		md3dDevice->IASetIndexBuffer(mIB, DXGI_FORMAT_R32_UINT, 0);
		md3dDevice->DrawIndexed(mNumFaces*3, 0, 0);
    }
}

Vertex Sphere::setVertex(float x, float y, float z, Color c)
{
	Vertex v;
	v.pos.x = x;
	v.pos.y = y;
	v.pos.z = z;
	//v.color = c;
	return v;
}

int Sphere::addVertex(Vector3 p)
{
	double length = sqrt(p.x * p.x + p.y * p.y + p.z * p.z);
	vertices.push_back(setVertex(p.x/length,p.y/length,p.z/length,color));
	return index++;
}

int Sphere::getMiddlePoint(int p1, int p2)
{
	//first check if we have it already
	bool firstIsSmaller = p1 < p2;
	int smallerIndex = firstIsSmaller ? p1 : p2;
	int greaterIndex = firstIsSmaller ? p2 : p1;
	map<pair<int,int>,int>::iterator it = middlePointIndexCache.find(pair<int,int>(smallerIndex,greaterIndex));
	if(it != middlePointIndexCache.end()) return (*it).second;

	//not in cache, calculate it
	Vertex point1 = vertices[p1];
	Vertex point2 = vertices[p2];
	Vertex middle = setVertex(
		(point1.pos.x + point2.pos.x) / 2.0,
		(point1.pos.y + point2.pos.y) / 2.0,
		(point1.pos.z + point2.pos.z) / 2.0,
		color);

	//add vertex makes sure point is on unit sphere
	int i = addVertex(middle.pos);

	//store it, return index
	middlePointIndexCache[pair<int,int>(smallerIndex,greaterIndex)] = i;
	return i;
}

void Sphere::addIndices(vector<DWORD>& arr, int a, int b, int c)
{
	arr.push_back(a);
	arr.push_back(b);
	arr.push_back(c);
}

void Sphere::create(int recursionLevel, float scale)
{
	/*int maxVertices;
	if(recursionLevel == 0) maxVertices = 12;
	else if(recursionLevel == 1) maxVertices = 20*pow(4.0,recursionLevel-1)*3 + 12;
	else maxVertices = 20*pow(4.0,recursionLevel-1)*3 + 20*pow(4.0,recursionLevel-2)*3;
	mNumVertices = 12;
	mNumFaces = 20*pow(4.0,recursionLevel);

	vertices = new Vertex[maxVertices];
	indices = new DWORD[mNumFaces*3];*/
	index = 0;

	//create 12 vertices of an icosahedron
	double t = (1.0 + sqrt(5.0)) / 2.0;

	addVertex(Vector3(-1,t,0));
	addVertex(Vector3(1,t,0));
	addVertex(Vector3(-1,-t,0));
	addVertex(Vector3(1,-t,0));

	addVertex(Vector3(0,-1,t));
	addVertex(Vector3(0,1,t));
	addVertex(Vector3(0,-1,-t));
	addVertex(Vector3(0,1,-t));

	addVertex(Vector3(t,0,-1));
	addVertex(Vector3(t,0,1));
	addVertex(Vector3(-t,0,-1));
	addVertex(Vector3(-t,0,1));

	//create 20 triangles of the icosahedron

	//5 faces around point 0
	addIndices(indices,0,11,5);
	addIndices(indices,0,5,1);
	addIndices(indices,0,1,7);
	addIndices(indices,0,7,10);
	addIndices(indices,0,10,11);

	//5 adjacent faces
	addIndices(indices,1,5,9);
	addIndices(indices,5,11,4);
	addIndices(indices,11,10,2);
	addIndices(indices,10,7,6);
	addIndices(indices,7,1,8);

	//5 faces around point 3
	addIndices(indices,3,9,4);
	addIndices(indices,3,4,2);
	addIndices(indices,3,2,6);
	addIndices(indices,3,6,8);
	addIndices(indices,3,8,9);

	//5 adjacent faces
	addIndices(indices,4,9,5);
	addIndices(indices,2,4,11);
	addIndices(indices,6,2,10);
	addIndices(indices,8,6,7);
	addIndices(indices,9,8,1);

	//refine triangles
	for(int i=0;i <recursionLevel; ++i)
	{
		vector<DWORD> indices2;
		for(int j=0; j<indices.size(); j+=3) //for each element in indices
		{
			//x = indices[j], y = indices[j+1], z = indices[j+2]
			int a = getMiddlePoint(indices[j],indices[j+1]);
			int b = getMiddlePoint(indices[j+1],indices[j+2]);
			int c = getMiddlePoint(indices[j+2],indices[j]);

			addIndices(indices2,indices[j],a,c);
			addIndices(indices2,indices[j+1],b,a);
			addIndices(indices2,indices[j+2],c,b);
			addIndices(indices2,a,b,c);
		}
		indices = indices2;
	}

	//scale vertices
	for(int i=0; i<vertices.size(); ++i)
	{
		vertices[i].pos.x *= scale;
		vertices[i].pos.y *= scale;
		vertices[i].pos.z *= scale;
	}
}