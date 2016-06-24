#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <iostream>
#include <vector>
#include <math.h>
#include <string>
#include "inout.h"
#include <opencv2/video/video.hpp>
#include "debug.hpp"
#include "coreFuncs.hpp"
#include "matplotlibcpp.h"
#include <cmath>
#include <python2.7/Python.h>

using namespace cv;
using namespace std;
namespace plt = matplotlibcpp;

#define Hi 255
#define binThresh 100.0
#define pixelThresh 5
#define appearThresh 15

vector<Tracker> trackerList;
int winCount;

void readVideo(string filename){
    VideoCapture capture(filename);
    Mat frame;

    if( !capture.isOpened() )
        throw "Error when reading steam_avi";

    namedWindow( "window", 1);
    for( ; ; ){
        capture >> frame;
        if(frame.empty())
            break;
        imshow("window", frame);
        waitKey(1);
    }
    waitKey(0);
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
        waitKey(1); 
    }
    waitKey(0);
}

void displayImage(Mat img){
	if (img.empty())
        cout << "Error : Image cannot be loaded..!!" << endl;
    namedWindow("MyWindow", CV_WINDOW_AUTOSIZE);
    imshow("MyWindow", img); 
	waitKey(0); 
	destroyWindow("MyWindow"); 
}

/*converts an 3 channel RGB to 1 channel binary image*/ 
void convert2Binary(Mat img, Mat &img_bw){
	Mat img_gray;
	cvtColor(img, img_gray,CV_RGB2GRAY);
    threshold(img_gray, img_bw, binThresh, Hi, THRESH_BINARY);
	imwrite("image_bw.jpg", img_bw);
}

int trackCars(string filename){
    VideoCapture capture(filename);
    Mat frame;
    winCount = 0;

    if( !capture.isOpened() )
        throw "Error when reading steam_avi";
    
    double precTick, dT, ticks = 0;
    namedWindow( "window", 1);
    setMouseCallback( "window", onMouse, 0);
    for(; ;){   
        capture >> frame;
        if(frame.empty())
            break;

        precTick = ticks;
        ticks = (double) cv::getTickCount();
        dT = (ticks - precTick) / cv::getTickFrequency();
        
        vector<Rect> bBoxes;
        bBoxes = getBlobs(frame);

        if(bBoxes.size() == 0)
            continue;

        vector<Tracker>::iterator nIter;
        for (nIter = trackerList.begin(); nIter != trackerList.end(); nIter++){

            int idx = findNext((*nIter),bBoxes);
            if(idx == -1){
                nIter->predict(dT,frame);
                nIter->notFound++;
                if(nIter->notFound >= appearThresh){
                    trackerList.erase(nIter);
                    nIter--;
                }
            }
            else{
                nIter->notFound = 0;
                nIter->measAndUpdate(dT, bBoxes[idx], frame);
                bBoxes.erase(bBoxes.begin()+idx);
            }
            
        }
        vector<Rect>::iterator mIter;
        /*adding a new tracker for blobs which dont match previous blobs*/
        for (mIter = bBoxes.begin(); mIter!= bBoxes.end(); mIter++){
            Tracker newTracker;
            newTracker.init_tracker();
            newTracker.measAndUpdate(dT, (*mIter), frame);
            trackerList.push_back(newTracker); ////wait a few frames?/////
            bBoxes.erase(mIter);
            mIter--;
        }

        imshow("window", frame);
        waitKey(1); 
    }
    return winCount;
}

int findNext(Tracker myTracker, vector<Rect>bBoxes){
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
        if(res <= pixelThresh)
            return i;
    }
    return -1;
}

void onMouse( int event, int x, int y, int, void* ){
    if( event != CV_EVENT_LBUTTONDOWN )
            return;
    Point pt = Point(x,y);
    Rect temp;
    //std::cout<<"x="<<pt.x<<"\t y="<<pt.y<<"\n";

    vector<Tracker>::iterator nIter;
    for (nIter = trackerList.begin(); nIter != trackerList.end(); nIter++){
        temp.x = nIter->oldMeas.x-5;
        temp.y = nIter->oldMeas.y-5;
        temp.width = nIter->oldMeas.width +5;
        temp.height = nIter->oldMeas.height +5;
        if(pt.inside(temp)){
            nIter->display = true;
            stringstream out;
            out<< winCount;
            nIter->fileName = out.str();
            winCount++;
            break;
        }
    }
}