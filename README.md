# 3D Eye Tracker

This software aims to provide an easy-to-compile C++ implementation of a 3D eye-tracking method.

Our software is designed for a wearable eye-tracking scnenario where a user wears a headset with an eye camera(s) that is equipped with infrared (IR) illuminations. The camera can capture close-up shots of the user's eyes.

![headset_and_image](https://raw.githubusercontent.com/YutaItoh/3D-Eye-Tracker/master/docs/headset_and_image.png)

With this software, you can obtain the following information:

* 3D information:
  * Gaze (normal) vector
  * Eyeball center w.r.t the camera
  * Pupil diameter [m]
* 2D information:
  * Pupil ellipse size [pixel] and angle [deg.]
  * (Glint position(s) [pixel], to be merged soon...)

[Sample video on youtube](https://www.youtube.com/watch?v=EH6UVQZgvJE) (this was taken from a preliminary version and will be updated with a newer version soon).

## Prerequisite

Currently, the software is compatible with Visual Studio 2015. The test was done on a 64bit Windows 10 machine only.

Most of external libraries (for vc14, x64) are bundled in `./external`, yet you still need to 
install the following two libraries:
* [OpenCV 3.1](http://opencv.org/downloads.html) 
  * We recommend to use [***a windows installer***](http://sourceforge.net/projects/opencvlibrary/files/opencv-win/3.1.0/opencv-3.1.0.exe/download) (vc14, x64)
* [Boost C++ Libraries ver. >1.62](https://sourceforge.net/projects/boost/files/boost-binaries/1.62.0/)
  * We recommend to use [***a windows installer***](https://sourceforge.net/projects/boost/files/boost-binaries/1.62.0/boost_1_62_0-msvc-14.0-64.exe/download) (boost_1_62_0-msvc-14.0-64.exe)

To setup your own Visual Studio project, you need to use [CMake](https://cmake.org/). You can find a windows installer [here](https://cmake.org/download/).
 
## How to compile

1. Clone the repository `git@github.com:YutaItoh/3D-Eye-Tracker.git` to your local folder (e.g., `/your-local-path/3D-Eye-Tracker`)
2. Open `./CMakeLists.txt` and edit the OpenCV path and the Boost path depending on your environment. For example, the default values are as follows:
   * `set(BOOST_ROOT "C:/SDK/boost_1_62_0")`
   * `set(OpenCV_DIR "C:/SDK/opencv-3.1.0/build")`
3. Start CMake GUI and set a source code path (e.g., `/your-local-path/3D-Eye-Tracker`) and a project build path (e.g., `/your-local-path/3D-Eye-Tracker/build_vc14_x64`)
<img src="https://raw.githubusercontent.com/YutaItoh/3D-Eye-Tracker/master/docs/cmake-1.png" width="600px" />
4. Press `Generate` buttons, then choose a right build environment (Visual Studio 14 2015 Win64)
<img src="https://raw.githubusercontent.com/YutaItoh/3D-Eye-Tracker/master/docs/cmake-2.png" width="300px" />
<img src="https://raw.githubusercontent.com/YutaItoh/3D-Eye-Tracker/master/docs/cmake-3.png" width="600px" />
5. Open `3d_eye_tracker.sln` in the build folder, and compile `main` project (recommend to set it as a startup project)
<img src="https://raw.githubusercontent.com/YutaItoh/3D-Eye-Tracker/master/docs/setup-1.png" width="250px" />

## How to run

The default setting uses a sinlge camera via the DirectShow filter. Check your camera name on a video capture program (e.g., Skype) and set the name in `main.cpp`:
``` c++
		case InputMode::CAMERA_MONO:
			eyecams[0]=std::make_unique<eyecamera::EyeCameraDS>("Pupil Cam1 ID0");
```
In this case, the program tries to find a camera named `"Pupil Cam1 ID0"`.

Once the program started, it initializes a 3D eye model from 2D pupil observatios:

<img src="https://raw.githubusercontent.com/YutaItoh/3D-Eye-Tracker/master/docs/runtime-2.png" width="600px" />

In this initialization step, a user needs to smoothly rotate his/her eye to capture various 2D shapes of the eye.

After the step, we get 3D eye tracking:

<img src="https://raw.githubusercontent.com/YutaItoh/3D-Eye-Tracker/master/docs/runtime-3.png" width="200px" />

### Tips:

Some debug keys are pre-assigned for a better control of the software:
* `p`: Takes some more 2D pupil observations. Useful when estimated 3D eye model is incorrect due to not-well-distributed 2D observations
* `r`: Resets the 3D eye model and 2D observations and restarts the initialization step
* `ESC`: Exit the program 	

# Acknowledgements

This program integrated/modified several existing codes. Especially, 
* [2D pupil detection code](https://github.com/YutaItoh/3D-Eye-Tracker/blob/master/main/pupilFitter.h) by [Jason Orlosky](http://www.jeoresearch.com/research)
* [3D eye model optimization code](https://github.com/LeszekSwirski/singleeyefitter) (./singleeyefitter) by Leszek Swirski 
