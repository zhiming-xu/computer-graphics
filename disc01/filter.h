#ifndef FILTER_H
#define FILTER_H

#include <iostream> // C++ input/output "stream" library
#include <vector> // Standard Template Library extendable array class
#include <string>
#include <fstream>


struct Filter {
	// Constructs the identity filter
	Filter();
	// Constructs a filter from a .filt file
	Filter(const std::string& filename);
	// Loads a filter from a .filt file
	void read(std::string filename);
	// Makes the entries in the kernel sum to one
	void normalize();
	// Returns the (x,y) value of the kernel (0-indexed)
	const float& at(int x, int y) const;
	
	// Width and height of kernel
	unsigned width, height;
	// Flattened 2D array storing actual filter values. Has width*height members.
	std::vector<float> kernel;
};

#endif 