#include "stdafx.h"
#include "Graphic.h"
#include "Imgui/imgui.h"
#include "Imgui/imgui_impl_dx11.h"
#include "Imgui/imgui_impl_win32.h"

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
	m_Text = nullptr;
	m_ModelList = nullptr;
	m_Frustum = nullptr;

	translationOffset[0] = 0.f;
	translationOffset[1] = 0.f;
	translationOffset[2] = -10.f;
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
	D3DXMATRIX baseViewMatrix;

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

	
	////////////카메라 생성////////////
	m_Camera = new Camera;
	if (!m_Camera)
		return false;

	//카메라 객체로부터 새로운 뷰 행렬을 생성하여 Text가 사용하도록 합니다. 이를 통하여 글자가 항상 화면의 같은 위치에 그려지게 됩니다.
	//m_Camera->SetPosition(0.0f, 0.0f, -1.0f); 
	m_Camera->Render();
	m_Camera->GetViewMatrix(baseViewMatrix);

	//카메라 포지션 설정
	m_Camera->SetPosition(0.0f, 0.0f, -1.0f);



	//////////모델 오브젝트 생성//////////
	m_Model = new Model;
	if (!m_Model)
		return false;

	const WCHAR* pwcsName = L"../EpicEngine/data/seafloor.dds";
	wchar_t* tempWide = const_cast <wchar_t*> (pwcsName);

	const char* temp0 = "../EpicEngine/data/cube.txt";

	//모델 오브젝트 초기화
	result = m_Model->Initialize(m_Direct3D->GetDevice(), "../EpicEngine/data/sphere.txt" , tempWide);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object", L"Error", MB_OK);
		return false;
	}

	///////////텍스쳐 셰이더 생성///////////
	m_TextureShader = new TextureShader;
	//텍스쳐 셰이더 오브젝트 초기화
	result = m_TextureShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}

	//////////라이트 셰이더 생성////////
	m_LightShader = new LightShader;
	if (!m_LightShader)
		return false;

	//라이트 셰이더 초기화
	result = m_LightShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the light shader object", L"Error", MB_OK);
		return false;
	}

	////////////라이트 생성//////////
	m_Light = new Light();
	if (!m_Light)
		return false;

	m_Light->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetDirection(0.0f, 0.0f, 1.0f);
	m_Light->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetSpecularPower(32.f);

	////////////모델 리스트 생성/////////
	m_ModelList = new ModelList;
	if (!m_ModelList)
		return false;

	result = m_ModelList->Initialize(25);
	if (!result)
	{
		MessageBox(hwnd, L"Could not Initialize the model list object", L"Error", MB_OK);
		return false;
	}

	///////////Frustum 클래스 생성////////////
	m_Frustum = new Frustum;
	if (!m_Frustum)
		return false;

	////////////비트맵 오브젝트 생성////////////
	m_Bitmap = new Bitmap;
	if (!m_Bitmap)
		return false;

	const WCHAR* pwcsName3 = L"../EpicEngine/data/black.png";
	wchar_t* tempWide3 = const_cast <wchar_t*> (pwcsName3);

	//비트맵 초기화
	result = m_Bitmap->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight, tempWide3, 128, 128);
	if (!result)
	{ 
		MessageBox(hwnd, L"Could not initialize the bitmap object.", L"Error", MB_OK);
		return false;
	}

	/////////////텍스트 클래스 생성///////////
	 m_Text = new Text; 
	 if(!m_Text)
		 return false; 
	 
	 //텍스트 클래스 초기화
	 result = m_Text->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), hwnd, screenWidth, screenHeight, baseViewMatrix);
	 if(!result)
	 { 
		 MessageBox(hwnd, L"Could not initialize the text object.", L"Error", MB_OK);
		 return false;
	 }

	 //setup imgui
	 IMGUI_CHECKVERSION();
	 ImGui::CreateContext();
	 ImGuiIO& io = ImGui::GetIO();
	 ImGui_ImplWin32_Init(hwnd);
	 ImGui_ImplDX11_Init(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext());
	 ImGui::StyleColorsDark();

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

	if (m_Text)
	{
		m_Text->Shutdown();
		delete m_Text;
		m_Text = nullptr;
	}

	if (m_ModelList)
	{
		m_ModelList->Shutdown();
		delete m_ModelList;
		m_ModelList = nullptr;
	}

	if (m_Frustum)
	{
		delete m_Frustum;
		m_Frustum = nullptr;
	}
}

bool Graphic::Frame(int fps, int cpu, int mouseX, int mouseY)
{
	bool result;
	
	result = m_Text->SetFps(fps, m_Direct3D->GetDeviceContext());
	if (!result)
		return false;

	result = m_Text->SetCpu(cpu, m_Direct3D->GetDeviceContext());
	if (!result)
		return false;
	
	/*result = m_Text->SetMousePosition(mouseX, mouseY, m_Direct3D->GetDeviceContext());
	if (!result)
		return false;*/

	//m_Camera->SetPosition(0.f, 0.f, -10.f);

	return true;
}

bool Graphic::Frame(float rotationY)
{
	m_Camera->SetPosition(0.f, 0.f, -10.0f);
	m_Camera->SetRotation(0.f, rotationY, 0.f);

	return true;
}

bool Graphic::Render()
{
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
	int modelCount, renderCount, index;
	float positionX, positionY, positionZ, radius;
	D3DXVECTOR4 color;
	bool renderModel, result;

	//씬을 그리기 위해 버퍼를 초기화
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	//카메라의 위치를 토대로 뷰 행렬을 만들기 위해 카메라의 Render 함수를 호출합니다. 
	m_Camera->Render();

	//카메라 및 d3d 객체에서 세계, 보기 및 투영 행렬을 가져옵니다
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	//또한 2D 렌더링을 하기 위해 정사영 행렬을 구합니다. 일반 투영행렬 대신 이 행렬을 전달할 것입니다.
	m_Direct3D->GetOrthoMatrix(orthoMatrix);

	////2D 렌더링을 시작하기 전에 Z버퍼를 끕니다.
	//m_Direct3D->TurnZBufferOff();

	////텍스트와 배경이 조화롭게 그려지기 위해 알파 블렌딩을 켭니다.
	//m_Direct3D->TurnOnAlphaBlending();

	/*
	매 프레임마다 뷰 행렬에 근거하여 시야 프러스텀을 구축한다는 것입니다.
	이 구축 과정은 뷰 행렬이 바뀌거나 우리의 프러스텀 확인이 맞지 않을 때 일어납니다.
	*/
	m_Frustum->ConstructFrustum(SCREEN_DEPTH, projectionMatrix, viewMatrix);
	//렌더링할 모델 수 확인
	modelCount = m_ModelList->GetModelCount();
	renderCount = 0;

	for (index = 0; index < modelCount; index++)
	{
		m_ModelList->GetData(index, positionX, positionY, positionZ, color);

		radius = 1.0f;

		renderModel = m_Frustum->CheckSphere(positionX, positionY, positionZ, radius);
	
		if (renderModel)
		{			
			D3DXMatrixTranslation(&worldMatrix, positionX, positionY, positionZ);

			m_Model->Render(m_Direct3D->GetDeviceContext());

			m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix,
				viewMatrix, projectionMatrix, m_Model->GetTexture(), m_Light->GetDirection(), m_Light->GetAmbientColor(),
				m_Light->GetDiffuseColor(), m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower());
			result = m_Text->SetRenderCount(renderCount, m_Direct3D->GetDeviceContext());
			
			m_Direct3D->GetWorldMatrix(worldMatrix);

			renderCount++;
		}
	}

	result = m_Text->SetRenderCount(renderCount, m_Direct3D->GetDeviceContext());
	if (!result)
	{
		return false;
	}

	//다시 알파블렌딩을 꺼서 그리는 물체 뒤에 뭔가 있을때 알파블렌딩 효과가 나오지 않게 합니다.
	m_Direct3D->TurnOffAlphaBlending();
	m_Direct3D->TurnZBufferOn();


	//일단 정점 / 인덱스 버퍼가 준비되었다면 텍스쳐 셰이더를 이용해 그리게 됩니다.
	//2D 렌더링을 수행하기 위해 projectionMatrix 대신 orthoMatrix를 인자로 보냈다는 점을 주의하기 바랍니다.
	//또한 뷰 행렬의 내용이 계속 바뀌는 것이라면 2D 렌더링만을 위한 기본 뷰 행렬을 따로 만들어 사용해야 합니다
	//이 튜토리얼에서는 카메라가 고정되어 있기 때문에 일반 뷰 행렬을 사용해도 같은 결과가 나옵니다.
	
	result = m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_Bitmap->GetIndexCount(),
		worldMatrix, viewMatrix, orthoMatrix, m_Bitmap->GetTexture());

	if (!result)
		return false;

	///////////////모델 돌려서 하이라이트 확인//////////////
	//static float rotation = 0.0f;
	//rotation += 0.01f;

	////월드를 돌려서 확인
	//D3DXMatrixRotationY(&worldMatrix, rotation);

	//m_Model->Render(m_Direct3D->GetDeviceContext());
	//result = m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix,
	//	viewMatrix, projectionMatrix, m_Model->GetTexture(), m_Light->GetDirection(), m_Light->GetAmbientColor(),
	//	m_Light->GetDiffuseColor(), m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower());

	//if (!result)
	//	return false;

	// 여기서 텍스트 객체의 render 함수를 호출하여 모든 문장들이 화면에 그려지도록 합니다.
		//그리고 2D 이미지의 경우와 마찬가지로 렌더링을 수행하기 전에 Z버퍼를 해제하고 다 그리면 Z버퍼를 켭니다.
	result = m_Text->Render(m_Direct3D->GetDeviceContext(), worldMatrix, orthoMatrix);
	if (!result)
		return false;

	m_Direct3D->TurnZBufferOff();
	m_Direct3D->TurnOnAlphaBlending();

	static int counter = 0;

	//Start the Dear ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();

	ImGui::NewFrame();

	ImGui::Begin("Test");
	ImGui::Text("This is Example Text");

	if (ImGui::Button("CLICK ME!"))
	{
		PostQuitMessage(0);
	}
	
	ImGui::SameLine();
	string clickCount = "CLICK Count : " + std::to_string(counter);
	ImGui::Text(clickCount.c_str());
	ImGui::DragFloat3("Translation X/Y/Z", translationOffset, 0.1f, -10.0f, 10.f);

	static bool checked = true;
	ImGui::Checkbox("checkbox", &checked);

	ImGui::End();
	ImGui::Render();

	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	//m_Camera->SetPosition(translationOffset[0], translationOffset[1], translationOffset[2]);

	//버퍼의 내용을 화면에 출력
	m_Direct3D->EndScene();
	return true;
}
