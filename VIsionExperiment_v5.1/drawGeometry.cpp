#include "pch.h"
#include "drawGeometry.h"

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