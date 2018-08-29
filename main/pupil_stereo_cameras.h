#pragma once
/*
example file for use of the pupil cam interface

Copyright
Alexander Plopski
NAIST Interactive Media Design Laboratory
12.01.2017

Last modified by Jason Orlosky
Osaka University, Cybermedia Center
Augusta 29th, 2018
*/

#include <pupilcam/FrameGrabber.hpp>

	const int exposure_max = (int)((1.f / 3) / 0.0001); //convert from seconds into multiple of 0.1ms

	PupilCamera::Camera_Manager *manager;
	std::vector<std::string> cameras;
	cv::Mat tmp2;
	cv::RotatedRect tmp;
	std::vector<int> exposure;

	void initialize() {
		manager = new PupilCamera::Camera_Manager();
		manager->init();
		manager->update(cameras);
		std::cout << "number of cameras:" << cameras.size() << "\n";
		if (cameras.empty())
		{
			delete manager;
			std::cout << "no cameras present. Press any key to exit." << std::endl;
			std::cin.get();
		}
		for (int i = 0; i < cameras.size(); ++i)
		{
			PupilCamera::PupilCameraResults status = manager->openCamera(i);
			if (status != PupilCamera::PupilCameraResults::SUCCESS)
			{
				std::cout << "failed to open camera" << i << "\n";
			}
			else
			{
				manager->startStream(i, 480, 640, 60, 3);
			}
		}
		exposure.resize(cameras.size(), exposure_max);
	}

	/*
	* fetches the frame for left (0) or right (1) camera
	*/
	void fetchFrame(cv::Mat &frame, int cameraNum) {
		manager->grabFrame(cameraNum, frame);
	}
