#include "stdafx.h"
#include "Input.h"

Input::Input()
{
	m_DirectInput = nullptr;
	m_Keyboard = nullptr;
	m_Mouse = nullptr;
}

Input::Input(const Input& other) : Input()
{
}

Input::~Input()
{
}

bool Input::Initialize(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight)
{
	HRESULT result;

	m_ScreenWidth = screenWidth;
	m_ScreenHeight = screenHeight;

	m_MouseX = 0;
	m_MouseY = 0;

	//Direct Input �� �������̽� �ʱ�ȭ , DirectInput ��ü�� ��� �Ǹ� �ٸ� �Է� ��ġ���� �ʱ�ȭ�� �� �ִ�.
	result = DirectInput8Create(hinstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_DirectInput, NULL);
	if (FAILED(result))
		return false;

	//Ű���� �������̽� �ʱ�ȭ
	result = m_DirectInput->CreateDevice(GUID_SysKeyboard, &m_Keyboard, NULL);
	if (FAILED(result))
		return false;

	//������ ���� ���� /�� ��� Ű���尡 �Ǳ� ������ �̸� ���ǵ� ������ ������ ����� �� �ִ�
	result = m_Keyboard->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(result))
		return false;

	result = m_Keyboard->SetCooperativeLevel(NULL, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	if (FAILED(result))
		return false;

	if (m_DirectInput)
	{
		HRESULT hr = m_Keyboard->Acquire();
		if FAILED(hr)
		{
			switch (hr)
			{
			case DIERR_INVALIDPARAM: MessageBox(hwnd, L"Acquire FAILED : Invalid parameter ", L"InitializeKeyboard()", MB_OK); break;
			case DIERR_NOTINITIALIZED: MessageBox(hwnd, L"Acquire FAILED : The object has not been initialized", L"InitializeKeyboard()", MB_OK); break;
			case DIERR_OTHERAPPHASPRIO: MessageBox(hwnd, L"Acquire FAILED : Access Denied", L"InitializeKeyboard()", MB_OK); break;

			default: MessageBox(hwnd, L"Acquire FAILED : Unknow Error", L"InitializeKeyboard()", MB_OK);
			}
		}
	}
	
	/*	if (h == DIERR_NOTINITIALIZED)
		{
			MessageBox(0, L"Direct3DInput::initDirectInput() 5", 0, MB_OK);
		}
		if (h == DIERR_OTHERAPPHASPRIO)
		{
			MessageBox(0, L"Direct3DInput::initDirectInput() 5", 0, MB_OK);
		}
		if (h == DIERR_INVALIDPARAM)
		{
			MessageBox(0, L"Direct3DInput::initDirectInput() 5", 0, MB_OK);
		}*/
	


	//Ű���尡 ���õǸ� Acquire �Լ��� ȣ���Ͽ� �� �����ͷ� Ű���忡 ���� ������ ����մϴ�.
	//HRESULT r = m_Keyboard->Acquire();
	//if (FAILED(result))
	//	return false;

	result = m_DirectInput->CreateDevice(GUID_SysMouse, &m_Mouse, NULL);
	if (FAILED(result))
		return false;

	//���콺�� ���ؼ��� ����� ���¸� �����մϴ�. ���� �Ź� ���콺�� ��Ŀ���� �Ҿ����� Ȯ���ؾ� �մϴ�.
	result = m_Mouse->SetCooperativeLevel(NULL, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(result))
		return false;

	//���콺�� �����Ǹ� Acquire �Լ��� ȣ���Ͽ� ���콺�� ���� ������ ����մϴ�.
	result = m_Mouse->Acquire();
	//if (FAILED(result))
	//	return false;

	return true;
}

void Input::ShutDown()
{
	if (m_Mouse)
	{
		m_Mouse->Unacquire();
		m_Mouse->Release();
		m_Mouse = nullptr;
	}

	if (m_Keyboard)
	{
		m_Keyboard->Unacquire();
		m_Keyboard->Release();
		m_Keyboard = nullptr;
	}

	if (m_DirectInput)
	{
		m_DirectInput->Release();
		m_DirectInput = nullptr;
	}

}


//��ġ�� ���� ���¸� �о� �ռ� ���� ���� ���ۿ� ����մϴ�. �� ��ġ��	 ���¸� ���� �� ��������� ó���ϰ� �մϴ�.
bool Input::Frame()
{
	bool result;

	result = ReadKeyboard();
	if (!result)
		return false;

	result = ReadMouse();
	if (!result)
		return false;

	ProcessInput();

	return true;
}

//�� �Լ��� ���� ��� Ư�� Ű�� ���ȴ��� Ȯ���� �� �ִ� ����� �� �� ���� ���Դϴ�.
//�� ���ø����̼ǿ��� �ʿ��� �ٸ� Ű���� Ȯ���ϴ� �Լ��� ���� ���� �ֽ��ϴ�.
bool Input::IsEscapePressed()
{
	if (m_KeyBoardState[DIK_ESCAPE] & 0x800)
	{
		return true;
	}

	return false;
}

void Input::GetMouseLocation(int & mouseX, int & mouseY)
{
	mouseX = m_MouseX;
	mouseY = m_MouseY;
}

bool Input::ReadKeyboard()
{
	bool result;

	result = m_Keyboard->GetDeviceState(sizeof(m_KeyBoardState), (LPVOID)&m_KeyBoardState);
	if (FAILED(result))
		return false;

	return true;
}

/*
���콺�� ���´� ���� �����Ӱ� �޶��� ��ġ(����)���� �����մϴ�.
���� ��� ���콺�� 5������ŭ ���������� �̵����״ٸ� ȭ����� ��ġ�� �˷������� �ʽ��ϴ�.
������ ����(delta) ������ �˾Ƴ� �� �ֱ� ������ �̸� �̿��Ͽ� ���콺�� ��ġ�� ��ü������ ������ �� �ֽ��ϴ�.
*/
bool Input::ReadMouse()
{
	HRESULT result;

	result = m_Mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&m_MouseState);
	if (FAILED(result))
	{
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			m_Mouse->Acquire();
		}
		else
		{
			return false;
		}
	}

	return true;
}

/*
ProcessInput �Լ��� ���� �����ӿ����� ��������� ������ ����Ǵ� ���Դϴ�.
�� Ʃ�丮�󿡼��� �ܼ��� �����쿡�� ���콺�� ��ġ�� ��� ��ó�� ���� ���콺�� ��ġ�� �����ϵ��� �� ���Դϴ�.
�׷��� �ϱ� ���ؼ� m_mouseX, m_mouseY ������ ����Ͽ� ���콺�� ��ġ�� ����մϴ�.
�̷��� �Ͽ� ������ ���콺�� �������� �� ��ġ�� ��� ���󰩴ϴ�.


�����ؾ� �� ���� ���콺�� ȭ�� �ٱ����� ������ �ʰ� �ؾ� �Ѵٴ� ���Դϴ�.
������ ������ ���콺�� 0�� ��ġ���� ��� �������� �����δ� �ϴ���
�ٽ� ���������� ���� �������� ���콺�� ��ġ�� ��� 0���� �������Ѿ� �մϴ�.
*/
void Input::ProcessInput()
{
	m_MouseX += m_MouseState.lX; //delta ���ε�
	m_MouseY += m_MouseState.lY;

	if (m_MouseX < 0) { m_MouseX = 0; }
	if (m_MouseY < 0) { m_MouseY = 0; }

	if(m_MouseX > m_ScreenWidth) {m_MouseX = m_ScreenWidth; }
	if(m_MouseY > m_ScreenHeight) {m_MouseY = m_ScreenHeight; }
}

