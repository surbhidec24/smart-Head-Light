#include <iostream>
#include <vector>
#include "coreFuncs.hpp"
#include "inout.h"
#include "debug.hpp"
#include "Tracker.h"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <cmath>

using namespace std;
using namespace cv;

int main( int argc, const char** argv ){
  	int num = trackCars("output2.avi");
  	cout << num << endl;
  	//trackCars("output4.avi");

  	// plt::plot({1,2,3,4});
   //  plt::show();
	return 0;
}