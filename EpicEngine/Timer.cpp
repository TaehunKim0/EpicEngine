#include "Timer.h"



Timer::Timer()
{
}

Timer::Timer(const Timer & other) 
{
}

Timer::~Timer()
{
}

bool Timer::Initialize()
{
	// 이 시스템이 고성능 타이머를 지원하는지 확인하십시오.
	QueryPerformanceCounter((LARGE_INTEGER*)&m_Frequency);
	if (m_Frequency == 0)
	{
		return false;
	}

	// 매 밀리초마다 주파수 카운터가 체크하는 횟수를 확인하십시오.
	m_TickPerMs = (float)(m_Frequency / 1000);

	QueryPerformanceCounter((LARGE_INTEGER*)&m_StartTime);

	return true;
}
//Frame 함수는 메인 프로그램에서 매 루프마다 호출하도록 합니다. 
//이렇게 매 루프마다 시간의 차이를 계산해서 프레임 간의 시간 간격을 알 수 있습니다.
//질의하고, 계산한 뒤 그 값을 m_frame에 저장하여 이를 호출하는 다른 오브젝트들이 동기화되리 수 있게 할 수 있습니다. 그리고 나서 현재 시간을 다음 프레임의 시작 시간으로 저장합니다.

void Timer::Frame()
{
	INT64 currentTime;
	float timeDifference;

	QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);

	timeDifference = (float)(currentTime - m_StartTime);

	m_FrameTime = timeDifference / m_TickPerMs;

	m_StartTime = currentTime;
}

float Timer::GetTime()
{
	return m_FrameTime;
}
