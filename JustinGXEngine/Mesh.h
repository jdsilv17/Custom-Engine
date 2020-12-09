#pragma once

#ifndef MeshUtils_h_
	#include "MeshUtils.h"
	#define MeshUtils_h_
#endif // MeshUtils_h_

#ifndef RenderUtils_h_
	#include "RenderUtils.h"
	#define RenderUtils_h_
#endif // !RenderUtils_h_


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
	Mesh() {} // maybe by default it is a cube

	~Mesh();								
	Mesh(const Mesh<T>& that);					
	Mesh<T>& operator=(const Mesh<T>& that);	

	void InitMesh(ID3D11Device* device);
	void Draw();

	ID3D11Buffer* GetVertexBuffer() /*const*/;
	ID3D11Buffer* const* GetAddressOfVB();
	ID3D11Buffer* GetIndexBuffer();
	ID3D11Buffer* const* GetAddressOfIB();
	ID3D11InputLayout* GetInputLayout();
	ID3D11InputLayout* const* GetAddressOfIL();

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

template<typename T>
void Mesh<T>::InitMesh(ID3D11Device* device)
{
	CreateVertexBuffer(device, this->vertexCount, this->VertexList.data(), this->VertexBuffer);
	CreateIndexBuffer(device, this->indexCount, this->IndicesList.data(), this->IndexBuffer);
}

template<typename T>
ID3D11Buffer* Mesh<T>::GetVertexBuffer()
{
	return VertexBuffer.Get();
}

template<typename T>
ID3D11Buffer* const* Mesh<T>::GetAddressOfVB()
{
	return VertexBuffer.GetAddressOf();
}

template<typename T>
ID3D11Buffer* Mesh<T>::GetIndexBuffer()
{
	return IndexBuffer.Get();
}

template<typename T>
ID3D11Buffer* const* Mesh<T>::GetAddressOfIB()
{
	return IndexBuffer.GetAddressOf();
}

template<typename T>
ID3D11InputLayout* Mesh<T>::GetInputLayout()
{
	return InputLayout.Get();
}

template<typename T>
ID3D11Buffer* const* Mesh<T>::GetAddressOfIL()
{
	return InputLayout.GetAddressOf();
}
