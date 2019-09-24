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

	//마우스에 대해서는 비배제 상태를 설정합니다. 따라서 매번 마우스가 포커스를 잃었는지 확인해야 합니다.
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


//장치의 현재 상태를 읽어 앞서 만든 상태 버퍼에 기록합니다. 각 장치의	 상태를 읽은 뒤 변경사항을 처리하게 합니다.
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

//이 함수를 통해 어떻게 특정 키가 눌렸는지 확인할 수 있는 방법을 알 수 있을 것입니다.
//또 어플리케이션에서 필요한 다른 키들을 확인하는 함수를 만들 수도 있습니다.
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
마우스의 상태는 이전 프레임과 달라진 위치(변위)만을 저장합니다.
예를 들어 마우스를 5단위만큼 오른쪽으로 이동시켰다면 화면상의 위치를 알려주지는 않습니다.
하지만 변위(delta) 정보는 알아낼 수 있기 때문에 이를 이용하여 마우스의 위치를 자체적으로 관리할 수 있습니다.
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
ProcessInput 함수는 이전 프레임에서의 변경사항이 실제로 적용되는 곳입니다.
이 튜토리얼에서는 단순히 윈도우에서 마우스의 위치를 잡는 것처럼 현재 마우스의 위치를 갱신하도록 할 것입니다.
그렇게 하기 위해서 m_mouseX, m_mouseY 변수를 사용하여 마우스의 위치를 기록합니다.
이렇게 하여 유저가 마우스를 움직여도 그 위치를 계속 따라갑니다.


주의해야 할 것은 마우스가 화면 바깥으로 나가지 않게 해야 한다는 것입니다.
심지어 유저가 마우스를 0의 위치에서 계속 왼쪽으로 움직인다 하더라도
다시 오른쪽으로 가기 전까지는 마우스의 위치를 계속 0으로 고정시켜야 합니다.
*/
void Input::ProcessInput()
{
	m_MouseX += m_MouseState.lX; //delta 값인듯
	m_MouseY += m_MouseState.lY;

	if (m_MouseX < 0) { m_MouseX = 0; }
	if (m_MouseY < 0) { m_MouseY = 0; }

	if(m_MouseX > m_ScreenWidth) {m_MouseX = m_ScreenWidth; }
	if(m_MouseY > m_ScreenHeight) {m_MouseY = m_ScreenHeight; }
}