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

	//정점 셰이더와 픽셀 셰이더를 초기화합니다.
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

	//렌더링에 사용할 셰이더의 인자를 입력합니다.
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix);
	if(!result)
		return false;

	//셰이더를 이용하여 준비된 버퍼를 그립니다.
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

	// 이함수에서 사용하는 포인터들을 null로 설정
	errorMsg = nullptr;
	vertexShaderBuffer = nullptr;
	pixelShaderBuffer = nullptr;

	//정점 셰이더를 컴파일합니다.
	result = D3DX11CompileFromFile(vsFilename, NULL, NULL, "ColorVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL,
		&vertexShaderBuffer, &errorMsg, NULL);
	
	if (FAILED(result))
	{
		//셰이더 컴파일에 실패하면 에러 메시지 기록
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

	//픽셀 셰이더를 컴파일합니다.
	result = D3DX11CompileFromFile(psFilename, NULL, NULL, "ColorPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS,
		0, NULL, &pixelShaderBuffer, &errorMsg, NULL);

	if (FAILED(result))
	{
		//셰이더 컴파일에 실패하면 에러 메시지를 기록합니다.
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
	정점 셰이더와 픽셀 셰이더가 버퍼로 잘 컴파일되면 이를 이용하여 셰이더 객체를 만들 수 있습니다.
	여기서 나온 포인터를 정점 셰이더와 픽셀 셰이더의 인터페이스로서 사용할 것입니다.
	*/

	//버퍼로부터 정점 셰이더 생성
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result))
		return false;

	//버퍼로부터 픽셀 셰이더 생성
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result))
		return false;

	//정점 입력 레이아웃 description 을 작성
	//이 설정은 Modelclass와 셰이더에 있는 vertextype과 일치해야 함
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

	//위의 레이아웃을 가지고 D3D 디바이스를 사용해 입력 레이아웃을 생성합니다. 
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	//정점 입력 레이아웃을 생성합니다.
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), &m_layout);

	if (FAILED(result))
		return false;

	//더 이상 사용되지 않는 정점 셰이더 버퍼와 픽셀 셰이더 버퍼를 해제합니다.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = nullptr;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = nullptr;

	//셰이더를 사용하기 위한 마지막 단계는 상수 버퍼. 지금은 하나의 상수 버퍼를 사용하기 때문에 이것을 세팅하여 
	//셰이더에 대한 인터페이스를 사용할 수 있다.
	//매 프레임마다 상수버퍼를 업데이트하기 때문에 버퍼의 사용은 동적이 되야함.

	//정점 셰이더에 있는 행렬 상수 버퍼의 description 을 작성
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	//상수 버퍼 포인터를 만들어 이 클래스에서 정점 셰이더 상수 버퍼에 접근할 수 있게 합니다.
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

	//에러 메세지를 담고 있는 문자열 버퍼의 포인터를 가져옵니다.
	compileError = (char*)(errorMessage->GetBufferPointer());

	//메세지의 길이를 가져옴
	bufferSize = errorMessage->GetBufferSize();

	//에러 메시지를 씀
	for (i = 0; i < bufferSize; i++)
	{
		fout << compileError[i];
	}

	//파일 닫기
	fout.close();

	//에러 메시지 반환
	errorMessage->Release();
	errorMessage = nullptr;

	//컴파일 에러가 있음을 팝업 메시지로 알려줍니다.
	MessageBox(hwnd, L"Error compiling shader. Check shader-error.txt for message.", shaderFilename, MB_OK);

}
//셰이더의 전역 변수를 쉽게 다룰 수 있도록 하기 위해 만들어졌습니다. 이 함수에 사용된 행렬들은 GraphicsClass에서 만들어진 것입니다.
bool ColorShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix)
{
	HRESULT	result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	unsigned int bufferNumber;

	//행렬을 transpose하여 셰이더에서 사용할 수 있게 합니다.
	D3DXMatrixTranspose(&worldMatrix, &worldMatrix);
	D3DXMatrixTranspose(&viewMatrix, &viewMatrix);
	D3DXMatrixTranspose(&projectionMatrix, &projectionMatrix);

	//상수 버퍼의 내용을 쓸 수 있게 잠급니다.
	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
		return false;

	//상수 버퍼의 데이터에 대한 포인터를 가져옵니다.
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	//상수 버퍼에 행렬을 복사합니다.
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	//상수 버퍼의 잠금을 풉니다.
	deviceContext->Unmap(m_matrixBuffer, 0);

	//정점 셰이더에서 상수 버퍼의 위치를 설정합니다.
	bufferNumber = 0;

	//마지막으로 정점 셰이더의 상수 버퍼를 바뀐 값으로 바꿉니다.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	return true;
}

void ColorShader::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	//정점 입력 레이아웃을 설정합니다.
	deviceContext->IASetInputLayout(m_layout);

	//삼각형을 그릴 정점 셰이더와 픽셀 셰이더를 설정합니다.
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	//삼각형을 그립니다.
	deviceContext->DrawIndexed(indexCount, 0, 0);
}