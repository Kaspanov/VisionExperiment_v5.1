#include "pch.h"
#include "Realsense.h"

// [Global Variables]
// -Video Mode (On / Off)
bool vMode_d;

// [Functions]
// -Initialize the Video Setting
void InitSetVideo_Align(bool mode)
{
	if (mode) {

		cv::namedWindow(window_depth, cv::WINDOW_AUTOSIZE);
		cv::moveWindow(window_depth, 650, 0);
	}

	vMode_d = mode;
}

// -Retrieve the Units for Depth Pixels
float get_depth_scale(rs2::device dev) {

	//Go over the device's sensors
	for (rs2::sensor& sensor:dev.query_sensors()) //.query_sensors() return the list of adjacent devices, sharing the same physical parent composite device.

		//Check if the sensor is a depth sensor
		if (rs2::depth_sensor dpt = sensor.as<rs2::depth_sensor>()) //.as figure out sensor is depth_sensor type or not
			return dpt.get_depth_scale();

	throw std::runtime_error("Device doesn't have a depth sensor");
}

// -Check a Depth STream and Find another Stream to Align Depth with
rs2_stream find_stream_to_align(const std::vector<rs2::stream_profile>& streams) {

	rs2_stream align_to = RS2_STREAM_ANY;
	bool depth_stream_found, color_stream_found = false;

	for (rs2::stream_profile sp : streams) {

		rs2_stream Pfstream = sp.stream_type(); //Rturn the stream format

		if (Pfstream != RS2_STREAM_DEPTH) {

			if (!color_stream_found) //Want to Pick out Color Stream
				align_to = Pfstream;

			if (Pfstream == RS2_STREAM_COLOR)
				color_stream_found = true;
		}

		else
			depth_stream_found = true;
	}

	if (!depth_stream_found)
		throw std::runtime_error("No Depth stream available");

	if (align_to == RS2_STREAM_ANY)
		throw std::runtime_error("No stream found to align with Depth");

	return align_to;

}

// -Check if the Profile was changed
bool profile_changed(const std::vector<rs2::stream_profile>& curr, const std::vector<rs2::stream_profile>& prev) {

	for (auto&& i : prev) {

		//If previous profile is in current
		auto itr = std::find_if(std::begin(curr), std::end(curr), [&i](const rs2::stream_profile& curr_i) { return i.unique_id() == curr_i.unique_id(); });
		if (itr == std::end(curr))
			return true;
	}

	return false;
}