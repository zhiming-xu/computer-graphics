#ifndef IMAGE_H
#define IMAGE_H

#include <iostream> // C++ input/output "stream" library
#include <vector> // Standard Template Library extendable array class
#include <string>
#include <fstream>

#include "filter.h"

class Image {
public:
	Image();
	// Construct image by loading it from a file
	Image(const std::string& filename);
	// Construct blank image with given size
	Image(const unsigned& w, const unsigned& h);

	// Read and write image from disk
	void read(std::string filename);
	void write(std::string filename) const;

	// Returns the location of the (x,y) pixel in memory
	uint8_t* at(int x, int y);
	
	// Convolves this image with a filter and returns a new image
	Image operator*(const Filter& filter);

private:
	// The width and height of the image
	unsigned width, height;
	// Flattened 2d vector of pixel values as sets of 3 unsigned chars (RGB order).
	std::vector<uint8_t> data;
};

#endif