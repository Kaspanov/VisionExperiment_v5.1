#include "pch.h"
#include "ReadSTL.h"

// [Global Variables]
// -Declare a triangle type object
triangle tri;

// - How many bytes of the file will be read to calculate the percentage of characters
unsigned long length = 256;

// [Functions]
// -Open the ASCII STL file and Read the contents
int read_ascii_STL_file(std::string STL_name, std::vector<triangle> & facet,
	double & x_min, double & x_max,
	double & y_min, double & y_max,
	double & z_min, double & z_max) {

	try {
		std::ifstream asciiInput; //Declare file object

		asciiInput.open((STL_path + STL_name).c_str(), std::ifstream::in);

		//Check Open Status
		if (asciiInput.fail()) {

			std::cout << "ERROR: Input STL file could not be opened" << std::endl;
			return(1);
		}

		// Read in the contents line by line
		//Initialize counter for counting the number of lines in the file
		int triangle_num = 0;

		//Parameters that will be used to store the minimum and maximum extents of the geometry descrived by the STL
		x_min = 1e+30, x_max = -1e+30;
		y_min = 1e+30, y_max = -1e+30;
		z_min = 1e+30, z_max = -1e+30;

		//Declare string objects     ----> 여기 본문이랑 다르다!
		std::string temp;
		std::string string;

		//Read in the first line
		getline(asciiInput, temp);

		// Start loop to read the rest of the file
		while (true) {

			//Read the components of the normal vector
			asciiInput >> string >> tri.normal.x >> tri.normal.y >> tri.normal.z;

			//Read the same line until \n and read next line
			getline(asciiInput, temp);
			getline(asciiInput, temp);

			//Read the 3 coordinates of vertex 1
			asciiInput >> string >> tri.point[0].x >> tri.point[0].y >> tri.point[0].z;
			getline(asciiInput, temp);

			//Read the 3 coordinates of vertex 2
			asciiInput >> string >> tri.point[1].x >> tri.point[1].y >> tri.point[1].z;
			getline(asciiInput, temp);

			//Read the 3 coordinates of vertex 3
			asciiInput >> string >> tri.point[2].x >> tri.point[2].y >> tri.point[2].z;
			getline(asciiInput, temp);

			//Read more lines
			getline(asciiInput, temp);
			getline(asciiInput, temp);

			//Update Geometry extents along 3 coordinates based on vertex 1
			if (tri.point[0].x < x_min) x_min = tri.point[0].x;
			if (tri.point[0].x > x_max) x_max = tri.point[0].x;
			if (tri.point[0].y < y_min) y_min = tri.point[0].y;
			if (tri.point[0].y > y_max) y_max = tri.point[0].y;
			if (tri.point[0].z < z_min) z_min = tri.point[0].z;
			if (tri.point[0].z > z_max) z_max = tri.point[0].z;

			//Update Geometry extents along 3 coordinates based on vertex 2
			if (tri.point[1].x < x_min) x_min = tri.point[1].x;
			if (tri.point[1].x > x_max) x_max = tri.point[1].x;
			if (tri.point[1].y < y_min) y_min = tri.point[1].y;
			if (tri.point[1].y > y_max) y_max = tri.point[1].y;
			if (tri.point[1].z < z_min) z_min = tri.point[1].z;
			if (tri.point[1].z > z_max) z_max = tri.point[1].z;
			
			//Update Geometry extents along 3 coordinates based on vertex 3
			if (tri.point[2].x < x_min) x_min = tri.point[2].x;
			if (tri.point[2].x > x_max) x_max = tri.point[2].x;
			if (tri.point[2].y < y_min) y_min = tri.point[2].y;
			if (tri.point[2].y > y_max) y_max = tri.point[2].y;
			if (tri.point[2].z < z_min) z_min = tri.point[2].z;
			if (tri.point[2].z > z_max) z_max = tri.point[2].z;

			//Break loop if "eof"
			if (asciiInput.eof()) break;

			triangle_num++;

			facet.emplace_back(tri);

		}

		asciiInput.close();

		return (0);
	}

	catch (std::runtime_error){
		std::cerr << "ERROR: read_ascii_STL_file function Fail" << std::endl;
		return (1);
	}
}


// -Open the BINARY STL file and Read the contents
int read_binary_STL_file(std::string STL_name, std::vector<triangle>& facet,
	double & x_min, double & x_max,
	double & y_min, double & y_max,
	double & z_min, double & z_max) {

	try {
		
		std::ifstream binaryInput; //Declare file object

		binaryInput.open((STL_path + STL_name).c_str(), std::ifstream::in | std::ifstream::binary);

		//Check Open Status
		if (binaryInput.fail()) {

			std::cout << "ERROR: Input STL file could not be opened" << std::endl;
			return (1);
		}

		//Position the pointer to byte num 80
		binaryInput.seekg(80);

		//Read facets num in the STL geometry
		int triangle_num;
		binaryInput.read((char*)&triangle_num, sizeof(int));

		//Parameters that will be used to store the minimum and maximum extents of the geometry descrived by the STL
		x_min = 1e+30, x_max = -1e+30;
		y_min = 1e+30, y_max = -1e+30;
		z_min = 1e+30, z_max = -1e+30;

		//Declare some variables
		char unused_bytes[2];
		float temp;

		// Start loop to read the file
		for (size_t i = 0; i < triangle_num; i++) {

			//Read the 3 components of the normal vector
			binaryInput.read((char*)&temp, 4); tri.normal.x = (double)temp;
			binaryInput.read((char*)&temp, 4); tri.normal.y = (double)temp;
			binaryInput.read((char*)&temp, 4); tri.normal.z = (double)temp;

			//Read the 3 coordinates of vertex 1 
			binaryInput.read((char*)&temp, 4); tri.point[0].x = (double)temp;
			binaryInput.read((char*)&temp, 4); tri.point[0].y = (double)temp;
			binaryInput.read((char*)&temp, 4); tri.point[0].z = (double)temp;

			//Read the 3 coordinates of vertex 2 
			binaryInput.read((char*)&temp, 4); tri.point[1].x = (double)temp;
			binaryInput.read((char*)&temp, 4); tri.point[1].y = (double)temp;
			binaryInput.read((char*)&temp, 4); tri.point[1].z = (double)temp;

			//Read the 3 coordinates of vertex 3
			binaryInput.read((char*)&temp, 4); tri.point[2].x = (double)temp;
			binaryInput.read((char*)&temp, 4); tri.point[2].y = (double)temp;
			binaryInput.read((char*)&temp, 4); tri.point[2].z = (double)temp;

			//Read the unused 2bytes
			binaryInput.read(unused_bytes, 2);

			//Update Geometry extents along 3 coordinates based on vertex 1
			if (tri.point[0].x < x_min) x_min = tri.point[0].x;
			if (tri.point[0].x > x_max) x_max = tri.point[0].x;
			if (tri.point[0].y < y_min) y_min = tri.point[0].y;
			if (tri.point[0].y > y_max) y_max = tri.point[0].y;
			if (tri.point[0].z < z_min) z_min = tri.point[0].z;
			if (tri.point[0].z > z_max) z_max = tri.point[0].z;

			//Update Geometry extents along 3 coordinates based on vertex 2
			if (tri.point[1].x < x_min) x_min = tri.point[1].x;
			if (tri.point[1].x > x_max) x_max = tri.point[1].x;
			if (tri.point[1].y < y_min) y_min = tri.point[1].y;
			if (tri.point[1].y > y_max) y_max = tri.point[1].y;
			if (tri.point[1].z < z_min) z_min = tri.point[1].z;
			if (tri.point[1].z > z_max) z_max = tri.point[1].z;

			//Update Geometry extents along 3 coordinates based on vertex 3
			if (tri.point[2].x < x_min) x_min = tri.point[2].x;
			if (tri.point[2].x > x_max) x_max = tri.point[2].x;
			if (tri.point[2].y < y_min) y_min = tri.point[2].y;
			if (tri.point[2].y > y_max) y_max = tri.point[2].y;
			if (tri.point[2].z < z_min) z_min = tri.point[2].z;
			if (tri.point[2].z > z_max) z_max = tri.point[2].z;

			facet.emplace_back(tri);
		}

		binaryInput.close();

		return (0);

	}

	catch (std::runtime_error){
		std::cerr << "ERROR: read_binary_STL_file function Fail" << std::endl;
		return (1);
		}
}

// -Reading input geometry files is STL format and finding out the required size of enviroment box
bool getGeometryInput(char* argv[], std::vector<triangle>& facet,
	double& x_min, double& x_max,
	double& y_min, double& y_max,
	double& z_min, double& z_max) {

	// Open STL file and read its contents

	//Get name of STL file, check VS Project Property
	std::string STL_name = argv[1];

	//Open the STL file specified as "read-only" and in "binary" mode
	FILE *fp; //file buffer
	fopen_s(&fp, (STL_path + STL_name).c_str(), "rb");
	
	//Dynamic Buffer for storing the contents of the file based on length
	unsigned char *buffer = new unsigned char[length];

	//Read an array of "length" elements from fp, return the number of bytes successfully read
	size_t read_length = fread(buffer, 1/*each element size*/, length, fp);

	//Close the STL file
	fclose(fp);

	// Loop over contents and count

	size_t text_count = 0;

	//Pointer Calculation
	const unsigned char *ptr = buffer; //array is pointer of starting loacaton of itself
	const unsigned char *buffer_end = buffer + read_length;

	//Loop over the entire buffer
	while (ptr != buffer_end) {
		if ((*ptr >= 0x20 && *ptr <= 0x7F) || *ptr == '\n' || *ptr == '\r' || *ptr == '\t') {

			text_count++;

		}

		ptr++;
	}

	//Free memory
	delete[] buffer; buffer = nullptr;

	//Calculate the fraction of characters(bytes) in part of the STL file that are BINARY
	double* current_percent_bin = new double(static_cast<double> (read_length - text_count) / static_cast<double> (read_length)); //why static_cast?

	//Anything above the cut-off value means STL file is BINARY type
	double* percent_bin = new double(0.05);

	std::string STL_type;

	if (current_percent_bin >= percent_bin)
		STL_type = "binary";
	else
		STL_type = "ascii";

	delete current_percent_bin, percent_bin; current_percent_bin, percent_bin = nullptr;

	// Functions call file and read the contents (depend on STL types)
	if (STL_type == "ascii") {

		int error = read_ascii_STL_file(STL_name, facet, x_min, x_max, y_min, y_max, z_min, z_max);

		if (error == 1)
			return true; //error check
	}

	if (STL_type == "binary") {

		int error = read_binary_STL_file(STL_name, facet, x_min, x_max, y_min, y_max, z_min, z_max);

		if (error == 1)
			return true; //error check
	}

	// Print the number of triangles
	std::cout << "The number of triangles in the STL file = " << facet.size() << std::endl;

	// Print the domain extent along X, Y and Z
	std::cout << std::endl;
	std::cout << "X range : " << x_min << " to " << x_max << " (Delta = " << x_max - x_min << ")" << std::endl;
	std::cout << "Y range : " << y_min << " to " << y_max << " (Delta = " << y_max - y_min << ")" << std::endl;
	std::cout << "Z range : " << z_min << " to " << z_max << " (Delta = " << z_max - z_min << ")" << std::endl;
	std::cout << std::endl;

	return false;


}