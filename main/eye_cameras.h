#ifndef EYE_CAMERAS_H
#define EYE_CAMERAS_H

#include <string>
#include <opencv2/highgui/highgui.hpp>
#include "DirectShowFrameGrabber.h"

namespace eyecamera
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

} // namespace
#endif // IRIS_DETECTOR_IR_H