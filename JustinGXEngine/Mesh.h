#pragma once

#include "Object.h"

#ifndef MeshUtils_h_
	#define MeshUtils_h_
	#include "MeshUtils.h"
#endif // MeshUtils_h_

#ifndef RenderUtils_h_
	#define RenderUtils_h_
	#include "RenderUtils.h"
#endif // !RenderUtils_h_



#include <d3d11_1.h>
#include <wrl/client.h>
#include <vector>

using Microsoft::WRL::ComPtr;

template <typename T>
class Mesh : 
	public Object
{
public:
	Mesh() {}
	// ================== DEPRECATED ======================
	Mesh(	ID3D11Device* device, 
			ID3D11DeviceContext* deviceContext, 
			std::vector<T>& _vertexList, 
			std::vector<int>& _indicesList,
			D3D_PRIMITIVE_TOPOLOGY _primitive );
	// ================== DEPRECATED ======================
	Mesh(	ID3D11Device* device,
			ID3D11DeviceContext* deviceContext,
			std::vector<T>& _vertexList,
			D3D_PRIMITIVE_TOPOLOGY _primitive );
	Mesh(	ID3D11Device* device,
			ID3D11DeviceContext* deviceContext,
			const T* _vertexList,
			const int& _vertexCount,
			D3D_PRIMITIVE_TOPOLOGY _primitive);
	Mesh(	ID3D11Device* device,
			ID3D11DeviceContext* deviceContext,
			const T* _vertexList,
			const int& _vertexCount,
			const int* _indicesList,
			const int& _indexCount,
			D3D_PRIMITIVE_TOPOLOGY _primitive);

	~Mesh();								
	Mesh(const Mesh<T>& that);
	Mesh<T>& operator=(const Mesh<T>& that);

	// ================== DEPRECATED ======================
	void InitMesh(ID3D11Device* device);

	void InitMesh2(ID3D11Device* device);
	void Draw();

	ID3D11Buffer* GetVertexBuffer();
	ID3D11Buffer* const* GetAddressOfVB();
	ID3D11Buffer* GetIndexBuffer();
	ID3D11Buffer* const* GetAddressOfIB();
	ID3D11InputLayout* GetInputLayout();
	ID3D11InputLayout* const* GetAddressOfIL();

private:
	ComPtr<ID3D11DeviceContext> DeviceContext = nullptr;
	ComPtr<ID3D11Buffer> VertexBuffer = nullptr;
	ComPtr<ID3D11Buffer> IndexBuffer = nullptr;
	ComPtr<ID3D11InputLayout> InputLayout = nullptr;
	D3D_PRIMITIVE_TOPOLOGY Primitive = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	std::vector<T> Textures;

	const T** pVertexList = nullptr;
	const int** pIndicesList = nullptr;
	std::vector<T> VertexList;
	std::vector<int> IndicesList;
	int vertexCount = NULL;
	int indexCount = NULL;

};

template<typename T>
Mesh<T>::Mesh(	ID3D11Device* device, 
				ID3D11DeviceContext* _deviceContext, 
				std::vector<T>& _vertexList, 
				std::vector<int>& _indicesList,
				D3D_PRIMITIVE_TOPOLOGY _primitive)
{
	this->DeviceContext = _deviceContext;
	this->VertexList = _vertexList;
	vertexCount = (int)_vertexList.size();
	indexCount = (int)_indicesList.size();
	this->IndicesList = _indicesList;
	this->Primitive = _primitive;

	this->InitMesh(device);
}

template<typename T>
Mesh<T>::Mesh(ID3D11Device* device,
	ID3D11DeviceContext* _deviceContext,
	std::vector<T>& _vertexList,  
	D3D_PRIMITIVE_TOPOLOGY _primitive )
{
	this->DeviceContext = _deviceContext;
	this->VertexList = _vertexList;
	this->vertexCount = (int)_vertexList.size();
	this->Primitive = _primitive;

	this->InitMesh(device);
}

template<typename T>
Mesh<T>::Mesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const T* _vertexList, const int& _vertexCount, D3D_PRIMITIVE_TOPOLOGY _primitive)
{
	this->DeviceContext = deviceContext;
	this->pVertexList = &_vertexList;
	this->vertexCount = _vertexCount;
	this->Primitive = _primitive;

	this->InitMesh2(device);
}

template<typename T>
inline Mesh<T>::Mesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const T* _vertexList, const int& _vertexCount, const int* _indicesList, const int& _indexCount, D3D_PRIMITIVE_TOPOLOGY _primitive)
{
	this->DeviceContext = deviceContext;
	this->pVertexList = &_vertexList;
	this->vertexCount = _vertexCount;
	this->pIndicesList = &_indicesList;
	this->indexCount = _indexCount;
	this->Primitive = _primitive;

	this->InitMesh2(device);
}

template<typename T>
Mesh<T>::~Mesh()
{
	this->VertexList.clear();
	this->IndicesList.clear();
	this->vertexCount = 0;
	this->indexCount = 0;
}

template<typename T>
Mesh<T>::Mesh(const Mesh<T>& that)
{
	Object::Object(that)
	*this = that;
}

template<typename T>
Mesh<T>& Mesh<T>::operator=(const Mesh<T>& that)
{
	if (this != &that)
	{
		Object::operator= (that);

		this->VertexList.clear();
		this->IndicesList.clear();
		this->VertexList.shrink_to_fit();
		this->IndicesList.shrink_to_fit();
		this->vertexCount = 0;
		this->indexCount = 0;

		this->VertexList.reserve(that.VertexList.capacity());
		this->IndicesList.reserve(that.IndicesList.capacity());

		for (size_t i = 0; i < that.VertexList.size(); ++i)
			this->VertexList.push_back(that.VertexList[i]);

		for (size_t i = 0; i < that.IndicesList.size(); ++i)
			this->IndicesList.push_back(that.IndicesList[i]);

		this->DeviceContext = that.DeviceContext;
		this->VertexBuffer = that.VertexBuffer;
		this->IndexBuffer = that.IndexBuffer;

		this->vertexCount = that.vertexCount;
		this->indexCount = that.indexCount;

		this->Primitive = that.Primitive;
	}
	return *this;
}

template<typename T>
void Mesh<T>::InitMesh(ID3D11Device* device)
{
	HRESULT hr;
	this->vertexCount = (int)this->VertexList.size();
	this->indexCount = (int)this->IndicesList.size();

	hr = CreateVertexBuffer(device, this->vertexCount, this->VertexList, this->VertexBuffer);

	if (this->Primitive == D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
		hr = CreateIndexBuffer(device, this->indexCount, this->IndicesList, this->IndexBuffer);
}

template<typename T>
void Mesh<T>::InitMesh2(ID3D11Device* device)
{
	HRESULT hr;

	hr = CreateVertexBuffer(device, this->vertexCount, *this->pVertexList, this->VertexBuffer);

	if (this->Primitive == D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
		hr = CreateIndexBuffer(device, this->indexCount, *this->pIndicesList, this->IndexBuffer);
}

template<typename T>
void Mesh<T>::Draw()
{
	UINT offset = 0;
	UINT strides = sizeof(T);

	this->DeviceContext->IASetVertexBuffers(0, 1, this->VertexBuffer.GetAddressOf(), &strides, &offset);

	switch (this->Primitive)
	{
	case D3D11_PRIMITIVE_TOPOLOGY_POINTLIST:
		this->DeviceContext->IASetPrimitiveTopology(this->Primitive);
		this->DeviceContext->Draw(this->vertexCount, 0);
		break;
	case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST:
		this->DeviceContext->IASetPrimitiveTopology(this->Primitive);
		this->DeviceContext->IASetIndexBuffer(this->IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		this->DeviceContext->DrawIndexed(this->indexCount, 0, 0);
		break;
	case D3D11_PRIMITIVE_TOPOLOGY_LINELIST:
		this->DeviceContext->IASetPrimitiveTopology(this->Primitive);
		this->DeviceContext->Draw(this->vertexCount, 0);
		break;
	case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP:
		this->DeviceContext->IASetPrimitiveTopology(this->Primitive);
		this->DeviceContext->Draw(this->vertexCount, 0);
		break;
	default:
		break;
	}
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
ID3D11InputLayout* const* Mesh<T>::GetAddressOfIL()
{
	return InputLayout.GetAddressOf();
}
