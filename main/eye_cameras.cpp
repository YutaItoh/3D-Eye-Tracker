#include <iostream>
#include "eye_cameras.h"

#include "timer.h"


#include <boost/filesystem.hpp>

//#include <spdlog/spdlog.h>
//#include <plog/Log.h> // Step1: include the header.

namespace eye_tracker
{
namespace fs = boost::filesystem;
	
void record_eyecams_mono_interactive(){
	const size_t kCcameraNums = 1;

	// Open and check cameras
	cv::Mat images[kCcameraNums];
	EyeCamera eyecams[kCcameraNums] = { EyeCamera(0,true)};
	std::string window_names[kCcameraNums] = { "Cam0" };
	for (size_t cam = 0; cam < kCcameraNums; cam++){
		// Check if the cameras are opened
		if (eyecams[cam].isOpened() == false){
			std::cout << "Could not open the camera" << std::endl;
			return;
		}
		// Capture single frames to get image sizes;
		for (size_t cam = 0; cam < kCcameraNums; cam++){
			eyecams[cam].fetchFrame(images[cam]);
			if (images[cam].empty()){
				std::cout << "Could not capture an image" << std::endl;
				return;
			}
		}
	}

	// Save buffered images to files
	std::ostringstream ost_frame_id;
	const std::string kDir = "./tmp/";
	const std::streamsize kStreamSize = 3;
	const char kPaddingChar = '0';
	const std::string kImageFormat = "png";
	cv::VideoWriter outputVideos[kCcameraNums];
	for (size_t cam = 0; cam < kCcameraNums; cam++){
		std::ostringstream ost_video_name;
		ost_video_name << kDir << "cam" << cam << ".avi";
		outputVideos[cam].open(ost_video_name.str(), -1, 30, images[cam].size());
	}
	// Capture and store images
	size_t frame_count = 0;
	timer timer0;
	timer0.pause();
	bool run = true;
	while (run){
		//	while (frame_count<kMaxCaptureFrame && cv::waitKey(5) != 'q'){

		// First fetch images
		for (size_t cam = 0; cam < kCcameraNums; cam++){
			eyecams[cam].fetchFrame(images[cam]);
			if (images[cam].empty()){
				std::cout << "Could not capture an image" << std::endl;
				return;
			}
		}
		for (size_t cam = 0; cam < kCcameraNums; cam++){
			cv::imshow(window_names[cam], images[cam]);
		}
		switch (cv::waitKey(5))
		{
		case 't':
		for (size_t cam = 0; cam < kCcameraNums; cam++){
			ost_frame_id.str("");
			ost_frame_id.clear();
			ost_frame_id << "cam" << cam << "_" << std::setw(kStreamSize) << std::setfill(kPaddingChar) << frame_count << "." << kImageFormat;
			cv::imwrite(kDir + ost_frame_id.str(), images[cam]);
			outputVideos[cam] << images[cam];
		}
			break;
		case 'q':
			run = false;
			break;
		default:
			break;
		}

		// Compute FPS
		frame_count++;
		const size_t kSkipFrameCount = 50;
		if (frame_count == kSkipFrameCount)timer0.resume(); /// Wait measuring time until the process gets stabilized
		const double kFPS = (frame_count - kSkipFrameCount) / timer0.elapsed();
		std::cout << "FPS=" << kFPS << ", 2D pupil params " << std::endl; // Print current frame data
	}
}

void record_eyecams_mono(){
	const size_t kMaxCaptureFrame = 300;
	const size_t kCcameraNums = 1;

	// Open and check cameras
	std::vector<std::unique_ptr<eye_tracker::EyeCameraParent>> eyecams(kCcameraNums); // Image sources
	cv::Mat images[kCcameraNums];
	eyecams[0] = std::make_unique<eye_tracker::EyeCameraDS>("Pupil Cam1 ID2");
	std::string window_names[kCcameraNums] = { "Cam0" };
	for (size_t cam = 0; cam < kCcameraNums; cam++){
		// Check if the cameras are opened
		if (eyecams[cam]->isOpened() == false){
			std::cout << "Could not open the camera" << std::endl;
			return;
		}
		// Capture single frames to get image sizes;
		for (size_t cam = 0; cam < kCcameraNums; cam++){
			eyecams[cam]->fetchFrame(images[cam]);
			if (images[cam].empty()){
				std::cout << "Could not capture an image" << std::endl;
				return;
			}
		}
	}
	
	// Create an image buffer size of the number of cameras and their frame length
	cv::Mat buffer_images[kCcameraNums][kMaxCaptureFrame];
	for (size_t i = 0; i < kMaxCaptureFrame; i++){
		for (size_t cam = 0; cam < kCcameraNums; cam++){
			buffer_images[cam][i] = cv::Mat::zeros(images[0].size(), images[0].type());
		}
	}

	// Capture and store images
	size_t frame_count = 0;
	timer timer0;
	timer0.pause();
	for( size_t i = 0; i < kMaxCaptureFrame; i++){
		//	while (frame_count<kMaxCaptureFrame && cv::waitKey(5) != 'q'){

		// First fetch images
		for (size_t cam = 0; cam < kCcameraNums; cam++){
			eyecams[cam]->fetchFrame(images[cam]);
			if (images[cam].empty()){
				std::cout << "Could not capture an image" << std::endl;
				return;
			}
		}

		// Copy the cpatured images to the buffer
		for (size_t cam = 0; cam < kCcameraNums; cam++){
			cv::Mat &img = images[cam];
			cv::imshow(window_names[cam], img);
			img.copyTo(buffer_images[cam][i]);
		}

		cv::waitKey(5);
		// Compute FPS
		frame_count++;
		const size_t kSkipFrameCount = 50;
		if (frame_count == kSkipFrameCount)timer0.resume(); /// Wait measuring time until the process gets stabilized
		const double kFPS = (frame_count - kSkipFrameCount) / timer0.elapsed();
		std::cout << "FPS=" << kFPS << ", 2D pupil params " << std::endl; // Print current frame data
	}
	
	// Save buffered images to files
	std::ostringstream ost_frame_id;
	const std::string kDir = "./tmp/";
	const std::streamsize kStreamSize = 3;
	const char kPaddingChar = '0';
	const std::string kImageFormat = "png";
	for (size_t cam = 0; cam < kCcameraNums; cam++){
		std::ostringstream ost_video_name;
		ost_video_name << kDir<<"cam" << cam << ".avi";
		cv::VideoWriter outputVideo(ost_video_name.str(), -1, 30, buffer_images[cam][0].size());
		for (size_t i = 0; i < kMaxCaptureFrame; i++){
			ost_frame_id.str("");
			ost_frame_id.clear();
			ost_frame_id << "cam"<<cam<<"_"<<std::setw(kStreamSize) << std::setfill(kPaddingChar) << i<<"."<<kImageFormat;
			cv::imwrite(kDir + ost_frame_id.str(),buffer_images[cam][i]);
			outputVideo << buffer_images[cam][i];
		}
	}


}

void record_eyecams(){
	const size_t kMaxCaptureFrame = 100;
	const size_t kCcameraNums = 2;

	// Open and check cameras
	cv::Mat images[kCcameraNums];
	EyeCamera eyecams[kCcameraNums] = { EyeCamera(0), EyeCamera(2, true) };
	std::string window_names[kCcameraNums] = { "Cam0", "Cam1" };
	for (size_t cam = 0; cam < kCcameraNums; cam++){
		// Check if the cameras are opened
		if (eyecams[cam].isOpened() == false){
			std::cout << "Could not open the camera" << std::endl;
			return;
		}
		// Capture single frames to get image sizes;
		for (size_t cam = 0; cam < kCcameraNums; cam++){
			eyecams[cam].fetchFrame(images[cam]);
			if (images[cam].empty()){
				std::cout << "Could not capture an image" << std::endl;
				return;
			}
		}
	}
	
	// Create an image buffer size of the number of cameras and their frame length
	cv::Mat buffer_images[kCcameraNums][kMaxCaptureFrame];
	for (size_t i = 0; i < kMaxCaptureFrame; i++){
		for (size_t cam = 0; cam < kCcameraNums; cam++){
			buffer_images[cam][i] = cv::Mat::zeros(images[0].size(), images[0].type());
		}
	}

	// Capture and store images
	size_t frame_count = 0;
	timer timer0;
	timer0.pause();
	for( size_t i = 0; i < kMaxCaptureFrame; i++){
		//	while (frame_count<kMaxCaptureFrame && cv::waitKey(5) != 'q'){

		// First fetch images
		for (size_t cam = 0; cam < kCcameraNums; cam++){
			eyecams[cam].fetchFrame(images[cam]);
			if (images[cam].empty()){
				std::cout << "Could not capture an image" << std::endl;
				return;
			}
		}

		// Copy the cpatured images to the buffer
		for (size_t cam = 0; cam < kCcameraNums; cam++){
			cv::Mat &img = images[cam];
			cv::imshow(window_names[cam], img);
			img.copyTo(buffer_images[cam][i]);
		}

		cv::waitKey(5);
		// Compute FPS
		frame_count++;
		const size_t kSkipFrameCount = 50;
		if (frame_count == kSkipFrameCount)timer0.resume(); /// Wait measuring time until the process gets stabilized
		const double kFPS = (frame_count - kSkipFrameCount) / timer0.elapsed();
		std::cout << "FPS=" << kFPS << ", 2D pupil params " << std::endl; // Print current frame data
	}
	
	// Save buffered images to files
	std::ostringstream ost_frame_id;
	const std::string kDir = "./tmp/";
	const std::streamsize kStreamSize = 3;
	const char kPaddingChar = '0';
	const std::string kImageFormat = "png";
	for (size_t cam = 0; cam < kCcameraNums; cam++){
		std::ostringstream ost_video_name;
		ost_video_name << kDir<<"cam" << cam << ".avi";
		cv::VideoWriter outputVideo(ost_video_name.str(), -1, 30, buffer_images[cam][0].size());
		for (size_t i = 0; i < kMaxCaptureFrame; i++){
			ost_frame_id.str("");
			ost_frame_id.clear();
			ost_frame_id << "cam"<<cam<<"_"<<std::setw(kStreamSize) << std::setfill(kPaddingChar) << i<<"."<<kImageFormat;
			cv::imwrite(kDir + ost_frame_id.str(),buffer_images[cam][i]);
			outputVideo << buffer_images[cam][i];
		}
	}


}


void test_eyecam(){

#if 0
	Ubitrack::Drivers::DirectShowFrameGrabber DSfg0("Pupil Cam1 ID0");
	DSfg0.start();
	while (cv::waitKey(5) != 'q');
	return;
#endif
	EyeCamera eyecamL(0);
	EyeCamera eyecamR(2);
	EyeCamera eyecamW(1);
	if (eyecamL.isOpened() && eyecamR.isOpened()){
		cv::Mat imgL, imgR, imgW;
		size_t frame_count = 0;
		size_t kSkipFrameCount = 50;
		timer timer0;
		timer0.pause();
		while (1){

			eyecamL.fetchFrame(imgL);
			eyecamR.fetchFrame(imgR);
			eyecamW.fetchFrame(imgW);
			cv::imshow("camera left", imgL);
			cv::imshow("camera right", imgR);
			cv::imshow("camera world", imgW);
			if (cv::waitKey(1) == 'q')break;

			// Compute and print FPS
			const size_t kSkipFrameCount = 50;
			if (frame_count++ == kSkipFrameCount)timer0.resume();/// Wait measuring time until the process gets stabilized
			const double kFPS = (frame_count - kSkipFrameCount) / timer0.elapsed();
			std::cout << "Frame #" << frame_count << ", FPS=" << kFPS << std::endl;
		}
	}
}

void EyeCamera::check_img_condition(){
	try{
		if (mono_img_.empty()){
			throw "EyeCamera: file open error";
		}
	}
	catch (char *c){
		std::cout << c << std::endl;
		throw;
	}
}
void EyeCamera::check_cap_condition(){
	try{
		if (cap_.isOpened() == false){
			throw "EyeCamera: camera/file open error";
		}
	}
	catch (char *c){
		std::cout << c << std::endl;
		throw;
	}
}

EyeCamera::EyeCamera(){

}
EyeCamera::EyeCamera(const int cam_id, bool is_flipped)
{
	init(cam_id, is_flipped);
}
void EyeCamera::init(const int cam_id, bool is_flipped)
{
	is_flipped_=is_flipped;
	std::cout << "EyeCamera: Open a camera of index: "<< cam_id << std::endl;
	cap_.open(CV_CAP_DSHOW+ cam_id);
	check_cap_condition();
	cap_.set(CV_CAP_PROP_FRAME_WIDTH, 640);
	cap_.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
	cap_.set(CV_CAP_PROP_FPS, 120);
	std::cout << "EyeCamera: Camera setting: " << std::endl;
	std::cout << "EyeCamera: Width: " << cap_.get(CV_CAP_PROP_FRAME_WIDTH) << std::endl;
	std::cout << "EyeCamera: Height: " << cap_.get(CV_CAP_PROP_FRAME_HEIGHT) << std::endl;
	std::cout << "EyeCamera: FPS: " << cap_.get(CV_CAP_PROP_FPS) << std::endl;
	is_image_ = false;
}

EyeCamera::EyeCamera(const std::string file_name, bool is_flipped)
{
	init(file_name, is_flipped);
}
void EyeCamera::init(const std::string file_name, bool is_flipped)
{
	is_flipped_ = is_flipped;
	fs::path data_file_path(file_name);
	std::cout << "EyeCamera: Open a video file: " << file_name << std::endl;
	const std::string kEXT = data_file_path.extension().string();
	if (kEXT == ".avi" || kEXT == ".mp4"){
		cap_.open(data_file_path.string());
		std::cout << "Open a video file: " << file_name << std::endl;
		check_cap_condition();
		is_image_ = false;
	}
	else{
		mono_img_ = cv::imread(data_file_path.string());
		check_img_condition();
		is_image_ = true;
	}
}

void EyeCamera::fetchFrame(cv::Mat &frame){
	if (is_image_==false){
		cap_ >> frame;
	}
	else
	{
		frame = mono_img_.clone();
	}
	if (is_flipped_) cv::flip(frame, frame, -1);//flip both
}




EyeCameraDS::EyeCameraDS(std::string cam_name)
	: DSfg(cam_name)
{
	DSfg.start();
}
EyeCameraDS::~EyeCameraDS(){
	DSfg.stop();
}
bool EyeCameraDS::isOpened(){
	return true;
}
void EyeCameraDS::fetchFrame(cv::Mat &frame){
	DSfg.getFrame(frame);
}



} // namespace