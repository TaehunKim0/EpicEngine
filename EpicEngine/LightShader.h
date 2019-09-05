#pragma once


#include<d3d11.h>
#include<d3dx10math.h>
#include<d3dx11async.h>
#include<fstream>
using namespace std;

class LightShader
{
private:
	struct MatrixBufferType
	{
		D3DXMATRIX world;
		D3DXMATRIX view;
		D3DXMATRIX projection;
	};

	struct LightBufferType
	{
		D3DXVECTOR4 ambientColor;
		D3DXVECTOR4 diffuseColor;
		D3DXVECTOR3 lightDirection;
		float padding; // 나중에 CreateBuffer를 할 때 버퍼가 16바이트의 배수가 되어야 하므로 padding이라는 더미 변수를 넣었다. //28byte -> 32byte
	};


public:
	LightShader();
	LightShader(const LightShader&);
	~LightShader();

	bool Initialize(ID3D11Device*, HWND);
	void ShutDown();
	bool Render(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, D3DXVECTOR3, D3DXVECTOR4, D3DXVECTOR4);

private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, D3DXVECTOR3, D3DXVECTOR4, D3DXVECTOR4);
	void RenderShader(ID3D11DeviceContext*, int);

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_lightBuffer;

	ID3D11SamplerState* m_sampleState;
};

/*
상수 버퍼를 만드는 이유는 셰이더에서 시맨틱을 할당한 변수들을 담을 공간이 필요한데 그 공간이 상수 버퍼이기 때문이다.
Dx9 에서는 셰이더의 변수들이 글로벌이라 상수 버퍼가 필요하지 않았다.

*/

/*
샘플러
1. 렌더링파이프라인 중 VertexShader 스테이지에서 정점 데이터를 수학적 연산처리를 한다.
2. 정점이 가지는 정보는 공간상의 좌표정보, 색상, 텍스쳐, 조명 정보 등등 프로그래머가 정의한 정보들이다.
3. 텍스쳐를 처리하는 경우, Sampler를 통해 텍스쳐 정보를 처리한다.
4. 텍스쳐를 반복해서 출력할지, 확대해서 출력할지 등이 설정가능하다.
*/