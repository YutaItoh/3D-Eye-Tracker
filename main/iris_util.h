#ifndef IRIS_UTIL_H
#define IRIS_UTIL_H

#include <opencv2/imgproc/imgproc.hpp>
#include <Eigen/Dense>

namespace eye_tracker{


	typedef std::complex<double> EigenVector3[3];

	cv::Point2f     toImgCoord(const cv::Point2f& point, const cv::Mat& m, double scale = 1, int shift = 0);
	cv::Point       toImgCoord(const cv::Point&   point, const cv::Mat& m, double scale = 1, int shift = 0);
	cv::RotatedRect toImgCoord(const cv::RotatedRect& rect, const cv::Mat& m, float scale = 1);
	cv::Point2f     toImgCoordInv(const cv::Point2f& point, const cv::Mat& m, double scale = 1, int shift = 0);
	cv::Point       toImgCoordInv(const cv::Point&   point, const cv::Mat& m, double scale = 1, int shift = 0);
	cv::RotatedRect toImgCoordInv(const cv::RotatedRect& rect, const cv::Mat& m, float scale = 1);

}
#endif // IRIS_UTIL_H