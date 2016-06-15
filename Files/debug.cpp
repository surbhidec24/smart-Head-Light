#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <iostream>
#include <vector>
#include "coreFuncs.hpp"
#include <math.h>
#include "inout.h"

using namespace cv;
using namespace std;

/*returns the datatype of matrix M, in the format: 
depth{U|S|F}C{No. of channels}*/
string type2str(int type) {
  string r;

  uchar depth = type & CV_MAT_DEPTH_MASK;
  uchar chans = 1 + (type >> CV_CN_SHIFT);

  switch ( depth ) {
    case CV_8U:  r = "8U"; break;
    case CV_8S:  r = "8S"; break;
    case CV_16U: r = "16U"; break;
    case CV_16S: r = "16S"; break;
    case CV_32S: r = "32S"; break;
    case CV_32F: r = "32F"; break;
    case CV_64F: r = "64F"; break;
    default:     r = "User"; break;
  }

  r += "C";
  r += (chans+'0');
  return r;
}

void printVectors(Mat finalVectors, int r, int c){
	for (int i = 0; i< r; i++){
		for (int j = 0; j<c; j++){
			cout << finalVectors.at<float>(i,j) << '\t';
		}
		cout << endl;
	}
}

void finalBoundingBoxes(vector<Rect> Pairs, Mat &image){
  Scalar color = Scalar(0,255,0);
  for (int i=0; i<Pairs.size(); i++){
    cout << Pairs[i].x << "\t" << Pairs[i].y << "\t" << Pairs[i].width << "\t" <<Pairs[i].height << endl;
    rectangle( image, Pairs[i], color, 1, 8, 0 );
  }
}