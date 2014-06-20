#include "stdafx.h"
#include "Detector.h"
using namespace cv;

Detector::Detector(void)
{
	Y_MIN  = 0;
	Y_MAX  = 255;
	Cr_MIN = 133;
	Cr_MAX = 173;
	Cb_MIN = 80;
	Cb_MAX = 120;
	face.load("haarcascade_frontalface_alt.xml") ;
	eyes_h.load("haarcascade_eye_tree_eyeglasses.xml") ;
}


Detector::~Detector(void)
{
}

Mat Detector::getSkin(Mat input)
{
	Mat skin;
	Mat bw;
	blur(input, input,Size(3,3));
	cvtColor(input,skin,CV_BGR2HSV);
	//inRange(skin,Scalar(Y_MIN,Cr_MIN,Cb_MIN),Scalar(Y_MAX,Cr_MAX,Cb_MAX),skin);
	inRange(skin, Scalar(0, 10, 50), Scalar(20, 150, 255), bw);
    erode(bw, bw, Mat(), Point(-1, -1), 2, 1, 1);
    dilate(bw, bw, Mat(), Point(-1, -1), 2, 1, 1);
	return bw;
}

void Detector::detectAndDisplay( Mat frame )
{
  std::vector<Rect> faces;
  Mat frame_gray;

  cvtColor( frame, frame_gray, CV_BGR2GRAY );
  equalizeHist( frame_gray, frame_gray );

  //-- Detect faces
  face.detectMultiScale( frame_gray, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(30, 30) );

  for( size_t i = 0; i < faces.size(); i++ )
  {
    Point center( faces[i].x + faces[i].width*0.5, faces[i].y + faces[i].height*0.5 );
    ellipse( frame, center, Size( faces[i].width*0.5, faces[i].height*0.5), 0, 0, 360, Scalar( 255, 0, 255 ), 4, 8, 0 );

    Mat faceROI = frame_gray( faces[i] );
	std::vector<Rect> eyes;

    //-- In each face, detect eyes
    eyes_h.detectMultiScale( faceROI, eyes, 1.1, 2, 0 |CV_HAAR_SCALE_IMAGE, Size(30, 30) );

    for( size_t j = 0; j < eyes.size(); j++ )
     {
       Point center( faces[i].x + eyes[j].x + eyes[j].width*0.5, faces[i].y + eyes[j].y + eyes[j].height*0.5 );
       int radius = cvRound( (eyes[j].width + eyes[j].height)*0.25 );
       circle( frame, center, radius, Scalar( 255, 0, 0 ), 4, 8, 0 );
     }
  }
}