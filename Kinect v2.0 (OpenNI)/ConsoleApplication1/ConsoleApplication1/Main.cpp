#include "stdafx.h"
#include "Main.h"

Main::Main():m_bDepthTreatementIsFinish(true),
	m_sliderDebut(0),
	m_sliderFin(150),
	m_sliderMax(150),
	m_bDisplayAnimation(true),
	m_bRecording(false),
	m_bContinueProcessing(true),
	m_recordingClasse(-1),
	m_recordingHandClasse(-1),
	m_recordingHandName(" "),
	m_recordingName(" "),
	m_recordingType(" ")
	
{
	m_kinect = new Kinect();
	m_gesturesAnalyzer = new GesturesAnalyzer();
	m_featuresRecorder = new FeaturesRecorder();
	m_hFeaturesRecorderMutex = CreateMutex(NULL, FALSE, NULL);
	m_hGestureRecorderMutex = CreateMutex(NULL, FALSE, NULL);
	m_hKinectMutex = CreateMutex(NULL, FALSE, NULL);
	m_hRecordingMutex = CreateMutex(NULL, FALSE, NULL);
	m_hContinueProcessingMutex = CreateMutex(NULL, FALSE, NULL);
	m_hDisplayAnimationMutex = CreateMutex(NULL, FALSE, NULL);
	m_hSaveHandEvent = CreateEvent(NULL, false, false, NULL);
}

Main::~Main()
{
	CloseHandle(m_hGestureRecorderMutex);
	CloseHandle(m_hKinectMutex);
	CloseHandle(m_hRecordingMutex);
	CloseHandle(m_hContinueProcessingMutex);
	CloseHandle(m_hDisplayAnimationMutex);
}

int _tmain(int argc, _TCHAR* argv[])
{
	
	Main mainApp = Main();
	mainApp.Run();
	//mainApp.SaveCode();
	cv::waitKey(5000);
}

HRESULT Main::Run()
{
	//Get system info
	SYSTEM_INFO siSysInfo;
	GetSystemInfo(&siSysInfo);
	printf("Number of processor : %u  Active Processor Mask : %u\n", siSysInfo.dwNumberOfProcessors, siSysInfo.dwActiveProcessorMask);

	//Init Gesture Recorder
	if( FAILED(m_gesturesAnalyzer->Init()))
	{
		printf("Gesture Recorder initialization failed");
		return E_FAIL;
	}

	//Start threads
	m_hDepthTreatementIsFinishMutex = CreateMutex(NULL, FALSE, NULL);
	m_hDepthTreatement = CreateEvent( NULL, FALSE, FALSE, NULL);
	m_hColorTreatement = CreateEvent( NULL, FALSE, FALSE, NULL);

	HANDLE ThreadHandles[10];
	int ThreadCount = 0;
	if( SUCCEEDED(m_kinect->Initialize()) )
	{
		ThreadHandles[0] = CreateThread(NULL, 0, ThreadAffichage, this, 0, NULL);
		ThreadCount++;
		ThreadHandles[1]= CreateThread(NULL, 0, ThreadGetAndDisplayNewDepthFrame, this, 0, NULL);
		ThreadCount++;
		ThreadHandles[2] = CreateThread(NULL, 0, ThreadGetAndDisplayNewColorFrame, this, 0, NULL);
		ThreadCount++;
		ThreadHandles[3] = CreateThread(NULL, 0, KeyboardListener, this, 0, NULL);
		WaitForMultipleObjects(ThreadCount, ThreadHandles, false, INFINITE);
	}	
	return S_OK;
}

void on_trackbar_debut( int, void* )
{
	
}

void on_trackbar_fin( int, void* )
{

}

void Main::SaveCode()
{
	//std::string str = "HELLO";
	//printf("%s\n", str.c_str());
	//str.erase(str.size()-1,1);
	//printf("%s\n", str.c_str());
	
	
	//WRITE ON MAT
			//		std::ostringstream str;
			//		str << count;
			//		putText(depth, str.str(), Point(10,100),FONT_HERSHEY_COMPLEX_SMALL,10,Scalar(255,102,0)); 


	//HAND TRACKING USE
			///*const nite::Array<nite::GestureData>& gestures = m_handFrame.getGestures();
			//for (int i = 0; i < gestures.getSize(); ++i)
			//{
			//	if (gestures[i].isComplete())
			//	{
			//		const nite::Point3f& position = gestures[i].getCurrentPosition();
			//		printf("Gesture %d at (%f,%f,%f)\n", gestures[i].getType(), position.x, position.y, position.z);

			//		nite::HandId newId;
			//		m_pHandTracker->startHandTracking(gestures[i].getCurrentPosition(), &newId);
			//	}
			//}

			//const nite::Array<nite::HandData>& hands= m_handFrame.getHands();
			//for (int i = 0; i < hands.getSize(); ++i)
			//{
			//	const nite::HandData& user = hands[i];

			//	if (!user.isTracking())
			//	{
			//		printf("Lost hand %d\n", user.getId());
			//		nite::HandId id = user.getId();
			//	}
			//	else
			//	{
			//		if (user.isNew())
			//		{
			//			printf("Found hand %d\n", user.getId());
			//		}
			//		float Yo, Xo;
			//		m_pHandTracker->convertHandCoordinatesToDepth(user.getPosition().x, user.getPosition().y, user.getPosition().z, &Xo, &Yo);
			//		circle(depth, Point(Xo, Yo), 10, Scalar(255,165,2), CV_FILLED);
			//	}
			//}*/
			//	
			//imshow("test", depth);
			//waitKey(1);


	//TEST AFFINER DETECTION MAIN
				vector< vector<cv::Point> > contours;
				vector<cv::Vec4i> hierarchy;
				vector<int> hull;
				double largestArea =0;
				int largestContourIndex=0;
				cv::Mat rightHand;
				rightHand = cv::imread("handtest.png");
				
				//Create GRAYSCALE frame
				cv::Mat rightHandbw;
				cv::cvtColor(rightHand, rightHandbw, CV_RGB2GRAY);

				//Find biggest contour
				findContours(rightHandbw, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);
				if(contours.size() != 0)
				{
					for(int i = 0; i < contours.size(); i++)
					{
						double a = contourArea( contours[i], false);
						if(a > largestArea)
						{
							largestArea = a;
							largestContourIndex = i;
						}
					}

					//Convert GRAYSCLAE to RGB frame
					cvtColor(rightHandbw, rightHandbw, CV_GRAY2RGB);

					//Find and draw approx polygon of biggest contour
					vector<vector<cv::Point>> contoursTemp;
					contoursTemp.push_back(contours[largestContourIndex]);
					approxPolyDP( contours[largestContourIndex], contoursTemp[0], arcLength(contours[largestContourIndex],true)*0.0025,  true );
					//Find and draw max inscribed circle in biggest contour
					cv::Point centerInscribedCircle;
					double dist, maxdist = -1;
					for(int i = 0;i< rightHandbw.cols;i++)
					{
						for(int j = 0;j< rightHandbw.rows;j++)
						{

							dist = pointPolygonTest(contoursTemp[0], cv::Point(i,j),true);
							if(dist > maxdist)
							{
								maxdist = dist;
								centerInscribedCircle = cv::Point(i,j);
							}
						}
					}
					circle(rightHandbw, centerInscribedCircle, maxdist, cv::Scalar(0, 255, 255), CV_FILLED);
					int maxY = centerInscribedCircle.y + maxdist;

					for(int i = 0; i < contoursTemp[0].size() ; i++)
					{
						if( contoursTemp[0][i].y > maxY )
						{
							contoursTemp[0].erase(contoursTemp[0].begin()+i);
						}
					}

					drawContours(rightHandbw, contoursTemp, 0, cv::Scalar(0, 154, 255), 1, CV_AA);

					//Find and draw rotated bounding rectangle
					cv::RotatedRect rotRect = minAreaRect(contoursTemp[0]);
					cv::Point2f rotRectPoints[4];
					rotRect.points(rotRectPoints);
					for( int j = 0; j < 4; j++ )
						line( rightHandbw, rotRectPoints[j], rotRectPoints[(j+1)%4], cv::Scalar(165,33,200), 1, 8 );
					
					imshow("drawContours", rightHandbw);

					cv::Mat rightHandtemp;
					rightHand.copyTo(rightHandtemp);
					cv::Mat toHist = rightHandtemp(cv::Rect(centerInscribedCircle.x - maxdist/2, centerInscribedCircle.y - maxdist/2,  maxdist, maxdist));
					cvtColor(toHist, toHist, CV_RGB2YCrCb);
					vector<cv::Mat> yCrCb_planes;
					split( toHist, yCrCb_planes );
					cv::Mat y_hist, Cr_hist, Cb_hist;
					int histsize = 256;
					float range[] = { 0, 256 } ;
					const float* histRange = { range };
					calcHist(&yCrCb_planes[0], 1, 0, cv::Mat(), y_hist, 1, &histsize, &histRange);
					calcHist(&yCrCb_planes[1], 1, 0, cv::Mat(), Cr_hist, 1, &histsize, &histRange);
					calcHist(&yCrCb_planes[2], 1, 0, cv::Mat(), Cb_hist, 1, &histsize, &histRange);
					cv::Mat histImage( 512, 400, CV_8UC3, cv::Scalar( 0,0,0) );
					normalize(y_hist, y_hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat() );
					normalize(Cr_hist, Cr_hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat() );
					normalize(Cb_hist, Cb_hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat() );

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


						line( histImage, cv::Point( bin_w*(i-1), 400 - cvRound(y_hist.at<float>(i-1)) ) ,
										 cv::Point( bin_w*(i), 400 - cvRound(y_hist.at<float>(i)) ),
										 cv::Scalar( 255, 0, 0), 2, 8, 0  );
						line( histImage, cv::Point( bin_w*(i-1), 400 - cvRound(Cr_hist.at<float>(i-1)) ) ,
										 cv::Point( bin_w*(i), 400 - cvRound(Cr_hist.at<float>(i)) ),
										 cv::Scalar( 0, 255, 0), 2, 8, 0  );
						line( histImage, cv::Point( bin_w*(i-1), 400 - cvRound(Cb_hist.at<float>(i-1)) ) ,
										 cv::Point( bin_w*(i), 400 - cvRound(Cb_hist.at<float>(i)) ),
										 cv::Scalar( 0, 0,255), 2, 8, 0  );						
					}
					
					printf("Y : %d // %d  Cr : %d // %d    Cb : %d // %d\n", tresholds[0], tresholds[1], tresholds[2], tresholds[3], tresholds[4], tresholds[5]);

					cvtColor(rightHand, rightHand, CV_RGB2YCrCb);
					inRange(rightHand,cv::Scalar(20,tresholds[2]-10,tresholds[4]-10),cv::Scalar(240,tresholds[3]+10,tresholds[5]+10), rightHand);
					int dilation_size = 2;
					cv::Mat element = cv::getStructuringElement( cv::MORPH_RECT,
                                       cv::Size( 2*dilation_size + 1, 2*dilation_size+1 ),
                                       cv::Point( dilation_size, dilation_size ) );
					dilate(rightHand, rightHand, element);
					erode(rightHand, rightHand, element);
					GaussianBlur(rightHand, rightHand, cv::Size(3,3), 1);
					
					cv::Mat M = getRotationMatrix2D(centerInscribedCircle, 90+rotRect.angle, 1);
					warpAffine(rightHand, rightHand, M, cv::Size(rightHand.cols, rightHand.rows));
					//imshow("toHist3", rightHand);
					//cv::waitKey(1);

					vector< vector<cv::Point> > contours2;
					vector<cv::Vec4i> hierarchy2;
					imshow("findContours", rightHand);
					cv::Mat newtemp;
					rightHand.copyTo(newtemp);
					findContours(newtemp, contours2, hierarchy2, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);
					cvtColor(rightHand, rightHand, CV_GRAY2RGB);
					
					cv::waitKey(0);
					largestArea = 0;
					if(contours2.size() != 0)
					{
						for(int i = 0; i < contours2.size(); i++)
						{
							double a = contourArea( contours2[i], false);
							if(a > largestArea)
							{
								largestArea = a;
								largestContourIndex = i;
							}
						}
					}			

					vector<vector<cv::Point>> contoursTemp2;
					contoursTemp2.push_back(contours2[largestContourIndex]);
					approxPolyDP( contours2[largestContourIndex], contoursTemp2[0], arcLength(contours2[largestContourIndex],true)*0.0025,  true );
					drawContours(rightHand, contoursTemp2, 0, cv::Scalar(0, 154, 255), 2, CV_AA);
					imshow("debut convexhull", rightHand);
					

					//Find and draw rotated bounding rectangle
					rotRect = minAreaRect(contoursTemp2[0]);
					rotRectPoints[4];
					rotRect.points(rotRectPoints);
					for( int j = 0; j < 4; j++ )
						line( rightHand, rotRectPoints[j], rotRectPoints[(j+1)%4], cv::Scalar(165,33,200), 1, 8 );
					

					//Find and draw max inscribed circle in biggest contour
					centerInscribedCircle;
					maxdist = -1;
					for(int i = 0;i< rightHand.cols;i++)
					{
						for(int j = 0;j< rightHand.rows;j++)
						{

							dist = pointPolygonTest(contoursTemp2[0], cv::Point(i,j),true);
							if(dist > maxdist)
							{
								maxdist = dist;
								centerInscribedCircle = cv::Point(i,j);
							}
						}
					}
					circle(rightHand, centerInscribedCircle, maxdist, cv::Scalar(0, 255, 255), CV_FILLED);



					//Find and draw Convex Hull of biggest contour
					convexHull(contoursTemp2[0], hull, true);

					

					int hullcount = (int)hull.size();
					cv::Point pt0 = contoursTemp2[0][hull[hullcount-1]];

					for(int i = 0; i < hullcount; i++ )
					{
						cv::Point pt = contoursTemp2[0][hull[i]];
						line(rightHand, pt0, pt, cv::Scalar(0, 255, 0), 2, CV_AA);
						if(sqrt(pow(pt.x - pt0.x, 2) + pow(pt.y - pt0.y, 2)) > 15)
						{
							circle(rightHand, pt, 5, cv::Scalar(255,0,0), CV_FILLED);
						}
						pt0 = pt;
					}

					

					//Find and draw Convexity Defect
					vector<cv::Vec4i> defect;
					cv::Point p;
					int count = 0;
					convexityDefects(contoursTemp2[0], hull, defect);

					for (int i = 0; i < defect.size(); i++)
					{
						int startIdx = defect[i].val[0];
						int endIdx = defect[i].val[1];
						int defectPtIdx = defect[i].val[2];
						double depth = (double)defect[i].val[3]/256.0f;
						if(depth > 10)
						{
							p = contoursTemp2[0][defectPtIdx];
							circle(rightHand, p, 5, cv::Scalar(0,0,255));
							count++ ;
						}
					}					
				}

				cv::imshow("tes", rightHand);
				cv::waitKey(0);
				rightHand.release();
				rightHandbw.release();
}

DWORD WINAPI Main::ThreadAffichage(LPVOID lpParam)
{
	Main* pThis = reinterpret_cast<Main *>(lpParam);
	return pThis->ThreadAffichage();
}
DWORD WINAPI Main::ThreadAffichage()
{
	sf::RenderWindow* main_window = new sf::RenderWindow(sf::VideoMode( MAINWINDOW_WIDTH, MAINWINDOW_HEIGHT), "Sign Interpretor", sf::Style::Default, sf::ContextSettings(32));
	engine = new Engine(main_window);
	engine->mainLoop();
	return S_OK;
}

DWORD WINAPI Main::ThreadGetAndDisplayNewColorFrame(LPVOID lpParam)
{
	Main* pThis = reinterpret_cast<Main*>(lpParam);
	return pThis->ThreadGetAndDisplayNewColorFrame();
}
DWORD WINAPI Main::ThreadGetAndDisplayNewColorFrame()
{
	#pragma region Attributs
		HRESULT hr;
		//Frame ref
		openni::VideoFrameRef pVfrColorFrame;

		//Color matrice
		cv::Mat pColorMat;

		//Create openCV window
		cv::namedWindow("Color Frame");
	#pragma endregion

	#pragma region Main Loop
		while( true )
		{
			hr = WaitForSingleObject( m_kinect->GetColorFrameHandle(), 100 );
			if(	hr == WAIT_OBJECT_0 )
			{
				//Get color frame and translate into matrice
				hr = WaitForSingleObject( m_hKinectMutex, INFINITE );
				if(hr == WAIT_OBJECT_0)
				{
					pVfrColorFrame = m_kinect->GetNextColorFrame();		
					if(!ReleaseMutex( m_hKinectMutex ))
						return E_FAIL;
				}else{printf("ThreadGetAndDisplayNewColorFrame() : Problem waiting for m_hKinectMutex"); return E_FAIL;}

				ImageTreatement::GetColorMat( &pColorMat, pVfrColorFrame );

				hr = WaitForSingleObject(m_hDepthTreatementIsFinishMutex, INFINITE);
				if( hr == WAIT_OBJECT_0 )
				{
					if( m_bDepthTreatementIsFinish )
					{
						pColorMat.copyTo(m_mColorFrameTemp);
						SetEvent(m_hColorTreatement);
					}
					if(!ReleaseMutex(m_hDepthTreatementIsFinishMutex))
						return E_FAIL;
				}else{printf("ThreadGetAndDisplayNewDepthFrame() : Problem waiting for m_hDepthTreatementIsFinishMutex"); return E_FAIL;}

				if(engine != NULL && engine->IsRunning())
				{
					cv::Mat littleMat;
					resize(pColorMat, littleMat, cv::Size(320,240) );
					engine->getGUI()->setRawImage(littleMat);
				}

				//Show image
				imshow( "Color Frame", pColorMat );
				cv::waitKey( 1 );

				//Release
				pColorMat.release();
				pVfrColorFrame.release();
			}else{printf("ThreadGetAndDisplayNewColorFrame() : Problem waiting for m_kinect->GetColorFrameHandle()");} // return E_FAIL;}
		}
	#pragma endregion

	cv::destroyWindow( "Color Frame" );
	return S_OK;
}

DWORD WINAPI Main::ThreadGetAndDisplayNewDepthFrame(LPVOID lpParam)
{
	Main* pThis = reinterpret_cast<Main*>(lpParam);
	return pThis->ThreadGetAndDisplayNewDepthFrame();
}
DWORD WINAPI Main::ThreadGetAndDisplayNewDepthFrame()
{
	#pragma region Attributs
	HRESULT hr;
	//Frame ref
	nite::UserTrackerFrameRef pUtfrFrame;
	openni::VideoFrameRef pVfrFrame;

	//Depth Matrice
	cv::Mat pMat;

	//Depth histogram
	float pHistogram[10000];

	//Thread Animation et Hand Analysis
	HANDLE pAnimationThread = NULL;
	HANDLE pProcessThread = NULL;

	//Timer de recording
	TimerRecord* pTimerRecoding = new TimerRecord();
	TimerRecord* pTimerRealTimeRecording = new TimerRecord();

	//Gesture related
	string pLastPredictedClass;
	vector<vector<cv::Point3d>> pGesture;
	vector<vector<cv::Point3d>> pPosition;
	int pTimeGestureClassLabel = 0;
	int pSignCount = 0;
	int pSignCount2 = 0;
	
	bool continueRecording = true;
	//Create OpenCV Window
	cv::namedWindow( "Depth Frame" );

	bool record = false;
	#pragma endregion

	#pragma region Main Loop
		while( true )
		{
			//printf("ThreadGetAndDisplayNewDepthFrame() : Waiting m_kinect->GetDepthFrameHandle()");
			hr = WaitForSingleObject(m_kinect->GetDepthFrameHandle(), 100);
			if( hr == WAIT_OBJECT_0 )
			{
				//printf("ThreadGetAndDisplayNewDepthFrame() : Waiting m_hKinectMutex");
				hr = WaitForSingleObject(m_hKinectMutex, INFINITE);
				if( hr == WAIT_OBJECT_0 )
				{
					pUtfrFrame = m_kinect->GetNextDepthFrame();
					if(!ReleaseMutex(m_hKinectMutex))
						return E_FAIL;
				}else{printf("ThreadGetAndDisplayNewDepthFrame() : Problem waiting for m_hKinectMutex"); return E_FAIL;}

				//Set traitement event when treatment is finished
				//printf("ThreadGetAndDisplayNewDepthFrame() : Waiting m_hDepthTreatementIsFinishMutex");
				hr = WaitForSingleObject(m_hDepthTreatementIsFinishMutex, INFINITE);
				if( hr == WAIT_OBJECT_0 )
				{
					if( m_bDepthTreatementIsFinish )
					{
						m_utfrUserFrameTemp = pUtfrFrame;
						SetEvent(m_hDepthTreatement);
					}
					if(!ReleaseMutex(m_hDepthTreatementIsFinishMutex))
						return E_FAIL;
				}else{printf("ThreadGetAndDisplayNewDepthFrame() : Problem waiting for m_hDepthTreatementIsFinishMutex"); return E_FAIL;}


				//Get depth frame into matrice
				pVfrFrame = pUtfrFrame.getDepthFrame();
				int test =pVfrFrame.getCropOriginX();
				ImageTreatement::calculateHistogram(pHistogram, 10000, pVfrFrame);
				ImageTreatement::GetDepthMat(&pMat, pVfrFrame, pHistogram);

				//Get userdata and store them in array
				const nite::Array<nite::UserData>& users = pUtfrFrame.getUsers();

				bool isRecording;
				//printf("ThreadGetAndDisplayNewDepthFrame() : Waiting m_hFeaturesRecorderMutex");
				hr = WaitForSingleObject(m_hFeaturesRecorderMutex, INFINITE);
				if( hr == WAIT_OBJECT_0 )
				{
					isRecording = m_featuresRecorder->IsRecording();
					//printf("ThreadGetAndDisplayNewDepthFrame() : Waiting m_featuresRecorder->GetFinishEvent()");
					hr = WaitForSingleObject(m_featuresRecorder->GetFinishEvent(), 0 );
					if (hr == WAIT_OBJECT_0)
					{
						pAnimationThread = CreateThread(NULL, 0, ThreadAnimation, this, 0, NULL);
						if(WaitForSingleObject(pAnimationThread, INFINITE) == WAIT_OBJECT_0)
							pAnimationThread = NULL;
						else
						{printf("ThreadGetAndDisplayNewDepthFrame() : Problem waiting for AnimationThread"); return E_FAIL;}
					}
					else if ( hr == WAIT_TIMEOUT )
					{
						if(isRecording)
							m_featuresRecorder->Perform(pMat, m_recordedSkeleton, users);
					}else{printf("ThreadGetAndDisplayNewDepthFrame() : Problem waiting for finishEvent"); return E_FAIL;} 

					if(!ReleaseMutex(m_hFeaturesRecorderMutex))
						return E_FAIL;	
				}else{printf("ThreadGetAndDisplayNewDepthFrame() : Problem waiting for m_hFeaturesRecorderMutex"); return E_FAIL;}

				if(!isRecording && pAnimationThread == NULL)
				{
						
					#pragma region Gestures and Position Prediction		
					//Save skeleton and try to do a pose prediction, then print it on the matrice
					GesturesAnalyzer::SaveSkeleton(pPosition, users);
					std::ostringstream str;
					//printf("ThreadGetAndDisplayNewDepthFrame() : Waiting m_hGestureRecorderMutex");
					hr = WaitForSingleObject(m_hGestureRecorderMutex, INFINITE);
					if (hr == WAIT_OBJECT_0)
					{
						str << "Position : " << m_gesturesAnalyzer->PredictCurrentPosition(pPosition);
						if(!ReleaseMutex(m_hGestureRecorderMutex))
							return E_FAIL;
					}else{printf("ThreadGetAndDisplayNewDepthFrame() : Problem waiting for m_hGestureRecorderMutex"); return E_FAIL;}

					putText(pMat, str.str(), cv::Point(20,30), cv::FONT_HERSHEY_COMPLEX_SMALL, 1, cv::Scalar(255,255,255));
					pPosition.clear();

					//If user is in sign position for more than 1s, start the hand analysis thread
					if(str.str() == "Position : SIGN_HAND" && pProcessThread == NULL)
					{
						if( pLastPredictedClass == str.str() )
						{
							pSignCount++;
							if(pSignCount > 30)
							{					
								pProcessThread = CreateThread(NULL, 0, ThreadTreatementDepthFrame, this, 0, NULL);
								pSignCount = 0;
							}
						}
					}
					else
					{
						//If hand analysis thread is running and user position is initial, then stop the thread
						if( str.str() == "Position : INITIALE" && pProcessThread != NULL )
						{
							m_bContinueProcessing = false;
							//printf("ThreadGetAndDisplayNewDepthFrame() : Waiting pProcessThread");
							hr = WaitForSingleObject(pProcessThread, INFINITE);
							if(hr == WAIT_OBJECT_0)
								pProcessThread = NULL;
							else{printf("ThreadGetAndDisplayNewDepthFrame() : Problem waiting for ProcessThread"); return E_FAIL; }
						}
						pSignCount = 0;
					}
				
					//Gesture recognition
					if( str.str() == "Position : START" && !record)
					{
						if( pLastPredictedClass == str.str() )
						{
							pSignCount2++;
							if(pSignCount2 > 30)
							{	
								record = true;
								pSignCount2 = 0;
							}
						}
					}
					else if (str.str() == "Position : INITIALE" && record )
					{
						if( pLastPredictedClass == str.str() )
						{
							pSignCount2++;
							if(pSignCount2 > 15)
							{	
								record = false;
								pSignCount2 = 0;
							}
						}						
					}

					if(record)
					{
						putText(pMat, "Do Gesture !", cv::Point(50,70), cv::FONT_HERSHEY_COMPLEX_SMALL, 1, cv::Scalar(255,255,255));
						GesturesAnalyzer::SaveSkeleton(pGesture, users);
					}
					else
					{
						if(pGesture.size() > 2)
						{
							//printf("ThreadGetAndDisplayNewDepthFrame() : Waiting m_hGestureRecorderMutex");
							hr = WaitForSingleObject(m_hGestureRecorderMutex, INFINITE);
							if( hr == WAIT_OBJECT_0 )
							{
								pTimeGestureClassLabel = m_gesturesAnalyzer->PredictGesture(pGesture);
								if(!ReleaseMutex(m_hGestureRecorderMutex))
									return E_FAIL;
							}else{printf("ThreadGetAndDisplayNewDepthFrame() : Problem waiting for m_hGestureRecorderMutex"); return E_FAIL;}
						}
						pGesture.clear();
					}

					std::ostringstream string;
					string << pTimeGestureClassLabel;
					putText(pMat, string.str(), cv::Point(20,70), cv::FONT_HERSHEY_COMPLEX_SMALL, 1, cv::Scalar(255,255,255));
					
					//Store last predicted class in order to compare with actual predicted class
					pLastPredictedClass = str.str();
					#pragma endregion

				}

				//printf("ThreadGetAndDisplayNewDepthFrame() : Waiting m_hKinectMutex");
				hr = WaitForSingleObject(m_hKinectMutex, INFINITE);
				if(hr == WAIT_OBJECT_0)
				{
					ImageTreatement::DrawSkeletons(pMat, m_kinect->GetUserTracker(), users);
					if(!ReleaseMutex(m_hKinectMutex))
						return E_FAIL;
				}
				else{printf("ThreadGetAndDisplayNewDepthFrame() : Problem waiting for m_hKinectMutex"); return E_FAIL;}

				//Show image
				cv::imshow("Depth Frame", pMat);
				cv::waitKey(1);

				//Release
				pMat.release();
				pUtfrFrame.release();
				pVfrFrame.release();
			}else{printf("ThreadGetAndDisplayNewDepthFrame() : Problem waiting for m_kinect->GetDepthFrameHandle()");}// return E_FAIL;}
		}	
	#pragma endregion

	cv::destroyWindow("Depth Frame");
	return S_OK;
}

DWORD WINAPI Main::ThreadTreatementDepthFrame(LPVOID lpParam)
{
	Main* pThis = reinterpret_cast<Main*>(lpParam);
	return pThis->ThreadTreatementDepthFrame();
}
DWORD WINAPI Main::ThreadTreatementDepthFrame()
{
	#pragma region Attributs
		HRESULT hr;
		//Frame ref
		openni::VideoFrameRef pVfrFrame;

		//Depth frame histogram
		float pHistogramTemp[10000];

		//Matrices
		cv::Mat depthTemp;
		cv::Mat finalDepth;
		cv::Mat testInterpolation;
		cv::Mat out;

		//Saved value of depth
		int pLastRightHandDepth = 0;
		int pLastLeftHandDepth = 0;
	
		//Create OpenCV Window
		cv::namedWindow( "Hand Analysis" );

		//Main thread loop	
		bool continueProc;
		hr = WaitForSingleObject(m_hContinueProcessingMutex, INFINITE);
		if(hr == WAIT_OBJECT_0)
		{
			m_bContinueProcessing = true;
			continueProc = m_bContinueProcessing;
			if(!ReleaseMutex(m_hContinueProcessingMutex))
				return E_FAIL;
		}else{printf("Problem waiting for m_hContinueProcessingMutex"); return E_FAIL;}

		vector<double> sampleVec;
		
	#pragma endregion

	#pragma region Main Loop
		while( continueProc )
		{
			HANDLE hTab[2] = {m_hDepthTreatement, m_hColorTreatement};
			hr = WaitForMultipleObjects(2, hTab, true, INFINITE );
			if(hr == WAIT_OBJECT_0)
			{
				//Lock treatement
				hr = WaitForSingleObject( m_hDepthTreatementIsFinishMutex, INFINITE );
				if(hr == WAIT_OBJECT_0)
				{
					m_bDepthTreatementIsFinish = !m_bDepthTreatementIsFinish;
					if(!ReleaseMutex( m_hDepthTreatementIsFinishMutex ))
						return E_FAIL;
				}else{printf("ThreadTreatementDepthFrame() : Problem waiting for m_hDepthTreatementIsFinishMutex"); return E_FAIL;}

				pVfrFrame = m_utfrUserFrameTemp.getDepthFrame();

				#pragma region get depth only for hands roi
				ImageTreatement::calculateHistogram( pHistogramTemp, 10000, pVfrFrame );
				int width = pVfrFrame.getWidth();
				int height = pVfrFrame.getHeight();

				openni::DepthPixel* pDepthRow = new openni::DepthPixel[ height * width ];
				openni::DepthPixel* pDepthRowTemp = pDepthRow;
				openni::RGB888Pixel* pDepthRowTex = new openni::RGB888Pixel[ height * width ];
				openni::RGB888Pixel* pDepthRowTexTemp = pDepthRowTex;

				memset(pDepthRowTex, 0, 3* height * width * sizeof( uint8_t ) );
				memcpy(pDepthRow, pVfrFrame.getData(), height * width * sizeof(uint16_t));

				const nite::Array<nite::UserData>& users = m_utfrUserFrameTemp.getUsers();

				nite::UserTracker* userTracker = m_kinect->GetUserTracker();
				Rec leftHandRoi = ImageTreatement::getHandROI(userTracker, users, nite::JOINT_LEFT_HAND);

				for (int y = 0; y < height ; ++y)
				{
					for ( int x = 0; x < width ; ++x, ++pDepthRow, ++pDepthRowTex)
					{
						if(leftHandRoi.handPos > 0)
						{
							if(*pDepthRow > 0 && (ImageTreatement::isInROI(x, y, leftHandRoi)))
							{
								if(pLastLeftHandDepth != 0 && pDepthRowTemp[leftHandRoi.handPos] > pLastLeftHandDepth + 500)
								{
									pLastLeftHandDepth = pLastLeftHandDepth;
								}
								else
								{
									pLastLeftHandDepth = pDepthRowTemp[leftHandRoi.handPos];
								}								

								if(*pDepthRow < pLastLeftHandDepth + 3*treshold)
								{
									pDepthRowTex->r = 255;
									pDepthRowTex->g = 255;
									pDepthRowTex->b = 255;
								}
							}
						}
					}
				}	
				depthTemp = cv::Mat( height , width , CV_8UC3 , pDepthRowTexTemp );
				finalDepth =cv:: Mat( height , width , CV_8UC3 , cv::Scalar(0,0,0) );
				m_mColorFrameTemp.copyTo(testInterpolation, depthTemp);
				
				
				//imshow("apres", out );
				#pragma endregion 

				if( leftHandRoi.handPos > 0 && SUCCEEDED(ImageTreatement::GetRealHand(depthTemp, testInterpolation, &out)))
				{
					if(SUCCEEDED(ImageTreatement::ExtractFeaturesFromHand(out, finalDepth, sampleVec)))
					{
						hr = WaitForSingleObject(m_hSaveHandEvent, 0);
						if(hr == WAIT_OBJECT_0)
						{
								hr = WaitForSingleObject(m_hGestureRecorderMutex, INFINITE);
								if(hr == WAIT_OBJECT_0)
								{
									m_gesturesAnalyzer->HandSaveSample( m_recordingHandClasse , m_recordingHandName , sampleVec);
									if(!ReleaseMutex(m_hGestureRecorderMutex))
										return E_FAIL;
								}else{printf("ThreadTreatementDepthFrame() : Problem waiting for m_hGestureRecorderMutex"); return E_FAIL;}
						}else if (hr == WAIT_TIMEOUT){  }
						else{printf("ThreadTreatementDepthFrame() : Problem waiting for m_hSaveHandEvent"); return E_FAIL;}

						hr = WaitForSingleObject(m_hGestureRecorderMutex, INFINITE);
						if(hr == WAIT_OBJECT_0)
						{
							std::ostringstream str;
							str << m_gesturesAnalyzer->PredictHandPosition( sampleVec);
							putText(finalDepth, str.str(), cv::Point(20,30), cv::FONT_HERSHEY_COMPLEX_SMALL, 1, cv::Scalar(255,255,255));
							
							if(!ReleaseMutex(m_hGestureRecorderMutex))
								return E_FAIL;
						}else{printf("ThreadTreatementDepthFrame() : Problem waiting for m_hGestureRecorderMutex"); return E_FAIL;}
					}
				}			
				

				imshow( "Hand Analysis", finalDepth );
				cv::waitKey( 1 );

				sampleVec.clear();
				depthTemp.release();
				finalDepth.release();
				out.release();
				testInterpolation.release();
				m_mColorFrameTemp.release();
				m_utfrUserFrameTemp.release();
				pVfrFrame.release();
				delete pDepthRowTexTemp;
				delete pDepthRowTemp;

				hr = WaitForSingleObject(m_hContinueProcessingMutex, INFINITE);
				if(hr == WAIT_OBJECT_0)
				{
					continueProc = m_bContinueProcessing;
					if(!ReleaseMutex(m_hContinueProcessingMutex))
						return E_FAIL;
				}else{printf("ThreadTreatementDepthFrame() : Problem waiting m_hContinueProcessingMutex"); return E_FAIL;}

				hr = WaitForSingleObject(m_hDepthTreatementIsFinishMutex, INFINITE);
				if(hr == WAIT_OBJECT_0)
				{
					m_bDepthTreatementIsFinish = !m_bDepthTreatementIsFinish;
					if(!ReleaseMutex(m_hDepthTreatementIsFinishMutex))
						return E_FAIL;
				}else{printf("ThreadTreatementDepthFrame() : Problem waiting for m_hDepthTreatementIsFinishMutex"); return E_FAIL;}

			}else{printf("ThreadTreatementDepthFrame() : Problem waiting m_hDepthTreatement"); return E_FAIL;}
		}
	#pragma endregion

	cv::destroyWindow("Hand Analysis");
	return S_OK;
}

DWORD WINAPI Main::ThreadAnimation(LPVOID lpParam)
{
	Main* pThis = reinterpret_cast<Main*>(lpParam);
	return pThis->ThreadAnimation();
}
DWORD WINAPI Main::ThreadAnimation()
{
	#pragma region Attributs
		HRESULT hr;
		cv::Mat window;
		vector<vector<cv::Point3d>> sampleVec;

		vector<vector<cv::Point3d>> recordedSkeleton = m_recordedSkeleton;

		cv::namedWindow("Skeleton recording", cv::WINDOW_AUTOSIZE);

		m_sliderDebut = 0;
		m_sliderMax = recordedSkeleton.size();
		m_sliderFin = m_sliderMax;
		cv::createTrackbar( "Debut", "Skeleton recording", &m_sliderDebut, m_sliderFin, on_trackbar_debut );
		cv::createTrackbar( "Fin", "Skeleton recording", &m_sliderFin, m_sliderMax, on_trackbar_fin );

		//Display animation for resizing
		nite::UserTracker* ut;
		hr = WaitForSingleObject(m_hKinectMutex, INFINITE);
		if( hr == WAIT_OBJECT_0)
		{
			ut = m_kinect->GetUserTracker();
			if(!ReleaseMutex(m_hKinectMutex))
				return E_FAIL;
		}else{printf("ThreadAnimation() : Problem waiting for m_hKinectMutex"); return E_FAIL;}
	#pragma endregion

	#pragma region Main Loop
		bool continueProc = true;
		while(continueProc)
		{
			for(int iterator = 0; iterator < (int)recordedSkeleton.size(); iterator++)
			{
				if( iterator > m_sliderDebut && iterator < m_sliderFin)
				{
					window = cv::Mat(480, 640, CV_8UC3, cv::Scalar(0,0,0));
					ImageTreatement::DrawSkeletonFromSample( window, recordedSkeleton[iterator], ut );
					cv::imshow("Skeleton recording", window);
					cv::waitKey(33);
					window.release();
				}
			}

			hr = WaitForSingleObject(m_hDisplayAnimationMutex, INFINITE);
			if( hr == WAIT_OBJECT_0)
			{
				continueProc = m_bDisplayAnimation;
				if(!ReleaseMutex(m_hDisplayAnimationMutex))
					return E_FAIL;
			}else{printf("ThreadAnimation() : Problem waiting for m_hDisplayAnimationMutex"); return E_FAIL; }
		}
	#pragma endregion

	#pragma region Save Data
		//Make new vector with only desired samples
		for(int iterator = 0; iterator < (int)recordedSkeleton.size(); iterator++)
		{
			if( iterator > m_sliderDebut && iterator < m_sliderFin)
			{
				sampleVec.push_back(recordedSkeleton[iterator]);
			}
		}	

		hr = WaitForSingleObject(m_hGestureRecorderMutex, INFINITE);
		if(hr == WAIT_OBJECT_0)
		{
			//Save training data
			if(m_recordingType == "gesture")
				m_gesturesAnalyzer->DTWSaveSampleAndTrain(m_recordingClasse, sampleVec);
			else if( m_recordingType == "position" )
				m_gesturesAnalyzer->ANBCSaveSampleAndTrain(m_recordingClasse, m_recordingName, sampleVec);
			m_recordedSkeleton.clear();
			if(!ReleaseMutex(m_hGestureRecorderMutex))
				return E_FAIL;
		}else{printf("ThreadAnimation() : Problem waiting for m_hGestureRecorderMutex"); return E_FAIL;}
	#pragma endregion

	cv::destroyWindow("Skeleton recording");

	hr = WaitForSingleObject(m_hDisplayAnimationMutex, INFINITE);
	if(hr == WAIT_OBJECT_0)
	{
		m_bDisplayAnimation = true;
		if(!ReleaseMutex(m_hDisplayAnimationMutex))
			return E_FAIL;
	}else{printf("ThreadAnimation() : Problem waiting for m_hDisplayAnimationMutex"); return E_FAIL;}

	hr = WaitForSingleObject(m_hRecordingMutex, INFINITE);
	if(hr == WAIT_OBJECT_0)
	{
		m_bRecording = false;
		if(!ReleaseMutex(m_hRecordingMutex))
			return E_FAIL;
	}else{printf("ThreadAnimation() : Problem waiting for m_hRecordingMutex"); return E_FAIL;}

	return S_OK;
}

DWORD WINAPI Main::KeyboardListener(LPVOID lpParam)
{
	Main* pThis = reinterpret_cast<Main*>(lpParam);
	return pThis->KeyboardListener();
}
DWORD WINAPI Main::KeyboardListener()
{
	HRESULT hr;
	while(true)
	{
		if(_kbhit())
		{
			char c = _getch();
			switch(c)
			{
				case 'r':
					{
						hr = WaitForSingleObject(m_hRecordingMutex, INFINITE);
						if(hr == WAIT_OBJECT_0)
						{
							if(!m_bRecording)
							{
								m_bRecording = !m_bRecording;
							}
							if(!ReleaseMutex(m_hRecordingMutex))
								return E_FAIL;
						}else{printf("KeyboardListener() : Problem waiting for m_hRecordingMutex"); return E_FAIL; }

						cout << "Classe : " ;
						cin >> m_recordingClasse;
						cin.clear();

						cout << endl << "Nom du geste : ";
						cin >> m_recordingName;
						cin.clear();

						cout<< endl << "Type : " ;
						cin >> m_recordingType;
						cin.clear();

						int Timeout = 0;
						cout<< endl << "Timeout : " ;
						cin >> Timeout;
						cin.clear();

						hr = WaitForSingleObject(m_hFeaturesRecorderMutex, INFINITE);
						if(hr == WAIT_OBJECT_0)
						{
							m_featuresRecorder->Start(Timeout);
							if(!ReleaseMutex(m_hFeaturesRecorderMutex))
								return E_FAIL;
						}else{printf("KeyboardListener() : Problem waiting for m_hFeaturesRecorderMutex"); return E_FAIL; }
					}
					break;
				case 's':
					hr = WaitForSingleObject(m_hDisplayAnimationMutex, INFINITE );
					if(hr == WAIT_OBJECT_0)
					{
						m_bDisplayAnimation = false;
						if(!ReleaseMutex(m_hDisplayAnimationMutex))
							return E_FAIL;
					}else{printf("KeyboardListener() : Problem waiting for m_hDisplayAnimationMutex"); return E_FAIL;}
					break;
				case 'p':
					hr = WaitForSingleObject(m_hFeaturesRecorderMutex, INFINITE);
					if(hr == WAIT_OBJECT_0)
					{
						m_featuresRecorder->Stop();
						if(!ReleaseMutex(m_hFeaturesRecorderMutex))
							return E_FAIL;
					}else{printf("KeyboardListener() : Problem waiting for m_hFeaturesRecorderMutex"); return E_FAIL; }
					break;
				case 'n':
					SetEvent(m_hSaveHandEvent);
					break;
				case 'b':
					cout << "Classe : " ;
					cin >> m_recordingHandClasse;
					cin.clear();

					cout << endl << "Nom de la position : ";
					cin >> m_recordingHandName;
					cin.clear();
					break;
				case 'v' :
					printf( " --------%s-------- \n", m_recordingType.c_str());
					printf( " Classe : %d \n", m_recordingClasse);
					printf( " Name : %s \n", m_recordingName.c_str());
					printf( " --------HAND-------- \n");
					printf( " Classe : %d \n", m_recordingHandClasse);
					printf( " Name : %s \n", m_recordingHandName.c_str());
				default:
					break;
			}
		}
	}
	return S_OK;
}