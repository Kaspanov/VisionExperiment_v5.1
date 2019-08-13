#include "pch.h"
#include "PupilTracking.h"

// [Global Variables]
// -Video Mode (On / Off)
bool vMode_p1, vMode_p2, detectFace, kSmoothFaceImage = false;
extern bool depthThresholdFiltering;

// -CV related 
cv::CascadeClassifier face_cascade;
cv::CascadeClassifier eye_cascade;
cv::Mat debugImg, faceROI;
extern float depthThreshold;

// -Window and Displayed Rect Variables
float i2[2], i2R[4], i2L[4], i3R[2], i3L[2];

// -Recalculated Pupil Center Coordinates
float Rcenter[2], Lcenter[2];

// -Dist3d
vector3 Pre_Cyclopean = { 0,0,0 };

// [Functions]
// -Initialize the Video Setting
void InitSetVideo_Pupil(bool mode1, bool mode2) {

	if (mode1) {

		cv::namedWindow(main_window_name, cv::WINDOW_NORMAL);
		cv::moveWindow(main_window_name, 0, 0);

	}

	if (mode2) {

		cv::namedWindow(face_window_name, cv::WINDOW_NORMAL);
		cv::moveWindow(face_window_name, 0, 300);
		cv::namedWindow(right_eye_name, cv::WINDOW_AUTOSIZE);
		cv::moveWindow(right_eye_name, 305, 0);
		cv::namedWindow(left_eye_name, cv::WINDOW_AUTOSIZE);
		cv::moveWindow(left_eye_name, 305, 500);
		cv::namedWindow(binary_right_eye_name, cv::WINDOW_AUTOSIZE);
		cv::moveWindow(binary_right_eye_name, 940, 0);
		cv::namedWindow(binary_left_eye_name, cv::WINDOW_AUTOSIZE);
		cv::moveWindow(binary_left_eye_name, 940, 500);
	}

	vMode_p1 = mode1;
	vMode_p2 = mode2;
}

// -Copy Frame Data
void TransAndCopyFrameData(cv::Mat frame) {

	cv::flip(frame, frame, 1);
	frame.copyTo(debugImg);

}

// -Detect Face based on XML
bool StartPupilTracking(cv::Mat frame, char mode) {

	try {
		// -Local Variables
		std::vector<cv::Rect> faces;
		cv::Mat Grayframe;

		//Check Cascade
		if (!face_cascade.load(face_cascade_name))
			std::cerr << "ERROR: FACE Cascade Loading Fail - StartPupilTracking" << std::endl; return true;

		if (mode == 'g')
			Grayframe = frame;
		else if (mode == 'c')
			cv::cvtColor(frame, Grayframe, cv::COLOR_BGR2GRAY);
		else
			std::cerr << "ERROR: Wrong Frame Mode - StartPupilTracking" << std::endl; return true;

		face_cascade.detectMultiScale(Grayframe, faces, 1.1, 2, 0 | cv::CASCADE_SCALE_IMAGE | cv::CASCADE_FIND_BIGGEST_OBJECT, cv::Size(70, 70));

		rectangle(debugImg, faces[0], 1234);
		i2[0] = faces[0].x;
		i2[1] = faces[0].y;

		if (faces.size() > 0)
		{
			FindEyes(Grayframe, faces[0]);

			detectFace = true;
		}
		else
			detectFace = false;

		return false;

	}

	catch(std::runtime_error){
		std::cerr << "ERROR: StartPupilTracking Malfunction" << std::endl;
		return true;
	}

}

// -Find Entire Eye Position
void FindEyes(cv::Mat Grayframe, cv::Rect face) {

	try {
		// -Local Variables
		std::vector<cv::Rect> eyes;
		faceROI = Grayframe(face);
		cv::Mat R_eye, L_eye;

		//Smoothing input image along x - axis (width)
		if (kSmoothFaceImage) {
			double sigma = 0.005 * face.width;
			cv::GaussianBlur(faceROI, faceROI, cv::Size(0, 0), sigma);
		}

		//Check Cascade
		if (!eye_cascade.load(eye_cascade_name))
			std::cerr << "ERROR: EYE Cascade Loading Fail - Findeyes" << std::endl; EXIT_FAILURE;

		//Find Eye regions
		eye_cascade.detectMultiScale(faceROI,eyes,1.1,2,0 | cv::CASCADE_SCALE_IMAGE, cv::Size(30,30));

		//Check number of eyes
		if (eyes.size() > 2)
			std::cerr << "ERROR: Normally Human has 2 eyes - Findeyes" << std::endl; EXIT_FAILURE;

		//Initialize eye regions
		i2R[0] = eyes[0].x;
		i2R[1] = eyes[0].y;
		i2R[2] = eyes[0].width;
		i2R[3] = eyes[0].height;

		i2L[0] = eyes[1].x;
		i2L[1] = eyes[1].y;
		i2L[2] = eyes[1].width;
		i2L[3] = eyes[1].height;

		//Resize eye regions
		cv::resize(faceROI(eyes[0]), R_eye, cv::Size(640, 480), 0, 0, cv::INTER_LINEAR);
		cv::resize(faceROI(eyes[1]), L_eye, cv::Size(640, 480), 0, 0, cv::INTER_LINEAR);

		//Initialize i3 array
		i3R[0] = R_eye.cols;
		i3R[1] = R_eye.rows;
		i3L[0] = L_eye.cols;
		i3L[1] = L_eye.rows;

		// TrackingPupil
		TrackingPupil(R_eye, L_eye);
		
	}

	catch (std::runtime_error) {
		std::cerr << "ERROR: Function Failure - Findeyes" << std::endl;
		EXIT_FAILURE;
	}


}

// -Tracking Pupil
void TrackingPupil(cv::Mat Rframe, cv::Mat Lframe) {
	try {
		// -Local Variables
		cv::Mat R_binary, R_labelImg, R_stats, R_centroids;
		cv::Mat L_binary, L_labelImg, L_stats, L_centroids;
		int area, left, top, width, height;
		float Rcentersrc[2], Lcentersrc[2],R[2], L[2];

		//Pre-treatment
		cv::equalizeHist(Rframe, Rframe);
		cv::equalizeHist(Lframe, Lframe);

		//Threshold 
		cv::threshold(Rframe, R_binary, R_threshold, 255, cv::THRESH_BINARY_INV);
		cv::threshold(Lframe, L_binary, L_threshold, 255, cv::THRESH_BINARY_INV);

		//Labeling and num of group
		int R_numofLabels = cv::connectedComponentsWithStats(R_binary, R_labelImg, R_stats, R_centroids, 8, CV_32S);
		int L_numofLabels = cv::connectedComponentsWithStats(L_binary, L_labelImg, L_stats, L_centroids, 8, CV_32S);

		//Initialize label stats - Righteye
		for (size_t j = 1; j < R_numofLabels; j++) {

			area = R_stats.at<int>(j, cv::CC_STAT_AREA);
			left = R_stats.at<int>(j, cv::CC_STAT_LEFT);
			top = R_stats.at<int>(j, cv::CC_STAT_TOP);
			width = R_stats.at<int>(j, cv::CC_STAT_WIDTH);
			height = R_stats.at<int>(j, cv::CC_STAT_HEIGHT);

			//Decision making - Is this pupil?
			if (area > 2500 && left + width / 2 > Rframe.cols / 2 - 150 && left + width / 2 < Rframe.cols / 2 + 150) {

				cv::rectangle(Rframe, cv::Point(left, top), cv::Point(left + width, top + height), cv::Scalar(0, 0, 255), 1);
				Rcentersrc[0] = left + width / 2;
				Rcentersrc[1] = top + height / 2;

				circle(Rframe, cv::Point(Rcentersrc[0], Rcentersrc[1]), 5, cv::Scalar(255, 0, 0), 1);
			}
		}

		//Initialize label stats - Lefteye
		for (size_t j = 1; j < L_numofLabels; j++) {

			area = L_stats.at<int>(j, cv::CC_STAT_AREA);
			left = L_stats.at<int>(j, cv::CC_STAT_LEFT);
			top = L_stats.at<int>(j, cv::CC_STAT_TOP);
			width = L_stats.at<int>(j, cv::CC_STAT_WIDTH);
			height = L_stats.at<int>(j, cv::CC_STAT_HEIGHT);

			//Decision making - Is this pupil?
			if (area > 2500 && left + width / 2 > Rframe.cols / 2 - 150 && left + width / 2 < Rframe.cols / 2 + 150) {

				cv::rectangle(Rframe, cv::Point(left, top), cv::Point(left + width, top + height), cv::Scalar(0, 0, 255), 1);
				Lcentersrc[0] = left + width / 2;
				Lcentersrc[1] = top + height / 2;

				circle(Lframe, cv::Point(Lcentersrc[0], Lcentersrc[1]), 5, cv::Scalar(255, 0, 0), 1);
			}
		}

		//Recalculate Center Position
		R[0] = i2R[0] + Rcentersrc[0] * i2R[2] / i3R[0];
		R[1] = i2R[1] + Rcentersrc[1] * i2R[3] / i3R[1];
		L[0] = i2L[0] + Lcentersrc[0] * i2L[2] / i3L[0];
		L[1] = i2L[1] + Lcentersrc[1] + i2L[3] / i3L[1];

		Rcenter[0] = i2[0] + R[0];
		Rcenter[1] = i2[1] + R[1];
		Lcenter[0] = i2[0] + L[0];
		Lcenter[1] = i2[1] + L[1];

		//Print mode
		if (vMode_p1)
		{
			circle(debugImg, cv::Point(Rcenter[0], Rcenter[1]), 10, cv::Scalar(255, 0, 0), 2);
			circle(debugImg, cv::Point(Rcenter[0], Rcenter[1]), 10, cv::Scalar(255, 0, 0), 2);
			cv::imshow(main_window_name, debugImg);
		}

		if (vMode_p2)
		{
			cv::imshow(right_eye_name, Rframe);
			cv::imshow(left_eye_name, Lframe);
			cv::imshow(binary_right_eye_name, R_binary);
			cv::imshow(binary_left_eye_name, L_binary);
		}

	}

	catch (std::runtime_error) {
		std::cerr << "ERROR: Function Failure - Trackingpupil" << std::endl;
		EXIT_FAILURE;
	}


}

// -Calculate the Distance between 2 points in 3D
vector3 dist_3d(const rs2::depth_frame& frame) {

	try {
		vector3 Cyclopean;
		if (detectFace) {

			// -Local Variables
			float Upixel[2], Vpixel[2], Upoint[3], Vpoint[3];

			Upixel[0] = WIDTH - Rcenter[0];
			Upixel[1] = Rcenter[1];
			Vpixel[0] = WIDTH - Lcenter[0];
			Vpixel[1] = Lcenter[1];

			auto Udist = frame.get_distance(Upixel[0], Upixel[1]);
			auto Vdist = frame.get_distance(Vpixel[0], Vpixel[1]);

			// Deproject from pixel ot point in 3D
			//Calibration data
			rs2_intrinsics intr = frame.get_profile().as<rs2::video_stream_profile>().get_intrinsics();

			rs2_deproject_pixel_to_point(Upoint, &intr, Upixel, Udist);
			rs2_deproject_pixel_to_point(Vpoint, &intr, Vpixel, Vdist);

			//Intialize Cyclopean eye
			Cyclopean.x = (Upoint[0] + Vpoint[0]) / 2;
			Cyclopean.y = (Upoint[1] + Vpoint[1]) / 2;
			Cyclopean.z = (Upoint[2] + Vpoint[2]) / 2;

			//Convert Unit M to mm
			Cyclopean.x *= 1000;
			Cyclopean.y *= 1000;
			Cyclopean.z *= 1000;

			if (depthThresholdFiltering)
				if (Cyclopean.z > depthThreshold)
					return Pre_Cyclopean;

			Pre_Cyclopean = Cyclopean;

			return Cyclopean;

		}
		else
			return Pre_Cyclopean;
	}

	catch (std::runtime_error) {
		std::cerr << "ERROR: Vector3 Failure - dist_3d" << std::endl;
		EXIT_FAILURE;
	}
}

// -Translate Coordinates
vector3 TransCoordinates(vector3 point, vector3 offset) {
	try {
		// -Local Variables
		vector3 Newpoint;
		float Xrad, Zrad;
		float X_rotation = 90, Z_rotation = 90;

		//Convert Unit
		Xrad = X_rotation * (PI / 180);
		Zrad = Z_rotation * (PI / 180);

		//Euler Rotation
		Newpoint.x = point.x*cos(Zrad) + point.y*sin(Zrad)*cos(Xrad) + point.z*sin(Zrad)*sin(Xrad);
		Newpoint.y = -point.x*sin(Zrad) + point.y*cos(Zrad)*cos(Xrad) + point.z*cos(Zrad)*sin(Xrad);
		Newpoint.z = -point.y*sin(Xrad) + point.z*cos(Xrad);

		//Translation
		Newpoint.x += offset.x;
		Newpoint.y += offset.y;
		Newpoint.z += offset.z;

		return Newpoint;
	}

	catch (std::runtime_error) {
		std::cerr << "ERROR: Vector3 Failure - TransCoordinates" << std::endl;
		EXIT_FAILURE;
	}
}