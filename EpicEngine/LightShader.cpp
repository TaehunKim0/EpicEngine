#include "stdafx.h"
#include "LightShader.h"

LightShader::LightShader()
{
	m_vertexShader = nullptr;
	m_pixelShader = nullptr;
	m_layout = nullptr;
	m_matrixBuffer = nullptr;
	m_sampleState = nullptr;
	m_lightBuffer = nullptr;
}

LightShader::LightShader(const LightShader& other) : LightShader()
{
}


LightShader::~LightShader()
{
}

bool LightShader::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;

	const WCHAR* pwcsName = L"../EpicEngine/Light.vs";
	wchar_t* tempWide = const_cast <wchar_t*> (pwcsName);

	const WCHAR* pwcsName2 = L"../EpicEngine/Light.ps";
	wchar_t* tempWide2 = const_cast <wchar_t*> (pwcsName2);

	//버텍스와 픽셀 셰이더 초기화
	result = InitializeShader(device, hwnd, tempWide, tempWide2);
	if (!result)
		return false;

	return true;
}

void LightShader::ShutDown()
{
	ShutdownShader();
}

bool LightShader::Render(ID3D11DeviceContext* deviceContext, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, D3DXVECTOR3 lightDirection, D3DXVECTOR4 ambientColor, D3DXVECTOR4 diffuseColor)
{
	bool result;

	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture, lightDirection, ambientColor, diffuseColor);
	if (!result)
		return false;

	//이제 준비된 버퍼를 셰이더로 렌더링하십시오. 
	RenderShader(deviceContext, indexCount);

	return true;
}

//정점 셰이더와 픽셀 셰이더에 새 텍스쳐를 불러옵니다.
bool LightShader::InitializeShader(ID3D11Device* device, HWND	hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT result;
	ID3D10Blob* errorMessage; //이 인터페이스는 임의의 길이의 데이터를 반환하는 데 사용됩니다.(ID3D10Blob)
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[3];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC lightBufferDesc;

	//초기화
	errorMessage = nullptr;
	vertexShaderBuffer = nullptr;
	pixelShaderBuffer = nullptr;

	//버텍스 셰이더 컴파일
	result = D3DX11CompileFromFile(vsFilename, NULL, NULL, "LightVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL,
		&vertexShaderBuffer, &errorMessage, NULL);

	if (FAILED(result))
	{
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		else
		{
			MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	result = D3DX11CompileFromFile(psFilename, NULL, NULL, "LightPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS,
		0, NULL, &pixelShaderBuffer, &errorMessage, NULL);

	if (FAILED(result))
	{
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		}
		else
		{
			MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	//버퍼에서 정점 셰이더 생성
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result))
		return false;

	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result))
		return false;

	/*
	입력 레이아웃이 색상 대신 텍스쳐를 사용하는 것으로 바뀌었습니다.
	첫번째인 POSITION은 바뀌지 않았지만 두번째 것의 SemanticName과 Format이 각각 TEXCOORD와 DXGI_FORMAT_R32G32_FLOAT로 바뀌었습니다.
	이 두 가지의 변화는 ModelClass의 정의와 셰이더 파일에 있는 VertexType와 형태가 일치하도록 만듭니다.
	*/

	//버텍스 입력 레이아웃 설명을 만듭니다
	//이 설정은 ModelClass와 셰이더의 VertexType 구조와 일치해야합니다.
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	polygonLayout[2].SemanticName = "NORMAL";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;


	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_layout);
	if (FAILED(result))
		return false;

	//레이아웃을 만드는 데 다 썻으니 해제
	vertexShaderBuffer->Release();
	vertexShaderBuffer = nullptr;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = nullptr;


	//텍스쳐 샘플러 상태 정의
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	//텍스쳐 샘플러 생성
	result = device->CreateSamplerState(&samplerDesc, &m_sampleState);
	if (FAILED(result))
		return false;

	//정점 셰이더에 있는 동적 매트릭스 상수 버퍼 설명 설정
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	//이 클래스 내에서 정점 셰이더 상수 버퍼에 액세스할 수 있도록 상수 버퍼 포인터를 생성하십시오.
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result))
		return false;

	//조명 상수 버퍼 설정
	/*
	조명의 색상과 방향을 다루는 조명 상수 버퍼의 description을 작성합니다. 특히 상수 버퍼의 크기가 16의 배수인지에 주의를 기울이세요. 만약 그렇지 않다면 구조의 마지막에16의 배수의 크기를 맞추도록 더미 변수를 넣어야 하며 그렇게 하지 않으면 CreateBuffer 함수가 실패할 것입니다.
	*/

	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	//조명 상수 버퍼 생성
	result = device->CreateBuffer(&lightBufferDesc, NULL, &m_lightBuffer);
	if (FAILED(result))
		return false;

	return true;
}

void LightShader::ShutdownShader()
{
	if (m_sampleState)
	{
		m_sampleState->Release();
		m_sampleState = nullptr;
	}

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

	if (m_lightBuffer)
	{
		m_lightBuffer->Release();
		m_lightBuffer = nullptr;
	}


}

void LightShader::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
	char* compileError;
	unsigned long bufferSize, i;
	ofstream fout;

	compileError = (char*)(errorMessage->GetBufferPointer());

	bufferSize = errorMessage->GetBufferSize();

	fout.open("shader-error.txt");

	for (i = 0; i < bufferSize; i++)
	{
		fout << compileError[i];
	}

	fout.close();

	errorMessage->Release();
	errorMessage = nullptr;

	MessageBox(hwnd, L"Error compiling shader. Check shader-error.txt for message.", shaderFilename, MB_OK);

}
//SetShaderParameters 함수는 이제 텍스쳐 자원의 포인터를 인자로 받고 그것을 셰이더에 등록합니다.
//참고로 텍스쳐는 반드시 버퍼에 렌더링이 일어나기 전에 설정되어 있어야 합니다.

bool LightShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, D3DXVECTOR3 lightDirection, D3DXVECTOR4 ambientColor, D3DXVECTOR4 diffuseColor)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	unsigned int bufferNumber;
	MatrixBufferType* dataPtr;
	LightBufferType* dataPtr2;

	//행렬을 변환하여 셰이더 준비
	D3DXMatrixTranspose(&worldMatrix, &worldMatrix);
	D3DXMatrixTranspose(&viewMatrix, &viewMatrix);
	D3DXMatrixTranspose(&projectionMatrix, &projectionMatrix);

	//상수 버퍼를 잠가 쓸 수 있도록 함
	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
		return false;

	//상수버퍼에서 데이터에 대한 포인터를 가져옴
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	//상수 버퍼에 행렬 복사
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	//상수 버퍼 풀음
	deviceContext->Unmap(m_matrixBuffer, 0);

	//정점 셰이더에 있는 상수 버퍼의 위치를 설정
	bufferNumber = 0;

	//이제 업데이트된 값으로 정점 셰이더에서 상수 버퍼를 설정
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	//픽셀 셰이더에서 쉐이더 텍스쳐 리소스를 설정
	deviceContext->PSSetShaderResources(0, 1, &texture);

	/*조명 버퍼*/
	result = deviceContext->Map(m_lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
		return false;

	//상수버퍼에서 데이터에 대한 포인터를 가져옴
	dataPtr2 = (LightBufferType*)mappedResource.pData;

	//상수 버퍼에 행렬 복사
	dataPtr2->ambientColor = ambientColor;
	dataPtr2->diffuseColor = diffuseColor;
	dataPtr2->lightDirection = lightDirection;
	dataPtr2->padding = 0.0f;

	//상수 버퍼 해제
	deviceContext->Unmap(m_lightBuffer, 0);

	//정점 셰이더에 있는 상수 버퍼의 위치를 설정
	bufferNumber = 0;

	//이제 업데이트된 값으로 픽셀 셰이더에서 상수 버퍼를 설정
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_lightBuffer);

	return true;
}

void LightShader::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	//버텍스 입력 레이아웃 설정
	deviceContext->IASetInputLayout(m_layout);

	//이 삼각형을 렌더링하는 데 사용할 정점 및 픽셀 쉐이더를 설정하십시오
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	//픽셀 셰이더에서 샘플러 상태를 설정하십시오.
	deviceContext->PSSetSamplers(0, 1, &m_sampleState);

	//삼각형을 그립니다.
	deviceContext->DrawIndexed(indexCount, 0, 0);
}