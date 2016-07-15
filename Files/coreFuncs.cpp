#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/video.hpp>
#include <opencv2/core/core.hpp>
#include <iostream>
#include <vector>
#include <math.h>
#include <string>
#include <cmath>
#include <stdbool.h>
#include "coreFuncs.hpp"
#include "Tracker.h"

using namespace cv;
using namespace std;

#define Hi 255
#define binThresh 100.0
#define pixelThresh 5
#define appearThresh 15
#define Low 0

#define TA_l 20
#define TA_h 150
#define TJ 5
#define TX_l 20
#define TX_h 150
#define TW 5
#define TH 5
#define thresh 100

vector<Tracker> trackerList;
int winCount;
Mat img_gray;

int trackCars(string filename){
    VideoCapture capture(filename);
    Mat frame;
    winCount = 0;

    clock_t init, final;
    

    if( !capture.isOpened() )
        throw "Error when reading steam_avi";
    
    double precTick, dT, ticks = 0;
    namedWindow( "window", 1);
    setMouseCallback( "window", onMouse, 0);
    for( int m =0; m<50000; m++ ){   
        capture >> frame;
        // Size s = frame.size();
        // cout << s.height << '\t' << s.width << endl;
        if(frame.empty())
            break;

        init=clock();
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

        // final=clock()-init;
        // init = clock();
        // if(m % 250 == 0)
        //     cout << (double)final / ((double)CLOCKS_PER_SEC) << endl;
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
        final=clock()-init;
        if(m % 250 == 0)
            cout << (double)final / ((double)CLOCKS_PER_SEC) << endl;
        // imshow("window", frame);
        // waitKey(1); 
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


vector<Rect> getBlobs(Mat img){
    cvtColor(img, img_gray,CV_RGB2GRAY);
    blur(img_gray, img_gray, Size(3,3));

    return thresh_callback( 0, 0 );
}


vector<Rect> thresh_callback(int, void* )
{
  Mat threshold_output;
  vector<vector<Point> > contours;
  vector<Vec4i> hierarchy;

  /// Detect edges using Threshold
  threshold( img_gray, threshold_output, thresh, 255, THRESH_BINARY );
  /// Find contours
  findContours( threshold_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

  /// Approximate contours to polygons + get bounding rects
  vector<vector<Point> > contours_poly( contours.size() );
  vector<Rect> boundRect( contours.size() );

  int count = 0;
  Rect tempRect;
  for( int i = 0; i < contours.size(); i++ )
     { approxPolyDP( Mat(contours[i]), contours_poly[i], 3, true );
       tempRect = boundingRect( Mat(contours_poly[i]) );
       if (verify(tempRect)){
            boundRect[count] = tempRect;
            count++;
       }
     }
     return boundRect;
}

bool verify(Rect myRect){
    // float ratio = myRect.height/myRect.width;
    if(myRect.height>3 && myRect.width > 5)
        return true;
    return false;
}

void getPairs(vector<Rect> &vList, vector<Rect> &pairs){
    vector<Rect> :: iterator nIter;
    for (nIter = vList.begin(); nIter!= vList.end(); nIter++){
        int area = nIter->width * nIter->height;
        if(area <TA_l || area > TA_h){
            vList.erase(nIter);
            nIter--;
            continue;
        }

        vector<Rect> :: iterator mIter;
        mIter = nIter+1;
        Rect tempRect;

        while (mIter!=vList.end() && abs(mIter->y - nIter->y) <TJ){
            int distance = abs(mIter->x - nIter->x);
            int width = abs(mIter->width - nIter->width);
            int height = abs(mIter->height - nIter->height);
            if(distance >TX_l && distance <TX_h && width <TW && height < TH){
                tempRect.y = min(mIter->y,nIter->y);
                tempRect.x = min(mIter->x,nIter->x);
                tempRect.width = distance + max( mIter->width, nIter->width);
                tempRect.height = max(mIter->height, nIter->height);
                pairs.push_back(tempRect);
                vList.erase(mIter);
                vList.erase(nIter);
                nIter--;
                break;
            }
            mIter++;
        }
    }
}