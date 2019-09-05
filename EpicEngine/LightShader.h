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

	/*
	LightBufferType 구조체는 픽셀 셰이더의 상수 버퍼와 마찬가지로 반사광의 색상과 강도를 저장하도록 바뀌었습니다. 
	주의해야 할 것은 기존의 16바이트의 배수 크기를 유지하기 위한 padding을 없애고 그 자리에 specularPower을 넣었다는 사실입니다. 
	만약 padding을 넣지 않고 lightDirection 바로 밑에 specularColor을 넣었다면 셰이더가 올바로 동작하지 않았을 것입니다. 
	구조체의 크기가 16byte의 배수이긴 하지만 각 변수들이 16byte로 정렬되지 않았기 때문입니다.
	*/
	struct LightBufferType
	{
		D3DXVECTOR4 ambientColor;
		D3DXVECTOR4 diffuseColor;
		D3DXVECTOR3 lightDirection;
		float specularPower;
		D3DXVECTOR4 specularColor;
	};

	struct CameraBufferType //새로운 카메라 버퍼 구조체를 만듭니다
	{
		D3DXVECTOR3 cameraPosition;
		float padding;
	};


public:
	LightShader();
	LightShader(const LightShader&);
	~LightShader();

	bool Initialize(ID3D11Device*, HWND);
	void ShutDown();
	bool Render(ID3D11DeviceContext* deviceContext, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, D3DXVECTOR3 lightDirection, D3DXVECTOR4 ambientColor, D3DXVECTOR4 diffuseColor, D3DXVECTOR3 cameraPosition, D3DXVECTOR4 specularColor, float specularPower);

private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext* deviceContext, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, D3DXVECTOR3 lightDirection, D3DXVECTOR4 ambientColor, D3DXVECTOR4 diffuseColor, D3DXVECTOR3 cameraPosition, D3DXVECTOR4 specularColor, float specularPower);
	void RenderShader(ID3D11DeviceContext*, int);

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_cameraBuffer;
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