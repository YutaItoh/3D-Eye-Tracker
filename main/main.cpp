/** @mainpage Eye position tracker documentation

 @author Yuta Itoh <itoh@in.tum.de>, \n<a href="http://wwwnavab.in.tum.de/Main/YutaItoh">Homepage</a>.

**/

#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <string>
#include <sstream>
#include <math.h>
#include "ubitrack_util.h" // claibration file handlers
#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/thread.hpp>
#include "opencv2/opencv.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/photo/photo.hpp>
#include "pupilFitter.h" // 2D pupil detector
#include "timer.h"
#include "eye_model_updater.h" // 3D model builder
#include "eye_cameras.h" // Camera interfaces
#include "pupil_stereo_cameras.h" //interface to pupil stereo cameras on a single USB
#include <pupilcam/FrameGrabber.hpp>
 
namespace {

enum InputMode { CAMERA, CAMERA_MONO, CAMERA_PUPIL, VIDEO, IMAGE };

}

int main(int argc, char *argv[]){
	
	InputMode input_mode =
		//InputMode::VIDEO;  // Set a video as a video source
		//InputMode::CAMERA; // Set two cameras (separate USB buses) as video sources
		InputMode::CAMERA_PUPIL; //Pupil stereo cameras (on a single cable, uses libuvc)
								 // InputMode::CAMERA_MONO; // Set a single camera as a video source
								 // InputMode::IMAGE;// Set an image as a the source

	string folderPath = "C:\\Storage\\Research\\Eye Tracking\\coordinates.txt";


	// Variables for FPS
	eye_tracker::FrameRateCounter frame_rate_counter;
	bool kVisualization = false;
	kVisualization = true;
	singleeyefitter::EyeModelFitter::Circle curr_circle;
	////// Command line opitions /////////////
	std::string kDir = "C:/Users/Yuta/Dropbox/work/Projects/20150427_Alex_EyeTracker/";
	std::string media_file;
	std::string media_file_stem;

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
		std::cout << "Calibration file open error: " << calib_path << std::endl;
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
	case InputMode::CAMERA_PUPIL:
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
			camera_indices[1] = 1;
#if 0
			// OpenCV HighGUI frame grabber
			eyecams[0] = std::make_unique<eye_tracker::EyeCamera>(camera_indices[0], false);
			eyecams[1] = std::make_unique<eye_tracker::EyeCamera>(camera_indices[1], false);
#else
			// DirectShow frame grabber
			eyecams[0] = std::make_unique<eye_tracker::EyeCameraDS>("Pupil Cam1 ID1");
			eyecams[1] = std::make_unique<eye_tracker::EyeCameraDS>("Pupil Cam2 ID2");
#endif
			eye_model_updaters[0] = std::make_unique<eye_tracker::EyeModelUpdater>(focal_length, 5, 0.5);
			eye_model_updaters[1] = std::make_unique<eye_tracker::EyeModelUpdater>(focal_length, 5, 0.5);
			camera_undistorters[0] = std::make_unique<eye_tracker::CameraUndistorter>(K, distCoeffs);
			camera_undistorters[1] = std::make_unique<eye_tracker::CameraUndistorter>(K, distCoeffs);
			window_names = { "Cam0", "Cam1" };
			file_stems = { "cam0", "cam1" };
			break;
		case InputMode::CAMERA_PUPIL:
		{
			camera_indices[0] = 0;
			camera_indices[1] = 1;
			eye_model_updaters[0] = std::make_unique<eye_tracker::EyeModelUpdater>(focal_length, 5, 0.5);
			eye_model_updaters[1] = std::make_unique<eye_tracker::EyeModelUpdater>(focal_length, 5, 0.5);
			camera_undistorters[0] = std::make_unique<eye_tracker::CameraUndistorter>(K, distCoeffs);
			camera_undistorters[1] = std::make_unique<eye_tracker::CameraUndistorter>(K, distCoeffs);
			window_names = { "Cam0", "Cam1" };
			file_stems = { "cam0", "cam1" };
			initialize();
			manager->setExposureTime(0, .035);
			manager->setExposureTime(1, .035);
			break;
		}
		case InputMode::CAMERA_MONO:
			eyecams[0] = std::make_unique<eye_tracker::EyeCameraDS>("Pupil Cam1 ID1"); //
			eye_model_updaters[0] = std::make_unique<eye_tracker::EyeModelUpdater>(focal_length, 5, 0.5);
			camera_undistorters[0] = std::make_unique<eye_tracker::CameraUndistorter>(K, distCoeffs);
			window_names = { "Cam1" };
			file_stems = { "cam1" };
			break;
		default:
			break;
		}
	}
	catch (char *c) {
		std::cout << "Exception: ";
		std::cout << c << std::endl;
		return 0;
	}


	////////////////////////
	// 2D pupil detector
	PupilFitter pupilFitter;
	pupilFitter.setDebug(false);
	/////////////////////////

	//For running a video
	//VideoCapture inputVideo1("C:\\Documents\\Osaka\\Research\\Eye Tracking\\Benchmark Videos\\eyetracking4.avi"); // Open input

	//for video writing
	/*
	VideoWriter outputVideo1;
	outputVideo1.open("C:\\Documents\\Osaka\\Research\\Eye Tracking\\Benchmark Videos\\outSaccade.avi",
		CV_FOURCC('W', 'M', 'V', '2'),
		20,
		cv::Size(640,480),
		true);
		
	Mat frame1;
	*/

	// Main loop
	const char kTerminate = 27;//Escape 0x1b
	bool is_run = true;
	bool isSaccade = false;
	bool isBlink = false;
	bool originalSet[2] = { false, false };
	bool prevSaccade = false; //added if a saccade value was detected in the previous frame
	int blinkCount = 0; //holds the number of blinks for this video
	int saccadeCount = 0; //holds the number of saccades for this video
	int medianTotal = 50;
	double originalEyeSizes[2] = { 0, 0 }; //stores original 2D radii 
	double camSpheres[6] = { 0, 0, 0, 0, 0, 0 }; //holds left cam (0-2) and right cam (3-5) sphere centers
	vector<float> timeData; //vector holding timestamps in ms corresponding to gaze data for N frames
	vector<float> xData; //corresponding x eye rotations for N frames
	vector<float> yData; //corresponding y eye rotations for N frames
	vector<float> intensityData; //holds average intensity of last N frames
	vector<singleeyefitter::EyeModelFitter::Sphere> eyes[2]; //holds a vector of spheres for the eye model filter (cam 0)
	singleeyefitter::EyeModelFitter::Sphere lastGoodEyes[2];
	singleeyefitter::EyeModelFitter::Sphere originalModels[2];


	//print instructions
	cout << endl;
	cout << "**********  This is the beta version of our open source eye tracker **********" << endl;
	cout << endl;
	cout << "Keyboard options (with either of the eye camera windows selected) include: " << endl;
	cout << "r: Resets the original eye models - do this if the initial model creation results in a poor fit. " << endl;
	cout << "d: debug mode - shows returned pupil and candidate points " << endl;
	cout << "o: debug mode off" << endl;
	cout << "x: Cleanly exists the stream - if you accidentally close the window using the mouse, you may need to unplug and replug your pupil labs USB cable" << endl;
	cout << endl;
	cout << "Edit the 'input_mode' variable to select the type of camera you are using (default is the stereo pupil labs rig on a single USB)." << endl;
	cout << "Edit the 'folderPath' variable to output coordinates to a text file in real time." << endl;
	cout << "The current 'folderPath' is: " << folderPath << endl;

	while (is_run) {

		//inputVideo1 >> frame1;//for video
		//if (frame1.empty()) {//for video
		//		break;
		//}

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
			if (InputMode::CAMERA_PUPIL) { //stereo on single/dual bus
					fetchFrame(images[cam], cam);
			}
			else { //any other camera solution
				eyecams[cam]->fetchFrame(images[cam]);
			}
		}

		//for writing data to file
		stringstream eyeVector[2];

		// Process each camera images
		for (size_t cam = 0; cam < kCameraNums; cam++) {
			
			cv::Mat &img = images[cam];
			//img = frame1; //for video
			//imshow("test", img);
			//waitKey(1);

			if (cam == 0) {
				flip(images[cam], img, -1);
			}
			
			if (img.empty()) {
				//is_run = false;
				break;
			}

			// Undistort a captured image
			//camera_undistorters[cam]->undistort(img, img);

			//cv::Mat img_rgb_debug = frame1.clone(); \\for video
			cv::Mat img_rgb_debug = img.clone();
			cv::Mat img_grey;


			switch (kKEY) {
			case 'r':
				eye_model_updaters[cam]->reset();
				break;
			case 'd':
				pupilFitter.setDebug(true);
				break;
			case 'o':
				pupilFitter.setDebug(false);
				break;
			case 'p':
				eye_model_updaters[cam]->add_fitter_max_count(10);
				break;
			case 'q':
				is_run = false;
				break;
			case 'x':
				is_run = false;
				manager->stopStream(0);
				manager->stopStream(1);
				exit(0);
			default:
				break;
			}

			const clock_t begin_time = clock();

			// 2D ellipse detection
			std::vector<cv::Point2f> inlier_pts;
			cv::cvtColor(img, img_grey, CV_RGB2GRAY);
			cv::RotatedRect rr_pf;

			bool is_pupil_found = pupilFitter.pupilAreaFitRR(img_grey, rr_pf, inlier_pts, 15, 0, 0, 15, 35, 250, 6);
			is_pupil_found = pupilFitter.badEllipseFilter(rr_pf, 250);

			//cout << "pupil fitter time: " << float(clock() - begin_time) / CLOCKS_PER_SEC << endl;

			const clock_t begin_time2 = clock();

			singleeyefitter::Ellipse2D<double> el = singleeyefitter::toEllipse<double>(eye_tracker::toImgCoordInv(rr_pf, img, 1.0));

			//cout << "singleeyefitter time: " << float(clock() - begin_time2) / CLOCKS_PER_SEC << endl;

			// 3D eye pose estimation
			bool is_reliable = false;
			bool is_added = false;
			const bool force_add = false;
			const double kReliabilityThreshold = 0;//0.96;
			double ellipse_reliability = 0.0; /// Reliability of a detected 2D ellipse based on 3D eye model

			if (is_pupil_found) {
				if (eye_model_updaters[cam]->is_model_built()) {
					ellipse_reliability = eye_model_updaters[cam]->compute_reliability(img, el, inlier_pts);
					is_reliable = (ellipse_reliability > kReliabilityThreshold);
										is_reliable = true;
					
					//remove oldest observation, add new, and rebuild model (drift correction)
					eye_model_updaters[cam]->rm_oldest_observation();
					eye_model_updaters[cam]->add_observation(img_grey, el, inlier_pts, false);
					eye_model_updaters[cam]->force_rebuild_model();

					if (eyes[cam].size() > medianTotal * .8 && originalSet[cam] == false) {
						// happens once when model is built for the first time to establish eye-box
						originalModels[cam] = eye_model_updaters[cam]->getEye();
						originalEyeSizes[cam] = eye_tracker::toImgCoord(singleeyefitter::toRotatedRect(
							singleeyefitter::project(eye_model_updaters[cam]->getEye(), focal_length)), img, 1.0f).size.height;
						originalSet[cam] = true;
					}
				}
				else { 
					is_added = eye_model_updaters[cam]->add_observation(img_grey, el, inlier_pts, force_add);
				}
			}

			// Visualize results
			if (kVisualization) {

				// 2D pupil
				if (is_pupil_found) {
					cv::ellipse(img_rgb_debug, rr_pf, cv::Vec3b(255, 128, 0), 1);
				}
				// 3D eye ball
				if (eye_model_updaters[cam]->is_model_built()) {
					
					if (is_reliable) {

						singleeyefitter::Sphere<double> medianCircle;
						//bool useDriftCorrection = false;
						//if (eyes.size() > 0) {
						//	medianCircle = eye_model_updaters[cam]->eyeModelFilter(curr_circle, eyes);
						//	useDriftCorrection = true;
						//}

						eye_model_updaters[cam]->render(img_rgb_debug, el, inlier_pts);
						eye_model_updaters[cam]->set_fitter_max_count(60); //manually sets max count
						//3D filtered eye model
						curr_circle = eye_model_updaters[cam]->unproject(img, el, inlier_pts);
						// 3D pupil (relative to filtered eye model)
						singleeyefitter::Ellipse2D<double> pupil_elTest(singleeyefitter::project(curr_circle, focal_length));
						cv::RotatedRect rr_pupilTest = eye_tracker::toImgCoord(singleeyefitter::toRotatedRect(pupil_elTest), img, 1.0f);

						bool ignoreNewEye = true;
						if (rr_pupilTest.center.x > 0 || rr_pupilTest.center.y > 0 ) {
							ignoreNewEye = false; //ignore eyes with 0 or negative origins
						}

						if (eye_model_updaters[cam]->fitter().eye) { //ensure eye model exists
							//insert current eye into filter and return a filtered model (very important for new model accuracy)
							singleeyefitter::Sphere<double> tempCircle = 
								eye_model_updaters[cam]->eyeModelFilter(eye_model_updaters[cam]->fitter().eye, eyes[cam], medianTotal, ignoreNewEye, originalModels[cam]);

							singleeyefitter::EyeModelFitter::Sphere filteredEye(tempCircle.centre, tempCircle.radius);
							cv::RotatedRect rr_eye = eye_tracker::toImgCoord(singleeyefitter::toRotatedRect(
								singleeyefitter::project(filteredEye, focal_length)), img, 1.0f);
							cv::RotatedRect originalRadius = eye_tracker::toImgCoord(singleeyefitter::toRotatedRect(
								singleeyefitter::project(originalModels[cam], focal_length)), img, 1.0f); //projection of last 3D radius into 2D coordinates
							cv::RotatedRect newRadius = eye_tracker::toImgCoord(singleeyefitter::toRotatedRect(
								singleeyefitter::project(tempCircle, focal_length)), img, 1.0f); //projection of new 3D radius into 2D coordinates 

							//note: radius of 0 returned from filter if model was bad
							if (eyes[cam].size() > 0 && tempCircle.radius != 0 && std::abs(originalRadius.size.height - newRadius.size.height) < 40){ 
								//2D projections of radii were non-zero and did not differ significantly from originals
								medianCircle = tempCircle; //pass on for model update
								lastGoodEyes[cam] = tempCircle;  //update last good eye (for possible use in next frame)
							}
							else if(lastGoodEyes[cam].radius != 0){ //filter returned 0
								medianCircle = lastGoodEyes[cam]; //use last known good eye model (last frame)
							}
						}
						else if (lastGoodEyes[cam].radius != 0){ //new model not built
							medianCircle = lastGoodEyes[cam]; //use last known good eye model
						}

						if (medianCircle.radius > 0) { // filtered eye was good -> render to screen and output coordinates

							eye_model_updaters[cam]->render_status(img_rgb_debug);
							eye_model_updaters[cam]->setEye(medianCircle); //set eye model to 

							curr_circle = eye_model_updaters[cam]->unproject(img, el, inlier_pts);
							// 3D pupil (relative to filtered eye model)
							singleeyefitter::Ellipse2D<double> pupil_el(singleeyefitter::project(curr_circle, focal_length));
							cv::RotatedRect rr_pupil = eye_tracker::toImgCoord(singleeyefitter::toRotatedRect(pupil_el), img, 1.0f);

							singleeyefitter::EyeModelFitter::Sphere filteredEye(medianCircle.centre, medianCircle.radius);
							cv::RotatedRect rr_eye = eye_tracker::toImgCoord(singleeyefitter::toRotatedRect(
								singleeyefitter::project(filteredEye, focal_length)), img, 1.0f);

							cv::ellipse(img_rgb_debug, rr_eye, cv::Vec3b(255, 255, 255), 2, CV_AA);
							cv::circle(img_rgb_debug, rr_eye.center, 3, cv::Vec3b(255, 32, 32), 2); // Eyeball center projection
							singleeyefitter::EyeModelFitter::Circle c_end = curr_circle;
							c_end.centre = curr_circle.centre + (10.0)*curr_circle.normal;
							cv::line(img_rgb_debug, rr_eye.center, rr_pupil.center, cv::Vec3b(25, 22, 222), 3, CV_AA);

							//External code
							//cout << "cam: " << cam << ", center x: " << rr_eye.center.x << ", center y: " << rr_eye.center.x << endl;
							//update time, xdata, and ydata vectors for input into saccade detector
							//dataAdd(curr_circle.centre(0), 5, xData);
							//dataAdd(curr_circle.centre(1), 5, yData);
							//dataAdd(clock(), 5, timeData);
							//float intensity = 0;

//							if (cam == 0) {//append eye 0 model data to output string (3D pupil center in c_end; 3D eye center in filteredEye)
								eyeVector[cam] << "" << c_end.centre.x() << "," << c_end.centre.y() << "," << c_end.centre.z()
									<< "," << filteredEye.centre[0] << "," << filteredEye.centre[1] << "," << filteredEye.centre[2];
								camSpheres[0 + cam * 3] = filteredEye.centre[0];
								camSpheres[1 + cam * 3] = filteredEye.centre[1];
								camSpheres[2 + cam * 3] = filteredEye.centre[2];
						}//end if: returned model radius was 0
					}
				}else{//model not built, just render current ellipses 
					eye_model_updaters[cam]->render_status(img_rgb_debug);
					cv::putText(img, "Sample #: " + std::to_string(eye_model_updaters[cam]->fitter_count()) + "/" + std::to_string(eye_model_updaters[cam]->fitter_end_count()),
						cv::Point(30, 440), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 128, 255), 2);
				}

				//outputVideo1 << img_rgb_debug; //custom video write

				//show eye camera windows
				cv::imshow(window_names[cam], img_rgb_debug);
			} // Visualization
		} // end loop for each camera 

		//OUTPUT DATA IS HERE*******
		//each eyeVector string contains six comma separated values:
		// eye center 0 (x, y, z); pupil center 0 (x, y, z); eye center 1 (x, y, z); pupil center 1 (x, y, z)
		if (eyeVector[0].str().length() > 0 && eyeVector[1].str().length() > 0) {//check to ensure strings both have data (both eyes found)
			std::ofstream myfile(folderPath); //to-Unity write
			//std::ofstream myfile; //for recording experiment data
			//myfile.open("C:\\Storage\\Research\\Eye Tracking\\testcoordinates.txt", std::ios_base::app);
			myfile << "" << eyeVector[0].str() << "," << eyeVector[1].str() << endl;
			myfile.close();
		}

		// Compute FPS
		//frame_rate_counter.count();
		//// Print current frame data
		//static int ss = 0;
		//if (ss++ > 100) {
		//	std::cout << "Frame #" << frame_rate_counter.frame_count() << ", FPS=" << frame_rate_counter.fps() << std::endl;
		//	ss = 0;
		//}

		//cout << "dist: " << pupilFitter.getInterpupillaryDifference(cam0Sphere, cam1Sphere) << endl;

		//singleeyefitter::EyeModelFitter::Circle curr_circle;
		//singleeyefitter::EyeModelFitter::Circle c_end = curr_circle;
		//c_end.centre = curr_circle.centre + (10.0)*curr_circle.normal; // Unit: mm

	}// Main capture loop

	//outputVideo1.release(); // custom video
	return 0;
}
