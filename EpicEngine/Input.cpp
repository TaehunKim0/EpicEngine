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

	//������ ���� ���� /�� ��� Ű���尡 �Ǳ� ������ �̸� ���ǵ� ������ ������ ����� �� �ִ�
	result = m_Keyboard->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(result))
		return false;

	/*
	Ű������ ���� ����(cooperative level)�� ���ϴ� ���� �� ��ġ�� ������ �ϴ���, �׸��� ��� ���� �������� �����ϹǷ� �߿��մϴ�.
	���⼭�� DISCL_EXCLUSIVE �÷��׷� �ٸ� ���α׷���� �������� �ʴ´ٰ� �˷��ݴϴ�(���� ����).
	�̷��� �ϸ� ���� �� ���ø����̼ǿ����� �� �Է��� ������ �� �ְ� �˴ϴ�.
	������ ���� �ٸ� ���ø����̼ǿ����� Ű������ �Է¿� �����ϰ� �ϰ� �ʹٸ� DISCL_NONEXCLUSIVE�� ����Ͽ� �׷��� �� �� �ֽ��ϴ�(����� ����).
	�׷��� print screen key�� �ٽ� ��ũ������ ���� �� �ְ� �ǰ� ���������� �ٸ� ���ø����̼ǵ鵵 Ű����� ������ �� �ְ� �˴ϴ�. 
	����ؾ� �� ���� ����� �����̰� Ǯ��ũ���� �ƴ϶�� ��ġ�� �ٸ� ������� ��Ŀ���� �̵��ߴ���,
	�׸��� �ٽ� ��Ŀ���� �� ��ġ�� ����� �� �ְ� �Ǿ����� Ȯ���ؾ� �Ѵٴ� ���Դϴ�. 
	
	�̷� ��Ŀ���� �Ҿ������ ���� �밳 �ٸ� �����찡 ���� �����찡 �Ǿ� ��Ŀ���� ����ٴ��� 
	�ƴϸ� �츮 ���ø����̼��� �����찡 �ּ�ȭ�Ǵ� ��� �߻��մϴ�.
	*/

	result = m_Keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	if (FAILED(result))
		return false;

	//Ű���尡 ���õǸ� Acquire �Լ��� ȣ���Ͽ� �� �����ͷ� Ű���忡 ���� ������ ����մϴ�.
	result = m_Keyboard->Acquire();
	if (FAILED(result))
		return false;

	result = m_DirectInput->CreateDevice(GUID_SysMouse, &m_Mouse, NULL);
	if (FAILED(result))
		return false;

	//���콺�� ���ؼ��� ����� ���¸� �����մϴ�. ���� �Ź� ���콺�� ��Ŀ���� �Ҿ����� Ȯ���ؾ� �մϴ�.
	result = m_Mouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(result))
		return false;

	//���콺�� �����Ǹ� Acquire �Լ��� ȣ���Ͽ� ���콺�� ���� ������ ����մϴ�.
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
	return false;
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

