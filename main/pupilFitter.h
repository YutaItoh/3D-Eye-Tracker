#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/video/background_segm.hpp>


#include <iostream>
#include <fstream>
#include <iostream> // for standard I/O
//#include <dirent.h>
#include <string>
#include <vector>
#include <sys/stat.h>
#include <time.h>
#include <sys/timeb.h>
#include <ctime>

#include "fit_ellipse.h"

using namespace std;
using namespace cv;

class PupilFitter{
public:
	PupilFitter(){
#if CV_MAJOR_VERSION ==3
		//initialize background subtractors
		pMOG = createBackgroundSubtractorMOG2();
		pMOG2_1 = createBackgroundSubtractorMOG2();
		pMOG2_2 = createBackgroundSubtractorMOG2();
		pGMG    = createBackgroundSubtractorMOG2();
#else
		//initialize background subtractors
		pMOG = new BackgroundSubtractorMOG();
		pMOG2_1 = new BackgroundSubtractorMOG2();
		pMOG2_2 = new BackgroundSubtractorMOG2();
		pGMG = new BackgroundSubtractorGMG();
#endif

		threshDebug = true;
	};
	~PupilFitter(){};

	void setDebug(bool threshDebug0){
		threshDebug = threshDebug0;
	};
/**
Fits an ellipse to a pupil area in an image
@param gray BGR input image (converted to grayscale during search process)
@return a RotatedRect representing the pupil ellipse, returns RotatedRect with all 0s if ellipse was not found
*/
	bool pupilAreaFitRR(Mat &gray, RotatedRect &rr, vector<Point2f> &allPts,
	int pupilSearchAreaIn = 20, int pupilSearchXMinIn = 0, int pupilSearchYMinIn = 0,
	int lowThresholdCannyIn = 10, int highThresholdCannyIn = 30,
		int sizeIn = 240, int darkestPixelL1In = 10, int darkestPixelL2In = 20) 
	{

		//global params (magic numbers) for setting, these should be set per-user, see main for params
		//default values
		lowThresholdCanny = lowThresholdCannyIn; //default 10: for detecting dark (low contrast) parts of pupil
		highThresholdCanny = highThresholdCannyIn; //default 30: for detecting lighter (high contrast) parts of pupil
		size = sizeIn; //default 280: max L/H of pupil
		darkestPixelL1 = darkestPixelL1In; //default 10: for setting low darkness threshold
		darkestPixelL2 = darkestPixelL2In; //default 20: for setting high darkness threshold
		pupilSearchArea = pupilSearchAreaIn; //default 20: for setting min size of pupil in pixels / 2 
		pupilSearchXMin = pupilSearchXMinIn; //default 0: distance from left side of image to start pupil search  
		pupilSearchYMin = pupilSearchYMinIn; //default 0: distance from right side of image to start pupil search  
		erodeOn = true; //perform erode operation: turn off for one-offs, where eroding the image may actually hurt accuracy

		//for timing funcitons
		unsigned long long Int64 = 0;
		clock_t Start = clock();

		//find pupil
		Point darkestPixelConfirm = getDarkestPixelArea(gray);


		//correct bounds
		darkestPixelConfirm = correctBounds(darkestPixelConfirm, size);

		//find darkest pixel (for thresholding
		int darkestPixel = getDarkestPixel(gray(cv::Rect(darkestPixelConfirm.x, darkestPixelConfirm.y, size, size)));

		int kernel_size = 3;
		int scale = 1;
		int delta = 0;
		int ddepth = CV_8U;

		int erosion_size = 3;
		int erosion_type = MORPH_ELLIPSE;
		Mat element = getStructuringElement(erosion_type,
			Size(2 * erosion_size + 1, 2 * erosion_size + 1),
			Point(erosion_size, erosion_size));

		/// Apply the erosion operation
		if (erodeOn) {
			erode(gray, gray, element);
		}

		//set ROI and thresh for testing
		threshold(gray(cv::Rect(darkestPixelConfirm.x, darkestPixelConfirm.y, size, size)), threshLow, (darkestPixel + darkestPixelL1), 255, 1);

		if (threshDebug) {
			//test threshing
			imshow("threshLow", threshLow);
			//waitKey(1);
		}

		//Find contours
		std::vector<std::vector<cv::Point>> contoursLow;
		cv::findContours(threshLow, contoursLow, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

		//get biggest contours (pupils)
		int biggest = getBiggest(contoursLow).at(0);

		//get bounding rect center 
		Rect minPts = boundingRect(contoursLow.at(biggest));
		minPts = Rect(minPts.x + darkestPixelConfirm.x, minPts.y + darkestPixelConfirm.y, minPts.width, minPts.height);
		Point rectCenter(minPts.x + minPts.width / 2, minPts.y + minPts.height / 2);

		//take height or width as max, whichever is bigger
		int max = minPts.height;
		if (max < minPts.width) {
			max = minPts.width;
		}
		if (max <= 0) { //check for 0 size case
			max = size;
		}

		//max size of pupil ROI
		int size2 = size;

		//Thresh 2
		threshold(gray(cv::Rect(darkestPixelConfirm.x, darkestPixelConfirm.y, size2, size2)), threshHigh, (darkestPixel + darkestPixelL2), 255, 1);

		if (threshDebug) {
			//test threshing
			imshow("threshMid", threshHigh);
			//waitKey(1);
		}

		//contours for high thresh
		std::vector<std::vector<cv::Point>> contoursHigh;
		cv::findContours(threshHigh, contoursHigh, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

		int biggestHigh = getBiggest(contoursHigh).at(0);

		//convert back to 3 channel for drawing
		cv::cvtColor(gray, gray, CV_GRAY2BGR);

		Scalar colorC = Scalar(0, 255, 0);
		Scalar colorE = Scalar(0, 0, 255);

		//canny parameters, other params are globally set
		int edgeThresh = 1;
		int const max_lowThreshold = 100;
		int ratio = 3;
		int kernel2 = 3;

		//run Canny to get best candiate points from contours 
		Canny(gray(cv::Rect(darkestPixelConfirm.x, darkestPixelConfirm.y, size2, size2)), thresh3, lowThresholdCanny, lowThresholdCanny*ratio, kernel2);
		Canny(gray(cv::Rect(darkestPixelConfirm.x, darkestPixelConfirm.y, size2, size2)), thresh4, highThresholdCanny, highThresholdCanny*ratio, kernel2);

		if (threshDebug) {
			imshow("cannyLow", thresh3);
			//waitKey(1);
			imshow("cannyHigh", thresh4);
			//waitKey(1);
		}

		//holds sets of candidate points for different points throughout refinement
	//	vector<Point2f> allPts;
		allPts.resize(0);
		vector<Point2f> allPts2;
		vector<Point2f> allPtsHigh;

		//logical AND of contours and canny images
		allPts = getCandidates(contoursLow, biggest, thresh3, false);
		allPtsHigh = getCandidates(contoursHigh, biggestHigh, thresh4, false);

		//merge remaining points for low and high point lists 
		allPts.insert(allPts.end(), allPtsHigh.begin(), allPtsHigh.end());
		std::vector<std::vector<cv::Point2f>> allPtsWithOutliers;
		allPtsWithOutliers.push_back(allPts);

		//convert to gray for refinement
		cv::cvtColor(gray, gray, CV_BGR2GRAY);

		//refine points based on line fitting - Thanks Yuta! 
		if (allPts.size() > 5) {
			allPts = refinePoints(allPts, gray(cv::Rect(darkestPixelConfirm.x, darkestPixelConfirm.y, size2, size2)), 8, 2);
		}
		else {
			return false;
		}

		//convert back to 3 channel for drawing if necessary
		cv::cvtColor(gray, gray, CV_GRAY2BGR);

		//remove outliers via ellipse method, basically a logical AND of candidate points with a drawn ellipse: great for removing outliers
		thresh3 = Mat::zeros(size2, size2, CV_8U); //black mat
		if (allPts.size() > 5) {
			RotatedRect ellipseRaw = fitEllipse(allPts);

			if (ellipseRaw.center.x < 300 && ellipseRaw.center.x > 0 && ellipseRaw.angle > 5) {
				//if possible and within bounds, draw
				ellipse(thresh3, ellipseRaw, 255, 2, 8); //draw white ellipse 
			}
			allPts2 = getCandidates(allPtsWithOutliers, 0, thresh3, false);
		}
		else {
			return false;
		}

		//re-run the ellipse method on a fitted ellipse, but with the original set of points: great for re-including inliers
		thresh3 = Mat::zeros(size2, size2, CV_8U); //black mat
		if (allPts2.size() > 5 && allPtsWithOutliers.size() > 0 && allPtsWithOutliers.at(0).size() > 5) {
			RotatedRect ellipseRaw = fitEllipse(allPts2);

			//check for impossible ellipses
			if (ellipseRaw.center.x < size2 && ellipseRaw.center.x > 0 && ellipseRaw.angle > 5) {
				//if possible and within bounds, draw
				ellipse(thresh3, ellipseRaw, 255, 2, 8); //draw white ellipse 
			}
			allPts = getCandidates(allPtsWithOutliers, 0, thresh3, false);
		}
		else {
			return false;
		}

		//convert to gray for refinement
		cv::cvtColor(gray, gray, CV_BGR2GRAY);

		//refine points based on line fitting - Thanks Yuta! 
		if (allPts.size() > 5) {
			allPts = refinePoints(allPts, gray(cv::Rect(darkestPixelConfirm.x, darkestPixelConfirm.y, size2, size2)), 8, 2);
		}
		else {
			return false;
		}

		//re-refine with another ellipse fit
		if (allPts.size() > 5) {
			thresh3 = Mat::zeros(frameHeight, frameWidth, CV_8U);
			RotatedRect ellipseRaw = fitEllipse(allPts);
			ellipse(thresh3, ellipseRaw, 255, 1, 8);

			std::vector<std::vector<cv::Point> > ellipseContour;
			cv::findContours(thresh3, ellipseContour, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

			if (ellipseContour.size() > 0 && ellipseContour.at(0).size() > 5) {
				thresh3 = Mat::zeros(frameHeight, frameWidth, CV_8U);
				RotatedRect ellipseRaw = fitEllipse(allPts);
				ellipse(thresh3, ellipseRaw, 255, 1, 8);
				allPts = refinePoints(ellipseContour.at(0), thresh3, 10, 2);
			}

			if (ellipseContour.size() > 0 && ellipseContour.at(0).size() > 5) {
				thresh3 = Mat::zeros(frameHeight, frameWidth, CV_8U);
				RotatedRect ellipseRaw = fitEllipse(allPts);
				ellipse(thresh3, ellipseRaw, 255, 1, 8);
				allPts = refinePoints(ellipseContour.at(0), thresh3, 8, 1);
			}
		}
		else {
			return false;
		}


		//returns RotatedRect with all 0s if ellipse was not found
		RotatedRect ellipseCorrect = RotatedRect(Point2f(0, 0), Size2f(0, 0), 0);


		//Regular Ellipse
		if (allPts.size() > 5) {

			RotatedRect ellipseRaw = fitEllipse(allPts);
			ellipseCorrect = RotatedRect(Point2f(ellipseRaw.center.x + darkestPixelConfirm.x, ellipseRaw.center.y + darkestPixelConfirm.y), ellipseRaw.size, ellipseRaw.angle);

		}
		else {
			return false;
		}


		///waitKey(1);
		for (int i = 0; i < allPts.size(); i++) {
			allPts[i].x += darkestPixelConfirm.x;
			allPts[i].y += darkestPixelConfirm.y;
		}
		rr = ellipseCorrect;
		return true;
	}

private:
//global variables  

//Mats for holding various images
Mat frame;
Mat frame1;
Mat frame2;
Mat edges;

//Mats for holding ROI images
Mat thresh1;
Mat thresh2;
Mat thresh3;
Mat thresh4;
Mat threshLow;
Mat threshMid;
Mat threshHigh;
Mat temp;

//image height/width (note that the algorithm isn't adapted to 320x240 yet!!)
int frameHeight = 480;
int frameWidth = 640;

//For glint/RANSAC code (currently unused)
std::vector < cv::KeyPoint > glints_;
Mat sobelX;
Mat sobelY;

//Mats for other functions: motion detection, resizing, etc
Mat resizeF1;
Mat resizeF2;
Mat gray, detected_edges;
Mat fgMaskMOG; //fg mask generated by MOG method  
Mat fgMaskMOG2_1; //fg mask fg mask generated by MOG2 method  
Mat fgMaskMOG2_2; //fg mask fg mask generated by MOG2 method  
Mat fgMaskGMG; //fg mask fg mask generated by MOG2 method  

Ptr< BackgroundSubtractor> pMOG; //MOG Background subtractor  
Ptr< BackgroundSubtractor> pMOG2_1; //MOG2 Background subtractor  
Ptr< BackgroundSubtractor> pMOG2_2; //MOG2 Background subtractor  
#if CV_MAJOR_VERSION ==3
Ptr< BackgroundSubtractor> pGMG; //MOG2 Background subtractor  
#else
Ptr< BackgroundSubtractorGMG> pGMG; //MOG2 Background subtractor  
#endif
cv::SimpleBlobDetector::Params params;

//global params for setting, these should be set per-user
int lowThresholdCanny = 10; //for detecting dark (low contrast) parts of pupil
int highThresholdCanny = 60; //for detecting lighter (high contrast) parts of pupil
int size = 280;//max L/H of pupil
int darkestPixelL1 = 8; //for setting 
int darkestPixelL2 = 20;
int pupilSearchArea = 20;
int pupilSearchXMin = 0;
int pupilSearchYMin = 0;
bool erodeOn = true;

//thickness for ANDing candidate points with Canny images: thicker = more candidates
int thickness = 3;
bool threshDebug = false;

//rect for comparing previous frame, used in bad ellipse filtering process
RotatedRect previousRect = RotatedRect(Point2f(0, 0), Size2f(0, 0), 0);

/**
Finds the approximate darkets pixel, used on ROI images generated by getDarkestPixel area
@param I input image (converted to grayscale during search process)
@param I2 copy of input image onto which green block of pixels is drawn (BGR), null ok
@return a point within the pupil region
*/
int getDarkestPixel(Mat& I)
{
	// accept only char type matrices
	CV_Assert(I.depth() == CV_8U);

	int channels = I.channels();

	int min = 255;

	int nRows = I.rows;
	int nCols = I.cols * channels;

	int nRowsT = I.rows;
	int nColsT = I.cols * channels;

	if (I.isContinuous())
	{
		nCols *= nRows;
		nRows = 1;
	}

	int i, j;
	uchar* p;
	for (i = 0; i < nRows; i = i + 5)
	{
		p = I.ptr<uchar>(i);
		for (j = 0; j < nCols; j = j + 5)
		{
			if (p[j] < min){
				min = p[j];
			}
		}
	}
	return min;
}

void rmGlints(Mat& I){
	// accept only char type matrices



	CV_Assert(I.depth() == CV_8U);

	temp = I.clone();

	int channels = I.channels();

	int min = 130;

	int nRows = I.rows;
	int nCols = I.cols * channels;

	int nRowsT = I.rows;
	int nColsT = I.cols * channels;

	if (I.isContinuous())
	{
		nCols *= nRows;
		nRows = 1;
	}

	int i, j;
	uchar* p;
	uchar* p2;

	int spacing = 10;

	for (i = 0; i < nRows; i = i + 1)
	{
		p = I.ptr<uchar>(i);
		p2 = temp.ptr<uchar>(i);

		for (j = 0; j < nCols; j = j + 1)
		{
			if (p[j] > min){
				p[j] = (p2[j - 10] + p2[j + 10]) / 2;
			}
		}
	}
}

/**
Finds a square area of dark pixels in the image
@param I input image (converted to grayscale during search process)
@param I2 copy of input image onto which green block of pixels is drawn (BGR), null ok
@return a point within the pupil region
*/
Point getDarkestPixelArea(Mat& I, Mat& I2)
{
	cv::cvtColor(I, I, CV_BGR2GRAY);

	// accept only char type matrices
	CV_Assert(I.depth() == CV_8U);

	Point ROI;

	int channels = I.channels();

	//for searching image
	int sArea = 20; //bound of outer search in any direction
	int outerSearchDivisor = 4; //sets spacing of outer search, equal to sArea*2/outerSearchDivisor

	//darkness calculation
	int width = 10; //width of darkness search area
	int searchDivisor = 2;

	//stdev calculation
	int widthSmall = width;
	int searchDivisorDev = widthSmall / 5;

	int min = 255;
	int areaMin = 255 * (9 * searchDivisor*searchDivisor);
	float stDevMin = 1000;

	int count = 0;

	bool draw = true;
	int finalColorCount = 0;

	for (int i = sArea * width / sArea + pupilSearchYMin; i < I.rows - sArea* width / sArea; i = i + sArea / outerSearchDivisor){
		for (int j = sArea* width / sArea + pupilSearchXMin; j < I.cols - sArea* width / sArea; j = j + sArea / outerSearchDivisor){

			int tempSum = 0; //holds current sum of pixel intensities
			float tempStDev = 1000;

			int colorCount = 0; //counts the number of pixels summed

			//darkness testing for single square
			for (int d = -width; d < width + 1; d = d + width / searchDivisor){
				for (int c = -width; c < width + 1; c = c + width / searchDivisor){

					if (d == -width&&c == -width || d == width&&c == -width || d == -width&&c == width || d == width&&c == width){
						//no comparison at corners
					}
					else{
						tempSum += I.at<uchar>(i + d, j + c);
					}

					//for efficiency, exit if darkness > current 
					if (tempSum > areaMin){
						c = 10000;
						d = 10000;
					}

					colorCount++;
				}
			}//end darkness calculation

			//color with darkness level (heatmap)
			//if ((255 * colorCount - tempSum) / colorCount > 220){
			//	I2.at<Vec3b>(i, j)[0] = 0;
			//	I2.at<Vec3b>(i, j)[1] = 0;
			//	I2.at<Vec3b>(i, j)[2] = (255 * colorCount - tempSum) / colorCount;
			//}

			//is darker than last calculated area?
			if (tempSum < areaMin){

				//progress to stdev calculation if area was darker
				//float stDev = 0;
				//vector<float> data;

				//for (int d2 = -widthSmall; d2 < widthSmall + 1; d2 = d2 + widthSmall / searchDivisorDev){
				//	for (int c2 = -widthSmall; c2 < widthSmall + 1; c2 = c2 + widthSmall / searchDivisorDev){
				//		data.push_back(I.at<uchar>(i + d2, j + c2));
				//		//I2.at<Vec3b>(i, j)[0] = 0;
				//		//I2.at<Vec3b>(i, j)[1] = 0;
				//		//I2.at<Vec3b>(i, j)[2] = 255;
				//	}
				//}
				//tempStDev = standard_deviation(&data[0], data.size());

				//in our videos, pupils don't exceed y>160 or x>530, remove for videos where pupil could be anywhere on the screen 
				if (i > 50 && j < 530){

					ROI = Point(j, i);
					//cout << "tempsum = " << tempSum << " @ " << j << ", " << i << endl;
					areaMin = tempSum;
					count++;

					finalColorCount = colorCount;

					//color points that are progressively darker
					//I2.at<Vec3b>(ROI)[0] = 0;
					//I2.at<Vec3b>(ROI)[1] = 0;
					//I2.at<Vec3b>(ROI)[2] = 255;
				}

				stDevMin = tempStDev;
			}

		}//end outerX for
	}//end outerY for

	//std::cout << "min avg pixel value was " << areaMin / finalColorCount;

	//float stDev = 0;
	//vector<float> data;

	//double test = stDevMin;
	//cout.precision(5);
	//cout << "stdev: " << fixed << test << "  darkness: " << areaMin << endl;

	//only draw if image was passed to I2
	if (&I2 != nullptr){
		//draw pupil marker
		for (int d2 = -widthSmall; d2 < widthSmall + 1; d2 = d2 + widthSmall / searchDivisorDev / 2){
			for (int c2 = -widthSmall; c2 < widthSmall + 1; c2 = c2 + widthSmall / searchDivisorDev / 2){

				if (d2 == -width&&c2 == -width || d2 == width&&c2 == -width || d2 == -width&&c2 == width || d2 == width&&c2 == width){
					//do nothing
				}
				else if (areaMin / finalColorCount < 80 && areaMin / finalColorCount > 0){
					I2.at<Vec3b>(ROI.y + c2, ROI.x + d2)[0] = 15;
					I2.at<Vec3b>(ROI.y + c2, ROI.x + d2)[1] = 255;
					I2.at<Vec3b>(ROI.y + c2, ROI.x + d2)[2] = 15;
				}
			}
		}
	}

	return ROI;
}

/**
Finds a square area of dark pixels in the image
@param I input image (converted to grayscale during search process)
@param I2 copy of input image onto which green block of pixels is drawn (BGR), null ok
@return a point within the pupil region
*/
Point getDarkestPixelArea(Mat& I)
{
	assert(I.channels() == 3 || I.channels() == 1);
	if (I.channels() == 3) {
		cv::cvtColor(I, I, CV_BGR2GRAY);
	}

	// accept only char type matrices
	CV_Assert(I.depth() == CV_8U);

	Point ROI;

	int channels = I.channels();

	//for searching image
	int sArea = 20; //bound of outer search in any direction
	int outerSearchDivisor = 2; //sets spacing of outer search, equal to sArea*2/outerSearchDivisor

	//darkness calculation
	int width = 30; //width of darkness search area (default 20)
	int searchDivisor = 3;

	//stdev calculation
	int widthSmall = width;
	int searchDivisorDev = widthSmall / 5;

	int min = 255;
	int areaMin = 255 * (9 * searchDivisor*searchDivisor);
	float stDevMin = 1000;

	int count = 0;

	bool draw = true;
	int finalColorCount = 0;

	for (int i = sArea * width / sArea + pupilSearchYMin; i < I.rows - sArea* width / sArea; i = i + sArea / outerSearchDivisor){
		for (int j = sArea* width / sArea + pupilSearchXMin; j < I.cols - sArea* width / sArea; j = j + sArea / outerSearchDivisor){

			int tempSum = 0; //holds current sum of pixel intensities
			float tempStDev = 1000;

			int colorCount = 0; //counts the number of pixels summed

			//darkness testing for single square
			for (int d = -width; d < width + 1; d = d + width / searchDivisor){
				for (int c = -width; c < width + 1; c = c + width / searchDivisor){

					if (d == -width&&c == -width || d == width&&c == -width || d == -width&&c == width || d == width&&c == width){
						//no comparison at corners
					}
					else{
						tempSum += I.at<uchar>(i + d, j + c);
					}

					//for efficiency, exit if darkness > current 
					if (tempSum > areaMin){
						c = 10000;
						d = 10000;
					}

					colorCount++;
				}
			}//end darkness calculation

			//color with darkness level (heatmap)
			//if ((255 * colorCount - tempSum) / colorCount > 220){
			//	I2.at<Vec3b>(i, j)[0] = 0;
			//	I2.at<Vec3b>(i, j)[1] = 0;
			//	I2.at<Vec3b>(i, j)[2] = (255 * colorCount - tempSum) / colorCount;
			//}

			//is darker than last calculated area?
			if (tempSum < areaMin){

				//progress to stdev calculation if area was darker
				//float stDev = 0;
				//vector<float> data;

				//for (int d2 = -widthSmall; d2 < widthSmall + 1; d2 = d2 + widthSmall / searchDivisorDev){
				//	for (int c2 = -widthSmall; c2 < widthSmall + 1; c2 = c2 + widthSmall / searchDivisorDev){
				//		data.push_back(I.at<uchar>(i + d2, j + c2));
				//		//I2.at<Vec3b>(i, j)[0] = 0;
				//		//I2.at<Vec3b>(i, j)[1] = 0;
				//		//I2.at<Vec3b>(i, j)[2] = 255;
				//	}
				//}
				//tempStDev = standard_deviation(&data[0], data.size());

				//in our videos, pupils don't exceed y>160 or x>530, remove for videos where pupil could be anywhere on the screen 
				if (i > 60 && j < 530){

					ROI = Point(j, i);
					//cout << "tempsum = " << tempSum << " @ " << j << ", " << i << endl;
					areaMin = tempSum;
					count++;

					finalColorCount = colorCount;

					//color points that are progressively darker
					//I2.at<Vec3b>(ROI)[0] = 0;
					//I2.at<Vec3b>(ROI)[1] = 0;
					//I2.at<Vec3b>(ROI)[2] = 255;
				}

				stDevMin = tempStDev;
			}

		}//end outerX for
	}//end outerY for

	//std::cout << "min avg pixel value was " << areaMin / finalColorCount;

	//float stDev = 0;
	//vector<float> data;

	//double test = stDevMin;
	//cout.precision(5);
	//cout << "stdev: " << fixed << test << "  darkness: " << areaMin << endl;

	//cv::cvtColor(I, I, CV_GRAY2BGR);

	return ROI;
}

vector<Point> multiFit(vector<Point> pointList){

	const size_t spacing = 3;
	const size_t numPoints = (pointList.size() / (3*spacing)) - 1;

	//holds an average center and average max radius
	vector<Point> output;

	//

	for (size_t i = 0; i < pointList.size() - numPoints * spacing; i = i + 2)
	{
		vector<Point> tempTen;

		//put numPoints worth of points into an array to pass to fitEllipse
		for (int j = 0; j < numPoints; j++){
			tempTen.push_back(pointList.at(i + j*spacing));
		}
		//cout << "tempTen length: " << tempTen.size() << endl;
		if (tempTen.size()>0){
			//output.push_back(tempTen);
		}
		//output.push_back(fitEllipse(tempTen));

		//cout << tempTen.size() << endl;
	}

	return output;
}

Mat markPupil(Mat &gray){

	frame2 = gray.clone();

	Point darkestPixelConfirm = getDarkestPixelArea(gray, frame2);

	//imshow("out",frame2);
	// cv::waitKey(1);

	return frame2;
}

Mat highlightPupil(Mat &gray){

	frame2 = gray.clone();

	cv::cvtColor(gray, gray, CV_BGR2GRAY);

	int kernel_size = 3;
	int scale = 1;
	int delta = 0;
	int ddepth = CV_8U;

	int darkestPixel = getDarkestPixel(gray);
	Point darkestPixelConfirm = getDarkestPixelArea(gray, frame2);

	//imshow("original", frame1);
	//waitKey(1);


	threshold(gray, edges, (darkestPixel + 12), 255, 1);


	//medianBlur(edges, edges, 5);


	//resize(edges, edges, Size(), .5, .5, 1);

	//imshow("dark thresh", edges);
	//waitKey(1);


	//Laplacian(gray, gray, ddepth, kernel_size, scale, delta, BORDER_DEFAULT);
	//bitwise_not(edges, edges);
	//threshold(gray, gray, (getDarkestPixel(gray) + 250), 255, 1);
	//threshold(gray, gray, 40, 255, 3);

	/*

	int edgeThresh = 1;
	int lowThreshold = 30;
	int const max_lowThreshold = 100;
	int ratio = 2;
	int kernel2 = 3;
	Canny(gray, gray, lowThreshold, lowThreshold*ratio, kernel2);

	imshow("canny", gray);
	//waitKey(1);

	bitwise_and(edges, gray, edges);

	imshow("combo", edges);
	//waitKey(1);

	*/

	/// Gradient X
	//Scharr( src_gray, grad_x, ddepth, 1, 0, scale, delta, BORDER_DEFAULT );
	//convertScaleAbs(gray, gray);


	//threshold(gray, gray, (getDarkestPixel(gray) + 25), 255, 1);



	//bitwise_not(gray, gray);




	//bitwise_and(edges, gray, edges);


	//imshow("combo", gray);
	//waitKey(1);


	// Find all contours
	std::vector<std::vector<cv::Point> > contours;
	cv::findContours(edges.clone(), contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

	// Fill holes in each contour
	cv::drawContours(gray, contours, -1, CV_RGB(255, 255, 255), -1);


	/// Get the moments
	vector<Moments> mu(contours.size());
	for (int i = 0; i < contours.size(); i++)
	{
		mu[i] = moments(contours[i], false);
	}

	///  Get the mass centers:
	vector<Point> mc(contours.size());
	//vector<Point2f> mc;
	for (int i = 0; i < contours.size(); i++)
	{

		/*	Point2f temp = Point2f(mu[i].m10 / mu[i].m00, mu[i].m01 / mu[i].m00);

		if (contours[i].size() > 185 && mc[i].y > 122){
		contours.push_back(temp);
		}*/
		mc[i] = Point((int)(mu[i].m10 / mu[i].m00), (int)(mu[i].m01 / mu[i].m00));
	}

	/// Find the rotated rectangles and ellipses for each contour
	//vector<RotatedRect> minRect(contours.size());

	vector<RotatedRect> minEllipse(contours.size());

	int j = 0;
	int biggestSize = 0;

	vector<vector<Point>> fittedRects;

	/*
	for (int i = 0; i < contours.size(); i++)
	{
	//minRect[i] = minAreaRect(Mat(contours[i]));

	if (contours[i].size() > 185 && mc[i].y>122){
	minEllipse[0] = fitEllipse(Mat(contours[i]));

	//need my supercool method here
	fittedRects = bestFit(contours[i]);
	}
	}
	*/

	int size = 250;

	if (mc.size() > 0){


		//&& mc[0].x - size / 2 > 0
		//	&& mc[0].y - size / 2 > 0
		//	&& mc[0].x < 640 - size / 2
		//	&& mc[0].y > 480 - size / 2

		int mcSize = 0;
		int mcX = 0;
		int mcY = 0;

		//cout << mc.size() << endl;

		for (int i = 0; i < mc.size(); i++){
			if (contours[i].size() > 125 && mc[i].y > 122 && mc[i].x < 500){
				//if (contours[i].size() > mcSize){
				int area = (int)contourArea(contours[i]);

				if (area > mcSize){
					mcX = mc[i].x;
					mcY = mc[i].y;
					mcSize = area;
				}
			}
		}

		int newX = mcX - size / 2;
		int newY = mcY - size / 2;

		if (newX < 0){
			newX += -newX;
		}
		else if (newX > 639 - size){
			newX -= newX - 639 + size;
			//std::cout << "oops" << endl;
		}

		if (newY < 0){
			newY += -newY;
		}
		else if (newY > 479 - size){
			newY -= newY - 479 + size;
			//std::cout << "oops2" << endl;
		}


		//using returned pupil area of interest -> 

		//in other Mats
		//get darkest pixel
		int darkestPixelSub = getDarkestPixel(gray);

	}




	/// Draw contours + rotated rects + ellipses
	//Mat drawing = Mat::zeros(gray.size(), CV_8UC3);
	/*
	for (int i = 0; i< contours.size(); i++)
	{
	//Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );

	Scalar colorC = Scalar(255, 0, 0);
	Scalar colorP = Scalar(255, 255, 255);

	// contour
	drawContours(gray, contours, i, colorC, 1, 8, vector<Vec4i>(), 0, Point());
	// ellipse
	ellipse(gray, minEllipse[i], colorP, 2, 8);
	// rotated rectangle
	//Point2f rect_points[4]; minRect[i].points(rect_points);
	//for( int j = 0; j < 4; j++ )
	//   line( drawing, rect_points[j], rect_points[(j+1)%4], color, 1, 8 );
	}
	*/

	cv::cvtColor(gray, gray, CV_GRAY2BGR);

	for (int i = 0; i < fittedRects.size(); i++)
		//for (int i = fittedRects.size()/2; i < fittedRects.size() /2+1; i++)
	{

		//Scalar colorC = Scalar(255, 0, 0);
		Scalar colorP = Scalar(0, 255, 0);


		RotatedRect test = fitEllipse(fittedRects[i]);

		//cout << "fitted ellipse" << endl;

		if (test.size.height>10 && test.size.height <240 && test.size.height / test.size.width > .5 && test.size.height / test.size.width < 2){
			ellipse(gray, test, colorP, 2, 8);
		}
		//cout << "drew ellipse" << endl;

		vector<Point> tempTen2;

		//put numPoints worth of points into an array to pass to fitEllipse
		for (int j = 0; j < fittedRects[i].size(); j++){
			tempTen2.push_back(fittedRects[i].at(j));
		}

		//for (int b = 0; b < tempTen2.size(); b++){
		//	//Vec3b color = gray.at<Vec3b>(tempTen2.at(b));
		//	gray.at<uchar>(tempTen2.at(b).y, tempTen2.at(b).x) = 255;
		//		//cout << "Pixel >200 :" << x << "," << y << endl;
		//}

		for (int b = 0; b < tempTen2.size(); b++){
			Vec3b color = (255, 255, 255);
			gray.at<Vec3b>(tempTen2.at(b).y, tempTen2.at(b).x) = color;
			//cout << "Pixel >200 :" << x << "," << y << endl;
		}

		// rotated rectangle
		//Point2f rect_points[4]; minRect[i].points(rect_points);
		//for( int j = 0; j < 4; j++ )
		//   line( drawing, rect_points[j], rect_points[(j+1)%4], color, 1, 8 );
	}


	for (int i = -1; i < 2; i++){
		for (int j = -1; j < 2; j++){
			Point pxConfirm;
			pxConfirm.x = darkestPixelConfirm.x + i;
			pxConfirm.y = darkestPixelConfirm.y + j;

			frame1.at<Vec3b>(pxConfirm)[0] = 0;
			frame1.at<Vec3b>(pxConfirm)[1] = 255;
			frame1.at<Vec3b>(pxConfirm)[2] = 0;
		}
	}

	imshow("output", frame1);
	// cv::waitKey(1);


	return frame2;
}

void detect_glints(const cv::Mat &img){
	glints_.resize(0);
	// glint removal
	cv::Mat_<uchar> mEye_without_glint = img.clone();
	cv::Mat_<uchar> mEye_without_glint_mask;
	// adaptive threshold
	double minVal;
	double maxVal;
	cv::Point minLoc;
	cv::Point maxLoc;
	cout << "debug 1" << endl;
	minMaxLoc(img, &minVal, &maxVal, &minLoc, &maxLoc);
	cout << "debug 2" << endl;
	if (maxVal >= 240){
		int glint_threshold = (int) (minVal + (maxVal - minVal)*0.3);
		cv::threshold(mEye_without_glint, mEye_without_glint_mask, glint_threshold, 255, cv::THRESH_BINARY);


		params.filterByArea = false;
		params.filterByCircularity = false;
		params.filterByConvexity = false;
		params.filterByInertia = false;
		params.filterByColor = false;
		//		params.minThreshold = 0;
		//		params.thresholdStep = glint_threshold;
		//		params.maxThreshold = 255;
#ifdef  DEBUG_GLINT_DETECTION
		cv::imshow("Binary", mEye_without_glint_mask);
#endif // DEBUG_GLINT_DETECTION


#if CV_MAJOR_VERSION ==3
		cv::Ptr<cv::SimpleBlobDetector> detector = cv::SimpleBlobDetector::create(params);
#else
		cv::Ptr<cv::SimpleBlobDetector> detector = SimpleBlobDetector::create("grid");
#endif

		// Detect blobs.
		std::vector<cv::KeyPoint> glint_candidates;
		glint_candidates.resize(0);
		detector->detect(mEye_without_glint_mask, glint_candidates);

		cv::Mat img_rgb;
		if (glint_candidates.size() > 0){
			if (glint_candidates.size() > 2){
				// Find true glints by using the distance from the image center
				cv::Point2f img_center(img.rows / 2.0f, img.cols / 2.0f);
				std::vector<float> glint_dists(glint_candidates.size());
				size_t n(0);
				std::generate(std::begin(glint_dists), std::end(glint_dists), [&]{return cv::norm(glint_candidates[n++].pt - img_center); });
				//for (size_t k = 0; k < glint_candidates.size(); k++){
				//	glint_dists[k] = cv::norm(glint_candidates[k].pt - img_center);
				//}
				std::vector<size_t>indices(glint_candidates.size());
				n = 0;
				std::generate(std::begin(indices), std::end(indices), [&]{ return n++; });
				std::sort(std::begin(indices),
					std::end(indices),
					[&](size_t i1, size_t i2) { return glint_dists[i1] < glint_dists[i2]; });
				glints_.resize(2);
				glints_[0] = glint_candidates[indices[0]];
				glints_[1] = glint_candidates[indices[1]];
			}
			else{
				glints_ = glint_candidates;
			}
			cv::cvtColor(img, img_rgb, CV_GRAY2BGR);
#ifdef  DEBUG_GLINT_DETECTION
			drawKeypoints(img, glint_candidates, img_rgb, cv::Scalar(0, 255, 0), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
			drawKeypoints(img, glints_, img_rgb, cv::Scalar(0, 0, 255), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
			cv::imshow("Detected blobs", img_rgb);
#endif // DEBUG_GLINT_DETECTION
		}
		if (glints_.size() > 0){
			cv::Size img_size;
			cv::Point img_offset;
			img.locateROI(img_size, img_offset);
			for (size_t k = 0; k < glints_.size(); k++){
				glints_[k].pt = glints_[k].pt + (cv::Point2f)img_offset;
			}
		}
	}
	else{
		//			cv::copyMakeBorder(mEye, mEyePad, padding, padding, padding, padding, cv::BORDER_REPLICATE);
	}


}

Point correctBounds(Point input, int maxSize){

	//maximum size (L or W) of pupil ROI
	int size = maxSize;

	//get x/y from input point
	int mcX = input.x;
	int mcY = input.y;


	int newX = mcX - size / 2;
	int newY = mcY - size / 2;

	if (newX < 0){
		newX += -newX;
	}
	else if (newX > 639 - size){
		newX -= newX - 639 + size;
		//std::cout << "oops" << endl;
	}

	if (newY < 0){
		newY += -newY;
	}
	else if (newY > 479 - size){
		newY -= newY - 479 + size;
		//std::cout << "oops2" << endl;
	}

	//new point is not out of bounds
	return Point(newX, newY);

}

vector<int> getBiggest(std::vector<std::vector<cv::Point>> contours){

	vector<int> biggestOutVec;
	int biggestOut = 0;

	if (contours.size() > 0){

		/// Get the moments
		vector<Moments> mu(contours.size());
		for (int i = 0; i < contours.size(); i++)
		{
			mu[i] = moments(contours[i], false);
		}

		///  Get the mass centers:
		vector<Point> mc(contours.size());

		for (int i = 0; i < contours.size(); i++)
		{
			mc[i] = Point((int)(mu[i].m10 / mu[i].m00), (int)(mu[i].m01 / mu[i].m00));
		}


		int mcSize = 0;
		int mcX = 0;
		int mcY = 0;

		//find contour with largest area and use it as the pupil
		for (int i = 0; i < mc.size(); i++){
			if (contours[i].size() > 40 && mc[i].y > 10 && mc[i].x < 620){

				int area = (int)contourArea(contours[i]);

				if (area > mcSize){
					mcX = mc[i].x;
					mcY = mc[i].y;
					mcSize = area;
					biggestOut = i;
				}
			}
		}
	}

	biggestOutVec.push_back(biggestOut);

	return biggestOutVec;
}

/**
* Gets candidate points from a list of contours and canny image
*/
vector<Point2f> getCandidates(std::vector<std::vector<cv::Point>>contours, int biggest, Mat& thresh, bool draw){

	vector<Point2f> allPts;

	//debug
	bool candidatesOn = draw;
	bool cannyOn = draw;

	//draw canny (red) on frame 2 for test
	if (cannyOn){
		for (int j = 0; j < thresh.size().height; j++){
			for (int i = 0; i < thresh.size().width; i++){
				if ((int)thresh.at<uchar>(i, j) > 0){

					frame2.at<Vec3b>(i, j)[0] = 15;
					frame2.at<Vec3b>(i, j)[1] = 0;
					frame2.at<Vec3b>(i, j)[2] = 255;

				}
			}
		}
	}

	//for (int j = 0; j < contours.size(); j++){
	if (contours[biggest].size() > 10){
		for (int i = 0; i < contours[biggest].size(); i++){
			const float x = (float)contours[biggest][i].x;
			const float y = (float)contours[biggest][i].y;
			int mult = 2;

			//border check
			if (x - mult * thickness > 0 &&
				x + mult  * thickness < thresh.size().width &&
				y - mult  * thickness > 0 &&
				y + mult  * thickness < thresh.size().height){

				//add contours that also exist in canny, and if debug = true, mark checked on image
				if (candidatesOn){
					frame2.at<Vec3b>(Point2f(x, y))[0] = 15;
					frame2.at<Vec3b>(Point2f(x, y))[1] = 0;
					frame2.at<Vec3b>(Point2f(x, y))[2] = 255;
				}

				if ((int)thresh.at<uchar>(Point2f(x, y)) > 0){

					allPts.push_back(Point2f(x, y));
					thresh.at<uchar>(Point2f(x, y)) = 0;

				}
				else if (candidatesOn){
					frame2.at<Vec3b>(Point2f(x, y))[0] = 15;
					frame2.at<Vec3b>(Point2f(x, y))[1] = 255;
					frame2.at<Vec3b>(Point2f(x, y))[2] = 0;
				}

				for (int z = 1; z <= mult * thickness; z = z + mult){
					if (((int)thresh.at<uchar>(Point2f(x, y + z))) > 0){
						allPts.push_back(Point2f(x, y + z));
						thresh.at<uchar>(Point2f(x, y + z)) = 0;
					}
					else if (candidatesOn){
						frame2.at<Vec3b>(Point2f(x, y + z))[0] = 15;
						frame2.at<Vec3b>(Point2f(x, y + z))[1] = 255;
						frame2.at<Vec3b>(Point2f(x, y + z))[2] = 0;
					}
					if (((int)thresh.at<uchar>(Point2f(x, y - z))) > 0){
						allPts.push_back(Point2f(x, y - z));
						thresh.at<uchar>(Point2f(x, y - z)) = 0;
					}
					else if (candidatesOn){
						frame2.at<Vec3b>(Point2f(x, y - z))[0] = 15;
						frame2.at<Vec3b>(Point2f(x, y - z))[1] = 255;
						frame2.at<Vec3b>(Point2f(x, y - z))[2] = 0;
					}
					if (((int)thresh.at<uchar>(Point2f(x + z, y))) > 0){
						allPts.push_back(Point2f(x + z, y));
						thresh.at<uchar>(Point2f(x + z, y)) = 0;
					}
					else if (candidatesOn){
						frame2.at<Vec3b>(Point2f(x + z, y))[0] = 15;
						frame2.at<Vec3b>(Point2f(x + z, y))[1] = 255;
						frame2.at<Vec3b>(Point2f(x + z, y))[2] = 0;
					}
					if (((int)thresh.at<uchar>(Point2f(x - z, y))) > 0){
						allPts.push_back(Point2f(x - z, y));
						thresh.at<uchar>(Point2f(x - z, y)) = 0;
					}
					else if (candidatesOn){
						frame2.at<Vec3b>(Point2f(x - z, y))[0] = 15;
						frame2.at<Vec3b>(Point2f(x - z, y))[1] = 255;
						frame2.at<Vec3b>(Point2f(x - z, y))[2] = 0;
					}

				}
			}

		}
	}
	else{
		cout << "contours.size was < 10. size = " << contours.size() << endl;
	}

	return allPts;
}

/**
* Overloaded function - accepts Point2f vectors in addition to Point vectors
*/
vector<Point2f> getCandidates(std::vector<std::vector<cv::Point2f>>contours, int biggest, Mat& thresh, bool draw){

	vector<Point2f> allPts;

	//debug
	bool candidatesOn = draw;
	bool cannyOn = draw;

	//draw canny (red) on frame 2 for test
	if (cannyOn){
		for (int j = 0; j < thresh.size().height; j++){
			for (int i = 0; i < thresh.size().width; i++){
				if ((int)thresh.at<uchar>(i, j) > 0){

					frame2.at<Vec3b>(i, j)[0] = 15;
					frame2.at<Vec3b>(i, j)[1] = 0;
					frame2.at<Vec3b>(i, j)[2] = 255;

				}
			}
		}
	}

	//for (int j = 0; j < contours.size(); j++){
	if (contours[biggest].size() > 40){
		for (int i = 0; i < contours[biggest].size(); i++){
			const float x = (float)contours[biggest][i].x;
			const float y = (float)contours[biggest][i].y;

			//border check
			if (x - thickness > 0 &&
				x + thickness < thresh.size().width &&
				y - thickness > 0 &&
				y + thickness < thresh.size().height){

				//add contours that also exist in canny, and if debug = true, mark checked on image
				if (candidatesOn){
					frame2.at<Vec3b>(Point2f(x, y))[0] = 15;
					frame2.at<Vec3b>(Point2f(x, y))[1] = 0;
					frame2.at<Vec3b>(Point2f(x, y))[2] = 255;
				}

				if ((int)thresh.at<uchar>(Point2f(x, y)) > 0){

					allPts.push_back(Point2f(x, y));
					//set to zero to prevent duplicates
					thresh.at<uchar>(Point2f(x, y)) = 0;

				}
				else if (candidatesOn){
					frame2.at<Vec3b>(Point2f(x, y))[0] = 15;
					frame2.at<Vec3b>(Point2f(x, y))[1] = 255;
					frame2.at<Vec3b>(Point2f(x, y))[2] = 0;
				}

				for (int z = 1; z <= thickness; z++){
					if (((int)thresh.at<uchar>(Point2f(x, y + z))) > 0){
						allPts.push_back(Point2f(x, y + z));
						thresh.at<uchar>(Point2f(x, y + z)) = 0;
					}
					else if (candidatesOn){
						frame2.at<Vec3b>(Point2f(x, y + z))[0] = 15;
						frame2.at<Vec3b>(Point2f(x, y + z))[1] = 255;
						frame2.at<Vec3b>(Point2f(x, y + z))[2] = 0;
					}
					if (((int)thresh.at<uchar>(Point2f(x, y - z))) > 0){
						allPts.push_back(Point2f(x, y - z));
						thresh.at<uchar>(Point2f(x, y - z)) = 0;
					}
					else if (candidatesOn){
						frame2.at<Vec3b>(Point2f(x, y - z))[0] = 15;
						frame2.at<Vec3b>(Point2f(x, y - z))[1] = 255;
						frame2.at<Vec3b>(Point2f(x, y - z))[2] = 0;
					}
					if (((int)thresh.at<uchar>(Point2f(x + z, y))) > 0){
						allPts.push_back(Point2f(x + z, y));
						thresh.at<uchar>(Point2f(x + z, y)) = 0;
					}
					else if (candidatesOn){
						frame2.at<Vec3b>(Point2f(x + z, y))[0] = 15;
						frame2.at<Vec3b>(Point2f(x + z, y))[1] = 255;
						frame2.at<Vec3b>(Point2f(x + z, y))[2] = 0;
					}
					if (((int)thresh.at<uchar>(Point2f(x - z, y))) > 0){
						allPts.push_back(Point2f(x - z, y));
						thresh.at<uchar>(Point2f(x - z, y)) = 0;
					}
					else if (candidatesOn){
						frame2.at<Vec3b>(Point2f(x - z, y))[0] = 15;
						frame2.at<Vec3b>(Point2f(x - z, y))[1] = 255;
						frame2.at<Vec3b>(Point2f(x - z, y))[2] = 0;
					}

				}
			}

		}
	}
	else{
		cout << "contours.size was < 40. size = " << contours.size() << endl;
	}

	return allPts;
}

//Point refinement code
//Better fits a set of candidate points to a pupil ellipse
vector<Point2f> refinePoints(vector<Point2f> allPts, Mat gray, int checkThickness, int checkSpacing){

	//vector holding returned points with sub-pixel accuracy
	vector<Point2f> refinedPoints;

	//loop through all points
	for (int i = 0; i < allPts.size(); i++){

		float finalX = allPts[i].x;
		float finalY = allPts[i].y;

		//cout << "old point: " << finalX << ", " << finalY;

		//calculate differences
		//int checkThickness = 8; // number of pixels to check in all 4 directions
		//int checkSpacing = 2; // spacing between each pixel to check

		//TODO only correct x on left/right side of ellipse and y on top/bottom of ellipse

		//loops checking pixels to reset best point for each candidate point
		//check x edge cases
		if (allPts[i].x - checkThickness*checkSpacing - 1 >= 0 &&
			allPts[i].x + checkThickness*checkSpacing + 1 < gray.size().width){

			float xNumerator = 0;
			float xDenominator = 0;

			int largestDiffX = 0;
//			int bestX = finalX;
			bool found = false;

			//x loop
			for (int j = -checkThickness*checkSpacing; j < checkThickness*checkSpacing; j = j + checkSpacing){

				//calculate and find diffs
				int leftDiff = abs(gray.at<uchar>(Point((int)allPts[i].x + j, (int)allPts[i].y)) - gray.at<uchar>(Point((int)allPts[i].x + j - 1, (int)allPts[i].y)));
				int rightDiff = abs(gray.at<uchar>(Point((int)allPts[i].x + j, (int)allPts[i].y)) - gray.at<uchar>(Point((int)allPts[i].x + j + 1, (int)allPts[i].y)));

				//add with weight 
				xNumerator += (allPts[i].x + j) * (leftDiff + rightDiff);
				xDenominator += (leftDiff + rightDiff);
			}

			//calculate final weighted point (ignored if bound condidions not met)
			if (xDenominator != 0){
				finalX = xNumerator / xDenominator;
			}
		}

		//check y edge cases
		if (allPts[i].y - checkThickness*checkSpacing - 1 >= 0 &&
			allPts[i].y + checkThickness*checkSpacing + 1 < gray.size().height){

			float yNumerator = 0;
			float yDenominator = 0;

			int largestDiffY = 0;
//			int bestY = finalY;
			bool found = false;

			//y loop
			for (int j = -checkThickness*checkSpacing; j < checkThickness*checkSpacing; j = j + checkSpacing){

				//calculate and find diffs
				int topDiff = abs(gray.at<uchar>(Point((int)allPts[i].x, (int)allPts[i].y + j)) - gray.at<uchar>(Point((int)allPts[i].x, (int)allPts[i].y + j - 1)));
				int botDiff = abs(gray.at<uchar>(Point((int)allPts[i].x, (int)allPts[i].y + j)) - gray.at<uchar>(Point((int)allPts[i].x, (int)allPts[i].y + j + 1)));

				//add with weight
				yNumerator += (allPts[i].y + j) * (topDiff + botDiff);
				yDenominator += (topDiff + botDiff);
			}

			if (yDenominator != 0){
				finalY = yNumerator / yDenominator;
			}
		}

		refinedPoints.push_back(Point2f(finalX, finalY));

	}//end for loop (for refining all candidates)

	return refinedPoints;

}//end point refinement

//Point refinement code
//Better fits a set of candidate points to a pupil ellipse
vector<Point2f> refinePoints(vector<Point> allPts, Mat gray, int checkThickness, int checkSpacing){

	//vector holding returned points with sub-pixel accuracy
	vector<Point2f> refinedPoints;

	//loop through all points
	for (int i = 0; i < allPts.size(); i++){

		float finalX = (float)allPts[i].x;
		float finalY = (float)allPts[i].y;

		//cout << "old point: " << finalX << ", " << finalY;

		//calculate differences
		//int checkThickness = 14; // number of pixels to check in all 4 directions
		//int checkSpacing = 1; // spacing between each pixel to check

		//TODO only correct x on left/right side of ellipse and y on top/bottom of ellipse

		//loops checking pixels to reset best point for each candidate point
		//check x edge cases
		if (allPts[i].x - checkThickness*checkSpacing - 1 >= 0 &&
			allPts[i].x + checkThickness*checkSpacing + 1 < gray.size().width){

			float xNumerator = 0;
			float xDenominator = 0;

			int largestDiffX = 0;
//			int bestX = finalX;
			bool found = false;

			//x loop
			for (int j = -checkThickness*checkSpacing; j < checkThickness*checkSpacing; j = j + checkSpacing){

				//calculate and find diffs
				int leftDiff = abs(gray.at<uchar>(Point(allPts[i].x + j, allPts[i].y)) - gray.at<uchar>(Point(allPts[i].x + j - 1, allPts[i].y)));
				int rightDiff = abs(gray.at<uchar>(Point(allPts[i].x + j, allPts[i].y)) - gray.at<uchar>(Point(allPts[i].x + j + 1, allPts[i].y)));

				//add with weight 
				xNumerator += (allPts[i].x + j) * (leftDiff + rightDiff);
				xDenominator += (leftDiff + rightDiff);
			}

			//calculate final weighted point (ignored if bound condidions not met)
			if (xDenominator != 0){
				finalX = xNumerator / xDenominator;
			}
		}

		//check y edge cases
		if (allPts[i].y - checkThickness*checkSpacing - 1 >= 0 &&
			allPts[i].y + checkThickness*checkSpacing + 1 < gray.size().height){

			float yNumerator = 0;
			float yDenominator = 0;

			int largestDiffY = 0;
//			int bestY = finalY;
			bool found = false;

			//y loop
			for (int j = -checkThickness*checkSpacing; j < checkThickness*checkSpacing; j = j + checkSpacing){

				//calculate and find diffs
				int topDiff = abs(gray.at<uchar>(Point(allPts[i].x, allPts[i].y + j)) - gray.at<uchar>(Point(allPts[i].x, allPts[i].y + j - 1)));
				int botDiff = abs(gray.at<uchar>(Point(allPts[i].x, allPts[i].y + j)) - gray.at<uchar>(Point(allPts[i].x, allPts[i].y + j + 1)));

				//add with weight
				yNumerator += (allPts[i].y + j) * (topDiff + botDiff);
				yDenominator += (topDiff + botDiff);
			}

			if (yDenominator != 0){
				finalY = yNumerator / yDenominator;
			}
		}

		refinedPoints.push_back(Point2f(finalX, finalY));

	}//end for loop (for refining all candidates)

	return refinedPoints;

}//end point refinement

bool badEllipseFilter(RotatedRect current, int maxSize){

	bool isGood = true;

	//test against last ttwo ellipse sizes and rotations, 
	//if difference is over a certain size and angle threshold, set isGood to false 
	if (current.size.width / current.size.height > 2 ||
		current.size.height / current.size.width > 2 ||
		current.size.height > maxSize ||
		current.size.width > maxSize ||
		current.size.height < 10 ||
		current.size.width < 10 ||
		current.size.width > maxSize ||
		current.size.height > maxSize ||
		//current.center.y < 40 ||
		current.size.height / previousRect.size.height > 1.3 ||
		previousRect.size.height / current.size.height > 1.3 ||
		current.size.width / previousRect.size.width > 1.3 ||
		previousRect.size.width / current.size.width > 1.3){
		cout << "returning false" << endl;
		isGood = false;
	}

	previousRect = current;

	return isGood;
}

Mat pupilAreaFit(Mat &gray){

	frame2 = gray.clone();

	//for timing funcitons
	unsigned long long Int64 = 0;
	clock_t Start = clock();

	//cout << "Start: " << clock() - Start << endl;


	//find pupil
	Point darkestPixelConfirm = getDarkestPixelArea(gray);


	//rmGlints(gray);

	//cout << "Get darkest pixel area: " << clock() - Start << endl;


	//correct bounds
	darkestPixelConfirm = correctBounds(darkestPixelConfirm, size);

	//cout << "fix bounds: " << clock() - Start << endl;



	//find darkest pixel (for thresholding
	int darkestPixel = getDarkestPixel(gray(cv::Rect(darkestPixelConfirm.x, darkestPixelConfirm.y, size, size)));

	int kernel_size = 3;
	int scale = 1;
	int delta = 0;
	int ddepth = CV_8U;

	int erosion_size = 3;
	int erosion_type = MORPH_ELLIPSE;
	Mat element = getStructuringElement(erosion_type,
		Size(2 * erosion_size + 1, 2 * erosion_size + 1),
		Point(erosion_size, erosion_size));

	/// Apply the erosion operation
	if (erodeOn){
		erode(gray, gray, element);
	}

	//set ROI and thresh for testing
	threshold(gray(cv::Rect(darkestPixelConfirm.x, darkestPixelConfirm.y, size, size)), threshLow, (darkestPixel + darkestPixelL1), 255, 1);

	if (threshDebug){
		//test threshing
		imshow("threshLow", threshLow);
		//waitKey(1);
	}

	//Find contours
	std::vector<std::vector<cv::Point>> contoursLow;
	cv::findContours(threshLow, contoursLow, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

	//get biggest contours (pupils)
	int biggest = getBiggest(contoursLow).at(0);

	//get bounding rect center 
	Rect minPts = boundingRect(contoursLow.at(biggest));
	minPts = Rect(minPts.x + darkestPixelConfirm.x, minPts.y + darkestPixelConfirm.y, minPts.width, minPts.height);
	Point rectCenter(minPts.x + minPts.width / 2, minPts.y + minPts.height / 2);

	//take height or width as max, whichever is bigger
	int max = minPts.height;
	if (max < minPts.width){
		max = minPts.width;
	}
	if (max <= 0){ //check for 0 size case
		max = size;
	}

	//code used to reset ROI, currently inactive
	/*
	float xMax = 1.4;
	float yMax = 1.3;

	int size2X = max * xMax;
	int size2Y = max * yMax;

	//set size2 to size if max <= 0
	if (max <= 0 || max > size){
	size2X = size;
	size2Y = size;
	}

	assert(xMax > yMax);

	Point rectCorner = correctBounds(Point(rectCenter.x, rectCenter.y), size2X);

	minPts = Rect(rectCorner.x, rectCorner.y, size2X, size2Y);
	*/

	//max size of pupil ROI
	int size2 = size;

	//Thresh 2
	threshold(gray(cv::Rect(darkestPixelConfirm.x, darkestPixelConfirm.y, size2, size2)), threshHigh, (darkestPixel + darkestPixelL2), 255, 1);

	if (threshDebug){
		//test threshing
		imshow("threshMid", threshHigh);
		//waitKey(1);
	}

	//contours for high thresh
	std::vector<std::vector<cv::Point>> contoursHigh;
	cv::findContours(threshHigh, contoursHigh, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

	int biggestHigh = getBiggest(contoursHigh).at(0);

	//convert back to 3 channel for drawing
	cv::cvtColor(gray, gray, CV_GRAY2BGR);

	Scalar colorC = Scalar(0, 255, 0);
	Scalar colorE = Scalar(0, 0, 255);

	//canny parameters, other params are globally set
	int edgeThresh = 1;
	int const max_lowThreshold = 100;
	int ratio = 3;
	int kernel2 = 3;

	//run Canny to get best candiate points from contours 
	Canny(gray(cv::Rect(darkestPixelConfirm.x, darkestPixelConfirm.y, size2, size2)), thresh3, lowThresholdCanny, lowThresholdCanny*ratio, kernel2);
	Canny(gray(cv::Rect(darkestPixelConfirm.x, darkestPixelConfirm.y, size2, size2)), thresh4, highThresholdCanny, highThresholdCanny*ratio, kernel2);

	if (threshDebug){
		imshow("cannyLow", thresh3);
		//waitKey(1);
		imshow("cannyHigh", thresh4);
		//waitKey(1);
	}

	//Laplacian(gray(cv::Rect(darkestPixelConfirm.x, darkestPixelConfirm.y, size, size)),
	//gray(cv::Rect(darkestPixelConfirm.x, darkestPixelConfirm.y, size, size)), CV_8U, kernel_size, scale, delta, BORDER_DEFAULT);
	//Sobel(gray(cv::Rect(darkestPixelConfirm.x, darkestPixelConfirm.y, size, size)), thresh3, ddepth, 1, 0, 5, scale, delta, BORDER_DEFAULT);
	//threshold(gray(cv::Rect(darkestPixelConfirm.x, darkestPixelConfirm.y, size, size)), thresh2, (darkestPixel + 15), 255, 1);
	//holds candidate points for 3 different levels of filtering

	//holds sets of candidate points for different points throughout refinement
	vector<Point2f> allPts;
	vector<Point2f> allPts2;
	vector<Point2f> allPtsHigh;

	//logical AND of contours and canny images
	allPts = getCandidates(contoursLow, biggest, thresh3, false);
	allPtsHigh = getCandidates(contoursHigh, biggestHigh, thresh4, false);

	//merge remaining points for low and high point lists 
	allPts.insert(allPts.end(), allPtsHigh.begin(), allPtsHigh.end());
	std::vector<std::vector<cv::Point2f>> allPtsWithOutliers;
	allPtsWithOutliers.push_back(allPts);

	//convert to gray for refinement
	cv::cvtColor(gray, gray, CV_BGR2GRAY);

	//refine points based on line fitting - Thanks Yuta! 
	if (allPts.size() > 5){
		allPts = refinePoints(allPts, gray(cv::Rect(darkestPixelConfirm.x, darkestPixelConfirm.y, size2, size2)), 8, 2);
	}

	//convert back to 3 channel for drawing if necessary
	cv::cvtColor(gray, gray, CV_GRAY2BGR);

	//remove outliers via ellipse method, basically a logical AND of candidate points with a drawn ellipse: great for removing outliers
	thresh3 = Mat::zeros(size2, size2, CV_8U); //black mat
	if (allPts.size() > 5){
		RotatedRect ellipseRaw = fitEllipse(allPts);

		if (ellipseRaw.center.x < 300 && ellipseRaw.center.x > 0 && ellipseRaw.angle > 5){
			//if possible and within bounds, draw
			ellipse(thresh3, ellipseRaw, 255, 2, 8); //draw white ellipse 
		}
		allPts2 = getCandidates(allPtsWithOutliers, 0, thresh3, false);
	}

	//re-run the ellipse method on a fitted ellipse, but with the original set of points: great for re-including inliers
	thresh3 = Mat::zeros(size2, size2, CV_8U); //black mat
	if (allPts2.size() > 5 && allPtsWithOutliers.size() > 0 && allPtsWithOutliers.at(0).size() > 5){
		RotatedRect ellipseRaw = fitEllipse(allPts2);

		//check for impossible ellipses
		if (ellipseRaw.center.x < size2 && ellipseRaw.center.x > 0 && ellipseRaw.angle > 5){
			//if possible and within bounds, draw
			ellipse(thresh3, ellipseRaw, 255, 2, 8); //draw white ellipse 
		}
		allPts = getCandidates(allPtsWithOutliers, 0, thresh3, false);
	}

	//convert to gray for refinement
	cv::cvtColor(gray, gray, CV_BGR2GRAY);

	//refine points based on line fitting - Thanks Yuta! 
	if (allPts.size() > 5){
		allPts = refinePoints(allPts, gray(cv::Rect(darkestPixelConfirm.x, darkestPixelConfirm.y, size2, size2)), 8, 2);
	}

	//re-refine with another ellipse fit
	if (allPts.size() > 5){
		thresh3 = Mat::zeros(frameHeight, frameWidth, CV_8U);
		RotatedRect ellipseRaw = fitEllipse(allPts);
		ellipse(thresh3, ellipseRaw, 255, 1, 8);

		std::vector<std::vector<cv::Point> > ellipseContour;
		cv::findContours(thresh3, ellipseContour, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

		if (ellipseContour.size() > 0 && ellipseContour.at(0).size() > 5){
			thresh3 = Mat::zeros(frameHeight, frameWidth, CV_8U);
			RotatedRect ellipseRaw = fitEllipse(allPts);
			ellipse(thresh3, ellipseRaw, 255, 1, 8);
			allPts = refinePoints(ellipseContour.at(0), thresh3, 10, 2);
		}

		if (ellipseContour.size() > 0 && ellipseContour.at(0).size() > 5){
			thresh3 = Mat::zeros(frameHeight, frameWidth, CV_8U);
			RotatedRect ellipseRaw = fitEllipse(allPts);
			ellipse(thresh3, ellipseRaw, 255, 1, 8);
			allPts = refinePoints(ellipseContour.at(0), thresh3, 8, 1);
		}
	}

	//convert back to 3 channel for drawing
	cv::cvtColor(gray, gray, CV_GRAY2BGR);
	//cout << "cvt color: " << clock() - Start << endl;


	//draw all final candidate pts on return image
	//if (allPts.size() > 0){
	//	for (int i = 0; i < allPts.size(); i++){
	//		if (allPts.at(i).y + darkestPixelConfirm.y > 0 &&
	//			allPts.at(i).y + darkestPixelConfirm.y < frame2.size().height &&
	//			allPts.at(i).x + darkestPixelConfirm.x > 0 &&
	//			allPts.at(i).x + darkestPixelConfirm.x < frame2.size().width){
	//				frame2.at<Vec3b>(allPts.at(i).y + darkestPixelConfirm.y, allPts.at(i).x + darkestPixelConfirm.x)[0] = 15;
	//				frame2.at<Vec3b>(allPts.at(i).y + darkestPixelConfirm.y, allPts.at(i).x + darkestPixelConfirm.x)[1] = 185;
	//				frame2.at<Vec3b>(allPts.at(i).y + darkestPixelConfirm.y, allPts.at(i).x + darkestPixelConfirm.x)[2] = 15;
	//		}
	//	}
	//}


	//Regular Ellipse
	if (allPts.size()>5){

		//do refinement here

		RotatedRect ellipseRaw = fitEllipse(allPts);
		RotatedRect ellipseCorrect = RotatedRect(Point2f(ellipseRaw.center.x + darkestPixelConfirm.x, ellipseRaw.center.y + darkestPixelConfirm.y), ellipseRaw.size, ellipseRaw.angle);
		Scalar colorLBlue = Scalar(255, 255, 15);


		//filter out bad ellipses
		if (badEllipseFilter(ellipseCorrect, size2)){

			ellipse(frame2, ellipseCorrect, colorLBlue, 1, 8);

			//block in center
			if (ellipseRaw.center.y + darkestPixelConfirm.y > 1 &&
				ellipseRaw.center.y + darkestPixelConfirm.y + 1 < frame2.size().height &&
				ellipseRaw.center.x + darkestPixelConfirm.x > 1 &&
				ellipseRaw.center.x + darkestPixelConfirm.x + 1 < frame2.size().width){
				for (int i = -1; i < 1; i++){
					for (int j = -1; j < 1; j++){
						frame2.at<Vec3b>(Point2f(ellipseRaw.center.x + darkestPixelConfirm.x + i, ellipseRaw.center.y + darkestPixelConfirm.y + j))[0] = 200;
						frame2.at<Vec3b>(Point2f(ellipseRaw.center.x + darkestPixelConfirm.x + i, ellipseRaw.center.y + darkestPixelConfirm.y + j))[1] = 200;
						frame2.at<Vec3b>(Point2f(ellipseRaw.center.x + darkestPixelConfirm.x + i, ellipseRaw.center.y + darkestPixelConfirm.y + j))[2] = 0;
					}
				}
				frame2.at<Vec3b>(Point2f(ellipseRaw.center.x + darkestPixelConfirm.x, ellipseRaw.center.y + darkestPixelConfirm.y))[0] = 0;
				frame2.at<Vec3b>(Point2f(ellipseRaw.center.x + darkestPixelConfirm.x, ellipseRaw.center.y + darkestPixelConfirm.y))[1] = 255;
				frame2.at<Vec3b>(Point2f(ellipseRaw.center.x + darkestPixelConfirm.x, ellipseRaw.center.y + darkestPixelConfirm.y))[2] = 0;
			}
		}
	}

	//waitKey(1);

	return frame2;
}

/**
conducts background subtraction on a single frame
@param input frame
@param size of frame
@return processed frame
*/
Mat doBackgroundSubtraction(Mat input, Size size){

	//cout << getDarkestPixel(input) << endl;


	threshold(input, edges, getDarkestPixel(input) + 20, 255, 1);

	resize(edges, resizeF1, size);

#if CV_MAJOR_VERSION ==3
	pMOG2_1->apply(resizeF1, fgMaskMOG2_1, .45);
#else
	pMOG2_1->operator()(resizeF1, fgMaskMOG2_1, .45);
#endif

	//highlightPupil(fgMaskMOG2_1.clone());

	//return fgMaskMOG2_1;


	return highlightPupil(fgMaskMOG2_1.clone());;
}

Mat doPupilFinder(Mat input, Size size){

	//highlightPupil(input);

	//markPupil(input);

	pupilAreaFit(input);


	resize(frame2, resizeF1, size);

	imshow("resized", resizeF1);

	return resizeF1;
}

int processVideo(String inputVideoPath, String outputVideoPath){

	VideoCapture inputVideo1(inputVideoPath); // Open input

	//check
	if (!inputVideo1.isOpened())
	{
		cout << "Could not open the input video: " << inputVideoPath << endl;
		cin.get();
		return -1;
	}

	int outSizeDivisor = 2;

	Size S = Size((int)inputVideo1.get(CV_CAP_PROP_FRAME_WIDTH) / outSizeDivisor,    // Acquire input size
		(int)inputVideo1.get(CV_CAP_PROP_FRAME_HEIGHT) / outSizeDivisor);

	VideoWriter outputVideo1(outputVideoPath, (int)inputVideo1.get(CV_CAP_PROP_FOURCC), (int)inputVideo1.get(CV_CAP_PROP_FPS), S); // Open the output

	if (!outputVideo1.isOpened())
	{
		cout << "Could not open the output video for write: " << outputVideoPath << endl;
		return -1;
	}

	int frameCount = 0;

	for (;;)
	{
		//Mat frame1;
		inputVideo1 >> frame1;
		if (frame1.empty()) break;

		frameCount += 1;
		cout << "frame # " << frameCount << endl;

		//outputVideo1 << doBackgroundSubtraction(frame1, S);
		outputVideo1 << doPupilFinder(frame1, S);
	}

	outputVideo1.release();

}//end processvideo

};
#if 0
bool is_file(const char* path) {
	struct stat buf;
	stat(path, &buf);
	return S_ISREG(buf.st_mode);
}

bool is_dir(const char* path) {
	struct stat buf;
	stat(path, &buf);
	return S_ISDIR(buf.st_mode);
}

int processDir(string path,
	int pupilSearchAreaIn, int pupilSearchXMinIn, int pupilSearchYMinIn,
	int lowThresholdCannyIn, int highThresholdCannyIn,
	int sizeIn, int darkestPixelL1In, int darkestPixelL2In) {

	//set global params
	//global params for setting
	lowThresholdCanny = lowThresholdCannyIn; //for detecting dark (low contrast) parts of pupil
	highThresholdCanny = highThresholdCannyIn; //for detecting lighter (high contrast) parts of pupil
	size = sizeIn;//max L/H of pupil
	darkestPixelL1 = darkestPixelL1In;
	darkestPixelL2 = darkestPixelL2In;
	pupilSearchArea = pupilSearchAreaIn;
	pupilSearchXMin = pupilSearchXMinIn;
	pupilSearchYMin = pupilSearchYMinIn;

	DIR*    dir;
	dirent* pdir;
	struct stat filestat;

	dir = opendir(path.c_str());

	cout << path.c_str() << endl;

	//strings holding output directories
	string outBS = "outputBS\\";
	vector<string> files;
	while (pdir = readdir(dir)){

		const string file_name = pdir->d_name;

		if (is_dir((path + file_name).c_str()))
		{
			cout << "found a directory: " << pdir->d_name << endl;
			continue;
		}

		if (file_name[0] == '.')
		{
			continue;
		}

		files.push_back(pdir->d_name);
		cout << pdir->d_name << endl;

	}

	closedir(dir);

	vector<string>::iterator it;
	int count = 1;

	for (it = files.begin(); it != files.end(); it++)
	{
		//cout << *it << " ";
		processVideo(path + *it, path + outBS + *it + ".avi");
		count++;
	}

	return 0;
}

int main(int, char**)
{
	threshDebug = true;

	return 0;
}
#endif
