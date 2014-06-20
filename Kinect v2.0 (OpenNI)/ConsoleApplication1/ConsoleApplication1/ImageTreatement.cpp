#include "stdafx.h"
#include "ImageTreatement.h"


ImageTreatement::ImageTreatement(void)
{
}

ImageTreatement::~ImageTreatement(void)
{
}

bool ImageTreatement::isInROI(int x, int y, Rec roi)
{
	return (x > roi.x1 && x < roi.x2 && y > roi.y1 && y < roi.y2);
}

Rec ImageTreatement::getHandROI(nite::UserTracker* pUserTracker, const nite::Array<nite::UserData>& users, nite::JointType joint)
{
	Rec roi = {0, 0, 0, 0, 0, 0};
	float handX = 0;
	float handY = 0;
	float current = 0;
	for (int i = 0; i < users.getSize(); ++i)
	{
		const nite::UserData& user = users[i];
		if (user.isNew())
		{
			pUserTracker->startSkeletonTracking(user.getId());
		}
		else if (!user.isLost())
		{
			if (users[i].getSkeleton().getState() == nite::SKELETON_TRACKED)
			{
				if ( users[i].getSkeleton().getJoint(joint).getPositionConfidence() > 0.5)
				{
					pUserTracker->convertJointCoordinatesToDepth(users[i].getSkeleton().getJoint(joint).getPosition().x, 
						users[i].getSkeleton().getJoint(joint).getPosition().y, 
						users[i].getSkeleton().getJoint(joint).getPosition().z, 
						&handX, &handY);
					if( (int)handY < captureHeight && (int)handY > 0 && (int)handX < captureWidth && (int)handX > 0)
					{
						current = (int)handY*captureWidth + (int)handX;
						if (current < 0 ) current = 0;
						roi.handPos = current;
						roi.x2 = min( (int)handX + roiWidth/2, captureWidth);
						roi.x1 = max( (int)handX - roiWidth/2, 0);
						roi.y1 = max( (int)handY - roiHeight/2, 0);
						roi.y2 = min( (int)handY + roiHeight/2, captureHeight);

						roi.width = roi.x2 - roi.x1;
						roi.height = roi.y2 - roi.y1 ;								
					}
				}
			}
		}
	}
	return roi;
}

void ImageTreatement::calculateHistogram(float* pHistogram, int histogramSize, const openni::VideoFrameRef& frame)
{
	const openni::DepthPixel* pDepth = (const openni::DepthPixel*)frame.getData();
	// Calculate the accumulative histogram (the yellow display...)
	memset(pHistogram, 0, histogramSize*sizeof(float));
	int restOfRow = frame.getStrideInBytes() / sizeof(openni::DepthPixel) - frame.getWidth();
	int height = frame.getHeight();
	int width = frame.getWidth();

	unsigned int nNumberOfPoints = 0;
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x, ++pDepth)
		{
			if (*pDepth != 0)
			{
				pHistogram[*pDepth]++;
				nNumberOfPoints++;
			}
		}
		pDepth += restOfRow;
	}
	for (int nIndex=1; nIndex<histogramSize; nIndex++)
	{
		pHistogram[nIndex] += pHistogram[nIndex-1];
	}
	if (nNumberOfPoints)
	{
		for (int nIndex=1; nIndex<histogramSize; nIndex++)
		{
			pHistogram[nIndex] = (256 * (1.0f - (pHistogram[nIndex] / nNumberOfPoints)));
		}
	}
}

void ImageTreatement::DrawLimb(cv::Mat pDepth, const nite::UserTracker* pUserTracker, const nite::SkeletonJoint& joint1, const nite::SkeletonJoint& joint2)
{
	float coordinates[4] = {0};
	pUserTracker->convertJointCoordinatesToDepth(joint1.getPosition().x, joint1.getPosition().y, joint1.getPosition().z, &coordinates[0], &coordinates[1]);
	pUserTracker->convertJointCoordinatesToDepth(joint2.getPosition().x, joint2.getPosition().y, joint2.getPosition().z, &coordinates[2], &coordinates[3]);


	if (joint1.getPositionConfidence() >= 0.5 && joint2.getPositionConfidence() >= 0.5)
	{
		line(pDepth, cv::Point(coordinates[0], coordinates[1]), cv::Point(coordinates[2], coordinates[3]), cv::Scalar(16,52,255));
	}
}

void ImageTreatement::DrawSkeleton(cv::Mat pDepth, const nite::UserTracker* pUserTracker, const nite::UserData& userData)
{
	DrawLimb(pDepth, pUserTracker, userData.getSkeleton().getJoint(nite::JOINT_HEAD), userData.getSkeleton().getJoint(nite::JOINT_NECK));

	DrawLimb(pDepth,pUserTracker, userData.getSkeleton().getJoint(nite::JOINT_LEFT_SHOULDER), userData.getSkeleton().getJoint(nite::JOINT_LEFT_ELBOW));
	DrawLimb(pDepth,pUserTracker, userData.getSkeleton().getJoint(nite::JOINT_LEFT_ELBOW), userData.getSkeleton().getJoint(nite::JOINT_LEFT_HAND));

	DrawLimb(pDepth,pUserTracker, userData.getSkeleton().getJoint(nite::JOINT_RIGHT_SHOULDER), userData.getSkeleton().getJoint(nite::JOINT_RIGHT_ELBOW));
	DrawLimb(pDepth,pUserTracker, userData.getSkeleton().getJoint(nite::JOINT_RIGHT_ELBOW), userData.getSkeleton().getJoint(nite::JOINT_RIGHT_HAND));

	DrawLimb(pDepth,pUserTracker, userData.getSkeleton().getJoint(nite::JOINT_LEFT_SHOULDER), userData.getSkeleton().getJoint(nite::JOINT_RIGHT_SHOULDER));

	DrawLimb(pDepth,pUserTracker, userData.getSkeleton().getJoint(nite::JOINT_LEFT_SHOULDER), userData.getSkeleton().getJoint(nite::JOINT_TORSO));
	DrawLimb(pDepth,pUserTracker, userData.getSkeleton().getJoint(nite::JOINT_RIGHT_SHOULDER), userData.getSkeleton().getJoint(nite::JOINT_TORSO));

	DrawLimb(pDepth,pUserTracker, userData.getSkeleton().getJoint(nite::JOINT_TORSO), userData.getSkeleton().getJoint(nite::JOINT_LEFT_HIP));
	DrawLimb(pDepth,pUserTracker, userData.getSkeleton().getJoint(nite::JOINT_TORSO), userData.getSkeleton().getJoint(nite::JOINT_RIGHT_HIP));

	DrawLimb(pDepth,pUserTracker, userData.getSkeleton().getJoint(nite::JOINT_LEFT_HIP), userData.getSkeleton().getJoint(nite::JOINT_RIGHT_HIP));


	DrawLimb(pDepth,pUserTracker, userData.getSkeleton().getJoint(nite::JOINT_LEFT_HIP), userData.getSkeleton().getJoint(nite::JOINT_LEFT_KNEE));
	DrawLimb(pDepth,pUserTracker, userData.getSkeleton().getJoint(nite::JOINT_LEFT_KNEE), userData.getSkeleton().getJoint(nite::JOINT_LEFT_FOOT));

	DrawLimb(pDepth,pUserTracker, userData.getSkeleton().getJoint(nite::JOINT_RIGHT_HIP), userData.getSkeleton().getJoint(nite::JOINT_RIGHT_KNEE));
	DrawLimb(pDepth,pUserTracker, userData.getSkeleton().getJoint(nite::JOINT_RIGHT_KNEE), userData.getSkeleton().getJoint(nite::JOINT_RIGHT_FOOT));
}

void ImageTreatement::DrawSkeletons(cv::Mat pDepth, nite::UserTracker* pUserTracker, const nite::Array<nite::UserData>& users)
{
	for (int i = 0; i < users.getSize(); ++i)
			{
				const nite::UserData& user = users[i];

				if (user.isNew())
				{
					pUserTracker->startSkeletonTracking(user.getId());
				}
				else if (!user.isLost())
				{
					if (users[i].getSkeleton().getState() == nite::SKELETON_TRACKED)
					{
						DrawSkeleton(pDepth,pUserTracker, user);
					}
				}
			}
}

HRESULT ImageTreatement::GetDepthMat(cv::Mat* pImg, const openni::VideoFrameRef& pVfrFrame, const float* pHistogram)
{
	int height = pVfrFrame.getHeight();
	int width = pVfrFrame.getWidth();

	openni::DepthPixel* pDepthRow = new openni::DepthPixel[height * width];
	openni::DepthPixel* pDepthRowTemp = pDepthRow;
	openni::RGB888Pixel* pDepthRowTex = new openni::RGB888Pixel[height * width];
	openni::RGB888Pixel* pDepthRowTexTemp = pDepthRowTex;

	memset(pDepthRowTex, 0, 3* height * width * sizeof( uint8_t ) );
	memcpy(pDepthRow, pVfrFrame.getData(), height * width * sizeof(uint16_t));

	for (int y = 0; y < height; ++y)
	{
		for ( int x = 0; x < width; ++x, ++pDepthRow, ++pDepthRowTex)
		{
				if(*pDepthRow > 0)
				{
					int nHistValue = pHistogram[*pDepthRow];
					pDepthRowTex->r = nHistValue;
					pDepthRowTex->g = nHistValue;
					pDepthRowTex->b = 0;
				}							
		}
	}				
	*pImg = cv::Mat(height, width, CV_8UC3);
	memcpy(pImg->data, pDepthRowTexTemp, 3* height * width * sizeof( uint8_t ));
	delete pDepthRowTexTemp;
	delete pDepthRowTemp;
	return S_OK;
}

HRESULT ImageTreatement::GetColorMat(cv::Mat* pImg, const openni::VideoFrameRef& pVfrFrame)
{
	int height = pVfrFrame.getHeight();
	int width = pVfrFrame.getWidth();

	*pImg = cv::Mat(height, width, CV_8UC3);
	memcpy(pImg->data, pVfrFrame.getData(), 3 * height * width * sizeof(uint8_t));	
	cvtColor(*pImg, *pImg, CV_BGR2RGB);	
	return S_OK;
}

void ImageTreatement::DrawLimbFromSample(cv::Mat pMat, nite::UserTracker* pUserTracker, cv::Point3d joint1, cv::Point3d joint2)
{
	float coordinates[4] = {0};
	pUserTracker->convertJointCoordinatesToDepth(joint1.x, joint1.y, joint1.z, &coordinates[0], &coordinates[1]);
	pUserTracker->convertJointCoordinatesToDepth(joint2.x, joint2.y, joint2.z, &coordinates[2], &coordinates[3]);

	line(pMat, cv::Point(coordinates[0], coordinates[1]), cv::Point(coordinates[2], coordinates[3]), cv::Scalar(16,52,255));
}

void ImageTreatement::DrawSkeletonFromSample( cv::Mat pMat, const std::vector<cv::Point3d>& recordedSkeleton , nite::UserTracker* pUserTracker)
{
	DrawLimbFromSample(pMat, pUserTracker, recordedSkeleton[nite::JOINT_HEAD], recordedSkeleton[nite::JOINT_NECK]);

	DrawLimbFromSample(pMat, pUserTracker, recordedSkeleton [nite::JOINT_LEFT_SHOULDER], recordedSkeleton [nite::JOINT_LEFT_ELBOW]);
	DrawLimbFromSample(pMat, pUserTracker, recordedSkeleton [nite::JOINT_LEFT_ELBOW], recordedSkeleton [nite::JOINT_LEFT_HAND]);

	DrawLimbFromSample(pMat, pUserTracker, recordedSkeleton [nite::JOINT_RIGHT_SHOULDER], recordedSkeleton [nite::JOINT_RIGHT_ELBOW]);
	DrawLimbFromSample(pMat, pUserTracker, recordedSkeleton [nite::JOINT_RIGHT_ELBOW], recordedSkeleton [nite::JOINT_RIGHT_HAND]);

	DrawLimbFromSample(pMat, pUserTracker, recordedSkeleton [nite::JOINT_LEFT_SHOULDER], recordedSkeleton [nite::JOINT_RIGHT_SHOULDER]);

	DrawLimbFromSample(pMat, pUserTracker, recordedSkeleton [nite::JOINT_LEFT_SHOULDER], recordedSkeleton [nite::JOINT_TORSO]);
	DrawLimbFromSample(pMat, pUserTracker, recordedSkeleton [nite::JOINT_RIGHT_SHOULDER], recordedSkeleton [nite::JOINT_TORSO]);

	DrawLimbFromSample(pMat, pUserTracker, recordedSkeleton [nite::JOINT_TORSO], recordedSkeleton [nite::JOINT_LEFT_HIP]);
	DrawLimbFromSample(pMat, pUserTracker, recordedSkeleton [nite::JOINT_TORSO], recordedSkeleton [nite::JOINT_RIGHT_HIP]);

	DrawLimbFromSample(pMat, pUserTracker, recordedSkeleton [nite::JOINT_LEFT_HIP], recordedSkeleton [nite::JOINT_RIGHT_HIP]);
}

HRESULT ImageTreatement::ExtractFeaturesFromHand( cv::Mat pMatIN, cv::Mat pMatOUT, std::vector<double>& pSampleVec )
{
	std::vector< std::vector<cv::Point> > contours;
	std::vector<cv::Vec4i> hierarchy;
	std::vector<int> hull;
	double largestArea = 0;
	int largestContourIndex = 0;

	cv::Mat depthTempbw;
	pMatIN.copyTo(depthTempbw);
	//cvtColor(pMatIN, depthTempbw, CV_RGB2GRAY);
	findContours(depthTempbw, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);
	depthTempbw.release();

	if( contours.size() != 0)
	{
		for(UINT i = 0; i < contours.size(); i++)
		{
			double a = contourArea( contours[i], false);
			if(a > largestArea)
			{
				largestArea = a;
				largestContourIndex = i;
			}
		}
				
		pSampleVec.push_back(largestArea);

		std::vector< std::vector< cv::Point>> contoursTemp;
		contoursTemp.push_back(contours[largestContourIndex]);

		approxPolyDP( contours[largestContourIndex],  contoursTemp[0], arcLength(contours[largestContourIndex],true)*0.0025,  true );
		drawContours(pMatOUT, contoursTemp, 0, cv::Scalar(0, 154, 255), 1, CV_AA);

		////Find and draw rotated bounding rectangle
		//cv::RotatedRect rotRect = minAreaRect(contoursTemp[0]);
		//cv::Point2f rotRectPoints[4];
		//rotRect.points(rotRectPoints);
		//for( int j = 0; j < 4; j++ )
		//	line( pMatOUT, rotRectPoints[j], rotRectPoints[(j+1)%4], cv::Scalar(165,33,200), 1, 8 );

		cv::Rect BoundRect = boundingRect( contoursTemp[0] );

		pSampleVec.push_back(BoundRect.width);
		pSampleVec.push_back(BoundRect.height);
		//Find and draw max inscribed circle in biggest contour
		cv::Point centerInscribedCircle;
		double dist, maxdist = -1;
		for(int i = BoundRect.x; i< BoundRect.x + BoundRect.width;i++)
		{
			for(int j = BoundRect.y; j< BoundRect.y + BoundRect.height;j++)
			{
				dist = pointPolygonTest(contoursTemp[0], cv::Point(i,j),true);
				if(dist > maxdist)
				{
					maxdist = dist;
					centerInscribedCircle = cv::Point(i,j);
				}
			}
		}
		circle(pMatOUT, centerInscribedCircle, (int)maxdist, cv::Scalar(0, 255, 255), CV_FILLED);

		pSampleVec.push_back(maxdist);

		//Find and draw Convex Hull of biggest contour
		convexHull(contoursTemp[0], hull, true);
		int hullcount = (int)hull.size();
		cv::Point pt0 = contoursTemp[0][hull[hullcount-1]];
		int number = 0;
		for(int i = 0; i < hullcount; i++ )
		{
			cv::Point pt = contoursTemp[0][hull[i]];
			line(pMatOUT, pt0, pt, cv::Scalar(0, 255, 0), 2, CV_AA);
			if(sqrt(pow(pt.x - pt0.x, 2) + pow(pt.y - pt0.y, 2)) > 15 && pt.y < centerInscribedCircle.y + maxdist)
			{
				circle(pMatOUT, pt, 5, cv::Scalar(0,0,255), CV_FILLED);
				number++;
			}
			pt0 = pt;
		}
		pSampleVec.push_back(number);
		//Find and draw Convexity Defect
		std::vector<cv::Vec4i> defect;
		cv::Point p;
		int count = 0;
		convexityDefects(contoursTemp[0], hull, defect);

		

		int depthMax = 0;
		for (UINT i = 0; i < defect.size(); i++)
		{
			int startIdx = defect[i].val[0];
			int endIdx = defect[i].val[1];
			int defectPtIdx = defect[i].val[2];
			double depth = (double)defect[i].val[3]/256.0f;
			if(depth > 10 )
			{
				if(depth > depthMax) 
					depthMax = depth;
				p = contoursTemp[0][defectPtIdx];
				circle(pMatOUT, p, 5, cv::Scalar(0,0,255));
				count++ ;
			}
		}
		pSampleVec.push_back(count);

		pSampleVec.push_back(depthMax);
		return S_OK;
	}
	else
	{
		return E_FAIL;
	}
}

HRESULT ImageTreatement::GetRealHand( cv::Mat pMatDepth, cv::Mat pMatColor, cv::Mat* pOut )
{
	std::vector< std::vector<cv::Point> > contours;
	std::vector<cv::Vec4i> hierarchy;
	std::vector<int> hull;
	double largestArea = 0;
	int largestContourIndex = 0;
	GaussianBlur(pMatDepth, pMatDepth, cv::Size(3,3), 1);

	cv::Mat depthTempbw;
	cvtColor(pMatDepth, depthTempbw, CV_RGB2GRAY);
	findContours(depthTempbw, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);
	depthTempbw.release();

	if( contours.size() != 0)
	{
		for(UINT i = 0; i < contours.size(); i++)
		{
			double a = contourArea( contours[i], false);
			if(a > largestArea)
			{
				largestArea = a;
				largestContourIndex = i;
			}
		}
				
		std::vector< std::vector< cv::Point>> contoursTemp;
		contoursTemp.push_back(contours[largestContourIndex]);

		cv::Rect BoundRect = boundingRect( contoursTemp[0] );
		//Find and draw max inscribed circle in biggest contour
		cv::Point centerInscribedCircle;
		double dist, maxdist = -1;
		for(int i = BoundRect.x; i< BoundRect.x + BoundRect.width;i++)
		{
			for(int j = BoundRect.y; j< BoundRect.y + BoundRect.height;j++)
			{
				dist = pointPolygonTest(contoursTemp[0], cv::Point(i,j),true);
				if(dist > maxdist)
				{
					maxdist = dist;
					centerInscribedCircle = cv::Point(i,j);
				}
			}
		}

		uchar* data = pMatColor.data;
		for(int i = 0; i < pMatColor.rows; i++)
		{
			for(int j = 0; j < pMatColor.cols*3; j++, data++)
			{
				if(i > (centerInscribedCircle.y + maxdist + 10) && *data > 0)
				{
					*data = 0;
				}
			}
		}

		cv::Mat pMatColorTemp;
		pMatColor.copyTo(pMatColorTemp);
		cv::Mat toHist = pMatColorTemp(cv::Rect(centerInscribedCircle.x - maxdist/2, centerInscribedCircle.y - maxdist/2,  maxdist, maxdist));
		imshow("toshit", toHist);
		cvtColor(toHist, toHist, CV_RGB2YCrCb);
		std::vector<cv::Mat> yCrCb_planes;
		split( toHist, yCrCb_planes );
		cv::Mat y_hist, Cr_hist, Cb_hist;
		int histsize = 256;
		float range[] = { 0, 256 } ;
		const float* histRange = { range };
		calcHist(&yCrCb_planes[0], 1, 0, cv::Mat(), y_hist, 1, &histsize, &histRange);
		calcHist(&yCrCb_planes[1], 1, 0, cv::Mat(), Cr_hist, 1, &histsize, &histRange);
		calcHist(&yCrCb_planes[2], 1, 0, cv::Mat(), Cb_hist, 1, &histsize, &histRange);

		bool boolTab[] = {false, false, false, false, false, false };
		int tresholds[] = {0, 0, 0, 0, 0, 0};
		int tresh = 50;
		int bin_w = cvRound( (double) 512/histsize );
		for( int i = 1; i < histsize; i++ )
		{
			if(y_hist.at<float>(i) > tresh && !boolTab[0]) {tresholds[0] = i; boolTab[0]=true;}
			if(y_hist.at<float>(i) < tresh && boolTab[0] && !boolTab[1]) {tresholds[1] = i; boolTab[1]=true;}

			if(Cr_hist.at<float>(i) > tresh && !boolTab[2]) {tresholds[2] = i; boolTab[2]=true;}
			if(Cr_hist.at<float>(i) < tresh && boolTab[2] && !boolTab[3]) {tresholds[3] = i; boolTab[3]=true;}

			if(Cb_hist.at<float>(i) > tresh && !boolTab[4]) {tresholds[4] = i; boolTab[4]=true;}
			if(Cb_hist.at<float>(i) < tresh && boolTab[4] && !boolTab[5]) {tresholds[5] = i; boolTab[5]=true;}					
		}

		cvtColor(pMatColor, pMatColor, CV_RGB2YCrCb);
		inRange(pMatColor,cv::Scalar(20,tresholds[2]-10,tresholds[4]-10),cv::Scalar(240,tresholds[3]+10,tresholds[5]+10), *pOut);
		int dilation_size = 1;
		cv::Mat element = cv::getStructuringElement( cv::MORPH_RECT,
                            cv::Size( 2*dilation_size + 1, 2*dilation_size+1 ),
                            cv::Point( dilation_size, dilation_size ) );
		dilate(*pOut, *pOut, element);
		erode(*pOut, *pOut, element);
		GaussianBlur(*pOut, *pOut, cv::Size(3,3), 1);
		imshow("out", *pOut);
		return S_OK;
	}
	else
	{
		return E_FAIL;
	}
}