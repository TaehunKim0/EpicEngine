#pragma once

#ifndef _MODEL_H_
#define  _MODEL_H_

#include <d3d11.h>
#include <d3dx10math.h>
#include <fstream>
#include "Texture.h"

class Texture;
class Model
{
private:
	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
		D3DXVECTOR3 normal;
	};

	//모델 포맷
	struct ModelType
	{
		float x, y, z; //위치
		float tu, tv; //텍스쳐
		float nx, ny, nz; //법선 벡터
	};

private:
	ID3D11Buffer* m_vertexBuffer, * m_indexBuffer;
	int m_vertexCount, m_indexCount;
	Texture* m_Texture;
	ModelType* m_Model;

public:
	Model();
	Model(const Model& other);
	~Model();

public:
	bool Initialize(ID3D11Device* device, char*, WCHAR*);
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

	bool LoadModel(char*);
	void ReleaseModel();
};

#endif