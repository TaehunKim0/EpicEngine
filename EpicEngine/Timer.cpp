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
	// �� �ý����� ���� Ÿ�̸Ӹ� �����ϴ��� Ȯ���Ͻʽÿ�.
	QueryPerformanceCounter((LARGE_INTEGER*)&m_Frequency);
	if (m_Frequency == 0)
	{
		return false;
	}

	// �� �и��ʸ��� ���ļ� ī���Ͱ� üũ�ϴ� Ƚ���� Ȯ���Ͻʽÿ�.
	m_TickPerMs = (float)(m_Frequency / 1000);

	QueryPerformanceCounter((LARGE_INTEGER*)&m_StartTime);

	return true;
}
//Frame �Լ��� ���� ���α׷����� �� �������� ȣ���ϵ��� �մϴ�. 
//�̷��� �� �������� �ð��� ���̸� ����ؼ� ������ ���� �ð� ������ �� �� �ֽ��ϴ�.
//�����ϰ�, ����� �� �� ���� m_frame�� �����Ͽ� �̸� ȣ���ϴ� �ٸ� ������Ʈ���� ����ȭ�Ǹ� �� �ְ� �� �� �ֽ��ϴ�. �׸��� ���� ���� �ð��� ���� �������� ���� �ð����� �����մϴ�.

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
