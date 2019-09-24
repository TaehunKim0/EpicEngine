#pragma once
#pragma comment(lib, "winmm.lib")

#include <Windows.h>
#include <mmsystem.h>

class Fps
{
public:
	Fps();
	Fps(const Fps&);
	~Fps();

	void Initialize();
	void Frame();
	int GetFps();

private:
	int m_Fps, m_Count;
	unsigned long m_StartTime;
};

