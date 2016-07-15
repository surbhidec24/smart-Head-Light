#define Low 0
#define TA_l 20
#define TA_h 150
#define TJ 5
#define TX_l 20
#define TX_h 150
#define TW 5
#define TH 5
#define thresh 100

void displayImage(Mat img){
	if (img.empty())
        cout << "Error : Image cannot be loaded..!!" << endl;
    namedWindow("MyWindow", CV_WINDOW_AUTOSIZE);
    imshow("MyWindow", img); 
	waitKey(0); 
	destroyWindow("MyWindow"); 
}

/*converts an 3 channel RGB to 1 channel binary image*/ 
void convert2Binary(Mat img, Mat &img_bw){
	Mat img_gray;
	cvtColor(img, img_gray,CV_RGB2GRAY);
    threshold(img_gray, img_bw, binThresh, Hi, THRESH_BINARY);
	imwrite("image_bw.jpg", img_bw);
}

void readVideoAndBox(string filename){
    VideoCapture capture(filename);
    Mat frame;

    if( !capture.isOpened() )
        throw "Error when reading steam_avi";

    namedWindow( "window", 1);
    for(; ;)
    {
        capture >> frame;
        if(frame.empty())
            break;
        int numPairs = 0;
        getBoundingBoxes(frame);
        imshow("window", frame);
        waitKey(1); 
    }
    waitKey(0);
}


void readVideoAndBox(string filename){
    VideoCapture capture(filename);
    Mat frame;

    if( !capture.isOpened() )
        throw "Error when reading steam_avi";

    namedWindow( "window", 1);
    for(; ;)
    {
        capture >> frame;
        if(frame.empty())
            break;
        int numPairs = 0;
        getBoundingBoxes(frame);
        imshow("window", frame);
        waitKey(1); 
    }
    waitKey(0);
}

int x[] = {0,1,1,1,0,-1,-1,-1}; 
int y[] = {1,1,0,-1,-1,-1,0,1}; 

vector<Rect> getBoundingBoxes(Mat img){

 	Mat img_bw;
	convert2Binary(img,img_bw);

	vector<Rect> blobList;
	getShape(img_bw,blobList);

	vector<Rect> pairs;
	getPairs(blobList,pairs);

	//finalBoundingBoxes(pairs, img);
	return pairs;
}

// vector<Rect> getBlobs(Mat img){

// 	Mat img_bw;
// 	convert2Binary(img,img_bw);

// 	vector<Rect> blobList;
// 	getShape(img_bw,blobList);

// 	//finalBoundingBoxes(blobList,img);
// 	return blobList;
// }

void getShape(Mat img_bw, vector<Rect> &blobList){
	int height = img_bw.rows;
	int width = img_bw.cols;
	int count, init; 

	Mat tempMatrix = Mat::zeros(height, width, CV_32F);

	for (int i = 0; i<height; i++){
    	for (int j = 0; j<width; j++){

    		if(img_bw.at<uchar>(i,j) == Hi && tempMatrix.at<float>(i,j) == 0){
    			/*taking care of points inside the contour*/
    			if((i-1 >=0) && (tempMatrix.at<float>(i-1,j) >0)){
    				tempMatrix.at<float>(i,j) = 1;
    				continue;
    			}

    			tempMatrix.at<float>(i,j) = 1;
        		init = 7;
        		count = 0;

       			/*look for white neighbouring pixel.*/
        		while(img_bw.at<uchar>(i+x[init],j+y[init])!= Hi && count <8){
        			init = (init+1)%8;
            		count = count+1;
        		}

				/*if not a lone point, call tracer function*/
            	if (count < 8)
            		tracer(img_bw, tempMatrix, i+x[init], j+y[init], init, blobList);
    		}
    	}
    }
}

void tracer(Mat img_bw, Mat &tempMatrix, int start_i, int start_j, int init, vector<Rect> &blobList){
	int width, height;

	/////////change to pair of points 2d point./////////
	///////correct the tracer x and y ///////
	Point myMin,myMax;
	myMin.x = myMax.x = start_i;
	myMin.y = myMax.y = start_j;

	tempMatrix.at<float>(start_i,start_j) = 1;
    tracerUtil(img_bw, tempMatrix ,start_i, start_j, start_i, start_j, init, myMin, myMax);

    width = myMax.y - myMin.y;
    height = myMax.x - myMin.x;

    Rect tempRect;
    tempRect.y = myMin.x;
    tempRect.x = myMin.y;
    tempRect.width = width;
    tempRect.height = height;

    blobList.push_back(tempRect);
}

void tracerUtil(Mat img_bw, Mat &tempMatrix , int start_i, int start_j, int i, int j, int init, 
			Point &myMin, Point &myMax){
	
	/*start with d+2(mod 8)*/
	init = (init+6)%8;

	int count = 0;

	if(i < myMin.x)
		 myMin.x = i;

	if(i > myMax.x)
		 myMax.x = i;

	if(j < myMin.y)
		 myMin.y = j;

	if(j > myMax.y)
		 myMax.y = j;

	while(img_bw.at<uchar>(i+x[init],j+y[init]) != Hi && count <8) {
		init = (init + 1) % 8;
    	count = count+1;
	}

	/*termination condition*/
	if(start_i == i+x[init] && start_j == j+y[init])
    	return;

    if(count<8 && tempMatrix.at<float>(i + x[init],j + y[init])!=1){
    	tempMatrix.at<float>(i + x[init],j + y[init]) = 1;
    	tracerUtil(img_bw, tempMatrix, start_i, start_j, i+x[init], j+y[init], init, myMin, myMax);
    }
}

