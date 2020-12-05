#pragma once

#ifndef MeshUtils
	#include "MeshUtils.h"
	#define MeshUtils
#endif // MeshUtils

#include <d3d11_1.h>
#include <wrl/client.h>
#include <vector>

using Microsoft::WRL::ComPtr;

template <typename T>
class Mesh
{
public:
	
	std::vector<T> VertexList;
	std::vector<int> IndicesList;

	int vertexCount = NULL;
	int indexCount = NULL;

	Mesh() {}

	~Mesh();								
	Mesh(const Mesh<T>& that);					
	Mesh<T>& operator=(const Mesh<T>& that);		

private:
	ComPtr<ID3D11Buffer> VertexBuffer = nullptr;
	ComPtr<ID3D11Buffer> IndexBuffer = nullptr;
	ComPtr<ID3D11InputLayout> InputLayout = nullptr;

};

template<typename T>
Mesh<T>::~Mesh()
{
	this->VertexList.clear();
	this->IndicesList.clear();
}

template<typename T>
Mesh<T>::Mesh(const Mesh<T>& that)
{
	*this = that;
}

template<typename T>
Mesh<T>& Mesh<T>::operator=(const Mesh<T>& that)
{
	if (this != &that)
	{
		this->VertexList.reserve(that.VertexList.capacity());
		this->IndicesList.reserve(that.IndicesList.capacity());

		this->VertexList.resize(that.VertexList.size());
		this->IndicesList.resize(that.IndicesList.size());

		for (size_t i = 0; i < that.VertexList.size(); ++i)
			this->VertexList.push_back(that.VertexList[i]);

		for (size_t i = 0; i < that.IndicesList.size(); ++i)
			this->IndicesList.push_back(that.IndicesList[i]);
	}
	return *this;
}
