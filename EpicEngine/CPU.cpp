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
Initialize 함수에서는 cpu 사용량을 질의하기 위해 필요한 핸들을 초기화합니다.
여기서 설정된 질의(query)는 개개 cpu의 사용량 대신 시스템의 모든 cpu들의 사용량 총합을 합하여 돌려줍니다.
만약 핸들이나 질의를 얻지 못하는 등의 이유로 cpu 사용량 정보를 읽을 수 없다면
m_canReadCpu 변수가 false로 바뀌어서 기록하는 cpu사용량을 0이 되도록 합니다.
일부 os에서는 권한 때문에 이 기능을 사용할 수 없을 수도 있습니다.
또한 타이머를 돌려서 cpu 사용량을 매 초마다 측정하도록 합니다.
*/

void CPU::Initialize()
{
	PDH_STATUS status;

	// 이 개체가 시스템 cpu 사용량을 읽을 수 있는지 여부를 나타내는 플래그를 초기화하십시오.
	m_CanReadCpu = true;

	// cpu 사용량을 폴링할 쿼리 개체를 생성하십시오.
	status = PdhOpenQuery(NULL, 0, &m_QueryHandle);
	if (status != ERROR_SUCCESS)
	{
		m_CanReadCpu = false;
	}

	// 시스템의 모든 cpus를 폴링하도록 쿼리 개체를 설정하십시오.
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
FpsClass처럼 이 클래스의 Frame 함수도 매 프레임마다 호출해야 합니다.
하지만 질의 횟수를 줄이기 위해서 m_lastSampleTime이라는 변수를 두어 1초마다
cpu사용량을 샘플링할 수 있도록 합니다.
그렇게 매 초마다 cpu에게 사용량을 물어보고 그 값을 m_cpuUsage 변수에 저장합니다.
그 외에 크게 중요한 내용은 없습니다.
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
