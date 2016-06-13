#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <iostream>
#include <vector>
#include "coreFuncs.hpp"
#include <math.h>
#include <opencv2/video/video.hpp>

using namespace cv;
using namespace std;

void readVideo(string filename);
void displayImage(Mat img);
void convert2Binary(Mat img, Mat &img_bw);
void readVideoAndBox(string filename);
void track(string filename);