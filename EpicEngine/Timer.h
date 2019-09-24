#pragma once

#include <Windows.h>

class Timer
{
public:
	Timer();
	Timer(const Timer&);
	~Timer();

	bool Initialize();
	void Frame();

	float GetTime();

private:
	INT64 m_Frequency;
	float m_TickPerMs;
	INT64 m_StartTime;
	float m_FrameTime;
};

