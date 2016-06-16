#include <iostream>
#include <vector>
#include "coreFuncs.hpp"
#include "inout.h"
#include "debug.hpp"
#include "Tracker.h"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

int main( int argc, const char** argv ){
  	//trackCars("output2.avi");
  	trackCars("output4.avi");
	return 0;
}