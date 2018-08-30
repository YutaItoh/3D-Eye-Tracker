This file provides instructions for calibrating in the Unity interface. 

5-point Calibration Steps:
1) On the c++ side in main.cpp, edit the "folderPath" variable to a path to a text file of your choice. This points to the folder in which you will write the coordinates of the 3D tracked eyeball spheres and corresponding pupil centers. The c++ side will constantly write to this file and Unity will constantly read from this file. 
2) Start the c++ interface (exe file) and ensure that the white circles roughly match your eyeball spheres. This usually takes somewhere between 10 and 15 seconds.
3) Start Unity and navigate to Assets -> Scripts -> eyecontrol.cs. Edit the "coordinateFile" variable to be the same as the "folderPath" you specified in c++.
4) Run the Unity interface with VR enabled for the HTC Vive. You should now see a set of instructions and a texture from a camera pointing toward your 3D registered eye ball spheres. Rays will be extending from the eyeball centers in a random direction. 
    * Note: This is currently only build for the HTC Vive, but you can replace the camera system and its heirarchy with any other VR device if desired.
5) Allow a few seconds to ensure the 3D eye model is stable on the c++ side. 
6) Press the 'c' key on your keyboard. You will now see a red cube at screen center. While looking at this cube, press the c key again. Repeat this process for the 4x remaining cubes. (In total, center->up->right->down->left-> calibration finished.)
7) The transparent red and green dots should now represent normalized lines of your eye-gaze vectors. A solid blue dot in the center represents the 3D midpoint of these two spheres. The rays will be visible on the camera texture in front of you. 
8) Repeat the process from 6) if the calibration was not successful. This may take 2 or 3 tries. 

1-point Recalibration:
1) Once a successful 5-point calibration has been completed, you can use the 'q' key for a quick 1-point recalibration. This will only present a single dot at screen center. The second press of 'q' will complete the 1-point calibration. 

Note: a successful calibration on the c++ side is necessary for an accurate Unity calibration. If the calibration seems to fail multiple times, try re-calibrating on the c++ side using the 'r' key with the eye camera windows highlighted. 
