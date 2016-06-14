#include <iostream>
#include <vector>
#include "coreFuncs.hpp"
#include "inout.h"
#include "debug.hpp"
#include "Tracker.h"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

int main( int argc, const char** argv ){
	// Mat img = imread("sample1.jpg", CV_LOAD_IMAGE_UNCHANGED); 
	// displayImage(img);
	// getBoundingBoxes(img);
	// displayImage(img);
	// string ty =  type2str( img.type() );
 //  	cout << "Matrix: " << ty.c_str() << '\t' << endl;
  	// readVideo("output2.avi");
  	//readVideoAndBox("output2.avi");
  	trackCars("output2.avi");
	return 0;
}