#pragma once
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\objdetect\objdetect.hpp>
class Detector
{
public:
	Detector(void);
	~Detector(void);
	cv::Mat getSkin(cv::Mat input);
	void detectAndDisplay( cv::Mat frame );

private:
int Y_MIN;
int Y_MAX;
int Cr_MIN;
int Cr_MAX;
int Cb_MIN;
int Cb_MAX;
cv::CascadeClassifier face;
cv::CascadeClassifier eyes_h;
};

