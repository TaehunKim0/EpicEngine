#pragma once

#include <d3d11.h>
#include <D3DX10math.h>
#include <fstream>
using namespace std;

#include "Texture.h"

class Font
{
private:
	struct FontType //�۲��� �ε��� ������ �ε��� �����͸� �����ϱ� ���� ���
	{
		float left, right; //�ؽ��� U ��ǥ
		int size; //������ �ȼ� �ʺ� ����
	};

	//������ ���ڰ� �׷��� �簢���� ����� �� �ʿ��� ���� �����͸� �����մϴ�. 
	//������ ���ڴ� �簢���� ����� ���� �ﰢ�� �ΰ��� �ʿ��մϴ�.
	struct VertexType
	{
		D3DXVECTOR3	position; //������ ��ġ 
		D3DXVECTOR2 texture; //�ؽ��� ��ġ
	};

public:
	Font();
	Font(const Font&);
	~Font();

	bool Initialize(ID3D11Device*, char *, WCHAR*);
	void ShutDown();

	ID3D11ShaderResourceView* GetTexture();
	void BuildVertexArray(void *, char*, float, float); //���ڿ��� �Է����� �޾� �׸� �ﰢ������ ���� �迭�� ����� ��ȯ��. �׷��� ������ ������ Text ���� ȣ��

public:
	bool LoadFontData(char*);
	void ReleaseFontData();
	bool LoadTexture(ID3D11Device*, WCHAR*);
	void ReleaseTexture();

private:
	FontType* m_Font;
	Texture* m_Texture;
};

