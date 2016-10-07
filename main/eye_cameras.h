#ifndef EYE_CAMERAS_H
#define EYE_CAMERAS_H

#include <string>
#include <opencv2/highgui/highgui.hpp>
#include "DirectShowFrameGrabber.h"

namespace eye_tracker
{
void test_eyecam();
void record_eyecams();
void record_eyecams_mono();
void record_eyecams_mono_interactive();


class EyeCameraParent{
public:
	virtual ~EyeCameraParent() {};
	virtual bool isOpened() = 0;
	virtual void fetchFrame(cv::Mat &frame)=0;
};

class EyeCameraDS :public EyeCameraParent{
public:
	EyeCameraDS(std::string cam_name);
	~EyeCameraDS();
	bool isOpened();
	void fetchFrame(cv::Mat &frame);
protected:
private:
	Ubitrack::Drivers::DirectShowFrameGrabber DSfg;
};

/**
* @class EyeCamera
* @brief A camera object to access eye cameras
*/
class EyeCamera:public EyeCameraParent{
public:
	EyeCamera();
	EyeCamera(const int cam_id, bool is_flipped = false);
	EyeCamera(const std::string file_name, bool is_flipped=false);
	void init(const int cam_id, bool is_flipped = false);
	void init(const std::string file_name, bool is_flipped=false);

	~EyeCamera(){
	}
	bool isOpened(){ return cap_.isOpened(); }
	void fetchFrame(cv::Mat &frame);
protected:
	cv::VideoCapture cap_;
	bool is_flipped_ = false;
	bool is_image_ = false;
	cv::Mat mono_img_;
	

	void check_img_condition();
	void check_cap_condition();
private:

	EyeCamera& operator=(const EyeCamera& rhs);
};


/**
* @class CameraUndistorter
* @brief Image undistortion class. This class keeps undistortion map for efficiency
*/
class CameraUndistorter
{
public:
	CameraUndistorter(const cv::Mat &K, const cv::Vec<double, 8> &distCoeffs)
		: K0_(K.clone()), distCoeffs_(distCoeffs)
	{
		std::cout << "Intrinsic (matrix): " << K0_ << std::endl;
		std::cout << "Intrinsic (distortion): " << distCoeffs_ << std::endl;

	}
	~CameraUndistorter() {
	}
	void init_maps(const cv::Size &s) {
		cv::initUndistortRectifyMap(K0_, distCoeffs_, cv::Mat(), K0_, s, CV_32FC1, mapx_, mapy_);
	}
	void undistort(const cv::Mat &in, cv::Mat &out) {
		if (is_dist_map_initialized_ == false) {
			init_maps(in.size());
			is_dist_map_initialized_ = true;
		}
		cv::remap(in, out, mapx_, mapy_, cv::INTER_LINEAR);
	}
protected:
	// Local variables initialized at the constructor
	cv::Mat K0_;
	cv::Vec<double, 8> distCoeffs_; // (k1 k2 p1 p2 [k3 [k4 k5 k6]])
	bool is_dist_map_initialized_ = false;
	cv::Mat mapx_, mapy_;
private:
	// Prevent copying
};

} // namespace
#endif // IRIS_DETECTOR_IR_H