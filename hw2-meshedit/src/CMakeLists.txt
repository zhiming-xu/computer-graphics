cmake_minimum_required(VERSION 2.8)

# Collada viewer source
set(COLLADA_VIEWER_SOURCE
    bezierPatch.cpp
    bezierCurve.cpp
    scene.cpp
    camera.cpp
    light.cpp
    mesh.cpp
    material.cpp
    texture.cpp
    collada.cpp
    halfEdgeMesh.cpp
    student_code.cpp
    meshEdit.cpp
    main.cpp
    png.cpp
)

# Collada viewer header
set(COLLADA_VIEWER_HEADER
    bezierPatch.h
    bezierCurve.h
    scene.h
    camera.h
    light.h
    mesh.h
    material.h
    texture.h
    collada.h
    halfEdgeMesh.h
    student_code.h
    meshEdit.h
    shaderUtils.h
    mergeVertices.h
    png.h
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
add_executable( meshedit
    ${COLLADA_VIEWER_SOURCE}
    ${COLLADA_VIEWER_HEADER}
)

target_link_libraries( meshedit
    CGL ${CGL_LIBRARIES}
    glew ${GLEW_LIBRARIES}
    glfw ${GLFW_LIBRARIES}
    ${OPENGL_LIBRARIES}
    ${FREETYPE_LIBRARIES}
    ${CMAKE_CURRENT_SOURCE_DIR}/dirent/glut32.lib
)

#-------------------------------------------------------------------------------
# Platform-specific configurations for target
#-------------------------------------------------------------------------------
if(APPLE)
  set_property( TARGET meshedit APPEND_STRING PROPERTY COMPILE_FLAGS
                "-Wno-deprecated-declarations -Wno-c++11-extensions")
endif(APPLE)


# Put executable in build directory root
set(EXECUTABLE_OUTPUT_PATH ..)

# Install to project root
install(TARGETS meshedit DESTINATION ${ColladaViewer_SOURCE_DIR})
