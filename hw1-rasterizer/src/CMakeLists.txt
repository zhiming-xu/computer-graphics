# Original file Copyright CMU462 Fall 2015: 
# Kayvon Fatahalian, Keenan Crane,
# Sky Gao, Bryce Summers, Michael Choquette.
cmake_minimum_required(VERSION 2.8)

# Application source
set(APPLICATION_SOURCE
    
    texture.cpp
    triangulation.cpp
    svgparser.cpp
    transforms.cpp
    drawrend.cpp
    svg.cpp
    main.cpp
    # Add headers for the sake of Xcode/Visual Studio projects
    drawrend.h
    svg.h
    svgparser.h
    texture.h
    transforms.h
    triangulation.h
)

#-------------------------------------------------------------------------------
# Set include directories
#-------------------------------------------------------------------------------
include_directories(
  ${GLEW_INCLUDE_DIRS}
  ${GLFW_INCLUDE_DIRS}
  ${CGL_INCLUDE_DIRS}
  ${FREETYPE_INCLUDE_DIRS}
)

if (WIN32)
    include_directories(dirent)
endif(WIN32)

#-------------------------------------------------------------------------------
# Set link directories
#-------------------------------------------------------------------------------
link_directories(
  ${GLEW_LIBRARY_DIRS}
  ${GLFW_LIBRARY_DIRS}
  ${CGL_LIBRARY_DIRS}
  ${FREETYPE_LIBRARY_DIRS}
)

if (WIN32)
    list(APPEND APPLICATION_SOURCE dirent/dirent.c)
endif(WIN32)

#-------------------------------------------------------------------------------
# Add executable
#-------------------------------------------------------------------------------
add_executable(draw ${APPLICATION_SOURCE})

target_link_libraries( draw
    CGL ${CGL_LIBRARIES}
    glew ${GLEW_LIBRARIES}
    glfw ${GLFW_LIBRARIES}
    ${OPENGL_LIBRARIES}
    ${FREETYPE_LIBRARIES}
    ${CMAKE_THREADS_INIT}
)

#-------------------------------------------------------------------------------
# Platform-specific configurations for target
#-------------------------------------------------------------------------------
if(APPLE)
  set_property( TARGET draw APPEND_STRING PROPERTY COMPILE_FLAGS
                "-Wno-deprecated-declarations -Wno-c++11-extensions")
endif(APPLE)

# Put executable in build directory root
set(EXECUTABLE_OUTPUT_PATH ..)

# Install to project root
install(TARGETS draw DESTINATION ${Assignment1_SOURCE_DIR})
