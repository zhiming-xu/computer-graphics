cmake_minimum_required (VERSION 2.6)
project (Disc01)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Application source
set(APPLICATION_SOURCE
    main.cpp

    filter.cpp
    lodepng.cpp
    image.cpp

    filter.h
    lodepng.h
    image.h
)

add_executable (convolve ${APPLICATION_SOURCE})