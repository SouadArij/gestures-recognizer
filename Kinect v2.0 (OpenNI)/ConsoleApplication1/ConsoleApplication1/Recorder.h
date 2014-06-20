#pragma once
#include "TimerRecord.h"
#include "GesturesAnalyzer.h"
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\core\core.hpp>

class FeaturesRecorder
{
public:
	FeaturesRecorder(void);
	~FeaturesRecorder(void);
	void Start(int pTimeout);
	void Stop();
	void Restart(int pTimeout);
	void Perform(cv::Mat pMat, vector<vector<cv::Point3d>>& pRecordedSkeleton, const nite::Array<nite::UserData>& pUsers);
	HANDLE GetFinishEvent() { return m_hIsFinish; }
	bool IsRecording() { return m_bIsRecording; }
private:
	TimerRecord m_timer;
	bool m_bIsRecording;
	HANDLE m_hIsFinish;
	int m_firstTimeout;
};

