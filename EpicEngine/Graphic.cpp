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

	//Direct3D 객체 생성
	m_Direct3D = (D3D*)_aligned_malloc(sizeof(D3D), 16);
	if (!m_Direct3D)
		return false;

	//Direct3D 객체 초기화
	if (!m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR))
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

	const WCHAR* pwcsName = L"../Engine/data/seafloor.dds";
	wchar_t* tempWide = const_cast <wchar_t*> (pwcsName);

	//모델 오브젝트 초기화
	result = m_Model->Initialize(m_Direct3D->GetDevice(), tempWide);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object", L"Error", MB_OK);
		return false;
	}

	//컬러 셰이더 생성
	m_ColorShader = new ColorShader;
	if (!m_ColorShader)
		return false;

	//컬러 셰이더 초기화
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

	//그래픽 씬 렌더
	result = Render();
	if (!result)
		return false;

	return true;
}

bool Graphic::Render()
{
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool result;

	//씬을 그리기 위해 버퍼를 초기화
	m_Direct3D->BeginScene(0.5f,0.5f,0.5f,1.0f);
	
	//카메라의 위치를 토대로 뷰 행렬을 만들기 위해 카메라의 Render 함수를 호출합니다. 
	m_Camera->Render();
	
	//카메라 및 d3d 객체에서 세계, 보기 및 투영 행렬을 가져옵니다
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	m_Model->Render(m_Direct3D->GetDeviceContext());

	result = m_ColorShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix);
	if (!(result))
		return false;

	//버퍼의 내용을 화면에 출력
	m_Direct3D->EndScene();
	return true;
}
