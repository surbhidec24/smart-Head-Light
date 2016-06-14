#ifndef TRACKER_1
#define TRACKER_1
#pragma once

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/video/video.hpp>
#include <iostream>
#include <vector>
#include <math.h>

using namespace std;
using namespace cv;

class Tracker{ 
	public: 
		KalmanFilter kf;
		bool found;
		int notFound;
		Rect oldMeas;

	public:
		Tracker();
		void init_tracker();
		void measAndUpdate(double dT, Rect box, Mat frame);
};

#endif