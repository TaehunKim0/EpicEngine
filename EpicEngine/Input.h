#pragma once

#define DIRECTINPUT_VERSION_0x0800
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#include <dinput.h>

class Input
{
public:
	Input();
	Input(const Input& other);
	~Input();

	bool Initialize(HINSTANCE, HWND, int, int);
	void ShutDown();
	bool Frame();

	bool IsEscapePressed();
	bool IsLeftArrowPressed();
	bool IsRightArrowPressed();

	void GetMouseLocation(int&, int&);

private:
	bool ReadKeyboard();
	bool ReadMouse();
	void ProcessInput();

private:
	IDirectInput8* m_DirectInput;
	IDirectInputDevice8* m_Keyboard;
	IDirectInputDevice8* m_Mouse;

	unsigned char m_KeyBoardState[256];
	DIMOUSESTATE m_MouseState;

	int m_ScreenWidth, m_ScreenHeight;
	int m_MouseX, m_MouseY;
};

