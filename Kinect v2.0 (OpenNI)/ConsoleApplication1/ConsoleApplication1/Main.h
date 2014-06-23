#pragma once
#include <iostream>
#include <time.h>
#include <conio.h>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\ml\ml.hpp>
#include "Kinect.h"
#include "ImageTreatement.h"
#include "TimerRecord.h"
#include "GesturesAnalyzer.h"
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
	static DWORD WINAPI ThreadAffichage(LPVOID lpParam);

	void OnRecordGestureButton( Bouton pButton );
	void OnSendClassTextButton( Bouton pButton );
	void OnRecordPositionButton( Bouton pButton );
	void OnRecordHandPositionButton( Bouton pButton );
	void OnSendNameTextButton(Bouton pButton);
	void OnSendTimeoutButton(Bouton pButton);
	void OnGetRecordInfoButton(Bouton pButton);
	void OnStopRecordButton(Bouton pButton);
	void OnSaveSampleButton(Bouton pButton);
	void Main::OnChangeMainFrameButton(Bouton pButton);

	HRESULT Run();
	void Main::SaveCode();

private:
	DWORD WINAPI ThreadGetAndDisplayNewColorFrame();
	DWORD WINAPI ThreadGetAndDisplayNewDepthFrame();
	DWORD WINAPI ThreadTreatementDepthFrame();
	DWORD WINAPI ThreadAnimation();
	DWORD WINAPI KeyboardListener();
	DWORD WINAPI ThreadAffichage();

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

	int m_sliderDebut;
	int m_sliderFin;
	int m_sliderMax;

	int m_recordingClasse;
	string m_recordingName;
	string m_recordingType;

	int m_recordingHandClasse;
	string m_recordingHandName;

	int m_timeout;

	HANDLE m_hFeaturesRecorderMutex;
	FeaturesRecorder* m_featuresRecorder;

	HANDLE m_hSaveHandEvent;

	Engine* m_engine;

	bool m_bPriorityFrameTab[3];
};