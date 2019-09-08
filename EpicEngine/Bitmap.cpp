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

	//스크린 사이즈 저장
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	//이 비트맵을 렌더링할 픽셀 크기(픽셀)를 저장하십시오
	m_bitmapWidth = bitmapWidth;
	m_bitmapHeight = bitmapHeight;

	/*
	이전 렌더링 위치 변수는 -1로 초기화합니다.
	이 변수는 이전 위치를 기억하는 용도이기 때문에 중요합니다. 
	만약 이전 프레임과 비교하여 위치가 변하지 않았다면 동적 정점 버퍼를 바꾸지 않기 때문에 성능의 향상을 꾀할 수 있습니다.
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
	Render 함수에서는 2D 이미지의 버퍼를 그래픽 카드에 넣습니다.
	또한 입력으로 이미지가 화면에 그려질 위치를 받습니다. UpdateBuffers 함수가 위치를 지정하는 데 사용됩니다. 
	이전 프레임과 위치가 바뀌었다면 동적 정점 버퍼의 정점들의 위치를 새로운 위치로 갱신합니다. 
	그렇지 않으면 UpdateBuffers 함수를 그냥 지나칩니다. 
	RenderBuffers 함수를 수행한 뒤 최종적으로 그릴 정점/인덱스 버퍼를 준비합니다.
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
	//GetIndexCount 함수는 2D 이미지의 정점 수를 리턴합니다. 언제나 6을 반환합니다.

	return m_indexCount;
}

ID3D11ShaderResourceView * Bitmap::GetTexture()
{
	//GetTexture 함수는 2D 이미지에 사용하는 텍스쳐 자원에 대한 포인터를 반환합니다.
	//세이더에서는 이 함수를 호출하여 버퍼를 그릴 때 이미지에 접근합니다.

	return m_Texture->GetTexture();
}

bool Bitmap::InitializeBuffers(ID3D11Device * device)
{
	//InitializeBuffer 함수는 2D이미지를 그리는데 사용하는 정점/인덱스 버퍼를 만드는 데 사용됩니다.

	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i;

	//두 개의 삼각형을 만들어야 하기 때문에 정점 수를 6으로 설정합니다. 인덱스 역시 같습니다.
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
	여기서 ModelClass와 큰 차이가 생깁니다. 
	여기서 동적 정점 버퍼를 생성하여 필요하다면 정점 버퍼 내부의 값을 수정할 수 있게 합니다.
	동적이라는 속성을 지정하기 위하여 description의 Usage 변수를 D3D11_USAGE_DYNAMIC으로 하고
	CPUAccessFlags를 D3D11_CPU_ACCESS_WRITE로 합니다
	*/

	//정적 정점 버퍼의 속성을 지정합니다.
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	//하위 리소스 구조에 정점 데이터에 포인터를 부여하십시오.
	vertexData.pSysMem = vertices;	
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	//정점 버퍼 생성
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


	//인덱스 버퍼의 내용은 정점의 위치가 바뀌어도 언제나 같은 여섯개의 정점을 가리키기 때문에
	//인덱스 버퍼를 동적 버퍼로 만들지는 않습니다
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

	//이미지의 위치가 이전과 비교하여 달라졌는지 확인합니다.
	//바뀌지 않았다면 버퍼를 수정할 필요가 없으므로 그냥 빠져나갑니다. 
	//이 확인과정은 많은 연산을 경감시켜줍니다.
	if ((positionX == m_previousPosX) && (positionY == m_previousPosY))
	{
		return true;
	}

	//만약 이미지 위치가 바뀌었다면 다음번에 그려질 위치를 위해 새로운 위치를 기록합니다.
	m_previousPosX = positionX;
	m_previousPosY = positionY;

	left = (float)((m_screenWidth / 2) * -1) + (float)positionX;
	right = left + (float)m_bitmapWidth;

	top = (float)(m_screenHeight / 2) - (float)positionY;
	bottom = top - (float)m_bitmapHeight;

	//좌표가 계산되면 임시로 정점 배열을 만들고 새로운 정점 위치를 채워넣습니다.
	vertices = new VertexType[m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	//첫번째 삼각형
	vertices[0].position = D3DXVECTOR3(left, top, 0.0f);  // Top left.
	vertices[0].texture = D3DXVECTOR2(0.0f, 0.0f);

	vertices[1].position = D3DXVECTOR3(right, bottom, 0.0f);  // Bottom right.
	vertices[1].texture = D3DXVECTOR2(1.0f, 1.0f);

	vertices[2].position = D3DXVECTOR3(left, bottom, 0.0f);  // Bottom left.
	vertices[2].texture = D3DXVECTOR2(0.0f, 1.0f);

	//두번째 삼각형
	vertices[3].position = D3DXVECTOR3(left, top, 0.0f);  // Top left.
	vertices[3].texture = D3DXVECTOR2(0.0f, 0.0f);

	vertices[4].position = D3DXVECTOR3(right, top, 0.0f);  // Top right.
	vertices[4].texture = D3DXVECTOR2(1.0f, 0.0f);

	vertices[5].position = D3DXVECTOR3(right, bottom, 0.0f);  // Bottom right.
	vertices[5].texture = D3DXVECTOR2(1.0f, 1.0f);

	//Map과 memcpy 함수를 사용하여 정점 배열의 내용을 정점 버퍼로 복사합니다.
	result = deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	//정점 버퍼의 데이터에 대한 포인터를 가져온다.
	verticesPtr = (VertexType*)mappedResource.pData;

	//정점 버퍼에 데이터 복사
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * m_vertexCount));

	deviceContext->Unmap(m_vertexBuffer, 0);

	delete[] vertices;
	vertices = 0;

	return true;
}

void Bitmap::RenderBuffers(ID3D11DeviceContext * deviceContext)
{
	//셰이더가 gpu에서 정점/인덱스 버퍼를 사용 할 수 있도록 설정합니다.
	
	unsigned int stride;
	unsigned int offset;

	//정점 버퍼 계단 및 오프셋을 설정하십시오
	stride = sizeof(VertexType);
	offset = 0;

	//정점 버퍼를 렌더링할 수 있도록 입력 조립기에서 활성화하도록 설정하십시오.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	//인덱스 버퍼를 렌더링할 수 있도록 입력 조립기에서 활성화하도록 설정하십시오.
	deviceContext->IASetIndexBuffer(m_indexBuffer , DXGI_FORMAT_R32_UINT,0);

	//이 경우 이 정점 버퍼에서 렌더링해야 하는 원시 유형을 설정하십시오.
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
