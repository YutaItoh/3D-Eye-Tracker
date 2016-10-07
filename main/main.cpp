/** @mainpage Eye position tracker documentation

 @author Yuta Itoh <itoh@in.tum.de>, \n<a href="http://wwwnavab.in.tum.de/Main/YutaItoh">Homepage</a>.

**/


#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <sstream>


#include "ubitrack_util.h" // claibration file handlers
#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/thread.hpp>


#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/photo/photo.hpp>


#include "pupilFitter.h" // 2D pupil detector

#include "timer.h"

#include "eye_model_updater.h" // 3D model builder
#include "eye_cameras.h" // Camera interfaces


 
namespace {

enum InputMode { CAMERA, CAMERA_MONO, VIDEO, IMAGE };

}


int main(int argc, char *argv[]){
	
	// Variables for FPS
	eye_tracker::FrameRateCounter frame_rate_counter;

	bool kVisualization = false;
	kVisualization = true;

	InputMode input_mode =
		//InputMode::VIDEO;  // Set a video as a video source
        // InputMode::CAMERA; // Set two cameras as video sources
		 InputMode::CAMERA_MONO; // Set a camera as video sources
	    // InputMode::IMAGE;// Set an image as a video source


	////// Command line opitions /////////////
	std::string kDir = "C:/Users/Yuta/Dropbox/work/Projects/20150427_Alex_EyeTracker/";
	std::string media_file;
	std::string media_file_stem;
	//std::string kOutputDataDirectory(kDir + "out/");	// Data output directroy
	if (argc > 2) {
		boost::filesystem::path file_name = std::string(argv[2]);
		kDir = std::string(argv[1]);
		media_file_stem = file_name.stem().string();
		media_file = kDir + file_name.string();
		//kOutputDataDirectory = kDir + "./";
		std::cout << "Load " << media_file << std::endl;
		std::string media_file_ext = file_name.extension().string();

		if (media_file_ext == ".avi" ||
			media_file_ext == ".mp4" ||
			media_file_ext == ".wmv") {
			input_mode = InputMode::VIDEO;
		}else{
			input_mode = InputMode::IMAGE;
		}
	}
	else {
		if (input_mode == InputMode::IMAGE || input_mode == InputMode::VIDEO) {
			switch (input_mode)
			{
			case InputMode::IMAGE:
				media_file = kDir + "data3/test.png";
				media_file_stem = "test";
				break;
			case InputMode::VIDEO:
				media_file = kDir + "out/test.avi";
				media_file_stem = "test";
				break;
			default:
				break;
			}
		}
	}
	///////////////

	
	//// Camera intrinsic parameters
	std::string calib_path="../../docs/cameraintrinsics_eye.txt";
	eye_tracker::UbitrackTextReader<eye_tracker::Caib> ubitrack_calib_text_reader;
	if (ubitrack_calib_text_reader.read(calib_path) == false){
		std::cout << "Calibration file onpen error: " << calib_path << std::endl;
		return -1;
	}
	cv::Mat K; // Camera intrinsic matrix in OpenCV format
	cv::Vec<double, 8> distCoeffs; // (k1 k2 p1 p2 [k3 [k4 k5 k6]]) // k: radial, p: tangential
	ubitrack_calib_text_reader.data_.get_parameters_opencv_default(K, distCoeffs);

	// Focal distance used in the 3D eye model fitter
	double focal_length = (K.at<double>(0,0)+K.at<double>(1,1))*0.5; //  Required for the 3D model fitting

	
	// Set mode parameters
	size_t kCameraNums;
	switch (input_mode)
	{
	case InputMode::IMAGE:
	case InputMode::VIDEO:
	case InputMode::CAMERA_MONO:
		kCameraNums = 1;
		break;
	case InputMode::CAMERA:
		kCameraNums = 2;
		break;
	default:
		break;
	}
	

	// Setup of classes that handle monocular/stereo camera setups
	// We can encapslate them into a wrapper class in future update
	std::vector<std::unique_ptr<eye_tracker::EyeCameraParent>> eyecams(kCameraNums);                 // Image sources
	std::vector<std::unique_ptr<eye_tracker::CameraUndistorter>> camera_undistorters(kCameraNums); // Camera undistorters
	std::vector<std::string> window_names(kCameraNums);                                            // Window names
	std::vector<cv::Mat> images(kCameraNums);                                                      // buffer images
	std::vector<std::string> file_stems(kCameraNums);                                              // Output file stem names
	std::vector<int> camera_indices(kCameraNums);                                                  // Camera indices for Opencv capture
	std::vector<std::unique_ptr<eye_tracker::EyeModelUpdater>> eye_model_updaters(kCameraNums);    // 3D eye models

	// Instantiate and initialize the class vectors
	try{
		switch (input_mode)
		{
		case InputMode::IMAGE:
			eyecams[0] = std::make_unique<eye_tracker::EyeCamera>(media_file, false);
			eye_model_updaters[0] = std::make_unique<eye_tracker::EyeModelUpdater>(focal_length, 5, 0.5);
			camera_undistorters[0] = std::make_unique<eye_tracker::CameraUndistorter>(K, distCoeffs);
			window_names = { "Video/Image" };
			file_stems = { media_file_stem };
			break;
		case InputMode::VIDEO:
			eyecams[0] = std::make_unique<eye_tracker::EyeCamera>(media_file, false);
			eye_model_updaters[0] = std::make_unique<eye_tracker::EyeModelUpdater>(focal_length, 5, 0.5);
			camera_undistorters[0] = std::make_unique<eye_tracker::CameraUndistorter>(K, distCoeffs);
			window_names = { "Video/Image" };
			file_stems = { media_file_stem };
			break;
		case InputMode::CAMERA:
			camera_indices[0] = 0;
			camera_indices[1] = 2;
#if 0
			// OpenCV HighGUI frame grabber
			eyecams[0] = std::make_unique<eye_tracker::EyeCamera>(camera_indices[0], false);
			eyecams[1] = std::make_unique<eye_tracker::EyeCamera>(camera_indices[1], false);
#else
			// DirectShow frame grabber
			eyecams[0] = std::make_unique<eye_tracker::EyeCameraDS>("Pupil Cam1 ID0");
			eyecams[1] = std::make_unique<eye_tracker::EyeCameraDS>("Pupil Cam1 ID2");
#endif
			eye_model_updaters[0] = std::make_unique<eye_tracker::EyeModelUpdater>(focal_length, 5, 0.5);
			eye_model_updaters[1] = std::make_unique<eye_tracker::EyeModelUpdater>(focal_length, 5, 0.5);
			camera_undistorters[0] = std::make_unique<eye_tracker::CameraUndistorter>(K, distCoeffs);
			camera_undistorters[1] = std::make_unique<eye_tracker::CameraUndistorter>(K, distCoeffs);
			window_names = { "Cam0", "Cam1" };
			file_stems = { "cam0", "cam1" };
			break;
		case InputMode::CAMERA_MONO:
			eyecams[0] = std::make_unique<eye_tracker::EyeCameraDS>("Pupil Cam1 ID0"); //
			eye_model_updaters[0] = std::make_unique<eye_tracker::EyeModelUpdater>(focal_length, 5, 0.5);
			camera_undistorters[0] = std::make_unique<eye_tracker::CameraUndistorter>(K, distCoeffs);
			window_names = { "Cam0" };
			file_stems = { "cam0" };
			break;
		default:
			break;
		}
	}
	catch (char *c){
		std::cout << "Exception: ";
		std::cout << c << std::endl;
		return 0;
	}


	////////////////////////
	// 2D pupil detector
	PupilFitter pupilFitter;
	pupilFitter.setDebug(false);
	/////////////////////////

	// Main loop
	const char kTerminate = 27;//Escape 0x1b
	bool is_run = true;
	while (is_run) {

		// Fetch key input
		char kKEY = 0;
		if (kVisualization) {
			kKEY = cv::waitKey(1);
		}
		switch (kKEY) {
		case kTerminate:
			is_run = false;
			break;
		}

		// Fetch images
		for (size_t cam = 0; cam < kCameraNums; cam++) {
			eyecams[cam]->fetchFrame(images[cam]);
		}
		// Process each camera images
		for (size_t cam = 0; cam < kCameraNums; cam++) {
			cv::Mat &img = images[cam];
			if (img.empty()) {
				//is_run = false;
				break;
			}

			// Undistort a captured image
			camera_undistorters[cam]->undistort(img, img);

			cv::Mat img_rgb_debug = img.clone();
			cv::Mat img_grey;

			switch (kKEY) {
			case 'r':
				eye_model_updaters[cam]->reset();
				break;
			case 'p':
				eye_model_updaters[cam]->add_fitter_max_count(10);
				break;
			default:
				break;
			}

			// 2D ellipse detection
			std::vector<cv::Point2f> inlier_pts;
			cv::cvtColor(img, img_grey, CV_RGB2GRAY);
			cv::RotatedRect rr_pf;
			bool is_pupil_found = pupilFitter.pupilAreaFitRR(img_grey, rr_pf, inlier_pts);

			singleeyefitter::Ellipse2D<double> el = singleeyefitter::toEllipse<double>(eye_tracker::toImgCoordInv(rr_pf, img, 1.0));

			// 3D eye pose estimation
			bool is_reliable = false;
			bool is_added = false;
			const bool force_add = false;
			const double kReliabilityThreshold = 0.8;// 0.96;
			double ellipse_realiability = 0.0; /// Reliability of a detected 2D ellipse based on 3D eye model
			if (is_pupil_found) {
				if (eye_model_updaters[cam]->is_model_built()) {
					ellipse_realiability = eye_model_updaters[cam]->compute_reliability(img, el, inlier_pts);
					is_reliable = (ellipse_realiability > kReliabilityThreshold);
					//					is_reliable = true;
				}
				else {
					is_added = eye_model_updaters[cam]->add_observation(img_grey, el, inlier_pts, force_add);
				}

			}

			// Visualize results
			if (cam == 0 && kVisualization) {

				// 2D pupil
				if (is_pupil_found) {
					cv::ellipse(img_rgb_debug, rr_pf, cv::Vec3b(255, 128, 0), 1);
				}

				// 3D eye ball
				if (eye_model_updaters[cam]->is_model_built()) {
					cv::putText(img, "Reliability: " + std::to_string(ellipse_realiability), cv::Point(30, 440), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 128, 255), 1);
					if (is_reliable) {
						eye_model_updaters[cam]->render(img_rgb_debug, el, inlier_pts);
					}
				}else{
					eye_model_updaters[cam]->render_status(img_rgb_debug);
					cv::putText(img, "Sample #: " + std::to_string(eye_model_updaters[cam]->fitter_count()) + "/" + std::to_string(eye_model_updaters[cam]->fitter_end_count()),
						cv::Point(30, 440), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 128, 255), 2);
				}

				cv::imshow(window_names[cam], img_rgb_debug);

			} // Visualization

		} // For each cameras

		// Compute FPS
		frame_rate_counter.count();
		// Print current frame data
		static int ss = 0;
		if (ss++ > 100) {
			std::cout << "Frame #" << frame_rate_counter.frame_count() << ", FPS=" << frame_rate_counter.fps() << std::endl;
			ss = 0;
		}

	}// Main capture loop

	return 0;

}
