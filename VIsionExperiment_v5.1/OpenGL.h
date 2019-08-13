#ifndef STL_VIEW_MAIN_H
#define STL_VIEW_MAIN_H

// [Headers]
// -OpenGLl headers
#include "GL/glew.h"
#include <GLFW/glfw3.h>
#include <glut.h> // for using gluLookAt

// -Std headers
#include "extraheaders.h"
#include <list>
#include <sstream>
#include <iomanip>
#include <locale>


// [Functions]
// -Local Functions
//Create Vertex Array - To initialize OpenGL
void createVertexArray(std::vector<triangle>& facet, float* vertices);

//Initializing OpenGL and Checking Error
bool opengl_initialize(vector2 FramePixel, int argc, char *argv[]);

//Rendering Objects
void drawGeometry(char* argv[], GLFWwindow *window,
	float *vertices, int vertives_num,
	double z_min, double z_max,
	vector3 Obj_origin, vector3 eye_pos, vector2 Screensize, vector2 FramePixel);

//Visualizing Overlay
void opengl_running(vector3 object_origin, vector3 eye_position,
	vector2 screen_size, int argc, char *argv[]);

//Terminate OpenGL
void opengl_close();


// -Extern Functions
//Reading input geometry files is STL format and finding out the required size of enviroment box
extern bool getGeometryInput(char* argv[],
	std::vector<triangle> & facet,
	double & x_min, double & x_max,
	double & y_min, double & y_max,
	double & z_min, double & z_max);

#endif
