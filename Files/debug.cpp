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

// void printList(vector<featVector> list_blobs){
// 	vector<featVector>::iterator iter;
// 	vector<int>::iterator nIter;
	
//     for (iter = list_blobs.begin(); iter != list_blobs.end(); ++iter){
//     	cout << iter->min_x <<'\t' << iter->max_x << '\t' << iter->min_y << '\t' << iter->max_y << '\t' << iter->height 
//     	<<'\t' << iter->width << endl;
//     	for (nIter = (*iter).shape.begin(); nIter != (*iter).shape.end(); ++nIter)
//     		cout << (*nIter);
//     	cout <<endl;
//     }
// }

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

void printfBoundingBoxes(Mat finalVectors, string type, Mat &image, int r, int c, int numRows, int Hi){
  int x,y,h,w;  
  for (int i = r; i<numRows; i++){
    y = finalVectors.at<float>(i,c);
    x = finalVectors.at<float>(i,c+1);
    w = finalVectors.at<float>(i,c+2);
    h = finalVectors.at<float>(i,c+3);

    /*debugging purpose, to get bounding box around blobs*/
    for (int j = y; j< y+h; j++){
      image.at<float>(j,x) = Hi;
      image.at<float>(j,x+w) = Hi;
    }
    for (int j = x; j< x+w; j++){
      image.at<float>(y,j) = Hi;
      image.at<float>(y+h,j) = Hi;
    }
  }
  displayImage(image);
} 