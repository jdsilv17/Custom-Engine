#pragma once

#include "Mesh.h"

template <typename T>
class Cube
{
public:
	Cube() {}
	~Cube() {}
	Cube(const Cube<T>& that);
	Cube<T>& operator=(const Cube<T>& that);

	Mesh<T> cube_mesh;

	std::vector<VERTEX_BASIC> _vertexList = 
	{
		// TOP
		VERTEX_BASIC({ -0.5f, 0.5f, -0.5f, 0.5f }, { 0.0f, 1.0f, 0.0f }, {-1.0f, 0.0f, 0.0f }),
		VERTEX_BASIC({ 0.5f, 0.5f, -0.5f, 0.5f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }),
		VERTEX_BASIC({ 0.5f, 0.5f, 0.5f, 0.5f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }),
		VERTEX_BASIC({ -0.5f, 0.5f, 0.5f, 0.5f }, { 0.0f, 1.0f, 0.0f },  { -1.0f, 1.0f, 0.0f }),
		// BOTTOM
		VERTEX_BASIC({ -0.5f, -0.5f, -0.5f, 0.5f }, { 0.0f, -1.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }),
		VERTEX_BASIC({ 0.5f, -0.5f, -0.5f, 0.5f }, { 0.0f, -1.0f, 0.0f }, { -1.0f, 0.0f, 0.0f }),
		VERTEX_BASIC({ 0.5f, -0.5f, 0.5f, 0.5f }, { 0.0f, -1.0f, 0.0f }, { -1.0f, 1.0f, 0.0f }),
		VERTEX_BASIC({ -0.5f, -0.5f, 0.5f, 0.5f }, { 0.0f, -1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }),
		// LEFT
		VERTEX_BASIC({ -0.5f, -0.5f, 0.5f, 0.5f }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }),
		VERTEX_BASIC({ -0.5f, -0.5f, -0.5f, 0.5f }, { -1.0f, 0.0f, 0.0f }, { -1.0f, 1.0f, 0.0f }),
		VERTEX_BASIC({ -0.5f, 0.5f, -0.5f, 0.5f }, { -1.0f, 0.0f, 0.0f }, { -1.0f, 0.0f, 0.0f }),
		VERTEX_BASIC({ -0.5f, 0.5f, 0.5f, 0.5f }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }),
		// RIGHT
		VERTEX_BASIC({ 0.5f, -0.5f, 0.5f, 0.5f }, { 1.0f, 0.0f, 0.0f }, { -1.0f, 1.0f, 0.0f }),
		VERTEX_BASIC({ 0.5f, -0.5f, -0.5f, 0.5f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }),
		VERTEX_BASIC({ 0.5f, 0.5f, -0.5f, 0.5f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }),
		VERTEX_BASIC({ 0.5f, 0.5f, 0.5f, 0.5f }, { 1.0f, 0.0f, 0.0f }, { -1.0f, 0.0f, 0.0f }),
		// FRONT
		VERTEX_BASIC({ -0.5f, -0.5f, -0.5f, 0.5f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 1.0f, 0.0f }),
		VERTEX_BASIC({ 0.5f, -0.5f, -0.5f, 0.5f }, { 0.0f, 0.0f, -1.0f }, { -1.0f, 1.0f, 0.0f }),
		VERTEX_BASIC({ 0.5f, 0.5f, -0.5f, 0.5f }, { 0.0f, 0.0f, -1.0f }, { -1.0f, 0.0f, 0.0f }),
		VERTEX_BASIC({ -0.5f, 0.5f, -0.5f, 0.5f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 0.0f, 0.0f }),
		// BACK
		VERTEX_BASIC({ -0.5f, -0.5f, 0.5f, 0.5f }, { 0.0f, 0.0f, 1.0f }, { -1.0f, 1.0f, 0.0f }),
		VERTEX_BASIC({ 0.5f, -0.5f, 0.5f, 0.5f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f }),
		VERTEX_BASIC({ 0.5f, 0.5f, 0.5f, 0.5f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }),
		VERTEX_BASIC({ -0.5f, 0.5f, 0.5f, 0.5f }, { 0.0f, 0.0f, 1.0f }, { -1.0f, 0.0f, 0.0f }),
	};

	std::vector<int> _indicesList =
	{
		0,1,3,
		3,1,2,

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

template<typename T>
Cube<T>::Cube(const Cube<T>& that)
{
	*this = that;
}

template<typename T>
Cube<T>& Cube<T>::operator=(const Cube<T>& that)
{
	if (this != &that)
	{
		this->cube_mesh = that.cube_mesh;
	}
	return *this;
}
