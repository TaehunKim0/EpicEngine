#include "stdafx.h"
#include "Model.h"


Model::Model()
{
	m_vertexBuffer = nullptr;
	m_indexBuffer = nullptr;
	m_Texture = nullptr;
	m_Model = nullptr;
}

Model::Model(const Model& other) : Model()
{
}


Model::~Model()
{
}

bool Model::Initialize(ID3D11Device* device, const char* modelFilename, WCHAR* textureFileName)
{
	bool result;

	result = LoadModel(modelFilename);
	if (!result)
		return false;

	//정점 버퍼와 인덱스 버퍼 초기화
	result = InitializeBuffers(device);
	if (!result)
		return false;

	result = LoadTexture(device, textureFileName);
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
	//모델 해제
	ReleaseModel();

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
	int i = 0;

	/*
	주의해야 할 것은, 앞으로는 더 이상 정점과 인덱스 수를 직접 설정하지 않는다는 것입니다.]
	그 대신 우선 ModelClass::LoadModel을 호출하고 난 뒤에 정점과 인덱스 수를 가져온다는 것을 볼 수 있습니다.
	*/

	//정점 배열을 생성합니다.
	vertices = new VertexType[m_vertexCount];

	if (!vertices)
		return false;

	//인덱스 배열을 생성합니다.
	indices = new unsigned long[m_indexCount];
	if (!indices)
		return false;


	/*
	정점과 인덱스 배열을 불러오는 것 역시 조금 바뀌었습니다.
	직접 그 값을 넣어주는 대신, for 루프를 돌면서 m_model 배열에 있는 정보들을 정점 배열로 복사합니다.
	인덱스 배열은 불러올 때 해당 배열에서의 위치가 곧 인덱스 번호이기 때문에 간단하게 지정할 수 있습니다.
	*/

	for (i = 0; i < m_vertexCount; i++)
	{
		vertices[i].position = D3DXVECTOR3(m_Model[i].x, m_Model[i].y, m_Model[i].z);
		vertices[i].texture = D3DXVECTOR2(m_Model[i].tu, m_Model[i].tv);
		vertices[i].normal = D3DXVECTOR3(m_Model[i].nx, m_Model[i].ny, m_Model[i].nz);

		indices[i] = i;
	}

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

bool Model::LoadModel(const  char* fileName)
{
	std::ifstream fin;
	char input;
	int i;


	//모델 파일 오픈
	fin.open(fileName);

	if (fin.fail())
	{
		return false;
	}

	//정점 카운트 수 읽기
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	//정점 수 읽기
	fin >> m_vertexCount;

	//인덱스 수를 정점 수와 같게 설정
	m_indexCount = m_vertexCount;

	//읽은 정점 카운트를 사용하여 모델을 생성하십시오.
	m_Model = new ModelType[m_vertexCount];
	if (!m_Model)
	{
		return false;
	}

	//데이터 시작 부분까지 읽기
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}
	fin.get(input);
	fin.get(input);

	//버텍스 데이터 읽기
	for (i = 0; i < m_vertexCount; i++)
	{
		fin >> m_Model[i].x >> m_Model[i].y >> m_Model[i].z;
		fin >> m_Model[i].tu >> m_Model[i].tv;
		fin >> m_Model[i].nx >> m_Model[i].ny >> m_Model[i].nz;
	}

	//모델 파일 닫음
	fin.close();

	return true;
}

void Model::ReleaseModel()
{
	if (m_Model)
	{
		delete[] m_Model;
		m_Model = nullptr;
	}
}
