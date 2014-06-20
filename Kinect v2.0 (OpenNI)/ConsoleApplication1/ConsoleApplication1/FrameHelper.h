#pragma once
#include "Kinect.h"
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\objdetect\objdetect.hpp>
using namespace std;
using namespace cv;

class FrameHelper
{

	static const Scalar SKELETON_COLORS[NUI_SKELETON_COUNT];
public:
	Point jointPositions[NUI_SKELETON_POSITION_COUNT];
	FrameHelper(void);
	~FrameHelper(void);
	void FrameHelper::CvtColorFrameToMat(BYTE* pColorBuffer, int pColorBufferPitch, Mat pImg);
	void FrameHelper::CvtDepthFrameToMat(BYTE* pDepthBuffer, int pDepthBufferPitch, Mat pImg);
	void FrameHelper::CvtDepthFrameToRGBMat(BYTE* pDepthBuffer, int pDepthBufferPitch, Mat pImg);
	void FrameHelper::DepthShortToRgb(USHORT depth, UINT8* redPixel, UINT8* greenPixel, UINT8* bluePixel);

	HRESULT FrameHelper::GetCoordinatesForSkeletonPoint(Vector4 point, LONG* pX, LONG* pY,NUI_IMAGE_RESOLUTION colorResolution, NUI_IMAGE_RESOLUTION depthResolution);
	void FrameHelper::DrawBone(Mat* pImg, NUI_SKELETON_DATA* pSkel, NUI_SKELETON_POSITION_INDEX joint0, NUI_SKELETON_POSITION_INDEX joint1, Point jointPositions[NUI_SKELETON_POSITION_COUNT], Scalar color);
	void FrameHelper::DrawSkeleton(Mat* pImg, NUI_SKELETON_DATA* pSkel, Scalar color, NUI_IMAGE_RESOLUTION colorResolution,NUI_IMAGE_RESOLUTION depthResolution);
	HRESULT FrameHelper::DrawSkeletons(Mat* pImg, NUI_SKELETON_FRAME* pSkeletons, NUI_IMAGE_RESOLUTION colorResolution,NUI_IMAGE_RESOLUTION depthResolution);
};

