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

	//Direct3D 객체 생성
	m_Direct3D = (D3D*)_aligned_malloc(sizeof(D3D), 16);
	if (!m_Direct3D)
		return false;

	//Direct3D 객체 초기화
	if (!(m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR)))
	{
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}

	//카메라 생성
	m_Camera = new Camera;
	if (!m_Camera)
		return false;

	//카메라 포지션 설정
	m_Camera->SetPosition(0.0f, 0.0f, -10.0f);

	//모델 오브젝트 생성
	m_Model = new Model;
	if (!m_Model)
		return false;

	const WCHAR* pwcsName = L"../EpicEngine/data/seafloor.dds";
	wchar_t* tempWide = const_cast <wchar_t*> (pwcsName);

	const char* temp0 = "../EpicEngine/data/cube.txt";

	//모델 오브젝트 초기화
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

	//텍스쳐 셰이더 오브젝트 초기화
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

	//비트맵 오브젝트 생성
	m_Bitmap = new Bitmap;
	if (!m_Bitmap)
		return false;

	result = m_Bitmap->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight, tempWide, 256, 256);
	if (!result)
	{ 
		MessageBox(hwnd, L"Could not initialize the bitmap object.", L"Error", MB_OK);
		return false;
	}

	////컬러 셰이더 생성
	//m_ColorShader = new ColorShader;
	//if (!m_ColorShader)
	//	return false;

	////컬러 셰이더 초기화
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

	//그래픽 씬 렌더
	result = Render(rotation);
	if (!result)
		return false;

	return true;
}

bool Graphic::Render(float rotation)
{
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix , orthMatrix;
	bool result;

	//씬을 그리기 위해 버퍼를 초기화
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	//카메라의 위치를 토대로 뷰 행렬을 만들기 위해 카메라의 Render 함수를 호출합니다. 
	m_Camera->Render();

	//카메라 및 d3d 객체에서 세계, 보기 및 투영 행렬을 가져옵니다
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	//또한 2D 렌더링을 하기 위해 정사영 행렬을 구합니다. 일반 투영행렬 대신 이 행렬을 전달할 것입니다.
	m_Direct3D->GetOrthoMatrix(orthMatrix);

	//2D 렌더링을 시작하기 전에 Z버퍼를 끕니다.
	m_Direct3D->TurnZBufferOff();

	result = m_Bitmap->Render(m_Direct3D->GetDeviceContext(), 100, 100);
	if (!result)
		return false;

	//일단 정점 / 인덱스 버퍼가 준비되었다면 텍스쳐 셰이더를 이용해 그리게 됩니다.
	//2D 렌더링을 수행하기 위해 projectionMatrix 대신 orthoMatrix를 인자로 보냈다는 점을 주의하기 바랍니다.
	//또한 뷰 행렬의 내용이 계속 바뀌는 것이라면 2D 렌더링만을 위한 기본 뷰 행렬을 따로 만들어 사용해야 합니다
	//이 튜토리얼에서는 카메라가 고정되어 있기 때문에 일반 뷰 행렬을 사용해도 같은 결과가 나옵니다.
	
	result = m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_Bitmap->GetIndexCount(),
		worldMatrix, viewMatrix, orthMatrix, m_Bitmap->GetTexture());

	if (!result)
		return false;


	//월드를 돌려서 확인
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

	//버퍼의 내용을 화면에 출력
	m_Direct3D->EndScene();
	return true;
}
