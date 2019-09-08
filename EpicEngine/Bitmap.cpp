#include "stdafx.h"
#include "Bitmap.h"


Bitmap::Bitmap()
{
	m_vertexBuffer = nullptr;
	m_indexBuffer = nullptr;
	m_Texture = nullptr;
	m_bitmapHeight = 0;
	m_bitmapWidth = 0;
	m_indexCount = 0;
	m_previousPosX = 0;
	m_previousPosY = 0;
}

Bitmap::Bitmap(const Bitmap &)
{
}

Bitmap::~Bitmap()
{
}

bool Bitmap::Initialize(ID3D11Device * device, int screenWidth, int screenHeight, WCHAR * textureFilename, int bitmapWidth, int bitmapHeight)
{
	bool result;

	//��ũ�� ������ ����
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	//�� ��Ʈ���� �������� �ȼ� ũ��(�ȼ�)�� �����Ͻʽÿ�
	m_bitmapWidth = bitmapWidth;
	m_bitmapHeight = bitmapHeight;

	/*
	���� ������ ��ġ ������ -1�� �ʱ�ȭ�մϴ�.
	�� ������ ���� ��ġ�� ����ϴ� �뵵�̱� ������ �߿��մϴ�. 
	���� ���� �����Ӱ� ���Ͽ� ��ġ�� ������ �ʾҴٸ� ���� ���� ���۸� �ٲ��� �ʱ� ������ ������ ����� ���� �� �ֽ��ϴ�.
	*/

	m_previousPosX = -1;
	m_previousPosY = -1;

	result = InitializeBuffers(device);
	if (!result)
		return false;

	result = LoadTexture(device, textureFilename);
	if (!result)
		return false;

	return true;
}

void Bitmap::Shutdown()
{
	ReleaseTexture();
	ShutdownBuffers();
	
	return;
}

bool Bitmap::Render(ID3D11DeviceContext * deviceContext, int positionX, int positionY)
{
	/*
	Render �Լ������� 2D �̹����� ���۸� �׷��� ī�忡 �ֽ��ϴ�.
	���� �Է����� �̹����� ȭ�鿡 �׷��� ��ġ�� �޽��ϴ�. UpdateBuffers �Լ��� ��ġ�� �����ϴ� �� ���˴ϴ�. 
	���� �����Ӱ� ��ġ�� �ٲ���ٸ� ���� ���� ������ �������� ��ġ�� ���ο� ��ġ�� �����մϴ�. 
	�׷��� ������ UpdateBuffers �Լ��� �׳� ����Ĩ�ϴ�. 
	RenderBuffers �Լ��� ������ �� ���������� �׸� ����/�ε��� ���۸� �غ��մϴ�.
	*/

	bool result;
	
	result = UpdateBuffers(deviceContext, positionX, positionY);
	if (!result)
		return false;

	RenderBuffers(deviceContext);

	return true;
}

int Bitmap::GetIndexCount()
{
	//GetIndexCount �Լ��� 2D �̹����� ���� ���� �����մϴ�. ������ 6�� ��ȯ�մϴ�.

	return m_indexCount;
}

ID3D11ShaderResourceView * Bitmap::GetTexture()
{
	//GetTexture �Լ��� 2D �̹����� ����ϴ� �ؽ��� �ڿ��� ���� �����͸� ��ȯ�մϴ�.
	//���̴������� �� �Լ��� ȣ���Ͽ� ���۸� �׸� �� �̹����� �����մϴ�.

	return m_Texture->GetTexture();
}

bool Bitmap::InitializeBuffers(ID3D11Device * device)
{
	//InitializeBuffer �Լ��� 2D�̹����� �׸��µ� ����ϴ� ����/�ε��� ���۸� ����� �� ���˴ϴ�.

	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i;

	//�� ���� �ﰢ���� ������ �ϱ� ������ ���� ���� 6���� �����մϴ�. �ε��� ���� �����ϴ�.
	m_vertexCount = 6;
	m_indexCount = m_vertexCount;

	vertices = new VertexType[m_vertexCount];
	if (!vertices)
		return false;	

	indices = new unsigned long[m_indexCount];
	if (!indices)
		return false;

	memset(vertices, 0, (sizeof(VertexType) * m_vertexCount));

	for (i = 0; i < m_indexCount; i++)
	{
		indices[i] = i;
	}

	/*
	���⼭ ModelClass�� ū ���̰� ����ϴ�. 
	���⼭ ���� ���� ���۸� �����Ͽ� �ʿ��ϴٸ� ���� ���� ������ ���� ������ �� �ְ� �մϴ�.
	�����̶�� �Ӽ��� �����ϱ� ���Ͽ� description�� Usage ������ D3D11_USAGE_DYNAMIC���� �ϰ�
	CPUAccessFlags�� D3D11_CPU_ACCESS_WRITE�� �մϴ�
	*/

	//���� ���� ������ �Ӽ��� �����մϴ�.
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	//���� ���ҽ� ������ ���� �����Ϳ� �����͸� �ο��Ͻʽÿ�.
	vertexData.pSysMem = vertices;	
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	//���� ���� ����
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
		return false;

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER; 
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0; 
	indexData.SysMemSlicePitch = 0; 
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer); 
	if(FAILED(result)) 
	{ 
		return false;
	}


	//�ε��� ������ ������ ������ ��ġ�� �ٲ� ������ ���� �������� ������ ����Ű�� ������
	//�ε��� ���۸� ���� ���۷� �������� �ʽ��ϴ�
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);

	if (FAILED(result))
	{
		return false;
	}

	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;
	
	return true;
}

void Bitmap::ShutdownBuffers()
{
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}
}

bool Bitmap::UpdateBuffers(ID3D11DeviceContext * deviceContext, int positionX, int positionY)
{
	float left, right, top, bottom;
	VertexType* vertices;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VertexType* verticesPtr;
	HRESULT result;

	//�̹����� ��ġ�� ������ ���Ͽ� �޶������� Ȯ���մϴ�.
	//�ٲ��� �ʾҴٸ� ���۸� ������ �ʿ䰡 �����Ƿ� �׳� ���������ϴ�. 
	//�� Ȯ�ΰ����� ���� ������ �氨�����ݴϴ�.
	if ((positionX == m_previousPosX) && (positionY == m_previousPosY))
	{
		return true;
	}

	//���� �̹��� ��ġ�� �ٲ���ٸ� �������� �׷��� ��ġ�� ���� ���ο� ��ġ�� ����մϴ�.
	m_previousPosX = positionX;
	m_previousPosY = positionY;

	left = (float)((m_screenWidth / 2) * -1) + (float)positionX;
	right = left + (float)m_bitmapWidth;

	top = (float)(m_screenHeight / 2) - (float)positionY;
	bottom = top - (float)m_bitmapHeight;

	//��ǥ�� ���Ǹ� �ӽ÷� ���� �迭�� ����� ���ο� ���� ��ġ�� ä���ֽ��ϴ�.
	vertices = new VertexType[m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	//ù��° �ﰢ��
	vertices[0].position = D3DXVECTOR3(left, top, 0.0f);  // Top left.
	vertices[0].texture = D3DXVECTOR2(0.0f, 0.0f);

	vertices[1].position = D3DXVECTOR3(right, bottom, 0.0f);  // Bottom right.
	vertices[1].texture = D3DXVECTOR2(1.0f, 1.0f);

	vertices[2].position = D3DXVECTOR3(left, bottom, 0.0f);  // Bottom left.
	vertices[2].texture = D3DXVECTOR2(0.0f, 1.0f);

	//�ι�° �ﰢ��
	vertices[3].position = D3DXVECTOR3(left, top, 0.0f);  // Top left.
	vertices[3].texture = D3DXVECTOR2(0.0f, 0.0f);

	vertices[4].position = D3DXVECTOR3(right, top, 0.0f);  // Top right.
	vertices[4].texture = D3DXVECTOR2(1.0f, 0.0f);

	vertices[5].position = D3DXVECTOR3(right, bottom, 0.0f);  // Bottom right.
	vertices[5].texture = D3DXVECTOR2(1.0f, 1.0f);

	//Map�� memcpy �Լ��� ����Ͽ� ���� �迭�� ������ ���� ���۷� �����մϴ�.
	result = deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	//���� ������ �����Ϳ� ���� �����͸� �����´�.
	verticesPtr = (VertexType*)mappedResource.pData;

	//���� ���ۿ� ������ ����
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * m_vertexCount));

	deviceContext->Unmap(m_vertexBuffer, 0);

	delete[] vertices;
	vertices = 0;

	return true;
}

void Bitmap::RenderBuffers(ID3D11DeviceContext * deviceContext)
{
	//���̴��� gpu���� ����/�ε��� ���۸� ��� �� �� �ֵ��� �����մϴ�.
	
	unsigned int stride;
	unsigned int offset;

	//���� ���� ��� �� �������� �����Ͻʽÿ�
	stride = sizeof(VertexType);
	offset = 0;

	//���� ���۸� �������� �� �ֵ��� �Է� �����⿡�� Ȱ��ȭ�ϵ��� �����Ͻʽÿ�.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	//�ε��� ���۸� �������� �� �ֵ��� �Է� �����⿡�� Ȱ��ȭ�ϵ��� �����Ͻʽÿ�.
	deviceContext->IASetIndexBuffer(m_indexBuffer , DXGI_FORMAT_R32_UINT,0);

	//�� ��� �� ���� ���ۿ��� �������ؾ� �ϴ� ���� ������ �����Ͻʽÿ�.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

bool Bitmap::LoadTexture(ID3D11Device * device, WCHAR * filename)
{
	bool result;

	m_Texture = new Texture;
	if (!m_Texture)
		return false;

	result = m_Texture->Initialize(device, filename);
	if (!result)
		return false;

	return true;
}

void Bitmap::ReleaseTexture()
{
	if (m_Texture)
	{
		m_Texture->ShutDown();
		delete m_Texture;
		m_Texture = nullptr;
	}
}
