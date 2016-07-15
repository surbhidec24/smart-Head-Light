#include <iostream>
#include <vector>
#include "coreFuncs.hpp"
#include "Tracker.h"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <cmath>
#include <time.h>

using namespace std;
using namespace cv;

int main( int argc, const char** argv ){
	clock_t init, final;
	init=clock();
  	trackCars("output5.avi");
 //  	//readVideo("output5.avi");
  	final=clock()-init;
  	cout << (double)final / ((double)CLOCKS_PER_SEC);
	return 0;
}
