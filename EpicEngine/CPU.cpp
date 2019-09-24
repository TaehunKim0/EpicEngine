#include "CPU.h"



CPU::CPU()
{
}

CPU::CPU(const CPU & other)
{
}


CPU::~CPU()
{
}
/*
Initialize �Լ������� cpu ��뷮�� �����ϱ� ���� �ʿ��� �ڵ��� �ʱ�ȭ�մϴ�.
���⼭ ������ ����(query)�� ���� cpu�� ��뷮 ��� �ý����� ��� cpu���� ��뷮 ������ ���Ͽ� �����ݴϴ�.
���� �ڵ��̳� ���Ǹ� ���� ���ϴ� ���� ������ cpu ��뷮 ������ ���� �� ���ٸ�
m_canReadCpu ������ false�� �ٲ� ����ϴ� cpu��뷮�� 0�� �ǵ��� �մϴ�.
�Ϻ� os������ ���� ������ �� ����� ����� �� ���� ���� �ֽ��ϴ�.
���� Ÿ�̸Ӹ� ������ cpu ��뷮�� �� �ʸ��� �����ϵ��� �մϴ�.
*/

void CPU::Initialize()
{
	PDH_STATUS status;

	// �� ��ü�� �ý��� cpu ��뷮�� ���� �� �ִ��� ���θ� ��Ÿ���� �÷��׸� �ʱ�ȭ�Ͻʽÿ�.
	m_CanReadCpu = true;

	// cpu ��뷮�� ������ ���� ��ü�� �����Ͻʽÿ�.
	status = PdhOpenQuery(NULL, 0, &m_QueryHandle);
	if (status != ERROR_SUCCESS)
	{
		m_CanReadCpu = false;
	}

	// �ý����� ��� cpus�� �����ϵ��� ���� ��ü�� �����Ͻʽÿ�.
	status = PdhAddCounter(m_QueryHandle, TEXT("\\Processor(_Total)\\% processor time"), 0, &m_CounterHandle);
	if (status != ERROR_SUCCESS)
	{ 
		m_CanReadCpu = false; 
	}


	m_LastSampleTime = GetTickCount();
	m_CpuUsage = 0;
}

void CPU::Shutdown()
{
	if (m_CanReadCpu)
	{
		PdhCloseQuery(m_QueryHandle);
	}
}

/*
FpsClassó�� �� Ŭ������ Frame �Լ��� �� �����Ӹ��� ȣ���ؾ� �մϴ�.
������ ���� Ƚ���� ���̱� ���ؼ� m_lastSampleTime�̶�� ������ �ξ� 1�ʸ���
cpu��뷮�� ���ø��� �� �ֵ��� �մϴ�.
�׷��� �� �ʸ��� cpu���� ��뷮�� ����� �� ���� m_cpuUsage ������ �����մϴ�.
�� �ܿ� ũ�� �߿��� ������ �����ϴ�.
*/

void CPU::Frame()
{
	PDH_FMT_COUNTERVALUE value;
	if (m_CanReadCpu)
	{
		if ((m_LastSampleTime + 1000) < GetTickCount())
		{ 
			m_LastSampleTime = GetTickCount();
			PdhCollectQueryData(m_QueryHandle);
			PdhGetFormattedCounterValue(m_CounterHandle, PDH_FMT_LONG, NULL, &value);
			m_CpuUsage = value.longValue; 
		} 
	}
}

int CPU::GetCpuCentage()
{
	int usage;
	
	if (m_CanReadCpu)
	{ 
		usage = (int)m_CpuUsage; 
	}
	else
	{ 
		usage = 0;
	}
	
	return usage;

}
