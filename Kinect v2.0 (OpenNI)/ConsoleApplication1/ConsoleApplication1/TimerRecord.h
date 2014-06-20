#pragma once
#include <conio.h>
#include <time.h>
#include <Windows.h>

class TimerRecord
{

private:
	clock_t m_cTime;
	bool m_bIsRunning;
	int m_timeout;
public:
	TimerRecord(void);
	~TimerRecord(void);
	HRESULT TimerRecord::Start(int pTimeout);
	HRESULT TimerRecord::Stop();
	int TimerRecord::GetTime();
	bool TimerRecord::IsRunning() { return m_bIsRunning; }
	int TimerRecord::getTimeout() { return m_timeout; }
};
