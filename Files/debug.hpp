#ifndef DEBUGGER
#define DEBUGGER
#pragma once

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <iostream>
#include <vector>
#include "coreFuncs.hpp"
#include <math.h>

using namespace cv;
using namespace std;

string type2str(int type);
void printfBoundingBoxes(Mat finalVectors, string type, Mat &image, int r, int c, int numRows, int Hi);
void finalBoundingBoxes(vector<Rect> Pairs, Mat &image);

#endif