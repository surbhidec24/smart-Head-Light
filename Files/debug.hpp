#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <iostream>
#include <vector>
#include "coreFuncs.hpp"
#include <math.h>

using namespace cv;
using namespace std;

// void printList(vector<featVector> list_blobs);
string type2str(int type);
void printVectors(Mat finalVectors, int r, int c);
void printfBoundingBoxes(Mat finalVectors, string type, Mat &image, int r, int c, int numRows, int Hi);
void finalBoundingBoxes(Mat finalVectors, Mat &image, int r, int c, 
      int numRows, int Hi);