#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/video/video.hpp>
#include <iostream>
#include <vector>
#include <math.h>

using namespace std;
using namespace cv;

class Tracker{ 
	public: 
		KalmanFilter kf;
		int x,y,width,height;

	public:
		Tracker();
		void Tracker(KalmanFilter kf, int x, int y, int width, int height); 
		void updateTracker(int x, int y, int width, int height);
}