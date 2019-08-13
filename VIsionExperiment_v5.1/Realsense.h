#ifndef MAPPING_H
#define MAPPING_H

// [Header]
#include "pch.h"

// [Global Variables]
// -Name of Window (Video)
const std::string window_depth = "Aligned Depth";

// [Functions]
// -Initialize the Video Setting
void InitSetVideo_Align(bool mode);

// -Retrieve the Units for Depth Pixels
float get_depth_scale(rs2::device dev);

// -Check a Depth Stream and Find Another Stream to Align Depth with
rs2_stream find_stream_to_align(const std::vector<rs2::stream_profile>& streams);

// -Check if the Profile was Changed
bool profile_changed(const std::vector<rs2::stream_profile>& curr, const std::vector<rs2::stream_profile>& prev);

#endif
