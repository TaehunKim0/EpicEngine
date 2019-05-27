#include "Model.h"



Model::Model()
{
	m_vertexBuffer = nullptr;
	m_indexBuffer = nullptr;
	m_Texture = nullptr;
}

Model::Model(const Model& other) : Model()
{
}


Model::~Model()
{
}

bool Model::Initialize(ID3D11Device* device, WCHAR* textureFileName)
{
	bool result;

	//정점 버퍼와 인덱스 버퍼 초기화
	result = InitializeBuffers(device);
	if (!result)
		return false;

	if (!result)
		return false;

	return true;
}

void Model::Shutdown()
{
	//모델 텍스쳐 해제
	ReleaseTexture();
	//정점과 인덱스 버퍼 해제.
	ShutdownBuffers();
	return;
}

void Model::Render(ID3D11DeviceContext* deviceContext)
{
	//정점 버퍼와 인덱스 버퍼를 그래픽스 파이프라인에 넣어 화면에 그릴 준비를 합니다.
	RenderBuffers(deviceContext);

	return;
}

int Model::GetIndexCount()
{
	return m_indexCount;
}

ID3D11ShaderResourceView* Model::GetTexture()
{
	return m_Texture->GetTexture();
}

/*
InitializeBuffers 함수는 정점 버퍼와 인덱스 버퍼를 생성하는 작업을 제어합니다.
보통 이 부분에서는 데이터 파일로부터 모델의 정보를 읽어 와서 버퍼들을 만드는 일을 합니다

출처: https://ppparkje.tistory.com/13?category=489719 [빠재의 노트]
*/

bool Model::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;

	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;

	HRESULT result;
	
	//정점,인덱스 배열의 길이를 설정합니다.
	m_vertexCount = 3;
	m_indexCount = 3;

	//정점 배열을 생성합니다.
	vertices = new VertexType[m_vertexCount];

	if (!vertices)
		return false;

	//인덱스 배열을 생성합니다.
	indices = new unsigned long[m_indexCount];
	if (!indices)
		return false;

	/* 정점 배열에 값을 넣습니다. */
	vertices[0].position = D3DXVECTOR3(-1.0f, -1.0f, 0.0f); //왼쪽 아래
	vertices[0].texture = D3DXVECTOR2(0.0f, 1.0f);
	//vertices[0].color = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);	

	vertices[1].position = D3DXVECTOR3(0.0f, 1.0f, 0.0f); //상단 가운데
	vertices[1].texture = D3DXVECTOR2(0.5f, 0.0f);
	//vertices[1].color = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);

	vertices[2].position = D3DXVECTOR3(1.0f, -1.0f, 0.0f); //오른쪽 아래
	vertices[2].texture = D3DXVECTOR2(1.0f, 1.0f);
	//vertices[2].color = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);

	/* 인덱스 배열에 값을 넣습니다.*/
	indices[0] = 0; //왼쪽 아래
	indices[1] = 1; //상단 가운데
	indices[2] = 2; //오른쪽 아래

	//정점 버퍼의 description을 작성합니다.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	//정점 데이터를 가리키는 보조 리소스 구조체를 작성합니다.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	//정점 버퍼를 생성합니다.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
		return false;

	//인덱스의 Description 작성
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	//인덱스 데이터를 가리키는 보조 리소스 구조체를 작성합니다.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	//인덱스 버퍼를 생성합니다.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
		return false;

	delete[] vertices;
	vertices = nullptr;

	delete[] indices;
	indices = nullptr;

	return true;
}

void Model::ShutdownBuffers()
{
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = nullptr;
	}
	
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = nullptr;
	}

	return;
}
/*
RenderBuffers 함수는 Render 함수에서 호출됩니다. 
이 함수의 목적은 바로 정점 버퍼와 인덱스 버퍼를 GPU의 어셈블러의 버퍼로서 활성화시키는 것입니다.

일단 GPU가 활성화된 정점 버퍼를 가지게 되면 셰이더를 이용하여 버퍼의 내용을 그릴 수 있게 됩니다.

또한 이 함수에서는 이 정점을 삼각형이나 선분, 부채꼴 등 어떤 모양으로 그리게 될지 정의합니다.

이 튜토리얼에서는 어셈블러의 입력에 정점 버퍼와 인덱스 버퍼를 넣고 DirectX의 IASetPrimitiveTopology 함수를 사용하여
GPU에게 이 정점들을 삼각형으로 그리도록 주문합니다.
*/
void Model::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	//정점 버퍼의 단위와 오프셋을 설정합니다.
	stride = sizeof(VertexType);
	offset = 0;

	//input assembler에 정점 버퍼를 활성화하여 그려질 수 있게 합니다.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	//input assembler에 인덱스 버퍼를 활성화하여 그려질 수 있게 합니다.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//정점 버퍼로 그릴 기본형을 설정, 여기서는 삼각형
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

bool Model::LoadTexture(ID3D11Device* device, WCHAR* fileName)
{
	bool result;

	m_Texture = new Texture;
	if (!m_Texture)
		return false;

	result = m_Texture->Initialize(device, fileName);
	if (!result)
		return false;

	return true;
}

void Model::ReleaseTexture()
{
	if (m_Texture)
	{
		m_Texture->ShutDown();
		delete m_Texture;
		m_Texture = nullptr;
	}
}
