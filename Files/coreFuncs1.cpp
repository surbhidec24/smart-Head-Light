#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <iostream>
#include <vector>
#include "coreFuncs.hpp"
#include <math.h>
#include "inout.h"
#include "debug.hpp"

using namespace cv;
using namespace std;

#define Hi 255
#define Low 0
#define TA_l 20
#define TA_h 150
#define TJ 5
#define TX_l 20
#define TX_h 150
#define TW 5
#define TH 5

int x[] = {0,1,1,1,0,-1,-1,-1}; 
int y[] = {1,1,0,-1,-1,-1,0,1}; 

struct minmax{
	int min_x, min_y, max_x, max_y;
};

vector<Rect> getBlobs(Mat img){
	int height = img.rows;
	int width = img.cols;

	Mat img_bw;
	convert2Binary(img,img_bw);

	vector<Rect> blobList;
	getShape(img_bw,blobList);

	return blobList;
}

void getShape(Mat img_bw, vector<Rect> &blobList){
	int height = img_bw.rows;
	int width = img_bw.cols;
	int count, init; 

	Mat tempMatrix = Mat::zeros(height, width, CV_32F);

	for (int i = 0; i<height; i++){
    	for (int j = 0; j<width; j++){

    		if(img_bw.at<uchar>(i,j) == Hi && tempMatrix.at<float>(i,j) == 0){
    			/*taking care of points inside the contour*/
    			if((i-1 >=0) && (tempMatrix.at<float>(i-1,j) >0)){
    				tempMatrix.at<float>(i,j) = 1;
    				continue;
    			}

    			tempMatrix.at<float>(i,j) = 1;
        		init = 7;
        		count = 0;

       			/*look for white neighbouring pixel.*/
        		while(img_bw.at<uchar>(i+x[init],j+y[init])!= Hi && count <8){
        			init = (init+1)%8;
            		count = count+1;
        		}

				/*if not a lone point, call tracer function*/
            	if (count < 8)
            		tracer(img_bw, tempMatrix, i+x[init], j+y[init], init, blobList);
    		}
    	}
    }
}

void tracer(Mat img_bw, Mat &tempMatrix, int start_i, int start_j, int init, vector<Rect> &blobList){
	int width, height;

	//change to pair of points 2d point. 
	minmax *newVector = new minmax;
	newVector->min_x = newVector->max_x = start_i;
	newVector->min_y = newVector->max_y = start_j;

	tempMatrix.at<float>(start_i,start_j) = 1;
    tracerUtil(img_bw, tempMatrix ,start_i, start_j, start_i, start_j, init, newVector);

    width = newVector->max_y - newVector -> min_y;
    height = newVector->max_x - newVector->min_x;

    Rect tempRect;
    tempRect.x = newVector->min_x;
    tempRect.y = newVector->min_y;
    tempRect.width = width;
    tempRect.height = height;

    blobList.push_back(tempRect);
}

void tracerUtil(Mat img_bw, Mat &tempMatrix , int start_i, int start_j, int i, int j, int init, 
			minmax *newVector){
	
	/*start with d+2(mod 8)*/
	init = (init+6)%8;

	int count = 0;

	if(i < newVector->min_x)
		 newVector->min_x = i;

	if(i > newVector->max_x)
		 newVector->max_x = i;

	if(j < newVector->min_y)
		 newVector->min_y = j;

	if(j > newVector->max_y)
		 newVector->max_y = j;

	while(img_bw.at<uchar>(i+x[init],j+y[init]) != Hi && count <8) {
		init = (init + 1) % 8;
    	count = count+1;
	}

	/*termination condition*/
	if(start_i == i+x[init] && start_j == j+y[init])
    	return;

    if(count<8 && tempMatrix.at<float>(i + x[init],j + y[init])!=1){
    	tempMatrix.at<float>(i + x[init],j + y[init]) = 1;
    	tracerUtil(img_bw, tempMatrix, start_i, start_j, i+x[init], j+y[init], init, newVector);
    }
}

void getPairs(Mat &vList, int idx, vector<Rect> &pairs){
	for (int i =0; i<idx-1; i++){
		if(vList.at<float>(i,0) == 1)
			continue;

		int area = vList.at<float>(i,3) * vList.at<float>(i,4);
		if(area <TA_l || area > TA_h){
			vList.at<float>(i,0) = 1;
			continue;
		}

		Rect tempRect;
		int j = i+1;
		while(j<idx-1 && abs(vList.at<float>(j,1)-vList.at<float>(i,1))<TJ && vList.at<float>(i,0) == 0){
			int distance = abs(vList.at<float>(j,2)-vList.at<float>(i,2));
			int width = abs(vList.at<float>(j,3)-vList.at<float>(i,3));
			int height = abs(vList.at<float>(j,4)-vList.at<float>(i,4));
			if(distance >TX_l && distance <TX_h && width <TW && height < TH){
				tempRect.y = min(vList.at<float>(j,1),vList.at<float>(i,1));
				tempRect.x = min(vList.at<float>(j,2),vList.at<float>(i,2));
				tempRect.width = distance + max(vList.at<float>(j,3),vList.at<float>(i,3));
				tempRect.height = max(vList.at<float>(j,4),vList.at<float>(i,4));
				vList.at<float>(i,0) = 1;
				vList.at<float>(j,0) = 1;
				pairs.push_back(tempRect);
				break;
			}
			j = j+1;
		}
	}
}



void getShape(Mat img_bw, Mat &finalVectors, int &idx){
	int height = img_bw.rows;
	int width = img_bw.cols;
	int count, init; 

	Mat tempMatrix = Mat::zeros(height, width, CV_32F);

	for (int i = 0; i<height; i++){
    	for (int j = 0; j<width; j++){

    		if(img_bw.at<uchar>(i,j) == Hi && tempMatrix.at<float>(i,j) == 0){
    			/*taking care of points inside the contour*/
    			if((i-1 >=0) && (tempMatrix.at<float>(i-1,j) >0)){
    				tempMatrix.at<float>(i,j) = tempMatrix.at<float>(i-1,j);
    				continue;
    			}

    			tempMatrix.at<float>(i,j) = idx;
        		init = 7;
        		count = 0;

       			/*look for white neighbouring pixel.*/
        		while(img_bw.at<uchar>(i+x[init],j+y[init])!= Hi && count <8){
        			init = (init+1)%8;
            		count = count+1;
        		}

				/*if not a lone point, call tracer function*/
            	if (count < 8){
            		// displayImage(tempMatrix);
            		tracer(img_bw, tempMatrix, i+x[init], j+y[init], init, idx, finalVectors);
            		idx++;
            	}
    		}
    	}
    }
}



void tracer(Mat img_bw, Mat &tempMatrix, int start_i, int start_j, int init, 
			int idx, Mat &finalVectors){
	int width, height;

	minmax *newVector = new minmax;
	newVector->min_x = newVector->max_x = start_i;
	newVector->min_y = newVector->max_y = start_j;

	tempMatrix.at<float>(start_i,start_j) = idx;
    tracerUtil(img_bw, tempMatrix ,start_i, start_j, start_i, start_j, init, newVector, idx);

    width = newVector->max_y - newVector -> min_y;
    height = newVector->max_x - newVector->min_x;

    finalVectors.at<float> (idx-1,1) = newVector->min_x;
    finalVectors.at<float> (idx-1,2) = newVector->min_y;
    finalVectors.at<float> (idx-1,3) = width;
    finalVectors.at<float> (idx-1,4) = height;
}

void tracerUtil(Mat img_bw, Mat &tempMatrix , int start_i, int start_j, int i, int j, int init, 
			minmax *newVector, int idx){
	
	/*start with d+2(mod 8)*/
	init = (init+6)%8;

	int count = 0;

	if(i < newVector->min_x)
		 newVector->min_x = i;

	if(i > newVector->max_x)
		 newVector->max_x = i;

	if(j < newVector->min_y)
		 newVector->min_y = j;

	if(j > newVector->max_y)
		 newVector->max_y = j;

	while(img_bw.at<uchar>(i+x[init],j+y[init]) != Hi && count <8) {
		init = (init + 1) % 8;
    	count = count+1;
	}

	/*termination condition*/
	if(start_i == i+x[init] && start_j == j+y[init])
    	return;

    if(count<8 && tempMatrix.at<float>(i + x[init],j + y[init]) !=Hi){
    	tempMatrix.at<float>(i + x[init],j + y[init]) = idx;
    	tracerUtil(img_bw, tempMatrix, start_i, start_j, i+x[init], j+y[init], init, newVector, idx);
    }
}

void getPairs(Mat &vList, int idx, vector<Rect> &pairs){
	for (int i =0; i<idx-1; i++){
		if(vList.at<float>(i,0) == 1)
			continue;

		int area = vList.at<float>(i,3) * vList.at<float>(i,4);
		if(area <TA_l || area > TA_h){
			vList.at<float>(i,0) = 1;
			continue;
		}

		Rect tempRect;
		int j = i+1;
		while(j<idx-1 && abs(vList.at<float>(j,1)-vList.at<float>(i,1))<TJ && vList.at<float>(i,0) == 0){
			int distance = abs(vList.at<float>(j,2)-vList.at<float>(i,2));
			int width = abs(vList.at<float>(j,3)-vList.at<float>(i,3));
			int height = abs(vList.at<float>(j,4)-vList.at<float>(i,4));
			if(distance >TX_l && distance <TX_h && width <TW && height < TH){
				tempRect.y = min(vList.at<float>(j,1),vList.at<float>(i,1));
				tempRect.x = min(vList.at<float>(j,2),vList.at<float>(i,2));
				tempRect.width = distance + max(vList.at<float>(j,3),vList.at<float>(i,3));
				tempRect.height = max(vList.at<float>(j,4),vList.at<float>(i,4));
				vList.at<float>(i,0) = 1;
				vList.at<float>(j,0) = 1;
				pairs.push_back(tempRect);
				break;
			}
			j = j+1;
		}
	}
}