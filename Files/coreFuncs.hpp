#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <iostream>
#include <vector>
#include <math.h>

using namespace cv;
using namespace std;

struct minmax;
Mat getBoundingBoxes(Mat img, int &numPairs);
void getShape(Mat img_bw, Mat &finalVectors, int &idx);
void tracer(Mat img_bw, Mat &tempMatrix, int start_i, int start_j, int init, 
			int idx, Mat &finalVectors);
void tracerUtil(Mat img_bw, Mat &tempMatrix , int start_i, int start_j, int i, int j, int init, 
			minmax *newVector, int idx);
void getPairs(Mat &vList, int idx, int &count, Mat &pairs);
void getPairs_new(Mat &vList, int idx, int &count, vector<Rect> &pairs);