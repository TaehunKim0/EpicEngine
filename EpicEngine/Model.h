#pragma once

#ifndef _MODEL_H_
#define  _MODEL_H_

#include <d3d11.h>
#include <d3dx10math.h>
#include "Texture.h"

class Texture;
class Model
{
private:
	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
	};

private:
	ID3D11Buffer* m_vertexBuffer, * m_indexBuffer;
	int m_vertexCount, m_indexCount;
	Texture* m_Texture;

public:
	Model();
	Model(const Model& other);
	~Model();

public:
	bool Initialize(ID3D11Device* device , WCHAR*);
	void Shutdown();
	void Render(ID3D11DeviceContext* deviceContext);

	int GetIndexCount();

	ID3D11ShaderResourceView* GetTexture();

private:
	bool InitializeBuffers(ID3D11Device* device);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext* deviceContext);

	bool LoadTexture(ID3D11Device*, WCHAR*);
	void ReleaseTexture();


};

#endif