#pragma once

#include <d3d11.h>
#include <D3DX10math.h>
#include <fstream>
using namespace std;

#include "Texture.h"

class Font
{
private:
	struct FontType //글꼴의 인덱스 파일의 인덱스 데이터를 저장하기 위해 사용
	{
		float left, right; //텍스쳐 U 좌표
		int size; //문자의 픽셀 너비 저장
	};

	//실제로 문자가 그려질 사각형을 만드는 데 필요한 정점 데이터를 저장합니다. 
	//각각의 문자는 사각형을 만들기 위해 삼각형 두개가 필요합니다.
	struct VertexType
	{
		D3DXVECTOR3	position; //공간상 위치 
		D3DXVECTOR2 texture; //텍스쳐 위치
	};

public:
	Font();
	Font(const Font&);
	~Font();

	bool Initialize(ID3D11Device*, char *, WCHAR*);
	void ShutDown();

	ID3D11ShaderResourceView* GetTexture();
	void BuildVertexArray(void *, char*, float, float); //문자열을 입력으로 받아 그릴 삼각형들의 정점 배열을 만들어 반환함. 그려낼 문장이 있을때 Text 에서 호출

public:
	bool LoadFontData(char*);
	void ReleaseFontData();
	bool LoadTexture(ID3D11Device*, WCHAR*);
	void ReleaseTexture();

private:
	FontType* m_Font;
	Texture* m_Texture;
};

