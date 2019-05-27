#include "stdafx.h"
#include "Graphic.h"


Graphic::Graphic()
{
	m_Direct3D = nullptr;
	m_Camera = nullptr;
	m_Model = nullptr;
	m_ColorShader = nullptr;
}

Graphic::Graphic(const Graphic& other) : Graphic()
{
}


Graphic::~Graphic()
{
}

bool Graphic::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;

	//Direct3D ��ü ����
	m_Direct3D = (D3D*)_aligned_malloc(sizeof(D3D), 16);
	if (!m_Direct3D)
		return false;

	//Direct3D ��ü �ʱ�ȭ
	if (!m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR))
	{
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}

	//ī�޶� ����
	m_Camera = new Camera;
	if (!m_Camera)
		return false;

	//ī�޶� ������ ����
	m_Camera->SetPosition(0.0f, 0.0f, -10.0f);

	//�� ������Ʈ ����
	m_Model = new Model;
	if (!m_Model)
		return false;

	const WCHAR* pwcsName = L"../Engine/data/seafloor.dds";
	wchar_t* tempWide = const_cast <wchar_t*> (pwcsName);

	//�� ������Ʈ �ʱ�ȭ
	result = m_Model->Initialize(m_Direct3D->GetDevice(), tempWide);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object", L"Error", MB_OK);
		return false;
	}

	//�÷� ���̴� ����
	m_ColorShader = new ColorShader;
	if (!m_ColorShader)
		return false;

	//�÷� ���̴� �ʱ�ȭ
	result = m_ColorShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the color shader object", L"Error", MB_OK);
		return false;
	}

	return true;
}

void Graphic::Shutdown()
{
	if (m_Direct3D)
	{
		m_Direct3D->Shutdown();
		_aligned_free(m_Direct3D);
		m_Direct3D = nullptr;
	}

	if (m_Model)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = nullptr;
	}

	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = nullptr;
	}

	if (m_ColorShader)
	{
		m_ColorShader->Shutdown();
		delete m_ColorShader;
		m_ColorShader = nullptr;
	}

}

bool Graphic::Frame()
{
	bool result;

	//�׷��� �� ����
	result = Render();
	if (!result)
		return false;

	return true;
}

bool Graphic::Render()
{
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool result;

	//���� �׸��� ���� ���۸� �ʱ�ȭ
	m_Direct3D->BeginScene(0.5f,0.5f,0.5f,1.0f);
	
	//ī�޶��� ��ġ�� ���� �� ����� ����� ���� ī�޶��� Render �Լ��� ȣ���մϴ�. 
	m_Camera->Render();
	
	//ī�޶� �� d3d ��ü���� ����, ���� �� ���� ����� �����ɴϴ�
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	m_Model->Render(m_Direct3D->GetDeviceContext());

	result = m_ColorShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix);
	if (!(result))
		return false;

	//������ ������ ȭ�鿡 ���
	m_Direct3D->EndScene();
	return true;
}
