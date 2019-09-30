#pragma once

class Input;
class Graphic;
class Sound;
class Timer;
class CPU;
class Fps;
class Position;
class System
{
public:
	System();
	System(const System&);
	~System();

	bool Initialize();
	void Shutdown();
	void Run();

	LRESULT CALLBACK MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam);

private:
	bool Frame();
	void InitWindows(int& screenWidth, int& screenHeight);
	void ShutdownWindows();

private:
	LPCWSTR m_ApplicationName;
	HINSTANCE m_HInstance;
	HWND m_Hwnd;

	Input* m_Input;
	Graphic* m_Graphic;
	Sound* m_Sound;
	Timer* m_Timer;
	Fps* m_Fps;
	CPU* m_Cpu;
	Position* m_Position;
};

static LRESULT CALLBACK WndProc(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam);
static System* ApplicationHandle = 0;