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
	{ }

	~Mesh()
	{ }

	void addFace(Face face)
	{
		vertices.push_back(face.p0);
		vertices.push_back(face.p1);
		vertices.push_back(face.p2);
	}

	// TODO: init()

	// TODO: draw()

//private: // DEBUG - commented
	std::vector<Vertex> vertices; // includes both positions and normals
	// Note: we're not using an index buffer for this class at the moment, since given the way it's
	// currently structured it'd be pointless (merely sequential). It'd be more efficient to store
	// a proper face list and build an index buffer from this when we buffer the vertices, but
	// since we have limited time (and not all that many vertices, relative to what our GPUs can
	// handle) it's not really worth the trouble right now.

	bool initialized;
};