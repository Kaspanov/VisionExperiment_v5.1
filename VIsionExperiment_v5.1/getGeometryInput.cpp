#include "pch.h"
#include "getGeometryInput.h"

//[Global Variables]
// - How many bytes of the file will be read to calculate the percentage of characters
unsigned long length = 256;

// [Functions]
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
	std::cout << "X range : " << x_min << " to " << x_max << " (delta = " << x_max - x_min << ")" << std::endl;
	std::cout << "Y range : " << y_min << " to " << y_max << " (delta = " << y_max - y_min << ")" << std::endl;
	std::cout << "Z range : " << z_min << " to " << z_max << " (delta = " << z_max - z_min << ")" << std::endl;
	std::cout << std::endl;

	return false;


}

