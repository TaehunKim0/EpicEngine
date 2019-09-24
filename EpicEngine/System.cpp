#include "stdafx.h"
#include "Input.h"
#include"Graphic.h"
#include "System.h"
#include "Sound.h"
#include "Fps.h"
#include "CPU.h"
#include "Timer.h"

System::System()
{
	m_ApplicationName = L"";
	m_Graphic = nullptr;
	m_HInstance = nullptr;
	m_Hwnd = nullptr;
	m_Input = nullptr;
	m_Sound = nullptr;
}

System::System(const System&) : System()
{
}

System::~System()
{
}

bool System::Initialize()
{
	int screenWidth = 0;
	int screenHeight = 0;

	bool result;

	//������ ���� �ʱ�ȭ
	InitWindows(screenWidth, screenHeight);

	//Input Ŭ���� ����
	m_Input = new Input();
	if (!m_Input)
		return false;

	//Input ��ü �ʱ�ȭ
	result = m_Input->Initialize(m_HInstance, m_Hwnd, screenWidth, screenHeight);
	if (!result)
	{
		MessageBox(m_Hwnd, L"Could not Initialize the input object.", L"Error", MB_OK);
		return false;
	}


	//�׷��� ��ü ����, �� ������ ��� �׷��� ��Ҹ� �׸��� ��. �׷��� �������� ó���ϱ� ���� ��ü
	m_Graphic = new Graphic();
	if (!m_Graphic)
		return false;

	//�׷��� ��ü �ʱ�ȭ
	result = m_Graphic->Initialize(screenWidth, screenHeight, m_Hwnd);
	if (!result)
		return false;

	m_Sound = new Sound();
	if (!m_Sound)
		return false;

	result = m_Sound->Initialize(m_Hwnd);
	if (!result)
	{
		MessageBox(m_Hwnd, L"Could not Initialize Sound class", NULL, MB_OK);
		return false;
	}

	//Fps Intialize
	m_Fps = new Fps();
	if (!m_Fps)
		return false;

	m_Fps->Initialize();

	//Cpu Initialize
	m_Cpu = new CPU();
	if (!m_Cpu)
		return false;

	m_Cpu->Initialize();

	//Timer Initialize
	m_Timer = new Timer();
	if (!m_Timer)
		return false;

	result = m_Timer->Initialize();
	if (!result)
	{
		return false;
	}

	return true;
}

void System::Shutdown()
{
	if (m_Graphic)
	{
		m_Graphic->Shutdown();
		delete m_Graphic;
		m_Graphic = 0;
	}

	if (m_Input)
	{
		m_Input->ShutDown();
		delete m_Input;
		m_Input = 0;
	}

	if (m_Sound)
	{
		m_Sound->Shutdown();
		delete m_Sound;
		m_Sound = nullptr;
	}

	if (m_Cpu)
	{
		m_Cpu->Shutdown();
		delete m_Cpu;
		m_Cpu = nullptr;
	}

	if (m_Fps)
	{
		delete m_Fps;
		m_Fps = nullptr;
	}

	if (m_Timer)
	{
		delete m_Timer;
		m_Timer = nullptr;
	}

	ShutdownWindows();
}

void System::ShutdownWindows()
{
	//Ǯ��ũ���̸� ���÷��� ���� �ʱ�ȭ
	if (FULL_SCREEN)
		ChangeDisplaySettings(NULL, 0);

	//â�� ����
	DestroyWindow(m_Hwnd);
	m_Hwnd = NULL;

	//���α׷� �ν��Ͻ� ����
	UnregisterClass(m_ApplicationName, m_HInstance);
	m_HInstance = NULL;

	//�ܺ������� ������ �ʱ�ȭ
	ApplicationHandle = NULL;
}
void System::Run()
{
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	bool done, result;

	done = false;
	while (true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT)
			done = true;

		else
		{
			if (!Frame())
				break;
		}

		//Run �Լ����� escŰ�� Ȯ���ϴ� �Լ��� InputClass�� ����� �Լ��� ������� Ȯ���ϴ� ������ ��¦ �ٲ�����ϴ�.
		if (m_Input->IsEscapePressed() == true)
			done = true;
	}
}

LRESULT System::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch (umsg)
	{
		default:
		{
			return DefWindowProc(hwnd, umsg, wparam, lparam);
		}
	}
}

bool System::Frame()
{
	bool result;
	int mouseX, mouseY;

	m_Timer->Frame();
	m_Fps->Frame();
	m_Cpu->Frame();

	result = m_Input->Frame();
	if (!result)
		return false;

	//�Է� ��ü�� �� ���ŵǾ��ٸ� GraphicsClass�� ���콺�� ��ġ�� �ٲ���� ���ŵǵ��� �մϴ�.
	m_Input->GetMouseLocation(mouseX, mouseY);

	//�׷��� ��ü�� �������� ó���մϴ�
	result = m_Graphic->Frame(m_Fps->GetFps(), m_Cpu->GetCpuCentage(), mouseX, mouseY);
	if (!result)
		return false;

	result = m_Graphic->Render();
	if (!result)
		return false;

	return true;
}

void System::InitWindows(int& screenWidth, int& screenHeight)
{
	//�ܺ� �����͸� �� ��ü�� �����մϴ�.
	ApplicationHandle = this;

	// �� ���α׷��� �ν��Ͻ��� ������
	m_HInstance = GetModuleHandle(NULL);

	//���α׷� �̸��� �����մϴ�.
	m_ApplicationName = L"Dx11 Demo";

	//windows Ŭ������ �Ʒ��� ���� ����
	WNDCLASSEX wc;
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_HInstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = m_ApplicationName;
	wc.cbSize = sizeof(WNDCLASSEX);

	//windows class ���
	RegisterClassEx(&wc);

	//����� ȭ���� �ػ󵵸� �о�ɴϴ�
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	int posX = 0;
	int posY = 0;

	if (FULL_SCREEN)
	{
		// Ǯ��ũ�� ���� �����ߴٸ� ����� ȭ�� �ػ󵵸� ����ũ�� �ػ󵵷� �����ϰ� ������ 32bit�� ����
		DEVMODE dmScreenSettings;
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		//Ǯ ��ũ������ ���÷��� ������ �����մϴ�.
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);
	}
	else
	{
		screenWidth = 800;
		screenHeight = 600;

		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;

	}

	m_Hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_ApplicationName, m_ApplicationName,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
		posX, posY, screenWidth, screenHeight, NULL, NULL, m_HInstance, NULL);

	ShowWindow(m_Hwnd, SW_SHOW);
	SetForegroundWindow(m_Hwnd);
	SetFocus(m_Hwnd);
}


extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK WndProc(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	if (ImGui_ImplWin32_WndProcHandler(hwnd, umsg, wparam, lparam))
		return true;

	switch (umsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;

	default:
		return ApplicationHandle->MessageHandler(hwnd, umsg, wparam, lparam);

	}
}