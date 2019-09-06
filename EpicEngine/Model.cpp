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

	//���� ���ۿ� �ε��� ���� �ʱ�ȭ
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
	//�� �ؽ��� ����
	ReleaseTexture();
	//������ �ε��� ���� ����.
	ShutdownBuffers();
	//�� ����
	ReleaseModel();

	return;
}

void Model::Render(ID3D11DeviceContext* deviceContext)
{
	//���� ���ۿ� �ε��� ���۸� �׷��Ƚ� ���������ο� �־� ȭ�鿡 �׸� �غ� �մϴ�.
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
InitializeBuffers �Լ��� ���� ���ۿ� �ε��� ���۸� �����ϴ� �۾��� �����մϴ�.
���� �� �κп����� ������ ���Ϸκ��� ���� ������ �о� �ͼ� ���۵��� ����� ���� �մϴ�

��ó: https://ppparkje.tistory.com/13?category=489719 [������ ��Ʈ]
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
	�����ؾ� �� ����, �����δ� �� �̻� ������ �ε��� ���� ���� �������� �ʴ´ٴ� ���Դϴ�.]
	�� ��� �켱 ModelClass::LoadModel�� ȣ���ϰ� �� �ڿ� ������ �ε��� ���� �����´ٴ� ���� �� �� �ֽ��ϴ�.
	*/

	//���� �迭�� �����մϴ�.
	vertices = new VertexType[m_vertexCount];

	if (!vertices)
		return false;

	//�ε��� �迭�� �����մϴ�.
	indices = new unsigned long[m_indexCount];
	if (!indices)
		return false;


	/*
	������ �ε��� �迭�� �ҷ����� �� ���� ���� �ٲ�����ϴ�.
	���� �� ���� �־��ִ� ���, for ������ ���鼭 m_model �迭�� �ִ� �������� ���� �迭�� �����մϴ�.
	�ε��� �迭�� �ҷ��� �� �ش� �迭������ ��ġ�� �� �ε��� ��ȣ�̱� ������ �����ϰ� ������ �� �ֽ��ϴ�.
	*/

	for (i = 0; i < m_vertexCount; i++)
	{
		vertices[i].position = D3DXVECTOR3(m_Model[i].x, m_Model[i].y, m_Model[i].z);
		vertices[i].texture = D3DXVECTOR2(m_Model[i].tu, m_Model[i].tv);
		vertices[i].normal = D3DXVECTOR3(m_Model[i].nx, m_Model[i].ny, m_Model[i].nz);

		indices[i] = i;
	}

	//���� ������ description�� �ۼ��մϴ�.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	//���� �����͸� ����Ű�� ���� ���ҽ� ����ü�� �ۼ��մϴ�.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	//���� ���۸� �����մϴ�.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
		return false;

	//�ε����� Description �ۼ�
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	//�ε��� �����͸� ����Ű�� ���� ���ҽ� ����ü�� �ۼ��մϴ�.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	//�ε��� ���۸� �����մϴ�.
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
RenderBuffers �Լ��� Render �Լ����� ȣ��˴ϴ�.
�� �Լ��� ������ �ٷ� ���� ���ۿ� �ε��� ���۸� GPU�� ������� ���۷μ� Ȱ��ȭ��Ű�� ���Դϴ�.

�ϴ� GPU�� Ȱ��ȭ�� ���� ���۸� ������ �Ǹ� ���̴��� �̿��Ͽ� ������ ������ �׸� �� �ְ� �˴ϴ�.

���� �� �Լ������� �� ������ �ﰢ���̳� ����, ��ä�� �� � ������� �׸��� ���� �����մϴ�.

�� Ʃ�丮�󿡼��� ������� �Է¿� ���� ���ۿ� �ε��� ���۸� �ְ� DirectX�� IASetPrimitiveTopology �Լ��� ����Ͽ�
GPU���� �� �������� �ﰢ������ �׸����� �ֹ��մϴ�.
*/

void Model::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	//���� ������ ������ �������� �����մϴ�.
	stride = sizeof(VertexType);
	offset = 0;

	//input assembler�� ���� ���۸� Ȱ��ȭ�Ͽ� �׷��� �� �ְ� �մϴ�.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	//input assembler�� �ε��� ���۸� Ȱ��ȭ�Ͽ� �׷��� �� �ְ� �մϴ�.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//���� ���۷� �׸� �⺻���� ����, ���⼭�� �ﰢ��
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


	//�� ���� ����
	fin.open(fileName);

	if (fin.fail())
	{
		return false;
	}

	//���� ī��Ʈ �� �б�
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	//���� �� �б�
	fin >> m_vertexCount;

	//�ε��� ���� ���� ���� ���� ����
	m_indexCount = m_vertexCount;

	//���� ���� ī��Ʈ�� ����Ͽ� ���� �����Ͻʽÿ�.
	m_Model = new ModelType[m_vertexCount];
	if (!m_Model)
	{
		return false;
	}

	//������ ���� �κб��� �б�
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}
	fin.get(input);
	fin.get(input);

	//���ؽ� ������ �б�
	for (i = 0; i < m_vertexCount; i++)
	{
		fin >> m_Model[i].x >> m_Model[i].y >> m_Model[i].z;
		fin >> m_Model[i].tu >> m_Model[i].tv;
		fin >> m_Model[i].nx >> m_Model[i].ny >> m_Model[i].nz;
	}

	//�� ���� ����
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
