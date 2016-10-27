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


		threshDebug = true;
	};
	~PupilFitter(){};

	void setDebug(bool threshDebug0){
		threshDebug = threshDebug0;
	};

/**
Fits an ellipse to a pupil area in an image
@param gray BGR input image (converted to grayscale during search process)
@param rr resulting RotatedRect representing the popil ellipse contour 
@param allPtsReturn Point2f vector containing all 
@return a RotatedRect representing the pupil ellipse, returns RotatedRect with all 0s if ellipse was not found
*/
bool pupilAreaFitRR(Mat &gray, RotatedRect &rr, vector<Point2f> &allPtsReturn,
	int pupilSearchAreaIn = 10, int pupilSearchXMinIn = 0, int pupilSearchYMinIn = 0,
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
		erodeOn = false; //perform erode operation: turn off for one-offs, where eroding the image may actually hurt accuracy

		//for timing funcitons
		unsigned long long Int64 = 0;
		clock_t Start = clock();

		//find pupil
		Point darkestPixelConfirm = getDarkestPixelArea(gray);


		//correct bounds
		darkestPixelConfirm = correctBounds(darkestPixelConfirm, size);

		//find darkest pixel (for thresholding
		int darkestPixel = getDarkestPixelBetter(gray(cv::Rect(darkestPixelConfirm.x, darkestPixelConfirm.y, size, size)));

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
	//	vector<Point> allPts;
		allPts.resize(0);
		vector<Point> allPts2;
		vector<Point> allPtsHigh;

		//logical AND of contours and canny images
		allPts = getCandidates(contoursLow, biggest, thresh3, false);
		allPtsHigh = getCandidates(contoursHigh, biggestHigh, thresh4, false);

		//merge remaining points for low and high point lists 
		allPts.insert(allPts.end(), allPtsHigh.begin(), allPtsHigh.end());
		std::vector<std::vector<cv::Point>> allPtsWithOutliers;
		allPtsWithOutliers.push_back(allPts);

		//convert to gray for refinement
		cv::cvtColor(gray, gray, CV_BGR2GRAY);

		//refine points based on line fitting - Thanks Yuta! 
		if (allPts.size() > 5) {
			allPts = refinePoints(allPts, gray(cv::Rect(darkestPixelConfirm.x, darkestPixelConfirm.y, size2, size2)), 
				8, 2, gray(cv::Rect(darkestPixelConfirm.x, darkestPixelConfirm.y, size2, size2)), true);
			//temp = gray.clone();
			////add contours that also exist in canny, and if candidatesOn == true, mark checked on image
			//for (int i = 0; i < allPts.size(); i++) {
			//	temp.at<uchar>(Point2f(darkestPixelConfirm.x+allPts[i].x, darkestPixelConfirm.y+allPts[i].y)) = 255;
			//}
			//imshow("gray", temp);
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
			allPts = refinePoints(allPts, gray(cv::Rect(darkestPixelConfirm.x, darkestPixelConfirm.y, size2, size2)),
				10, 2, gray(cv::Rect(darkestPixelConfirm.x, darkestPixelConfirm.y, size2, size2)), true);
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
				allPts = refinePoints(ellipseContour.at(0), thresh3, 6, 2, 
					gray(cv::Rect(darkestPixelConfirm.x, darkestPixelConfirm.y, size2, size2)), true);
			}

			//temp = gray.clone();
			//////add contours that also exist in canny, and if candidatesOn == true, mark checked on image
			//for (int i = 0; i < allPts.size(); i++) {
			//	temp.at<uchar>(Point2f(darkestPixelConfirm.x+allPts[i].x, darkestPixelConfirm.y+allPts[i].y)) = 0;
			//}
			//imshow("gray", temp);

			/* //found that this additional refinement doesn't really help
			if (ellipseContour.size() > 0 && ellipseContour.at(0).size() > 5) {
				thresh3 = Mat::zeros(frameHeight, frameWidth, CV_8U);
				RotatedRect ellipseRaw = fitEllipse(allPts);
				ellipse(thresh3, ellipseRaw, 255, 1, 8);
				allPts = refinePoints(ellipseContour.at(0), thresh3, 8, 1, gray(cv::Rect(darkestPixelConfirm.x, darkestPixelConfirm.y, size2, size2)));
			}
			*/
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
			allPtsReturn.push_back(Point2f(darkestPixelConfirm.x, darkestPixelConfirm.y));
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

//Mats for other functions: motion detection, resizing, etc
Mat resizeF1;
Mat resizeF2;
Mat gray, detected_edges;

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
vector<Point> allPts;

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
Finds the approximate darkest pixels (an average of many), used on ROI images generated by getDarkestPixel area
@param I input image (converted to grayscale during search process)
@return a grayscale value
*/
int getDarkestPixelBetter(Mat& I)
{
	// accept only char type matrices
	CV_Assert(I.depth() == CV_8U);
	CV_Assert(I.size().width > 50);
	CV_Assert(I.size().height > 50);

	int channels = I.channels();

	int min = 255;
	float minDenominator = 0;
	float minNumerator = 0;

	//holds array of 50 min values
	vector<float> minVector;

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
	for (i = 2; i < nRows - 2; i = i + 5)
	{
		p = I.ptr<uchar>(i);
		for (j = 2; j < nCols - 2; j = j + 5)
		{
			minVector.push_back(p[j]);
			if ((p[j] + p[j - 2] + p[j + 2]) / 3 < min) {
				min = p[j];
			}
		}
	}


	//average last 50 values in minVector and set that to min (HxW of orig image must be > 50)
	//min = (int)(minDenominator / minNumerator);
	sort(minVector.begin(), minVector.end());
	min = minVector.at(minVector.size() / 100);


	return min;
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
vector<Point> getCandidates(std::vector<std::vector<cv::Point>>contours, int biggest, Mat& thresh, bool draw){

	vector<Point> allPts;

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
					frame2.at<Vec3b>(Point(x, y))[0] = 15;
					frame2.at<Vec3b>(Point(x, y))[1] = 0;
					frame2.at<Vec3b>(Point(x, y))[2] = 255;
				}

				if ((int)thresh.at<uchar>(Point(x, y)) > 0){

					allPts.push_back(Point(x, y));
					thresh.at<uchar>(Point(x, y)) = 0;

				}
				else if (candidatesOn){
					frame2.at<Vec3b>(Point(x, y))[0] = 15;
					frame2.at<Vec3b>(Point(x, y))[1] = 255;
					frame2.at<Vec3b>(Point(x, y))[2] = 0;
				}

				for (int z = 1; z <= mult * thickness; z = z + mult){
					if (((int)thresh.at<uchar>(Point(x, y + z))) > 0){
						allPts.push_back(Point(x, y + z));
						thresh.at<uchar>(Point(x, y + z)) = 0;
					}
					else if (candidatesOn){
						frame2.at<Vec3b>(Point(x, y + z))[0] = 15;
						frame2.at<Vec3b>(Point(x, y + z))[1] = 255;
						frame2.at<Vec3b>(Point(x, y + z))[2] = 0;
					}
					if (((int)thresh.at<uchar>(Point(x, y - z))) > 0){
						allPts.push_back(Point(x, y - z));
						thresh.at<uchar>(Point(x, y - z)) = 0;
					}
					else if (candidatesOn){
						frame2.at<Vec3b>(Point(x, y - z))[0] = 15;
						frame2.at<Vec3b>(Point(x, y - z))[1] = 255;
						frame2.at<Vec3b>(Point(x, y - z))[2] = 0;
					}
					if (((int)thresh.at<uchar>(Point(x + z, y))) > 0){
						allPts.push_back(Point(x + z, y));
						thresh.at<uchar>(Point(x + z, y)) = 0;
					}
					else if (candidatesOn){
						frame2.at<Vec3b>(Point(x + z, y))[0] = 15;
						frame2.at<Vec3b>(Point(x + z, y))[1] = 255;
						frame2.at<Vec3b>(Point(x + z, y))[2] = 0;
					}
					if (((int)thresh.at<uchar>(Point(x - z, y))) > 0){
						allPts.push_back(Point(x - z, y));
						thresh.at<uchar>(Point(x - z, y)) = 0;
					}
					else if (candidatesOn){
						frame2.at<Vec3b>(Point(x - z, y))[0] = 15;
						frame2.at<Vec3b>(Point(x - z, y))[1] = 255;
						frame2.at<Vec3b>(Point(x - z, y))[2] = 0;
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

//Point refinement code
//Better fits a set of candidate points to a pupil ellipse
vector<Point> refinePoints(vector<Point> allPts, Mat gray, int checkThickness, int checkSpacing, Mat grayOriginal, bool rmOutliers = false){

	//vector holding returned points with sub-pixel accuracy
	vector<Point> refinedPoints;


	//loop through all points
	for (int i = 0; i < allPts.size(); i++){

		bool isGlint = false;

		float finalX = allPts[i].x;
		float finalY = allPts[i].y;

		//cout << "old point: " << finalX << ", " << finalY;
		
		//loops checking pixels to reset best point for each candidate point
		//check x edge cases
		if (allPts[i].x - checkThickness*checkSpacing - 1 >= 0 &&
			allPts[i].x + checkThickness*checkSpacing + 1 < gray.size().width){

			float xNumerator = 0;
			float xDenominator = 0;
			
			//x loop
			for (int j = -checkThickness*checkSpacing; j < checkThickness*checkSpacing; j = j + checkSpacing){

				int centerValue = gray.at<uchar>(Point((int)allPts[i].x + j, (int)allPts[i].y));

				//int centerValueOriginal = grayOriginal.at<uchar>(Point((int)allPts[i].x, (int)allPts[i].y));
				//if (centerValueOriginal > 200 && rmOutliers) {
				//	isGlint = true;
				//	//cout << "color val" << centerValueOriginal << endl;
				//}

				//calculate and find diffs
				int leftDiff = abs(centerValue - gray.at<uchar>(Point((int)allPts[i].x + j - 1, (int)allPts[i].y)));
				int rightDiff = abs(centerValue - gray.at<uchar>(Point((int)allPts[i].x + j + 1, (int)allPts[i].y)));

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
			
			//y loop
			for (int j = -checkThickness*checkSpacing; j < checkThickness*checkSpacing; j = j + checkSpacing){

				int centerValue2 = gray.at<uchar>(Point((int)allPts[i].x, (int)allPts[i].y + j));

				//calculate and find diffs
				int topDiff = abs(centerValue2 - gray.at<uchar>(Point((int)allPts[i].x, (int)allPts[i].y + j - 1)));
				int botDiff = abs(centerValue2 - gray.at<uchar>(Point((int)allPts[i].x, (int)allPts[i].y + j + 1)));

				//add with weight
				yNumerator += (allPts[i].y + j) * (topDiff + botDiff);
				yDenominator += (topDiff + botDiff);
			}

			if (yDenominator != 0){
				finalY = yNumerator / yDenominator;
			}
		}

		if(isGlint){ //point was or near a glint, do not refine (border obscured)
			refinedPoints.push_back(Point(allPts[i].x, allPts[i].y));
			
		}
		else{ //point was not on glint, refine as usual (border visible)
			refinedPoints.push_back(Point(finalX, finalY));
		}

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

};
