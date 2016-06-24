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
#include <string>

using namespace std;
using namespace cv;

class Tracker{ 
	public: 
		KalmanFilter kf;
		Mat plot;
		bool found;
		int notFound;
		Rect oldMeas;
		double startTick;
		int count;
		bool display;
		/*vectors store dx,dy for graph display*/
		vector<double>dx;
		vector<double>dy;
		vector<double>time;
		String fileName;

	public:
		Tracker();
		void init_tracker();
		void measAndUpdate(double dT, Rect box, Mat &frame);
		void predict(double dT, Mat &frame);
		void plotGraph(float dx, float dy);
};

#endif