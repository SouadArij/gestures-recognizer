#pragma once
#include "Kinect.h"
#include "ImageTreatement.h"
#include "TimerRecord.h"
#include "GesturesAnalyzer.h"
#include <iostream>
#include <time.h>
#include <conio.h>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\ml\ml.hpp>
#include "GRT\GRT.h"
#include "Recorder.h"
#include "GUI\Engine.h"

class Main
{
public:
	Main(void);
	~Main(void);
	static DWORD WINAPI ThreadGetAndDisplayNewColorFrame(LPVOID lpParam);
	static DWORD WINAPI ThreadGetAndDisplayNewDepthFrame(LPVOID lpParam);
	static DWORD WINAPI ThreadTreatementDepthFrame(LPVOID lpParam);
	static DWORD WINAPI ThreadAnimation(LPVOID lpParam);
	static DWORD WINAPI KeyboardListener(LPVOID lpParam);
	static DWORD WINAPI Main::ThreadAffichage(LPVOID lpParam);
	HRESULT Run();
	void Main::SaveCode();

private:
	DWORD WINAPI ThreadGetAndDisplayNewColorFrame();
	DWORD WINAPI ThreadGetAndDisplayNewDepthFrame();
	DWORD WINAPI ThreadTreatementDepthFrame();
	DWORD WINAPI ThreadAnimation();
	DWORD WINAPI KeyboardListener();
	DWORD WINAPI Main::ThreadAffichage();

	HANDLE m_hDisplayAnimationMutex;
	bool m_bDisplayAnimation;

	HANDLE m_hRecordingMutex;
	bool m_bRecording;

	HANDLE m_hContinueProcessingMutex;
	bool m_bContinueProcessing;

	HANDLE m_hDepthTreatement;
	HANDLE m_hColorTreatement;

	HANDLE m_hDepthTreatementIsFinishMutex;
	bool m_bDepthTreatementIsFinish;
	nite::UserTrackerFrameRef m_utfrUserFrameTemp;
	cv::Mat m_mColorFrameTemp;

	openni::VideoFrameRef m_depthFrame;
	openni::VideoFrameRef m_colorFrame;
	nite::HandTrackerFrameRef m_handFrame;
	nite::UserTrackerFrameRef m_userFrame;

	HANDLE m_hKinectMutex;
	Kinect* m_kinect ;

	HANDLE m_hGestureRecorderMutex;
	GesturesAnalyzer* m_gesturesAnalyzer;

	vector<vector<cv::Point3d>> m_recordedSkeleton;

	int m_sliderDebut, m_sliderFin, m_sliderMax;

	int m_recordingClasse;
	string m_recordingName;
	string m_recordingType;

	int m_recordingHandClasse;
	string m_recordingHandName;

	HANDLE m_hFeaturesRecorderMutex;
	FeaturesRecorder* m_featuresRecorder;

	HANDLE m_hSaveHandEvent;

	Engine* engine;
};