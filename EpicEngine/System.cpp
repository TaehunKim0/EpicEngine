#include "stdafx.h"
#include "Input.h"
#include"Graphic.h"
#include "System.h"


System::System()
{
	m_ApplicationName = L"";
	m_Graphic = nullptr;
	m_HInstance = nullptr;
	m_Hwnd = nullptr;
	m_Input = nullptr;
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

	//������ ���� �ʱ�ȭ
	InitWindows(screenWidth, screenHeight);

	//Input Ŭ���� ����
	m_Input = new Input();
	if (!m_Input)
		return false;

	//Input ��ü �ʱ�ȭ
	m_Input->Initialize();
	
	//�׷��� ��ü ����, �׷��� �������� ó���ϱ� ���� ��ü
	m_Graphic = new Graphic();
	if (!m_Graphic)
		return false;

	//�׷��� ��ü �ʱ�ȭ
	return m_Graphic->Initialize(screenWidth, screenHeight, m_Hwnd);
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
		delete m_Input;
		m_Input = 0;
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

	while (true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			if (!Frame())
				break;
		}
	}
}

LRESULT System::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch (umsg)
	{
		case WM_KEYDOWN:
		{
			m_Input->KeyDown((unsigned int)wparam);
			return 0;
		}
		case WM_KEYUP:
		{
			m_Input->KeyDown((unsigned int)wparam);
			return 0;
		}
		default:
		{
			return DefWindowProc(hwnd, umsg, wparam, lparam);
		}
	}
}

bool System::Frame()
{
	//ESC ���� �� ���� ���θ� ó���մϴ�
	if (m_Input->IsKeyDown(VK_ESCAPE))
		return false;

	//�׷��� ��ü�� �������� ó���մϴ�
	return m_Graphic->Frame();
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

LRESULT CALLBACK WndProc(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
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