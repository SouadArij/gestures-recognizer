#include "stdafx.h"
#include "FrameHelper.h"
#include <iostream>
using namespace std;

const Scalar FrameHelper::SKELETON_COLORS[NUI_SKELETON_COUNT] =
{
	Scalar(255, 0, 0),      // Blue
	Scalar(0, 255, 0),      // Green
	Scalar(64, 255, 255),   // Yellow
	Scalar(255, 255, 64),   // Light blue
	Scalar(255, 64, 255),   // Purple
	Scalar(128, 128, 255)   // Pink
};

FrameHelper::FrameHelper(void)
{
}

FrameHelper::~FrameHelper(void)
{
}

void FrameHelper::CvtColorFrameToMat(BYTE* pColorBuffer, int pColorBufferPitch, Mat pImg) {
		for (UINT y = 0; y < 480; ++y)
		{
			Vec4b* pColorRow = pImg.ptr<Vec4b>(y);

			for (UINT x = 0; x < 640; ++x)
			{
				pColorRow[x] = Vec4b(pColorBuffer[y * pColorBufferPitch + x * 4 + 0],
					pColorBuffer[y * pColorBufferPitch + x * 4 + 1],
					pColorBuffer[y * pColorBufferPitch + x * 4 + 2],
					pColorBuffer[y * pColorBufferPitch + x * 4 + 3]);
			}
		}
    }


void FrameHelper::CvtDepthFrameToMat(BYTE* pDepthBuffer, int pDepthBufferPitch, Mat pImg)
{
		USHORT* pBufferRun = reinterpret_cast<USHORT*>(pDepthBuffer);
		for (UINT y = 0; y < 480; ++y)
		{
			USHORT* pDepthRow =  pImg.ptr<USHORT>(y);
			for (UINT x = 0; x < 640; ++x)
			{
				pDepthRow[x] = pBufferRun[y * 640 + x];
			}
		}
}

void FrameHelper::CvtDepthFrameToRGBMat(BYTE* pDepthBuffer, int pDepthBufferPitch, Mat pImg)
{
		Mat depthImage;
		depthImage.create(480, 640, 2);
		CvtDepthFrameToMat(pDepthBuffer, pDepthBufferPitch, depthImage);
		for (UINT y = 0; y < 480; ++y)
		{
			// Get row pointer for depth Mat
			USHORT* pDepthRow = depthImage.ptr<USHORT>(y);
			Vec4b* pDepthRgbRow = pImg.ptr<Vec4b>(y);
			for (UINT x = 0; x < 640; ++x)
			{
				USHORT raw_depth = pDepthRow[x];
				if (raw_depth != 65535)
				{
					UINT8 redPixel, greenPixel, bluePixel;
					DepthShortToRgb(pDepthRow[x], &redPixel, &greenPixel, &bluePixel);
					pDepthRgbRow[x] = Vec4b(redPixel, greenPixel, bluePixel, 1);
				}
				else
				{
					pDepthRgbRow[x] = 0;
				}
			}
		}
}

void FrameHelper::DepthShortToRgb(USHORT depth, UINT8* redPixel, UINT8* greenPixel, UINT8* bluePixel)
        {
            SHORT realDepth = NuiDepthPixelToDepth(depth);
            USHORT playerIndex = NuiDepthPixelToPlayerIndex(depth);

            // Convert depth info into an intensity for display
            BYTE b = 255 - static_cast<BYTE>(256 * realDepth / 0x0fff);

            // Color the output based on the player index
            switch(playerIndex)
            {
            case 0:
                *redPixel = b / 2;
                *greenPixel = b / 2;
                *bluePixel = b / 2;
                break;

            case 1:
                *redPixel = b;
                *greenPixel = 0;
                *bluePixel = 0;
                break;

            case 2:
                *redPixel = 0;
                *greenPixel = b;
                *bluePixel = 0;
                break;

            case 3:
                *redPixel = b / 4;
                *greenPixel = b;
                *bluePixel = b;
                break;

            case 4:
                *redPixel = b;
                *greenPixel = b;
                *bluePixel = b / 4;
                break;

            case 5:
                *redPixel = b;
                *greenPixel = b / 4;
                *bluePixel = b;
                break;

            case 6:
                *redPixel = b / 2;
                *greenPixel = b / 2;
                *bluePixel = b;
                break;

            case 7:
                *redPixel = 255 - (b / 2);
                *greenPixel = 255 - (b / 2);
                *bluePixel = 255 - (b / 2);
                break;

            default:
                *redPixel = 0;
                *greenPixel = 0;
                *bluePixel = 0;
                break;
            }
        }

HRESULT FrameHelper::DrawSkeletons(Mat* pImg, NUI_SKELETON_FRAME* pSkeletons, NUI_IMAGE_RESOLUTION colorResolution, NUI_IMAGE_RESOLUTION depthResolution)
{
    // Fail if either pointer is invalid
    if (!pImg || !pSkeletons) 
    {
        return E_POINTER;
    }

    // Fail if Mat contains no data or has insufficient channels or if depth resolution is invalid
    if (pImg->empty() || pImg->channels() < 3 || depthResolution == NUI_IMAGE_RESOLUTION_INVALID)
    {
        return E_INVALIDARG;
    }

    // Draw each tracked skeleton
    for (int i=0; i < NUI_SKELETON_COUNT; ++i)
    {
        NUI_SKELETON_TRACKING_STATE trackingState = pSkeletons->SkeletonData[i].eTrackingState;
        if (trackingState == NUI_SKELETON_TRACKED)
        {
            // Draw entire skeleton
            NUI_SKELETON_DATA *pSkel = &(pSkeletons->SkeletonData[i]);
            DrawSkeleton(pImg, pSkel, SKELETON_COLORS[i], colorResolution, depthResolution);
        } 
        else if (trackingState == NUI_SKELETON_POSITION_INFERRED) 
        {
            // Draw a filled circle at the skeleton's inferred position
            LONG x, y;
            GetCoordinatesForSkeletonPoint(pSkeletons->SkeletonData[i].Position, &x, &y, colorResolution, depthResolution);
            circle(*pImg, Point(x, y), 7, SKELETON_COLORS[i], CV_FILLED);
        }
    }

    return S_OK;
}

void FrameHelper::DrawSkeleton(Mat* pImg, NUI_SKELETON_DATA* pSkel, Scalar color, NUI_IMAGE_RESOLUTION colorResolution, NUI_IMAGE_RESOLUTION depthResolution)
{
    // Convert joint positions into the coordinates for this resolution and view
   // Point jointPositions[NUI_SKELETON_POSITION_COUNT];

    for (int j = 0; j < NUI_SKELETON_POSITION_COUNT; ++j)
    {
        LONG x, y;
        GetCoordinatesForSkeletonPoint(pSkel->SkeletonPositions[j], &x, &y, colorResolution, depthResolution);
        jointPositions[j] = Point(x, y);
    }
	

    // Draw torso
    DrawBone(pImg, pSkel, NUI_SKELETON_POSITION_HEAD, NUI_SKELETON_POSITION_SHOULDER_CENTER, jointPositions, color);
    DrawBone(pImg, pSkel, NUI_SKELETON_POSITION_SHOULDER_CENTER, NUI_SKELETON_POSITION_SHOULDER_LEFT, jointPositions, color);
    DrawBone(pImg, pSkel, NUI_SKELETON_POSITION_SHOULDER_CENTER, NUI_SKELETON_POSITION_SHOULDER_RIGHT, jointPositions, color);
    DrawBone(pImg, pSkel, NUI_SKELETON_POSITION_SHOULDER_CENTER, NUI_SKELETON_POSITION_SPINE, jointPositions, color);
    DrawBone(pImg, pSkel, NUI_SKELETON_POSITION_SPINE, NUI_SKELETON_POSITION_HIP_CENTER, jointPositions, color);
    DrawBone(pImg, pSkel, NUI_SKELETON_POSITION_HIP_CENTER, NUI_SKELETON_POSITION_HIP_LEFT, jointPositions, color);
    DrawBone(pImg, pSkel, NUI_SKELETON_POSITION_HIP_CENTER, NUI_SKELETON_POSITION_HIP_RIGHT, jointPositions, color);

    // Draw left arm
    DrawBone(pImg, pSkel, NUI_SKELETON_POSITION_SHOULDER_LEFT, NUI_SKELETON_POSITION_ELBOW_LEFT, jointPositions, color);
    DrawBone(pImg, pSkel, NUI_SKELETON_POSITION_ELBOW_LEFT, NUI_SKELETON_POSITION_WRIST_LEFT, jointPositions, color);
    DrawBone(pImg, pSkel, NUI_SKELETON_POSITION_WRIST_LEFT, NUI_SKELETON_POSITION_HAND_LEFT, jointPositions, color);

    // Draw right arm
    DrawBone(pImg, pSkel, NUI_SKELETON_POSITION_SHOULDER_RIGHT, NUI_SKELETON_POSITION_ELBOW_RIGHT, jointPositions, color);
    DrawBone(pImg, pSkel, NUI_SKELETON_POSITION_ELBOW_RIGHT, NUI_SKELETON_POSITION_WRIST_RIGHT, jointPositions, color);
    DrawBone(pImg, pSkel, NUI_SKELETON_POSITION_WRIST_RIGHT, NUI_SKELETON_POSITION_HAND_RIGHT, jointPositions, color);

    // Draw left leg
    DrawBone(pImg, pSkel, NUI_SKELETON_POSITION_HIP_LEFT, NUI_SKELETON_POSITION_KNEE_LEFT, jointPositions, color);
    DrawBone(pImg, pSkel, NUI_SKELETON_POSITION_KNEE_LEFT, NUI_SKELETON_POSITION_ANKLE_LEFT, jointPositions, color);
    DrawBone(pImg, pSkel, NUI_SKELETON_POSITION_ANKLE_LEFT, NUI_SKELETON_POSITION_FOOT_LEFT, jointPositions, color);

    // Draw right leg
    DrawBone(pImg, pSkel, NUI_SKELETON_POSITION_HIP_RIGHT, NUI_SKELETON_POSITION_KNEE_RIGHT, jointPositions, color);
    DrawBone(pImg, pSkel, NUI_SKELETON_POSITION_KNEE_RIGHT, NUI_SKELETON_POSITION_ANKLE_RIGHT, jointPositions, color);
    DrawBone(pImg, pSkel, NUI_SKELETON_POSITION_ANKLE_RIGHT, NUI_SKELETON_POSITION_FOOT_RIGHT, jointPositions, color);

    // Draw joints on top of bones
    for (int j = 0; j < NUI_SKELETON_POSITION_COUNT; ++j)
    {
        // Draw a colored circle with a black border for tracked joints
        if (pSkel->eSkeletonPositionTrackingState[j] == NUI_SKELETON_POSITION_TRACKED) 
        {
            circle(*pImg, jointPositions[j], 5, color, CV_FILLED);
            circle(*pImg, jointPositions[j], 6, Scalar(0, 0, 0), 1);
        } 
        // Draw a white, unfilled circle for inferred joints
        else if (pSkel->eSkeletonPositionTrackingState[j] == NUI_SKELETON_POSITION_INFERRED) 
        {
            circle(*pImg, jointPositions[j], 4, Scalar(255,255,255), 2);
        }
    }
}

void FrameHelper::DrawBone(Mat* pImg, NUI_SKELETON_DATA* pSkel, NUI_SKELETON_POSITION_INDEX joint0, NUI_SKELETON_POSITION_INDEX joint1, Point jointPositions[NUI_SKELETON_POSITION_COUNT], Scalar color)
{
    NUI_SKELETON_POSITION_TRACKING_STATE joint0state = pSkel->eSkeletonPositionTrackingState[joint0];
    NUI_SKELETON_POSITION_TRACKING_STATE joint1state = pSkel->eSkeletonPositionTrackingState[joint1];

    // Don't draw unless at least one joint is tracked
    if (joint0state == NUI_SKELETON_POSITION_NOT_TRACKED || joint1state == NUI_SKELETON_POSITION_NOT_TRACKED) 
    {
        return;
    }

    if (joint0state == NUI_SKELETON_POSITION_INFERRED && joint1state == NUI_SKELETON_POSITION_INFERRED) 
    {
        return;
    }

    // If both joints are tracked, draw a colored line
    if (joint0state == NUI_SKELETON_POSITION_TRACKED && joint1state == NUI_SKELETON_POSITION_TRACKED) 
    {
        line(*pImg, jointPositions[joint0], jointPositions[joint1], color, 2);
    } 
    // If only one joint is tracked, draw a thinner white line
    else 
    {
        line(*pImg, jointPositions[joint0], jointPositions[joint1], Scalar(255,255,255), 1);
    }
}

HRESULT FrameHelper::GetCoordinatesForSkeletonPoint(Vector4 point, LONG* pX, LONG* pY,NUI_IMAGE_RESOLUTION colorResolution, NUI_IMAGE_RESOLUTION depthResolution)
{
    // Fail if either pointer is invalid
    if (!pX || !pY) 
    {
        return E_POINTER;
    }

    // Convert the point from skeleton space to depth space
    LONG depthX, depthY;
    USHORT depth;
    NuiTransformSkeletonToDepthImage(point, &depthX, &depthY, &depth, depthResolution);

    // If the color resolution is invalid, return these coordinates
    if (colorResolution == NUI_IMAGE_RESOLUTION_INVALID) 
    {
        *pX = depthX;
        *pY = depthY;
    } 
    // Otherwise, convert the point from depth space to color space
    else 
    {
        HRESULT hr = NuiImageGetColorPixelCoordinatesFromDepthPixelAtResolution(colorResolution, depthResolution, NULL, depthX, depthY, depth, pX, pY);
        if (FAILED(hr))
        {
            return hr;
        }
    }

    return S_OK;
}