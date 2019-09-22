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

	//Direct Input 의 인터페이스 초기화 , DirectInput 객체를 얻게 되면 다른 입력 장치들을 초기화할 수 있다.
	result = DirectInput8Create(hinstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_DirectInput, NULL);
	if (FAILED(result))
		return false;

	//데이터 포맷 세팅 /이 경우 키보드가 되기 때문에 미리 정의된 데이터 형식을 사용할 수 있다
	result = m_Keyboard->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(result))
		return false;

	/*
	키보드의 협력 레벨(cooperative level)을 정하는 것은 이 장치가 무엇을 하는지, 그리고 어떻게 사용될 것인지를 결정하므로 중요합니다.
	여기서는 DISCL_EXCLUSIVE 플래그로 다른 프로그램들과 공유하지 않는다고 알려줍니다(배제 상태).
	이렇게 하면 오직 이 어플리케이션에서만 이 입력을 감지할 수 있게 됩니다.
	하지만 만일 다른 어플리케이션에서도 키보드의 입력에 접근하게 하고 싶다면 DISCL_NONEXCLUSIVE를 사용하여 그렇게 할 수 있습니다(비배제 상태).
	그러면 print screen key로 다시 스크린샷을 찍을 수 있게 되고 마찬가지로 다른 어플리케이션들도 키보드로 제어할 수 있게 됩니다. 
	기억해야 할 것은 비배제 상태이고 풀스크린이 아니라면 장치가 다른 윈도우로 포커스가 이동했는지,
	그리고 다시 포커스를 얻어서 장치를 사용할 수 있게 되었는지 확인해야 한다는 것입니다. 
	
	이런 포커스를 잃어버리는 경우는 대개 다른 윈도우가 메인 윈도우가 되어 포커스를 얻었다던가 
	아니면 우리 어플리케이션의 윈도우가 최소화되는 경우 발생합니다.
	*/

	result = m_Keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	if (FAILED(result))
		return false;

	//키보드가 세팅되면 Acquire 함수를 호출하여 이 포인터로 키보드에 대한 접근을 취득합니다.
	result = m_Keyboard->Acquire();
	if (FAILED(result))
		return false;

	result = m_DirectInput->CreateDevice(GUID_SysMouse, &m_Mouse, NULL);
	if (FAILED(result))
		return false;

	//마우스에 대해서는 비배제 상태를 설정합니다. 따라서 매번 마우스가 포커스를 잃었는지 확인해야 합니다.
	result = m_Mouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(result))
		return false;

	//마우스가 설정되면 Acquire 함수를 호출하여 마우스에 대한 접근을 취득합니다.
	result = m_Mouse->Acquire();
	if (FAILED(result))
		return false;

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
	return false;
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

