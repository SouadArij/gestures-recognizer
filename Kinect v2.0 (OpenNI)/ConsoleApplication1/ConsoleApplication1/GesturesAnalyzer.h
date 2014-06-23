#pragma once
#include "stdafx.h"
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\core\core.hpp>
#include <Windows.h>
#include <OpenNI.h>
#include <NiTE.h>
#include "GRT\GRT.h"

typedef enum
{
	POSITION_NULL,
	POSITION_INITIALE,
	POSITION_SIGNE_MAIN,
} PositionType;

class GesturesAnalyzer
{
	public:
		GesturesAnalyzer(void);
		~GesturesAnalyzer(void);
		static HRESULT SaveSkeleton(vector<vector<cv::Point3d>>& pRecordedSkeleton, const nite::Array<nite::UserData>& pUsers);
		string PredictCurrentPosition(vector<vector<cv::Point3d>>& pRecordedSkeleton);
		UINT PredictGesture(const vector<vector<cv::Point3d>>& pSampleVec);
		string PredictHandPosition(const vector<double>& pSampleVec);
		HRESULT Init();
		HRESULT SVMSaveSampleAndTrain(UINT pClasse, string pName, const vector<vector<cv::Point3d>>& pSampleVec);
		HRESULT DTWSaveSampleAndTrain(UINT pClasse, const vector<vector<cv::Point3d>>& pSampleVec);
		HRESULT ANBCSaveSampleAndTrain(UINT pClasse, string pName, const vector<vector<cv::Point3d>>& pSampleVec);
		HRESULT HandSaveSample(UINT pClasse, string pName, const vector<double>& pSampleVec);

	private:
		GRT::GestureRecognitionPipeline m_dtw;
		GRT::LabelledTimeSeriesClassificationData m_DTWTrainingData;
		
		GRT::ANBC m_anbc;
		GRT::ClassificationData m_ANBCTrainingData;

		GRT::ANBC m_hand;
		GRT::ClassificationData m_HandTrainingData;
};

