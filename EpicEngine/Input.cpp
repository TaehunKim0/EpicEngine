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


	// Store the screen size which will be used for positioning the mouse cursor.
	 m_ScreenWidth = screenWidth;
	 m_ScreenHeight = screenHeight;

	// Initialize the location of the mouse on the screen.
	m_MouseX = 0;
	m_MouseY = 0;

	// Initialize the main direct input interface.
	result = DirectInput8Create(hinstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_DirectInput, NULL);
	if (FAILED(result))
	{
		return false;
	}

	// Initialize the direct input interface for the keyboard.
	result = m_DirectInput->CreateDevice(GUID_SysKeyboard, &m_Keyboard, NULL);
	if (FAILED(result))
	{
		return false;
	}

	// Set the data format.  In this case since it is a keyboard we can use the predefined data format.
	result = m_Keyboard->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(result))
	{
		return false;
	}

	// Set the cooperative level of the keyboard to not share with other programs.
	result = m_Keyboard->SetCooperativeLevel(hwnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	if (FAILED(result))
	{
		return false;
	}
	else
	{
		HRESULT hr;
		DIPROPDWORD buffsize;
		buffsize.diph.dwSize = sizeof(buffsize);
		buffsize.diph.dwHeaderSize = sizeof(buffsize.diph);
		buffsize.diph.dwObj = 0;
		buffsize.diph.dwHow = DIPH_DEVICE;
		buffsize.dwData = 32;
		hr = m_Keyboard->SetProperty(DIPROP_BUFFERSIZE, &buffsize.diph);

		if (FAILED(m_Keyboard->SetProperty(DIPROP_BUFFERSIZE, &buffsize.diph)))
		{
			return false;
		}

		else if (FAILED(m_Keyboard->Acquire()))
		{
			return false;
		}
	}

	/*if FAILED(result)
	{
		switch (result)
		{
		case DIERR_INVALIDPARAM: MessageBox(hwnd, L"Acquire FAILED : Invalid parameter ", L"InitializeKeyboard()", MB_OK); break;
		case DIERR_NOTINITIALIZED: MessageBox(hwnd, L"Acquire FAILED : The object has not been initialized", L"InitializeKeyboard()", MB_OK); break;
		case DIERR_OTHERAPPHASPRIO: MessageBox(hwnd, L"Acquire FAILED : Access Denied", L"InitializeKeyboard()", MB_OK); break;
		default: MessageBox(hwnd, L"Acquire FAILED : Unknow Error", L"InitializeKeyboard()", MB_OK);
		}
	}*/
	
	result = m_DirectInput->CreateDevice(GUID_SysMouse, &m_Mouse, NULL);
	if (FAILED(result))
		return false;

	if (FAILED(m_Mouse->SetDataFormat(&c_dfDIMouse)))
		return false;

	//���콺�� ���ؼ��� ����� ���¸� �����մϴ�. ���� �Ź� ���콺�� ��Ŀ���� �Ҿ����� Ȯ���ؾ� �մϴ�.
	result = m_Mouse->SetCooperativeLevel(hwnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);

	if (FAILED(result))
	{	
		return false;
	}
	else
	{
		DIPROPDWORD buffsize;
		buffsize.diph.dwSize = sizeof(buffsize);
		buffsize.diph.dwHeaderSize = sizeof(buffsize.diph);
		buffsize.diph.dwObj = 0;
		buffsize.diph.dwHow = DIPH_DEVICE;
		buffsize.dwData = 32;
		if (FAILED(m_Mouse->SetProperty(DIPROP_BUFFERSIZE, &buffsize.diph)))
		{
			return false;
		}
		else if (FAILED(m_Mouse->Acquire()))
		{
			return false;
		}
		else
		{
			return true;
		}
	}
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
	if (m_KeyBoardState[VK_SPACE] & 0x8000)
	{
		return true;
	}

	if (m_KeyBoardState[DIK_ESCAPE] & 0x8000)
	{
		PostQuitMessage(0);
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

	HRESULT hr;

	//m_Keyboard->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), &m_KeyboardBuffer ,(LPVOID)&m_KeyBoardState,)
	hr = m_Keyboard->GetDeviceState(sizeof(m_KeyBoardState), (LPVOID)&m_KeyBoardState);
	if (FAILED(hr))
	{
		hr = m_Keyboard->Acquire();
	}

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