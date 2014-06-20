#pragma once
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <Windows.h>
#include <OpenNI.h>
#include <NiTE.h>

#define userWidth 1920
#define userHeight 1080

#define captureWidth 640
#define captureHeight 480

#define roiWidth 150
#define roiHeight 150

#define treshold 30

typedef struct Rec { 
	   LONG x1;
	   LONG x2;
	   LONG y1;
	   LONG y2;
	   LONG width;
	   LONG height;
	   LONG handPos;
	} Rec;

class ImageTreatement
{
public:
	ImageTreatement(void);
	~ImageTreatement(void);
	static void ImageTreatement::calculateHistogram(float* pHistogram, int histogramSize, const openni::VideoFrameRef& frame);
	static void ImageTreatement::DrawLimb(cv::Mat pDepth, const nite::UserTracker* pUserTracker, const nite::SkeletonJoint& joint1, const nite::SkeletonJoint& joint2);
	static void ImageTreatement::DrawSkeleton(cv::Mat pDepth, const nite::UserTracker* pUserTracker, const nite::UserData& userData);
	static void ImageTreatement::DrawSkeletons(cv::Mat pDepth, nite::UserTracker* pUserTracker, const nite::Array<nite::UserData>& users);
	static Rec getHandROI(nite::UserTracker* pUserTracker, const nite::Array<nite::UserData>& users, nite::JointType joint);
	static bool isInROI(int x, int y, Rec roi);
	static HRESULT ImageTreatement::GetDepthMat(cv::Mat* pImg, const openni::VideoFrameRef& pVfrFrame, const float* pHistogram);
	static HRESULT  ImageTreatement::GetColorMat(cv::Mat* pImg, const openni::VideoFrameRef& pVfrFrame);
	static void ImageTreatement::DrawLimbFromSample(cv::Mat pMat, nite::UserTracker* pUserTracker, cv::Point3d joint1, cv::Point3d joint2);
	static void ImageTreatement::DrawSkeletonFromSample( cv::Mat pMat, const std::vector<cv::Point3d>& recordedSkeleton, nite::UserTracker* pUserTracker );
	static HRESULT ImageTreatement::ExtractFeaturesFromHand( cv::Mat pMat, cv::Mat pMatOUT, std::vector<double>& pSampleVec);
	static HRESULT ImageTreatement::GetRealHand( cv::Mat pMatDepth, cv::Mat pMatColor , cv::Mat* pOut);
};

