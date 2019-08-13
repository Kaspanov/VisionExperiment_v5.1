#ifndef PCH_H
#define PCH_H

//Library List

#include <iostream>
#include <cmath>
#include <queue>
#include <atomic>
#include <mutex>
#include <stdexcept>



#include <opencv2/opencv.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <librealsense2/rsutil.h>
#include <librealsense2/rs.hpp>


//Path

const cv::String face_cascade_name = "./haarcascade_frontalface_alt2.xml";
const cv::String eye_cascade_name = "./haarcascade_eye_tree_eyeglasses.xml";
const std::string STL_path = "../STL/";



//Structure

struct vector2
{
	double y;
	double z;
};

struct vector3
{
	double x;
	double y;
	double z;
};

//Const Val

const double PI = 3.141592;

#endif 
