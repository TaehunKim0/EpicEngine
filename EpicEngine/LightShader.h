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
	LightBufferType ����ü�� �ȼ� ���̴��� ��� ���ۿ� ���������� �ݻ籤�� ����� ������ �����ϵ��� �ٲ�����ϴ�. 
	�����ؾ� �� ���� ������ 16����Ʈ�� ��� ũ�⸦ �����ϱ� ���� padding�� ���ְ� �� �ڸ��� specularPower�� �־��ٴ� ����Դϴ�. 
	���� padding�� ���� �ʰ� lightDirection �ٷ� �ؿ� specularColor�� �־��ٸ� ���̴��� �ùٷ� �������� �ʾ��� ���Դϴ�. 
	����ü�� ũ�Ⱑ 16byte�� ����̱� ������ �� �������� 16byte�� ���ĵ��� �ʾұ� �����Դϴ�.
	*/
	struct LightBufferType
	{
		D3DXVECTOR4 ambientColor;
		D3DXVECTOR4 diffuseColor;
		D3DXVECTOR3 lightDirection;
		float specularPower;
		D3DXVECTOR4 specularColor;
	};

	struct CameraBufferType //���ο� ī�޶� ���� ����ü�� ����ϴ�
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
��� ���۸� ����� ������ ���̴����� �ø�ƽ�� �Ҵ��� �������� ���� ������ �ʿ��ѵ� �� ������ ��� �����̱� �����̴�.
Dx9 ������ ���̴��� �������� �۷ι��̶� ��� ���۰� �ʿ����� �ʾҴ�.

*/

/*
���÷�
1. ���������������� �� VertexShader ������������ ���� �����͸� ������ ����ó���� �Ѵ�.
2. ������ ������ ������ �������� ��ǥ����, ����, �ؽ���, ���� ���� ��� ���α׷��Ӱ� ������ �������̴�.
3. �ؽ��ĸ� ó���ϴ� ���, Sampler�� ���� �ؽ��� ������ ó���Ѵ�.
4. �ؽ��ĸ� �ݺ��ؼ� �������, Ȯ���ؼ� ������� ���� ���������ϴ�.
*/