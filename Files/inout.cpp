#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <iostream>
#include <vector>
#include <math.h>
#include "inout.h"

using namespace cv;
using namespace std;

#define Hi 255

void readVideo(string filename){
    VideoCapture capture(filename);
    Mat frame;

    if( !capture.isOpened() )
        throw "Error when reading steam_avi";

    namedWindow( "w", 1);
    for( ; ; )
    {
        capture >> frame;
        if(frame.empty())
            break;
        imshow("w", frame);
        waitKey(0.001); // waits to display frame
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