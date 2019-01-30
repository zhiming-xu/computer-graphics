#include <iostream> // C++ input/output "stream" library
#include <vector> // Standard Template Library extendable array class
#include <string>

#include "lodepng.h" // When including a local file, use quotes instead of angle brackets
#include "image.h"

using namespace std;

#include <fstream>
#include <math.h>

int main(int argc, char* argv[]) {

	// ofstream f("gauss_19x19_sig8.filt");
	// f << "19 19" << endl;
	// int n = 19;
	// for (int i = 0; i < n*n; ++i) {
	// 	float x = i%n-n/2-1, y = i/n-n/2-1;
	// 	float g = exp(-.5 * (x*x+y*y) / powf(8.,2.));
	// 	f << g << " ";
	// 	if (i % n == n-1)
	// 		f << endl;
	// }

	// return 0;

	// printf is the C function for printing to the console.
	// You are probably familiar with this from previous classes.
	printf("Hello! This is a simple image processing program,\n");

	// cout is the standard output "stream" for C++. The operator << 
	// inserts output into cout. endl inserts an endline and flushes
	// the stream's buffer out to the console.
	cout << "written in C++" << endl;

	// argc counts the number of command line arguments, including the 
	// name of the program.
	if (argc < 3) {
		cout << "Usage: ./convolve img.png filter.filt" << endl;
		return 0;
	}

	Image image(argv[1]);
	Filter filter(argv[2]);
	Image filtered = image * filter;
	filtered.write("filtered.png");

	return 0;
}