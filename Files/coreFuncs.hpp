#ifndef COREFUNCS
#define COREFUNCS
#pragma once

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <iostream>
#include <vector>
#include <math.h>

using namespace cv;
using namespace std;

struct minmax;
vector<Rect> getBoundingBoxes(Mat img);
vector<Rect> getBlobs(Mat img);
//void getShape(Mat img_bw, Mat &finalVectors, int &idx);
// void tracer(Mat img_bw, Mat &tempMatrix, int start_i, int start_j, int init, 
// 			int idx, Mat &finalVectors);
// void tracerUtil(Mat img_bw, Mat &tempMatrix , int start_i, int start_j, int i, int j, int init, 
// 			minmax *newVector, int idx);
void getPairs(vector<Rect> &vList, vector<Rect> &pairs);
void getShape(Mat img_bw, vector<Rect> &blobList);
void tracer(Mat img_bw, Mat &tempMatrix, int start_i, int start_j, int init, vector<Rect> &blobList);
void tracerUtil(Mat img_bw, Mat &tempMatrix , int start_i, int start_j, int i, int j, int init, 
			minmax *newVector);

#endif