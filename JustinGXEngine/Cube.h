#pragma once

#include "Mesh.h"

class Cube :
	public Mesh<VERTEX_BASIC>
{
public:
	Cube() {}
	~Cube() {}
	//Cube(const Cube& that);
	//Cube& operator=(const Cube& that);
	Mesh<VERTEX_BASIC> cube_mesh;

	std::vector<VERTEX_BASIC> _vertexList = 
	{
		// TOP
		VERTEX_BASIC({ -0.5f, 0.5f, 0.5f }, { 0.0f, 1.0f, 0.0f }, {-1.0f, 0.0f, 0.0f }),
		VERTEX_BASIC({ 0.5f, 0.5f, 0.5f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }),
		VERTEX_BASIC({ -0.5f, 0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }),
		VERTEX_BASIC({ 0.5f, 0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f },  { -1.0f, 1.0f, 0.0f }),
		// BOTTOM
		VERTEX_BASIC({ -0.5f, -0.5f, -0.5f }, { 0.0f, -1.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }),
		VERTEX_BASIC({ 0.5f, -0.5f, -0.5f }, { 0.0f, -1.0f, 0.0f }, { -1.0f, 0.0f, 0.0f }),
		VERTEX_BASIC({ 0.5f, -0.5f, 0.5f }, { 0.0f, -1.0f, 0.0f }, { -1.0f, 1.0f, 0.0f }),
		VERTEX_BASIC({ -0.5f, -0.5f, 0.5f }, { 0.0f, -1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }),
		// LEFT
		VERTEX_BASIC({ -0.5f, -0.5f, 0.5f }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }),
		VERTEX_BASIC({ -0.5f, -0.5f, -0.5f }, { -1.0f, 0.0f, 0.0f }, { -1.0f, 1.0f, 0.0f }),
		VERTEX_BASIC({ -0.5f, 0.5f, -0.5f }, { -1.0f, 0.0f, 0.0f }, { -1.0f, 0.0f, 0.0f }),
		VERTEX_BASIC({ -0.5f, 0.5f, 0.5f }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }),
		// RIGHT
		VERTEX_BASIC({ 0.5f, -0.5f, 0.5f }, { 1.0f, 0.0f, 0.0f }, { -1.0f, 1.0f, 0.0f }),
		VERTEX_BASIC({ 0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }),
		VERTEX_BASIC({ 0.5f, 0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }),
		VERTEX_BASIC({ 0.5f, 0.5f, 0.5f }, { 1.0f, 0.0f, 0.0f }, { -1.0f, 0.0f, 0.0f }),
		// FRONT
		VERTEX_BASIC({ -0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 1.0f, 0.0f }),
		VERTEX_BASIC({ 0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f, -1.0f }, { -1.0f, 1.0f, 0.0f }),
		VERTEX_BASIC({ 0.5f, 0.5f, -0.5f }, { 0.0f, 0.0f, -1.0f }, { -1.0f, 0.0f, 0.0f }),
		VERTEX_BASIC({ -0.5f, 0.5f, -0.5f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 0.0f, 0.0f }),
		// BACK
		VERTEX_BASIC({ -0.5f, -0.5f, 0.5f }, { 0.0f, 0.0f, 1.0f }, { -1.0f, 1.0f, 0.0f }),
		VERTEX_BASIC({ 0.5f, -0.5f, 0.5f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f }),
		VERTEX_BASIC({ 0.5f, 0.5f, 0.5f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }),
		VERTEX_BASIC({ -0.5f, 0.5f, 0.5f }, { 0.0f, 0.0f, 1.0f }, { -1.0f, 0.0f, 0.0f }),
	};

	std::vector<int> _indicesList =
	{
		0,1,2,
		2,1,3,

		7,6,4,
		4,6,5,

		11,10,8,
		8,10,9,

		14,15,13,
		13,15,12,

		19,18,16,
		16,18,17,

		22,23,21,
		21,23,20
	};
};


