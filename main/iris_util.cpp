
#include "iris_util.h"

#include <iostream>
#include <opencv2/core/cuda.hpp>// CUDA

namespace eye_tracker{


cv::Point2f toImgCoord(const cv::Point2f& point, const cv::Mat& m, double scale , int shift ) {
	return cv::Point2f(static_cast<float>((m.cols / 2 + scale*point.x) * (1 << shift)),
		static_cast<float>((m.rows / 2 + scale*point.y) * (1 << shift)));
}
cv::Point toImgCoord(const cv::Point& point, const cv::Mat& m, double scale, int shift ) {
	return cv::Point(static_cast<int>((m.cols / 2 + scale*point.x) * (1 << shift)),
		static_cast<int>((m.rows / 2 + scale*point.y) * (1 << shift)));
}
cv::RotatedRect toImgCoord(const cv::RotatedRect& rect, const cv::Mat& m, float scale) {
	return cv::RotatedRect(toImgCoord(rect.center, m, scale),
		cv::Size2f(scale*rect.size.width,
		scale*rect.size.height),
		rect.angle);
}

cv::Point2f toImgCoordInv(const cv::Point2f& point, const cv::Mat& m, double scale , int shift ) {
	return cv::Point2f(
		static_cast<float>((point.x / (1 << shift) - m.cols / 2) / scale),
		static_cast<float>((point.y / (1 << shift) - m.rows / 2) / scale)
		);
}
cv::Point toImgCoordInv(const cv::Point& point, const cv::Mat& m, double scale, int shift) {
	return cv::Point(
		static_cast<int>((point.x / (1 << shift) - m.cols / 2) / scale),
		static_cast<int>((point.y / (1 << shift) - m.rows / 2) / scale)
		);
}
cv::RotatedRect toImgCoordInv(const cv::RotatedRect& rect, const cv::Mat& m, float scale) {
	return cv::RotatedRect(toImgCoordInv(rect.center, m, scale),
		cv::Size2f(rect.size.width/scale,
		rect.size.height/scale),
		rect.angle);
}
}