#include "stdafx.h"
#include "Graphic.h"


Graphic::Graphic()
{
	m_Direct3D = nullptr;
	m_Camera = nullptr;
	m_Model = nullptr;
	//m_ColorShader = nullptr;
	m_LightShader = nullptr;
	m_Light = nullptr;
	m_Bitmap = nullptr;
	m_TextureShader = nullptr;
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
	if (!(m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR)))
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

	const WCHAR* pwcsName = L"../EpicEngine/data/seafloor.dds";
	wchar_t* tempWide = const_cast <wchar_t*> (pwcsName);

	const char* temp0 = "../EpicEngine/data/cube.txt";

	//�� ������Ʈ �ʱ�ȭ
	result = m_Model->Initialize(m_Direct3D->GetDevice(), "../EpicEngine/data/cube.txt" , tempWide);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object", L"Error", MB_OK);
		return false;
	}

	m_TextureShader = new TextureShader;
	result = m_TextureShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}

	m_LightShader = new LightShader;
	if (!m_LightShader)
		return false;

	//�ؽ��� ���̴� ������Ʈ �ʱ�ȭ
	result = m_LightShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the light shader object", L"Error", MB_OK);
		return false;
	}

	m_Light = new Light();
	if (!m_Light)
		return false;

	m_Light->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetDirection(0.0f, 0.0f, 1.0f);
	m_Light->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetSpecularPower(32.f);

	//��Ʈ�� ������Ʈ ����
	m_Bitmap = new Bitmap;
	if (!m_Bitmap)
		return false;

	result = m_Bitmap->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight, tempWide, 256, 256);
	if (!result)
	{ 
		MessageBox(hwnd, L"Could not initialize the bitmap object.", L"Error", MB_OK);
		return false;
	}

	////�÷� ���̴� ����
	//m_ColorShader = new ColorShader;
	//if (!m_ColorShader)
	//	return false;

	////�÷� ���̴� �ʱ�ȭ
	//result = m_ColorShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	//if (!result)
	//{
	//	MessageBox(hwnd, L"Could not initialize the color shader object", L"Error", MB_OK);
	//	return false;
	//}

	return true;
}

void Graphic::Shutdown()
{
	if (m_Bitmap)
	{
		m_Bitmap->Shutdown();
		delete m_Bitmap;
		m_Bitmap = nullptr;
	}

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

	//if (m_ColorShader)
	//{
	//	m_ColorShader->Shutdown();
	//	delete m_ColorShader;
	//	m_ColorShader = nullptr;
	//}

	if (m_LightShader)
	{
		m_LightShader->ShutDown();
		delete m_LightShader;
		m_LightShader = nullptr;
	}

	if (m_Light)
	{
		delete m_Light;
		m_Light = nullptr;
	}

}

bool Graphic::Frame()
{
	bool result;

	static float rotation = 0.0f;

	rotation += (float)D3DX_PI * 0.005f;
	if (rotation > 360.f)
		rotation -= 360.f;

	//�׷��� �� ����
	result = Render(rotation);
	if (!result)
		return false;

	return true;
}

bool Graphic::Render(float rotation)
{
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix , orthMatrix;
	bool result;

	//���� �׸��� ���� ���۸� �ʱ�ȭ
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	//ī�޶��� ��ġ�� ���� �� ����� ����� ���� ī�޶��� Render �Լ��� ȣ���մϴ�. 
	m_Camera->Render();

	//ī�޶� �� d3d ��ü���� ����, ���� �� ���� ����� �����ɴϴ�
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	//���� 2D �������� �ϱ� ���� ���翵 ����� ���մϴ�. �Ϲ� ������� ��� �� ����� ������ ���Դϴ�.
	m_Direct3D->GetOrthoMatrix(orthMatrix);

	//2D �������� �����ϱ� ���� Z���۸� ���ϴ�.
	m_Direct3D->TurnZBufferOff();

	result = m_Bitmap->Render(m_Direct3D->GetDeviceContext(), 100, 100);
	if (!result)
		return false;

	//�ϴ� ���� / �ε��� ���۰� �غ�Ǿ��ٸ� �ؽ��� ���̴��� �̿��� �׸��� �˴ϴ�.
	//2D �������� �����ϱ� ���� projectionMatrix ��� orthoMatrix�� ���ڷ� ���´ٴ� ���� �����ϱ� �ٶ��ϴ�.
	//���� �� ����� ������ ��� �ٲ�� ���̶�� 2D ���������� ���� �⺻ �� ����� ���� ����� ����ؾ� �մϴ�
	//�� Ʃ�丮�󿡼��� ī�޶� �����Ǿ� �ֱ� ������ �Ϲ� �� ����� ����ص� ���� ����� ���ɴϴ�.
	
	result = m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_Bitmap->GetIndexCount(),
		worldMatrix, viewMatrix, orthMatrix, m_Bitmap->GetTexture());

	if (!result)
		return false;


	//���带 ������ Ȯ��
	D3DXMatrixRotationY(&worldMatrix, rotation);

	m_Model->Render(m_Direct3D->GetDeviceContext());

	/*result = m_ColorShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix);
	if (!(result))
		return false;*/

	result = m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix,
		viewMatrix, projectionMatrix, m_Model->GetTexture(), m_Light->GetDirection(), m_Light->GetAmbientColor(),
		m_Light->GetDiffuseColor(), m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower());

	if (!result)
		return false;

	m_Direct3D->TurnZBufferOn();

	//������ ������ ȭ�鿡 ���
	m_Direct3D->EndScene();
	return true;
}
