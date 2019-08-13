// Experiment Code Version 5.1 - Coded by JonMin Lim, Editted by JoonSoo Lee

//Application of CAD (STL File) - 5.0 ver
//Workable at OpenCV 4.1.0 - 5.1 ver
//Little Optimization - 5.1 ver
//Eye Detection Include - 5.1 ver

/*
	[Coordinates of the System]
	----------------------------------------------------------
	|								   Transparent Screen    |
	|														 |
	|						  z - axis				         |
	|						    ↑					         |
	|				   x - axis ⊙ → y - axis		         |
	|							Origin					     |
	|														 |
	|														 |
	|														 |
	 ----------------------------------------------------------
*/
/*
	[Coordinates of the OpenGL]
	----------------------------------------------------------
	|								   Transparent Screen    |
	|														 |
	|						  y - axis				         |
	|						    ↑					         |
	|				   z - axis ⊙ → x - axis		         |
	|							Origin = Eye			     |
	|														 |
	|														 |
	|														 |
	 ----------------------------------------------------------
*/
/*
	[Coordinates of the Motion Tracker(Real World Input)]
	----------------------------------------------------------
	|								   Transparent Screen    |
	|														 |
	|										                 |
	|						    					         |
	|				   		                                 |
	|												         |
	|														 |
	|						   y - axis				    	 |
	|							 ↑							 |
	 ------------------ z - axis ⊙ → x - axis --------------- Floor
								 Origin
*/


// [Header]
#include "pch.h"
#include "PupilTracking.h"
#include "Realsense.h"
#include "OpenGL.h"
#include "windows.h"

// [Mode of Video]

const bool videoMode_pupil1 = true;      // Turn On or Off the Main Pupil Tracking Video Windows
const bool videoMode_pupil2 = false;     // Turn On or Off the Extra Pupil Tracking Video Windows
const bool videoMode_depth = false;      // Turn On or Off the Color-Depth Mapping Video Windows

// [Mode of Filtering]

bool depthFrameDataFiltering = false;    // Turn On or Off the Depth Frame Data Filtering
bool depthThresholdFiltering = true;     // Turn On or Off the Depth Threshold Filtering for Detecting Face
float depthThreshold = 1500;              // Threshold Value for Depth Threshold Filtering (unit: mm)

// [Parameters of System]

double ballOffset = 12;                  // Length from the bottom to the center of the ball for the Motion Tracker (unit: mm)

vector2 FramePixel = { 1024,768 };         // Pixel Data of the Frame for Projected Screen { Width, Height } (unit: px)

vector3 Rup_Screen = {};                 // The Right Upper Point Coordinates of Screen Measured by the Motion Tracker { x, y, z } (unit: m)
vector3 Rdw_Screen = {};                 // The Right Lower Point Coordinates of Screen Measured by the Motion Tracker { x, y, z } (unit: m)
vector3 Lup_Screen = {};                 // The Left Upper Point Coordinates of Screen Measured by the Motion Tracker { x, y, z } (unit: m)
vector3 Ldw_Screen = {};                 // The Left Lower Point Coordinates of Screen Measured by the Motion Tracker { x, y, z } (unit: m)

vector3 Cam_Pos = {};                    // Position of the RGB Camera Origin Measured by the Motion Tracker { x, y, z } (unit: m)

vector3 Obj_Pos = {};                    // Arbitrary Point Coordinates on the Object Measured by the Motion Tracker { x, y, z } (unit: m)
vector3 Obj_Offset = { 0,166,0 };                 // Offset between the Arbitrary Point Coordinates on the Object and the Origin of the Object { x, y, z } (unit: mm)

// [Variables]

bool loop = true;                        // Control While Loop
bool e = false;                          // Check Error

vector2 Screensize;                      // The Size of the Projected Screen { Width, Height } (unit: mm)
vector3 Sys_Origin;                      // The Position of the System Coordinates Origin { x, y, z } (unit: mm)
vector3 Obj_Origin;                      // The Position of the Obejct Origin in the System Coordinates { x, y, z } (unit: mm)

vector3 CyclopeanEye_Cam;                // Position of the Cyclopean Eye with respect to the Coordinates of the Camera { x, y, z } (unit: mm)
vector3 CyclopeanEye_Sys;                // Position of the Cyclopean Eye with respect to the Coordinates of the System { x, y, z } (unit: mm)




int main(int argc, char *argv[]) {

	// Calculate the System Parameters from m unit to mm unit + Transfer coordinate

	Sys_Origin.x = (Rup_Screen.x + Rdw_Screen.x + Lup_Screen.x + Ldw_Screen.x) / 4 * 1000;
	Sys_Origin.y = (Rup_Screen.y + Rdw_Screen.y + Lup_Screen.y + Ldw_Screen.y) / 4 * 1000;
	Sys_Origin.z = (Rup_Screen.z + Rdw_Screen.z + Lup_Screen.z + Ldw_Screen.z) / 4 * 1000 - ballOffset;

	Screensize.y = (Rup_Screen.x - Lup_Screen.x + Rdw_Screen.x - Ldw_Screen.x) / 2 * 1000;
	Screensize.z = (Rup_Screen.y - Rdw_Screen.y + Lup_Screen.y - Ldw_Screen.y) / 2 * 1000;

	double *coortemp = new double(Cam_Pos.x);
	Cam_Pos.x = (Cam_Pos.z * 1000) - Sys_Origin.z - ballOffset;
	Cam_Pos.z = (Cam_Pos.y * 1000) - Sys_Origin.y;
	Cam_Pos.y = (*coortemp * 1000) - Sys_Origin.x;
	delete coortemp; coortemp = NULL;

	Obj_Origin.x = (Obj_Pos.z * 1000) - Obj_Offset.z - Sys_Origin.z;
	Obj_Origin.y = (Obj_Pos.x * 1000) - Obj_Offset.x - Sys_Origin.x;
	Obj_Origin.z = (Obj_Pos.y * 1000) - ballOffset - Obj_Offset.y - Sys_Origin.y;

	// Result of Calculation and Transfer
	std::cout << "\n-----------------------------------------------------------------" << std::endl;
	std::cout << "[ Calculated System Parameters ]\n" << std::endl;
	std::cout << "Screen Width: " << Screensize.y << std::endl;
	std::cout << "Screen Height: " << Screensize.z << std::endl;
	std::cout << "Camera Position: (" << Cam_Pos.x << ", " << Cam_Pos.y << ", " << Cam_Pos.z << ")" << std::endl;
	std::cout << "Object Origin: (" << Obj_Origin.x << ", " << Obj_Origin.y << ", " << Obj_Origin.z << ")" << std::endl;
	std::cout << "-----------------------------------------------------------------\n" << std::endl;

	// 1st Monitor Screen
	size_t *mainWidth = new size_t; size_t *mainHeight = new size_t;
	*mainWidth = GetSystemMetrics(SM_CXSCREEN); //Define by Running hardware info
	*mainHeight = GetSystemMetrics(SM_CYSCREEN);

	std::cout << "Main Monitor : " << *mainWidth << ", " << *mainHeight << std::endl;
	delete mainWidth, mainHeight; mainWidth = NULL; mainHeight = NULL;

	//2nd Monitor Screen
	size_t *dualWidth = new size_t; size_t *dualHeight = new size_t;
	*dualWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN); //Define by Running hardware info
	*dualHeight = GetSystemMetrics(SM_CYVIRTUALSCREEN);

	std::cout << "Dual Monitor : " << *dualWidth << ", " << *dualHeight << std::endl;
	delete dualWidth, dualHeight; dualWidth = NULL; dualHeight = NULL;

	// Initialize Video Setting
	InitSetVideo_Pupil(videoMode_pupil1, videoMode_pupil2);
	InitSetVideo_Align(videoMode_depth);

	// Define transformations from and to Disparity domain
	rs2::disparity_transform depth2disparity;
	rs2::disparity_transform disparity2depth(false);

	// Define Spatial Filter (Edge - Preserving)
	rs2::spatial_filter spat;

	// Hole filling is an agressive heuristic and it gets the depth wrong many times
	spat.set_option(RS2_OPTION_HOLES_FILL, 5); // 5 = fill all the zero pixels

	// Define Temporal Filter
	rs2::temporal_filter temp;
	
	// Contruct a pipeline which astracts the device
	rs2::pipeline pipe;

	// Creat a configuration for configuring the pipeline with a non default profile
	rs2::config cfg;

	// Add desired streams to configuration
	cfg.enable_stream(COLOR_STREAM, WIDTH, HEIGHT, COLOR_FORMAT, FPS);
	cfg.enable_stream(DEPTH_STREAM, WIDTH, HEIGHT, DEPTH_FORMAT, FPS);

	// Instruct pipeline to start streaming with the requested configuration
	rs2::pipeline_profile profile = pipe.start(cfg);

	// Each depth camera might have different units for depth pixels, so we get it here
	// Using the pipeline's profile, we can retrieve the device that the pipeline uses
	float depth_scale = get_depth_scale(profile.get_device());

	// Reduce the number of black pixels
	rs2::depth_sensor sensor = profile.get_device().first<rs2::depth_sensor>(); //get device as depth sensor that figured out first
	rs2::option_range range = sensor.get_option_range(RS2_OPTION_VISUAL_PRESET); // provide access to several recommend sets of option presets for the depth camera	

	for (float i = range.min; i < range.max; i += range.step)
		if (std::string(sensor.get_option_value_description(RS2_OPTION_VISUAL_PRESET, i)) == "High Density")
			sensor.set_option(RS2_OPTION_VISUAL_PRESET, i);

	// Pipeline could choose a device that does not have a color stream
	// If there is no color stream, choose to align depth to another stream
	rs2_stream align_to = find_stream_to_align(profile.get_streams());

	// Creat a rs2::align object
	rs2::align align(align_to);

	// Check OpenGL initialize error

	e = opengl_initialize(FramePixel, argc, argv);

	if (e)return EXIT_FAILURE;

	while (loop) {
		//Camera warm up - dropping severeal first frames to let auto-exposure stabilize
		rs2::frameset Rsframe = pipe.wait_for_frames();

		//Get each color frame
		rs2::frame Colorframe = Rsframe.get_color_frame();

		//Create OpenCv Mat
		cv::Mat frame(cv::Size(WIDTH, HEIGHT), CV_8UC3, (void*)Colorframe.get_data(), cv::Mat::AUTO_STEP);

		TransAndCopyFrameData(frame);

		//Check if profile info was changed
		if (profile_changed(pipe.get_active_profile().get_streams(), profile.get_streams())){

			//Update changed profile information
			profile = pipe.get_active_profile();
			align_to = find_stream_to_align(profile.get_streams());
			align = rs2::align(align_to);
			depth_scale = get_depth_scale(profile.get_device());
		}

		//Get processed aligned frame
		rs2::frameset Prcsframe = align.process(Rsframe);

		rs2::video_frame Otherframe = Prcsframe.first(align_to);
		rs2::depth_frame AligneDepthframe = Prcsframe.get_depth_frame();

		//Check frame load
		if (!AligneDepthframe || !Otherframe)
			continue;

		// Filtering depth frame data
		if (depthFrameDataFiltering) {
			
			//Far-away objects are filtered proportionally (try to switch to disparity domain)
			AligneDepthframe = depth2disparity.process(AligneDepthframe);
			
			//Applt spatial and temporal filtering
			AligneDepthframe = spat.process(AligneDepthframe);
			AligneDepthframe = temp.process(AligneDepthframe);

			//Switch back to depth
			AligneDepthframe = disparity2depth.process(AligneDepthframe);
		}

		// Visualize colorized depth frame image
		if (videoMode_depth) {

			rs2::colorizer c;
			rs2::frame ColorizeDepth = c.process(AligneDepthframe);
			cv::Mat DMat(cv::Size(WIDTH, HEIGHT), CV_8UC3, (void*)ColorizeDepth.get_data(), cv::Mat::AUTO_STEP);

			cv::imshow(window_depth, DMat);
		}

		// Start Pupil Tracking
		//Check frame load
		if (frame.empty())
			std::cerr << "ERROR: No Captured Frame" << std::endl; return EXIT_FAILURE;

		//Pupil tracking
		if (frame.channels() == 1)
			e = StartPupilTracking(frame, 'g'); //Grayframe
		else if (frame.channels() == 3)
			e = StartPupilTracking(frame, 'c'); //Colorframe

		//Check tracking error
		if (e) return EXIT_FAILURE;

		CyclopeanEye_Cam = dist_3d(AligneDepthframe);
		CyclopeanEye_Sys = TransCoordinates(CyclopeanEye_Cam, Cam_Pos);

		opengl_running(Obj_Origin, CyclopeanEye_Sys, Screensize, argc, argv);

		if ((char)cv::waitKey() == 'q')
			loop = false;

	}

	cfg.disable_all_streams();
	opengl_close();
	return EXIT_SUCCESS;

}

