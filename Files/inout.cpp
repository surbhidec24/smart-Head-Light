#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <iostream>
#include <vector>
#include <math.h>
#include "inout.h"
#include <opencv2/video/video.hpp>
#include "debug.hpp"
#include "coreFuncs.hpp"

using namespace cv;
using namespace std;

#define Hi 255

void readVideo(string filename){
    VideoCapture capture(filename);
    Mat frame;

    if( !capture.isOpened() )
        throw "Error when reading steam_avi";

    namedWindow( "window", 1);
    for( ; ; )
    {
        capture >> frame;
        if(frame.empty())
            break;
        imshow("window", frame);
        waitKey(1); // waits to display frame
    }
    waitKey(0); // key press to close window
}

void readVideoAndBox(string filename){
    VideoCapture capture(filename);
    Mat frame;

    if( !capture.isOpened() )
        throw "Error when reading steam_avi";

    namedWindow( "window", 1);
    for(; ;)
    {
        capture >> frame;
        if(frame.empty())
            break;
        int numPairs = 0;
        getBoundingBoxes(frame);
        imshow("window", frame);
        waitKey(5); // waits to display frame
    }
    waitKey(0); // key press to close window
}

void displayImage(Mat img){
	//check whether the image is loaded or not
	if (img.empty()){ 
        cout << "Error : Image cannot be loaded..!!" << endl;
          //system("pause"); //wait for a key press
    }
	
	//create a window with the name "MyWindow"
    namedWindow("MyWindow", CV_WINDOW_AUTOSIZE);
	//display the image which is stored in the 'img' in the "MyWindow" window	 
    imshow("MyWindow", img); 
	
	//wait infinite time for a keypress
	waitKey(0); 
	
	//destroy the window with the name, "MyWindow"
	destroyWindow("MyWindow"); 
}

/*converts an 3 channel RGB to 1 channel binary image*/ 
void convert2Binary(Mat img, Mat &img_bw){
	Mat img_gray;
	cvtColor(img, img_gray,CV_RGB2GRAY);
    threshold(img_gray, img_bw, 150.0, Hi, THRESH_BINARY);
	imwrite("image_bw.jpg", img_bw);
}

void track(string filename){
    VideoCapture capture(filename);
    Mat frame;
    int stateSize = 6;
    int measSize = 4;
    int contrSize = 0;
 
    unsigned int type = CV_32F;
    KalmanFilter kf(stateSize, measSize, contrSize, type);
    bool found = false;

    Mat state(stateSize, 1, type);  
    Mat meas(measSize, 1, type);   

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

    setIdentity(kf.measurementNoiseCov, cv::Scalar(1e-1));

    if( !capture.isOpened() )
        throw "Error when reading steam_avi";
    namedWindow( "window", 1);
    double precTick, dT, ticks = 0;

    for(int i =0; i<216;i++)
    {   
        capture >> frame;
        if(frame.empty())
            break;
        vector <Rect> pairs = getBoundingBoxes(frame);

        precTick = ticks;
        ticks = (double) cv::getTickCount();
        dT = (ticks - precTick) / cv::getTickFrequency();

    //     if (pairs.at<float>(2) == 0 || pairs.at<float>(3) == 0)
    //         continue;
    //     //printVectors(pairs, 1, pairs.cols);
        if(pairs.size()==0)
            continue;

        if (found) //will not execute in 1st find
        {
         // >>>> Matrix A
            kf.transitionMatrix.at<float>(2) = dT;
            kf.transitionMatrix.at<float>(9) = dT;
         // <<<< Matrix A
            //cout << "dT:" << endl << dT << endl;
 
            state = kf.predict();
            cout << "State post:" << endl << state << endl;            
            cv::Rect predRect;         
            predRect.width = state.at<float>(4);         
            predRect.height = state.at<float>(5);          
            predRect.x = state.at<float>(0) - predRect.width / 2;          
            predRect.y = state.at<float>(1) - predRect.height / 2;            
            cv::Point center;          
            center.x = state.at<float>(0);          
            center.y = state.at<float>(1); 

            Scalar color = Scalar(255,0,0);
            rectangle( frame, predRect, color, 1, 8, 0 );
            imshow("window", frame);
            waitKey(5); 
                       
        } 
        ////////// try making pair, a vector of rect /////////////
        meas.at<float>(0) = pairs[0].x + pairs[0].width / 2;
        meas.at<float>(1) = pairs[0].y + pairs[0].height / 2;
        meas.at<float>(2) = (float)pairs[0].width;
        meas.at<float>(3) = (float)pairs[0].height; 

        if (!found){
            // first detection
            // >>>> Initialization
            kf.errorCovPre.at<float>(0) = 1; // px
            kf.errorCovPre.at<float>(7) = 1; // px
            kf.errorCovPre.at<float>(14) = 1;
            kf.errorCovPre.at<float>(21) = 1;
            kf.errorCovPre.at<float>(28) = 1; // px
            kf.errorCovPre.at<float>(35) = 1; // px
 
            state.at<float>(0) = meas.at<float>(0);
            state.at<float>(1) = meas.at<float>(1);
            state.at<float>(2) = 0;
            state.at<float>(3) = 0;
            state.at<float>(4) = meas.at<float>(2);
            state.at<float>(5) = meas.at<float>(3);
            // <<<< Initialization
    cout << i <<endl;
            found = true;
         }
         else
            kf.correct(meas); // Kalman Correction
 
          cout << "Measure matrix:" << endl << meas << endl;
    }
    waitKey(0);
}

void trackCars(string filename){
    VideoCapture capture(filename);
    Mat frame;
    vector<Tracker> trackerList;

    if( !capture.isOpened() )
        throw "Error when reading steam_avi";
    
    double precTick, dT, ticks = 0;

    for(int i = 0; i<21000; i++ ){   
        capture >> frame;
        if(frame.empty())
            break;

        precTick = ticks;
        ticks = (double) cv::getTickCount();
        dT = (ticks - precTick) / cv::getTickFrequency();
        
        vector<Rect> bBoxes;
        bBoxes = getBoundingBoxes(frame);

        if(bBoxes.size() == 0)
            continue;

        vector<Tracker>::iterator nIter;
        for (nIter = trackerList.begin(); nIter != trackerList.end(); nIter++){

            int idx = findNext((*nIter),bBoxes,5);
            if(idx == -1){
                nIter->notFound++;
                if(nIter->notFound >=10){
                    trackerList.erase(nIter);
                    nIter--;
                }
            }
            else{
                nIter->measAndUpdate(dT, bBoxes[idx], frame);
                bBoxes.erase(bBoxes.begin()+idx);
            }
            
        }
        vector<Rect>::iterator mIter;

        for (mIter = bBoxes.begin(); mIter!= bBoxes.end(); mIter++){
            Tracker newTracker;
            newTracker.init_tracker();
            newTracker.measAndUpdate(dT, (*mIter), frame);
            trackerList.push_back(newTracker); ////wait a few frames?/////
            bBoxes.erase(mIter);
            mIter--;
        }
    }
}

int findNext(Tracker myTracker, vector<Rect>bBoxes, double thresh){
    Rect oldMeasurements = myTracker.oldMeas;
    double res;
    
    Point center;
    center.x = oldMeasurements.x + (oldMeasurements.width/2);
    center.y = oldMeasurements.y + (oldMeasurements.height/2);

    for (int i = 0; i<bBoxes.size(); i++){
        Point temp;
        temp.x = bBoxes[i].x + (bBoxes[i].width/2);
        temp.y = bBoxes[i].y + (bBoxes[i].height/2);
        res = cv::norm(center-temp);
        if(res <= thresh)
            return i;
    }
    return -1;
}