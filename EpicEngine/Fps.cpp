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
Frame �Լ��� �� �����Ӹ��� �ҷ����� 1�� �����ϵ��� �ؾ� �մϴ�.
���� 1�� �̻� �����ٸ� ���� �����Ӽ��� m_fps ������ �����մϴ�.
�׸��� �����Ӽ��� �ʱ�ȭ�ϰ� �ٽ� Ÿ�̸Ӹ� �����ϴ�.
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
