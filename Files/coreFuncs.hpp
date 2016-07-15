#ifndef COREFUNCS
#define COREFUNCS
#pragma once

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <iostream>
#include <vector>
#include <math.h>
#include "Tracker.h"
#include <stdbool.h>

using namespace cv;
using namespace std;

vector<Rect> getBlobs(Mat img);
void getPairs(vector<Rect> &vList, vector<Rect> &pairs);
vector<Rect> thresh_callback(int, void* );
int trackCars(string filename);
bool verify(Rect myRect);
int findNext(Tracker myTracker, vector<Rect>bBoxes);
void onMouse( int event, int x, int y, int, void* );


#endif