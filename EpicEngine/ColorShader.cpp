#include"stdafx.h"
#include "ColorShader.h"


ColorShader::ColorShader()
{
	m_vertexShader = nullptr;
	m_pixelShader = nullptr;
	m_layout = nullptr;
	m_matrixBuffer = nullptr;
	
}

ColorShader::ColorShader(const ColorShader& other) : ColorShader()
{
}

ColorShader::~ColorShader()
{
}
typedef WCHAR TCHAR;

bool ColorShader::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result; 
	const WCHAR* pwcsName = L"../EpicEngine/color.vs";
	wchar_t* tempWide = const_cast <wchar_t*> (pwcsName);
	
	const WCHAR* pwcsName1 = L"../EpicEngine/color.ps";
	wchar_t* tempWide1 = const_cast <wchar_t*> (pwcsName1);

	//���� ���̴��� �ȼ� ���̴��� �ʱ�ȭ�մϴ�.
	result = InitializeShader(device, hwnd, tempWide, tempWide1);
	if (!result)
		return false;

	return true;	
}

void ColorShader::Shutdown()
{
	ShutdownShader();
}

bool ColorShader::Render(ID3D11DeviceContext* deviceContext, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix)
{
	bool result;

	//�������� ����� ���̴��� ���ڸ� �Է��մϴ�.
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix);
	if(!result)
		return false;

	//���̴��� �̿��Ͽ� �غ�� ���۸� �׸��ϴ�.
	RenderShader(deviceContext, indexCount);

	return true;
}

bool ColorShader::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT result;
	ID3D10Blob* errorMsg;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;	
	D3D11_BUFFER_DESC matrixBufferDesc;

	// ���Լ����� ����ϴ� �����͵��� null�� ����
	errorMsg = nullptr;
	vertexShaderBuffer = nullptr;
	pixelShaderBuffer = nullptr;

	//���� ���̴��� �������մϴ�.
	result = D3DX11CompileFromFile(vsFilename, NULL, NULL, "ColorVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL,
		&vertexShaderBuffer, &errorMsg, NULL);
	
	if (FAILED(result))
	{
		//���̴� �����Ͽ� �����ϸ� ���� �޽��� ���
		if (errorMsg)
		{
			OutputShaderErrorMessage(errorMsg, hwnd, vsFilename);
		}
		else
		{
			MessageBox(hwnd, vsFilename, L"Missing Shader", MB_OK);
		}

		return false;
	}

	//�ȼ� ���̴��� �������մϴ�.
	result = D3DX11CompileFromFile(psFilename, NULL, NULL, "ColorPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS,
		0, NULL, &pixelShaderBuffer, &errorMsg, NULL);

	if (FAILED(result))
	{
		//���̴� �����Ͽ� �����ϸ� ���� �޽����� ����մϴ�.
		if (errorMsg)
		{
			OutputShaderErrorMessage(errorMsg, hwnd, psFilename);
		}
		else
		{
			MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	/*
	���� ���̴��� �ȼ� ���̴��� ���۷� �� �����ϵǸ� �̸� �̿��Ͽ� ���̴� ��ü�� ���� �� �ֽ��ϴ�.
	���⼭ ���� �����͸� ���� ���̴��� �ȼ� ���̴��� �������̽��μ� ����� ���Դϴ�.
	*/

	//���۷κ��� ���� ���̴� ����
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result))
		return false;

	//���۷κ��� �ȼ� ���̴� ����
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result))
		return false;

	//���� �Է� ���̾ƿ� description �� �ۼ�
	//�� ������ Modelclass�� ���̴��� �ִ� vertextype�� ��ġ�ؾ� ��
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;	
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "COLOR";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	//���� ���̾ƿ��� ������ D3D ����̽��� ����� �Է� ���̾ƿ��� �����մϴ�. 
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	//���� �Է� ���̾ƿ��� �����մϴ�.
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), &m_layout);

	if (FAILED(result))
		return false;

	//�� �̻� ������ �ʴ� ���� ���̴� ���ۿ� �ȼ� ���̴� ���۸� �����մϴ�.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = nullptr;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = nullptr;

	//���̴��� ����ϱ� ���� ������ �ܰ�� ��� ����. ������ �ϳ��� ��� ���۸� ����ϱ� ������ �̰��� �����Ͽ� 
	//���̴��� ���� �������̽��� ����� �� �ִ�.
	//�� �����Ӹ��� ������۸� ������Ʈ�ϱ� ������ ������ ����� ������ �Ǿ���.

	//���� ���̴��� �ִ� ��� ��� ������ description �� �ۼ�
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	//��� ���� �����͸� ����� �� Ŭ�������� ���� ���̴� ��� ���ۿ� ������ �� �ְ� �մϴ�.
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result))
		return false;

	return true;
}

void ColorShader::ShutdownShader()
{
	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = nullptr;
	}

	if (m_layout)
	{
		m_layout->Release();
		m_layout = nullptr;
	}

	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = nullptr;
	}

	if (m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = nullptr;
	}
}

void ColorShader::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
	char* compileError;
	unsigned long bufferSize, i;
	ofstream fout;

	//���� �޼����� ��� �ִ� ���ڿ� ������ �����͸� �����ɴϴ�.
	compileError = (char*)(errorMessage->GetBufferPointer());

	//�޼����� ���̸� ������
	bufferSize = errorMessage->GetBufferSize();

	//���� �޽����� ��
	for (i = 0; i < bufferSize; i++)
	{
		fout << compileError[i];
	}

	//���� �ݱ�
	fout.close();

	//���� �޽��� ��ȯ
	errorMessage->Release();
	errorMessage = nullptr;

	//������ ������ ������ �˾� �޽����� �˷��ݴϴ�.
	MessageBox(hwnd, L"Error compiling shader. Check shader-error.txt for message.", shaderFilename, MB_OK);

}
//���̴��� ���� ������ ���� �ٷ� �� �ֵ��� �ϱ� ���� ����������ϴ�. �� �Լ��� ���� ��ĵ��� GraphicsClass���� ������� ���Դϴ�.
bool ColorShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix)
{
	HRESULT	result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	unsigned int bufferNumber;

	//����� transpose�Ͽ� ���̴����� ����� �� �ְ� �մϴ�.
	D3DXMatrixTranspose(&worldMatrix, &worldMatrix);
	D3DXMatrixTranspose(&viewMatrix, &viewMatrix);
	D3DXMatrixTranspose(&projectionMatrix, &projectionMatrix);

	//��� ������ ������ �� �� �ְ� ��޴ϴ�.
	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
		return false;

	//��� ������ �����Ϳ� ���� �����͸� �����ɴϴ�.
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	//��� ���ۿ� ����� �����մϴ�.
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	//��� ������ ����� Ǳ�ϴ�.
	deviceContext->Unmap(m_matrixBuffer, 0);

	//���� ���̴����� ��� ������ ��ġ�� �����մϴ�.
	bufferNumber = 0;

	//���������� ���� ���̴��� ��� ���۸� �ٲ� ������ �ٲߴϴ�.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	return true;
}

void ColorShader::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	//���� �Է� ���̾ƿ��� �����մϴ�.
	deviceContext->IASetInputLayout(m_layout);

	//�ﰢ���� �׸� ���� ���̴��� �ȼ� ���̴��� �����մϴ�.
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	//�ﰢ���� �׸��ϴ�.
	deviceContext->DrawIndexed(indexCount, 0, 0);
}