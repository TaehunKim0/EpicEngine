#include "stdafx.h"
#include "Fps.h"

Fps::Fps()
{
}

Fps::Fps(const Fps &)
{
}

Fps::~Fps()
{
}

void Fps::Initialize()
{
	m_Fps = 0;
	m_Count = 0;
	m_StartTime = timeGetTime();
}

/*
Frame 함수는 매 프레임마다 불려져서 1씩 증가하도록 해야 합니다.
만약 1초 이상 지났다면 현재 프레임수를 m_fps 변수에 저장합니다.
그리고 프레임수를 초기화하고 다시 타이머를 돌립니다.
*/

void Fps::Frame()
{
	m_Count++;

	if (timeGetTime() >= (m_StartTime + 1000))
	{
		m_Fps = m_Count;
		m_Count = 0;
		m_StartTime = timeGetTime();
	}
}

int Fps::GetFps()
{
	return m_Fps;
}
