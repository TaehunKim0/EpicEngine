#pragma once

#pragma comment(lib, "pdh.lib")
#include <Pdh.h>

class CPU
{
public:
	CPU();
	CPU(const CPU&);
	~CPU();

	void Initialize();
	void Shutdown();
	void Frame();
	int GetCpuCentage();

private:
	bool m_CanReadCpu;
	HQUERY m_QueryHandle;
	HCOUNTER m_CounterHandle;
	unsigned long m_LastSampleTime;
	long m_CpuUsage;
};

