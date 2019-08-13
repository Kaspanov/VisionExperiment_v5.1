#ifndef PUPILTRACKING_H
#define PUPILTRACKING_H

// [Headers]
#include "pch.h"

// [Camera Frame Data Setting]
#define COLOR_STREAM	RS2_STREAM_COLOR
#define DEPTH_STREAM    RS2_STREAM_DEPTH
#define COLOR_FORMAT	RS2_FORMAT_BGR8
#define DEPTH_FORMAT	RS2_FORMAT_Z16

// [Video Setting]
#define WIDTH           640			// Number of Frame Width Pixel          
#define HEIGHT          480			// Number of Frame Height Pixel     
#define FPS             60			// Frame Per Second (Hz)

// [Global Variables]
// -Name of Window (Video)
const std::string main_window_name = "Capture - Face Detection";
const std::string face_window_name = "Capture - Face";
const std::string right_eye_name = "Right Eye";
const std::string left_eye_name = "Left Eye";
const std::string binary_right_eye_name = "Binary [Right Eye]";
const std::string binary_left_eye_name = "Binary [Left Eye]";
const int R_threshold = 10;
const int L_threshold = 10;


// -Initialize the Video Setting
void InitSetVideo_Pupil(bool mode1, bool mode2);

// -Copy Frame Data
void TransAndCopyFrameData(cv::Mat frame);

// -Detect Face based on XML
bool StartPupilTracking(cv::Mat frame, char mode);

// -Find Entire Eye Position
void FindEyes(cv::Mat Grayframe, cv::Rect face);

// -Tracking Pupil
void TrackingPupil(cv::Mat Rframe, cv::Mat Lframe);

// -Calculate the Distance between 2 points in 3D
vector3 dist_3d(const rs2::depth_frame& frame);

// -Translate Coordinates
vector3 TransCoordinates(vector3 point, vector3 offset);



#endif
