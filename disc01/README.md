# Discussion 1: C++ review via convolution

## Objectives
This coding exercise is meant give you a chance to apply the concepts of image filtering and convolution from lecture, as well as increase your familiarity with C++ before starting the first assignment.

You are responsible for filling in the member functions for the `Filter` and `Image` classes in C++.

After this discussion section, you should feel a little more comfortable with

* cloning a git repo
* using `cmake` and `make`
* C++ command line, text file, and image file I/O
* C++ program structure (*.h* and *.cpp* file organization)
* declarations versus definitions in C++
* structs versus classes in C++
* C++ constructors
* using the STL `vector` class in C++
* passing by value versus passing by reference in C++
* references versus pointers in C++
* 2D array indexing
* storing pixel colors as sets of 4 `unsigned char` variables (RGBA channels)
* blurring images using filters
* 2D discrete convolution

Feel free to ask for more detail on any of these concepts during section.

Some of our great TAs wrote a [C++ primer](https://cs184.eecs.berkeley.edu/sp19/article/11/c-basics) to help you get familiarized with the language, we highly recommend taking a look at it if you've never used C++ before.

A great C++ resource is [cplusplus.com](http://www.cplusplus.com/), which has a helpful C++ [tutorial](http://www.cplusplus.com/doc/tutorial/), as well as good documentation for referencing. For example, you can look up Standard Template Library (STL) classes and their member functions -- you might find their page on the [vector](http://www.cplusplus.com/reference/vector/vector/) class useful today.

## Setup
Please clone the [disc01](https://github.com/cal-cs184/disc01) repository and navigate to its root directory.

    git clone https://github.com/cal-cs184/disc01
    cd disc01

From this folder, run 

    mkdir build; cd build

to create a build directory and enter it, then 

    cmake ..

to have CMake generate the appropriate Makefiles for your system. To actually compile the code, run

    make 

You should see an executable called `convolve` appear.

Like the other assignments in the class, this codebase uses a Makefile-creation system called `cmake` that greatly simplifies C++ compilation. After running `cmake` for the first time, you will simply run `make` to rebuild your program every time you change the source code. A nice thing about a build system using `make` and `cmake` is that they check file timestamps to determine which files really need to be recompiled.

Common C++ compilers include `g++/gcc` and `clang`.

## C++ program structure

To run the program, use the command

    ./convolve some_img.png some_filter.filt
    
The *convolve* executable is the one we just compiled with the `make` command. 

Read over *main.cpp*. Like C and Java, C++ always starts program execution  at the `main()` function.  It takes two parameters:

1. `int argc` is the number of command line arguments, including the name of the program.
2. `char* argv[]` provides pointers to the `char*` strings containing each space-separated argument. `argv[0]` here is `"./convolve"`.

The filtered output image will be written to `filtered.png` for viewing.

`main.cpp` also contains some examples of text output in  C++ (using the standard output stream), as well as the main body of the program:


    Image image(argv[1]);
    Filter filter(argv[2]);
    Image filtered = image * filter;
    filtered.write("filtered.png");


Here the `*` operator is overloaded to convolve an `Image` with a `Filter`. The first two lines load an image and filter file, respectively, and the final line writes the filtered result to a file. Your job will be implementing the member functions of `Image` and `Filter`, found in other files.

## Filter struct

Take a look at *filter.h*, a C++ header file. It contains the `Filter` struct and a declaration of its member functions and variables. Generally, declarations of structs and classes and their member functions and variables are placed in header files (*filename.h*), while their definitions are placed in corresponding cpp files (*filename.cpp*). This way, if we need to reference code in another file, we can simply `#include` its header file. The preprocessor will paste in the text of an `#include`-ed file wherever that macro is placed. Note that headers are never compiled alone -- only when they occur at the top of some *.cpp* file.

Also, notice the `#ifndef` guard in *filter.h*: this prevents the compiler from double-pasting the header in a single file, which could cause problems with multiple definitions (if anything is not only declared but also defined within the header). It is generally good practice not to define functions fully within a header, but exceptions can be made for short, self-explanatory functions.

`Filter` is a struct, which has a different meaning in C++ compared to C. In C++, structs and classes are nearly identical. The difference is only that data and functions are public by default in a struct, but private by default in a class. Semantically, people interpret a struct to be more like a bundle of public data (originating from its usage in C) and a class to be more like a fully responsive, protected object. Although `Filter` should perhaps ideally be a class, we've made it a struct so you can compare its difference in structure to the `Image` class.

The member function definitions for `Filter` reside in *filter.cpp*. The first two are *constructors*, which are only ever called upon creation of a new `Filter`. The one with no arguments is the default constructor, which creates a simple "identity filter" function. The second loads a filter from a file.


### Your job

**Implement the `Filter::read` function.**

Filter files are stored in the following format:


    w h
    f f ... f
    f f ... f
    ...
    f f ... f


where `w` is the width and `h` is the height of the filter. The width is the number of columns and the height is the number of rows. Each `f` represents one entry in the rectangular filter kernel. **NOTE**: filters are not stored "normalized", so you need to add up all the `f` values and then divide each one by the sum so that your `Filter`'s kernel adds up to one.

You may want to look up how to use an `std::ifstream` to read the data, and `std::vector` to store the data into the `kernel` member variable.

Hint: Here's a code snippet to read in <code>int</code>s.

    int x, y;
    std::ifstream mystream(filename);
    mystream >> x >> y;


**Implement the `Filter::normalize` function.**

This function adds up all the numbers in the kernel and then divides each one by that sum, ensuring that the new values will add up to one.

**Implement the `Filter::at` function.**

This is a one line function that returns a *reference* to one entry of the `Filter` kernel. Note that references are somewhat unique to C++ and aren't present in C or Java, which are both exclusively pass-by-value. References act like a permanently dereferenced pointer -- like another name for the same variable. For example, this:
 
    void square(int x, int& squared) {
        squared = x * x;
    }
    
    int y; 
    square(4, y);
   
actually stores `16` in `y`. Or, if we did this:

    myFilter.at(3, 1) = .71f;

it would actually change the (3,1) entries in the kernel vector. However, since this is considered undesirable behavior for a vector, the compiler will complain and force you to ensure vector references are constant if possible.

## Image class

Before getting started, please take a look at this [guide](https://cs184.eecs.berkeley.edu/sp19/article/7/images-as-data) that helps clarify the concept of an "image" in the realm of computer graphics and imaging. Fundamentally, an image is a 2D grid of pixels, or color values. In the case of this discussion, each of our pixels consist of *4 different channels - red, green, blue, and alpha*. We sometimes discard the alpha channel, but not in this discussion.

With that in mind, now we can move on to the `Image` class. This class is fairly similar to the `Filter` struct except that it supports an additional convolution operator `*`.

### Your job

**Implement the `Image::read` and `Image::write` functions.**

Look up how to use the lodepng `lodepng::decode` and `lodepng::encode` functions to do this.

**Implement the `Image::at` function.**

This is a one line function that returns the address of a pixel in the `data` vector of pixels. Each pixel takes up four bytes (a.k.a. `unsigned char`s or `uint8_t`s). The pixels are laid out such that the `width` pixels in the first row (where `y=0`) come first, then the `y=1` row, and so on, until the `y=height-1` row. This means that you need to use the `y` argument to "jump" up by `y` rows, or `y * width` pixels. Then you can access the `x` pixel in that row with


    y * width + x


Finally, you need to multiply all of that by 4 to get the right pointer offset from the start of the data array, since each pixel is 4 unsigned chars.

**Implement the `Image::operator*` (convolution) function.**

This function is the most complicated. Wikipedia has an [excellent article](https://en.wikipedia.org/wiki/Convolution) on the convolution operator which includes some animations that make understanding the operator a little easier.

You will need to:

1. Create a new image (of the same size as the current image) that can hold the output.
2. For each pixel location in the old image, compute a weighted sum of the neighboring colors, each weighted by the filter's value at that relative offset (where the center of the pixel is at the location of your current pixel in the image).
3. Assign this weighted sum to the same location in the new image.
4. Return the new image.

Now, you should be able to run the program from the command line and inspect your filtered output at *filtered.png*. Try running it with your own PNG image with any of the *.filt* files, or you can try making your own filter!

