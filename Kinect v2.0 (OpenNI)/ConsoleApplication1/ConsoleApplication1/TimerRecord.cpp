#include "stdafx.h"
#include "TimerRecord.h"

TimerRecord::TimerRecord(void) : m_timeout(0)
{
}

TimerRecord::~TimerRecord(void)
{
}

HRESULT TimerRecord::Start(int pTimeout)
{
	if(pTimeout > 0)
		m_timeout = pTimeout;
	else
		m_timeout = 32767;

	m_cTime = clock();
	m_bIsRunning = true;
	return S_OK;
}

HRESULT TimerRecord::Stop()
{
	m_cTime = NULL;
	m_bIsRunning = false;
	return S_OK;
}

int TimerRecord::GetTime()
{
	if(m_bIsRunning)
	{
		return double(clock()-m_cTime)/CLOCKS_PER_SEC;
	}
	else
	{
		return -1;
	}
}