#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/video/video.hpp>
#include <iostream>
#include <vector>
#include <math.h>
#include "Tracker.h"

using namespace std;
using namespace cv;

int stateSize = 6;
int measSize = 4;
int contrSize = 0;
unsigned int type = CV_32F;
Mat state(stateSize, 1, type);  
Mat meas(measSize, 1, type);

Tracker:: Tracker(){}

void Tracker:: init_tracker(){
    KalmanFilter kf_(stateSize, measSize, contrSize, type);
    kf = kf_;  

    setIdentity(kf.transitionMatrix);
    kf.measurementMatrix = Mat::zeros(measSize, stateSize, type);
    kf.measurementMatrix.at<float>(0) = 1.0f;
    kf.measurementMatrix.at<float>(7) = 1.0f;
    kf.measurementMatrix.at<float>(16) = 1.0f;
    kf.measurementMatrix.at<float>(23) = 1.0f;

    kf.processNoiseCov.at<float>(0) = 1e-2;
    kf.processNoiseCov.at<float>(7) = 1e-2;
    kf.processNoiseCov.at<float>(14) = 2.0f;
    kf.processNoiseCov.at<float>(21) = 1.0f;
    kf.processNoiseCov.at<float>(28) = 1e-2;
    kf.processNoiseCov.at<float>(35) = 1e-2;

    kf.errorCovPre.at<float>(0) = 1; 
    kf.errorCovPre.at<float>(7) = 1; 
    kf.errorCovPre.at<float>(14) = 1;
    kf.errorCovPre.at<float>(21) = 1;
    kf.errorCovPre.at<float>(28) = 1; 
    kf.errorCovPre.at<float>(35) = 1;

    setIdentity(kf.measurementNoiseCov, cv::Scalar(1e-1));

    found = false;
    notFound = 0;
}

void Tracker:: measAndUpdate(double dT, Rect box, Mat &frame){
	oldMeas = box;
    predict(dT,frame); 

    meas.at<float>(0) = box.x + box.width / 2;
    meas.at<float>(1) = box.y + box.height / 2;
    meas.at<float>(2) = (float)box.width;
    meas.at<float>(3) = (float)box.height; 

    if (!found){
        // first detection
        // >>>> Initialization
        state.at<float>(0) = meas.at<float>(0);
        state.at<float>(1) = meas.at<float>(1);
        state.at<float>(2) = 0;
        state.at<float>(3) = 0;
        state.at<float>(4) = meas.at<float>(2);
        state.at<float>(5) = meas.at<float>(3);
        // <<<< Initialization
 
        found = true;
    }
    else
	    kf.correct(meas); // Kalman Correction
    
    //cout << "Measure matrix:" << endl << meas << endl;
}

void Tracker:: predict(double dT, Mat &frame){
    if (found){
         // >>>> Matrix A
        kf.transitionMatrix.at<float>(2) = dT;
        kf.transitionMatrix.at<float>(9) = dT;
         // <<<< Matrix A
        //cout << "dT:" << endl << dT << endl;
        
        state = kf.predict();
        //cout << "State post:" << endl << state << endl;            
        cv::Rect predRect;         
        predRect.width = state.at<float>(4);         
        predRect.height = state.at<float>(5);          
        predRect.x = state.at<float>(0) - predRect.width / 2;          
        predRect.y = state.at<float>(1) - predRect.height / 2;            
        cv::Point center;          
        center.x = state.at<float>(0);          
        center.y = state.at<float>(1); 

        Scalar color = Scalar(255,0,0);
        rectangle(frame, predRect, color, 1, 8, 0 );
    }
}
