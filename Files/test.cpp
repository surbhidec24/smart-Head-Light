#include <iostream>
#include <vector>
#include "coreFuncs.hpp"
#include "inout.h"
#include "debug.hpp"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

int main( int argc, const char** argv ){
	Mat img = imread("sample1.jpg", CV_LOAD_IMAGE_UNCHANGED); 
	displayImage(img);
	getBoundingBoxes(img);
	string ty =  type2str( img.type() );
  	cout << "Matrix: " << ty.c_str() << '\t' << endl;

	return 0;
}