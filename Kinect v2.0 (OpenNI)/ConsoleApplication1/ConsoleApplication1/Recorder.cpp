#include "stdafx.h"
#include "Recorder.h"

FeaturesRecorder::FeaturesRecorder(void) : m_timer(), m_bIsRecording(false), m_firstTimeout(5)
{
	m_hIsFinish = CreateEvent(NULL, false, false, NULL);
}


FeaturesRecorder::~FeaturesRecorder(void)
{
}

void FeaturesRecorder::Start(int pTimeout)
{
	m_timer.Start(pTimeout);
	m_bIsRecording = true;
}

void FeaturesRecorder::Stop()
{
	m_timer.Stop();
	m_bIsRecording = false;
	SetEvent(m_hIsFinish);
}

void FeaturesRecorder::Restart(int pTimeout)
{
	m_timer.Stop();
	m_timer.Start(pTimeout);
}

void FeaturesRecorder::Perform(cv::Mat pMat, vector<vector<cv::Point3d>>& pRecordedSkeleton, const nite::Array<nite::UserData>& pUsers)
{
	int time = m_timer.GetTime();
	if(time < 0 )
	{
		m_timer.Stop();
		m_bIsRecording = false;
	}
	else if( time < m_firstTimeout)
	{	
		std::ostringstream str;
		str << time;
		putText(pMat, str.str(), cv::Point(550,20),cv::FONT_HERSHEY_COMPLEX_SMALL,1,cv::Scalar(0,255,0));
	}
	else if(time < m_timer.getTimeout() + m_firstTimeout)
	{
		std::ostringstream str;
		str << time-m_firstTimeout;
		putText(pMat, str.str(), cv::Point(550,20),cv::FONT_HERSHEY_COMPLEX_SMALL,1,cv::Scalar(0,0,255));
		GesturesAnalyzer::SaveSkeleton(pRecordedSkeleton, pUsers);
	}				
	else
	{
		m_timer.Stop();
		m_bIsRecording = false;
		SetEvent(m_hIsFinish);
	}
}
