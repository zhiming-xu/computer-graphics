cmake_minimum_required(VERSION 2.8)

# Application source
if(BUILD_3-1)
    set(APPLICATION_SOURCE

        # Collada Parser
        collada/collada.cpp
        collada/camera_info.cpp
        collada/light_info.cpp
        collada/sphere_info.cpp
        collada/polymesh_info.cpp
        collada/material_info.cpp

        # Dynamic Scene
        dynamic_scene/mesh.cpp
        dynamic_scene/scene.cpp
        dynamic_scene/sphere.cpp

        # Static scene
        static_scene/object.cpp
        static_scene/environment_light.cpp
        static_scene/light.cpp
        static_scene/sphere.cpp
        static_scene/triangle.cpp

        # MeshEdit
        halfEdgeMesh.cpp
        meshEdit.cpp

        # PathTracer
        bsdf.cpp
        camera.cpp
        sampler.cpp
        bbox.cpp
        bvh.cpp
        pathtracer.cpp
        part1_code.cpp

        # misc
        misc/sphere_drawing.cpp

        # Application
        application.cpp
        main.cpp
    )
else(BUILD_3-1)
    set(APPLICATION_SOURCE

        # Collada Parser
        collada/collada.cpp
        collada/camera_info.cpp
        collada/light_info.cpp
        collada/sphere_info.cpp
        collada/polymesh_info.cpp
        collada/material_info.cpp

        # Dynamic Scene
        dynamic_scene/mesh.cpp
        dynamic_scene/scene.cpp
        dynamic_scene/sphere.cpp

        # Static scene
        static_scene/object.cpp
        static_scene/environment_light.cpp
        static_scene/light.cpp

        # MeshEdit
        halfEdgeMesh.cpp
        meshEdit.cpp

        # PathTracer
        bsdf.cpp
        camera.cpp
        sampler.cpp
        pathtracer.cpp

        # misc
        misc/sphere_drawing.cpp

        # Application
        application.cpp
        main.cpp
    )
endif(BUILD_3-1)

# Windows-only sources
if(WIN32)
list(APPEND APPLICATION_SOURCE
    # For get-opt
    misc/getopt.c
)
endif(WIN32)

#-------------------------------------------------------------------------------
# Set include directories
#-------------------------------------------------------------------------------
include_directories(
  ${GLEW_INCLUDE_DIRS}
  ${GLFW_INCLUDE_DIRS}
  ${CGL_INCLUDE_DIRS}
  ${FREETYPE_INCLUDE_DIRS}
)

#-------------------------------------------------------------------------------
# Set link directories
#-------------------------------------------------------------------------------
link_directories(
  ${GLEW_LIBRARY_DIRS}
  ${GLFW_LIBRARY_DIRS}
  ${CGL_LIBRARY_DIRS}
  ${FREETYPE_LIBRARY_DIRS}
)

#-------------------------------------------------------------------------------
# Add executable
#-------------------------------------------------------------------------------
add_executable(pathtracer ${APPLICATION_SOURCE})
if (WIN32)
    target_compile_definitions( pathtracer PUBLIC -DNNG_STATIC_LIB)
endif(WIN32)

target_link_libraries( pathtracer
    CGL ${CGL_LIBRARIES}
    glew ${GLEW_LIBRARIES}
    glfw ${GLFW_LIBRARIES}
    ${OPENGL_LIBRARIES}
    ${FREETYPE_LIBRARIES}
    ${CMAKE_THREADS_INIT}
)

#-------------------------------------------------------------------------------
# Platform-specific 3-1 starter library
#-------------------------------------------------------------------------------
if(NOT BUILD_3-1)
    set( LIB3-1 ${PathTracer_SOURCE_DIR}/lib3-1)
    if (LINUX)
        target_link_libraries( pathtracer
            ${LIB3-1}/lib3-1_linux.a
        )
    endif()
    if (APPLE)
        target_link_libraries( pathtracer
            ${LIB3-1}/lib3-1_mac.a
        )
    endif()
    if (WIN32)
        # Generate for both debug and release in visual studio
        #[[if (EXISTS ${PathTracer_SOURCE_DIR}/src/part1_code.cpp)
            add_library(lib3-1_windows STATIC
                static_scene/sphere.cpp
                static_scene/triangle.cpp
                bvh.cpp
                bbox.cpp
                part1_code.cpp
            )
            set_target_properties(lib3-1_windows PROPERTIES
                ARCHIVE_OUTPUT_DIRECTORY ${LIB3-1}
                LIBRARY_OUTPUT_DIRECTORY ${LIB3-1}
            )
        endif()]]
        target_link_libraries( pathtracer debug
            ${LIB3-1}/Debug/lib3-1_windows.lib
        )
        target_link_libraries( pathtracer optimized
            ${LIB3-1}/Release/lib3-1_windows.lib
        )
    endif()
endif(NOT BUILD_3-1)

#-------------------------------------------------------------------------------
# Platform-specific configurations for target
#-------------------------------------------------------------------------------
if(APPLE)
  set_property( TARGET pathtracer APPEND_STRING PROPERTY COMPILE_FLAGS
                "-Wno-deprecated-declarations -Wno-c++11-extensions")
endif(APPLE)

# Put executable in build directory root
set(EXECUTABLE_OUTPUT_PATH ..)

# Install to project root
install(TARGETS pathtracer DESTINATION ${PathTracer_SOURCE_DIR})
