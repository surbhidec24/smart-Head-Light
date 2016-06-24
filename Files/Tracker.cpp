#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/video/video.hpp>
#include <iostream>
#include <vector>
#include <math.h>
#include <string>
#include <fstream>

#include "Tracker.h"

using namespace std;
using namespace cv;

int stateSize = 6;
int measSize = 4;
int contrSize = 0;
unsigned int type = CV_32F;
Mat state(stateSize, 1, type);  
Mat meas(measSize, 1, type);
int size_w = 500;
int size_h = 500;
Point origin(10,size_w/2);

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
    display = false;
    notFound = 0;
    startTick = (double) cv::getTickCount();

    //plt::plot({1,2,3,4});
    //plt::show();

    // Mat plot_(size_h,size_w,type);
    // plot = plot_;
    // count = 0;
    
    // Point y1(10,size_h/2);
    // Point y2(plot.rows - 10,size_h/2);
    // Point x1(size_w/2,plot.cols-10);
    // Point x2(size_w/2,10);

    // Scalar color = Scalar(255,255,255);
    // arrowedLine(plot,origin,x2,color,1,8,0);
    // arrowedLine(plot,origin,y2,color,1,8,0);
    // arrowedLine(plot,origin,x1,color,1,8,0);
    // arrowedLine(plot,origin,y1,color,1,8,0);
    
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

        dx.push_back(state.at<float>(2));
        dy.push_back(state.at<float>(3));
        double currentTick = (double) cv::getTickCount();
        time.push_back(currentTick- startTick / cv::getTickFrequency());
        if(display == true){
            ostringstream ss;
            ss << "Output/" << fileName << ".csv";
            string name = ss.str();
            cout << name << endl;
            ofstream myfile(name.c_str());
            for (int i = 0; i<dx.size(); i++)
                if(*(dx.begin()+i)!=0)
                    myfile << *(time.begin()+i) << "\t" << *(dx.begin()+i) << "\t" << *(dy.begin()+i) <<endl;
            myfile.close();
            display = false;
        }
        // state(2) and state(3) have dx and dy;
        //plotGraph(state.at<float>(2), state.at<float>(3));
        //cout << time <<endl;
        Scalar color = Scalar(255,0,0);
        rectangle(frame, predRect, color, 1, 8, 0 );
    }
}

void Tracker:: plotGraph(float dx, float dy){
    Scalar color1 = Scalar(255,0,0);
    Scalar color2 = Scalar(0,255,0);

    Point centre;
    centre.x = origin.x + count;
    centre.y = (origin.y-dx);
    circle(plot,centre,1,color1,1,8,0);

    Point centre1;
    centre1.x = origin.x + count;
    centre1.y = (origin.y-dy);
    circle(plot,centre1,1,color2,1,8,0);
    count++;
}