#pragma once

/*
ȭ�鿡 �׸��µ� �ʿ��� �� �̹����� ǥ���ϴµ� ��G�մϴ�.
3D��ü ��� 2D�̹����� �ٷ�� ModelClass�� �����̶�� ��
*/

#include"Texture.h"

class Bitmap
{
private:
	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
	};

public:
	Bitmap();
	Bitmap(const Bitmap&);
	~Bitmap();

	bool Initialize(ID3D11Device*, int, int, WCHAR*, int, int);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, int);

	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture();

private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	bool UpdateBuffers(ID3D11DeviceContext*, int, int);
	void RenderBuffers(ID3D11DeviceContext*);

	bool LoadTexture(ID3D11Device*, WCHAR*);
	void ReleaseTexture();

private:
	ID3D11Buffer* m_vertexBuffer, *m_indexBuffer;
	int m_vertexCount, m_indexCount;
	Texture* m_Texture;

	/*
	3D �𵨰��� �޸� BitmapClass������ ȭ�� ũ��, �̹��� ũ��, ������ �׷����� ��ġ�� ����ؾ� �մϴ�.
	���� �� �������� �����ϴ� ���� ������ ���մϴ�.
	*/

	int m_screenWidth, m_screenHeight;
	int m_bitmapWidth, m_bitmapHeight;
	int m_previousPosX, m_previousPosY;
	
};

