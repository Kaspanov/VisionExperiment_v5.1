#include "pch.h"
#include "OpenGL.h"

// [Global Variables]
// -Extern Variables 
extern vector2 FramePixel;
extern vector3 CyclopeanEye_Sys, Obj_Pos;

// -STL Viewer related Variables
static float move_x, move_y, move_z;
float *vertices;

double x_min, x_max;		 // Geometry extents along each Axis		
double y_min, y_max;				
double z_min, z_max;

GLFWwindow *window;
std::vector<triangle> facet; // define a vector object (container) 

// [Functions]
// Create Vertex Array - To initialize OpenGL
void createVertexArray(std::vector<triangle>& facet, float* vertices) {

	int count = 0;
	float comp;

	float B = 1.0;
	float G = 0.5;
	float R = 0.3;
	float A = 1.0;

	for (size_t i/*facet_index*/ = 0; i < facet.size(); i++) {

		// Vertex 1
		{
			//3 coordinates of the point
			vertices[count + 0] = facet[i].point[0].x;
			vertices[count + 1] = facet[i].point[0].y;
			vertices[count + 2] = facet[i].point[0].z;

			//3 coordinates of the normal
			vertices[count + 3] = facet[i].normal.x;
			vertices[count + 4] = facet[i].normal.y;
			vertices[count + 5] = facet[i].normal.z;

			//(R,G,B,A) color values
			comp = fabs(facet[i].normal.x) + fabs(facet[i].normal.y) + fabs(facet[i].normal.z);
			vertices[count + 6] = R * comp;
			vertices[count + 7] = G * comp;
			vertices[count + 8] = B * comp;
			vertices[count + 9] = A;
		}

		// Vertex 2
		{
			//3 coordinates of the point
			vertices[count + 10] = facet[i].point[1].x;
			vertices[count + 11] = facet[i].point[1].y;
			vertices[count + 12] = facet[i].point[1].z;

			//3 coordinates of the normal
			vertices[count + 13] = facet[i].normal.x;
			vertices[count + 14] = facet[i].normal.y;
			vertices[count + 15] = facet[i].normal.z;

			//(R,G,B,A) color values
			comp = fabs(facet[i].normal.x) + fabs(facet[i].normal.y) + fabs(facet[i].normal.z);
			vertices[count + 16] = R * comp;
			vertices[count + 17] = G * comp;
			vertices[count + 18] = B * comp;
			vertices[count + 19] = A;
		}

		// Vertex 3
		{
			//3 coordinates of the point
			vertices[count + 20] = facet[i].point[2].x;
			vertices[count + 21] = facet[i].point[2].y;
			vertices[count + 22] = facet[i].point[2].z;

			//3 coordinates of the normal
			vertices[count + 23] = facet[i].normal.x;
			vertices[count + 24] = facet[i].normal.y;
			vertices[count + 25] = facet[i].normal.z;

			//(R,G,B,A) color values
			comp = fabs(facet[i].normal.x) + fabs(facet[i].normal.y) + fabs(facet[i].normal.z);
			vertices[count + 26] = R * comp;
			vertices[count + 27] = G * comp;
			vertices[count + 28] = B * comp;
			vertices[count + 29] = A;
		}

		count += 30;
	}

}

// -Initializing OpenGL and Checking Error
bool opengl_initialize(vector2 FramePixel, int argc, char *argv[]) {

	//Check if all arguments were supplied ???
	const int* CARGS = new int(1);

	if (argc < 1 + *CARGS) {

		std::cout << "Please provide the following:\n\n";
		std::cout << "     [name of the STL file]  \n";

		delete CARGS; CARGS = nullptr;
		return true;
	}

	delete CARGS; CARGS = nullptr;

	//Read STL geometry and Check error
	bool* error = new bool(getGeometryInput(argv, facet, x_min, x_max, y_min, y_max, z_min, z_max));

	if (*error)
		std::cerr << "ERROR: getGeometryInput Failure - opengl_initialize" << std::endl; return true;

	delete error; error = nullptr;

	//Calculate translation parameters to relocate the center of the geometry to the origin
	float* x_center = new float((x_min + x_max) / 2.0);
	float* y_center = new float((y_min + y_max) / 2.0);
	float* z_center = new float((z_min + z_max) / 2.0);

	//Translation along axis
	move_x = 0.0 - *x_center;
	move_y = 0.0 - *y_center;
	move_z = 0.0 - *z_center;

	delete x_center, y_center, z_center; x_center, y_center, z_center = NULL;

	std::cout << "Need to translate the geometry by: " << move_x << " " << move_y << " " << move_z << std::endl;

	//Create a vertex array based on facet data
	vertices = new float[facet.size() * 30];

	createVertexArray(facet, vertices);

	// Create a WINDOW using GLFW
	*window;

	//Check library load
	if (!glfwInit())
		return true;

	//Set WINDOW display mode
	window = glfwCreateWindow(FramePixel.y, FramePixel.z, "Overlay Viewer", NULL, NULL);

	glfwMaximizeWindow(window);

	//Check WINDOW initialize
	if (!window)
		glfwTerminate(); return true;

	//Make the windows context current
	glfwMakeContextCurrent(window);

	//Create a Vertex Buffer Object(VBO) and bind the vertex array to it makes rendering faster
	GLuint bufferID;
	glewInit();
	glGenBuffers(1, &bufferID);
	glBindBuffer(GL_ARRAY_BUFFER, bufferID);
	glBufferData(GL_ARRAY_BUFFER, facet.size() * 30 * sizeof(GLfloat), vertices, GL_STATIC_DRAW);

	//Fill mode or wireframe mode
	glPolygonMode(GL_FRONT/*| GL_BACK | GL_FRONT_AND_BACK*/, GL_FILL/*| GL_POINT | GL_LINE */);

	//Shading model
	glShadeModel(GL_SMOOTH);

	return false;






}

// -Rendering Objects
void drawGeometry(char* argv[], GLFWwindow *window,
	float *vertices, int vertives_num,
	double z_min, double z_max,
	vector3 Obj_origin, vector3 eye_pos, vector2 Screensize, vector2 FramePixel) {
	try {
		//Coordinates Transform from System to OpenGL
		Obj_origin.x -= eye_pos.x;
		Obj_origin.y -= eye_pos.y;
		Obj_origin.z -= eye_pos.z;

		double*temp = new double;
		*temp = Obj_origin.x;
		Obj_origin.x = Obj_origin.y;
		Obj_origin.y = Obj_origin.z;
		Obj_origin.z = *temp;

		delete temp; temp = nullptr;

		//Select background color(black)
		glClearColor(0, 0, 0, 0);

		//Hidden surface removal
		glEnable(GL_DEPTH_TEST);

		//Clear all pixels in the window with the color
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Transformations on the 3D object

		//Change the current mode
		glMatrixMode(GL_MODELVIEW);

		//Current transformation matrix = identity matrix
		glLoadIdentity();

		//Translate the geometry along axis
		glTranslatef(Obj_origin.x, Obj_origin.y, Obj_origin.z);

		// Viewing transformation

		//Change the current mode
		glMatrixMode(GL_PROJECTION);

		//Current transformation matrix = identity matrix
		glLoadIdentity();

		glPushMatrix();

		//Calculation
		float Model[16] = { 0, };

		float R = Screensize.y / 2 - eye_pos.y;
		float L = -Screensize.y / 2 - eye_pos.y;
		float T = Screensize.z / 2 - eye_pos.z;
		float B = -Screensize.z / 2 - eye_pos.z;
		float N = eye_pos.x;
		float F = N - Obj_origin.z + z_max - z_min;

		Model[0] = 2 * N / (R - 1);
		Model[5] = 2 * N / (T - B);
		Model[8] = (R + 1) / (R - 1);
		Model[9] = (T + B) / (T - B);
		Model[10] = -(F + N) / (F - N);
		Model[11] = -1;
		Model[14] = -(2 * F * N) / (F - N);

		//Viewing
		glGetFloatv(GL_PROJECTION, Model);

		glPopMatrix();

		glLoadMatrixf(Model);

		gluLookAt(0.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 1.0, 0.0);

		// Enable use of vertex data

		//Enanle use of vertex coordinate information from the array
		glEnableClientState(GL_VERTEX_ARRAY);

		glVertexPointer(3/*num of axis*/, GL_FLOAT/*type*/, sizeof(float) * 10/*stride - gap between each set of axis*/, (GLvoid*)((char*)NULL)/*offset location of initial axis*/);

		//Enable use of vertex normal information from the array
		glEnableClientState(GL_NORMAL_ARRAY);

		glNormalPointer(GL_FLOAT, sizeof(float) * 10, (GLvoid*)(((char*)NULL) + 12));

		//Enable use of vertex color information from the array
		glEnableClientState(GL_COLOR_ARRAY);

		glColorPointer(4/*R,G,B,A*/, GL_FLOAT, sizeof(float) * 10, (GLvoid*)(((char*)NULL) + 24));

		//Draw Geometry
		glDrawArrays(GL_TRIANGLES, 0/*offset loacation*/, vertives_num/*num of vertices to be used for rendering*/);

	}

	catch (std::runtime_error) {
		std::cerr << "ERROR: Function Failure - Trackingpupil" << std::endl;
		EXIT_FAILURE;
	}
}

// -Visualizing Overlay
void opengl_running(vector3 Obj_origin, vector3 eye_pos, vector2 Screensize,  int argc, char *argv[]) {
	
	glViewport(0, 0, FramePixel.y, FramePixel.z);

	//Render Objects in the window
	drawGeometry(argv, window, vertices, 3 * facet.size(), z_min, z_max, Obj_origin, eye_pos, Screensize, FramePixel);

	//Swap front and back buffers
	glfwSwapBuffers(window);

	//Poll ofr and process events
	glfwPollEvents();

}

// -Terminate OpenGL
void opengl_close() {
	glfwDestroyWindow(window);
	glfwTerminate();
}
