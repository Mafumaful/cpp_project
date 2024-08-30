cmake_minimum_required(VERSION 3.10)

# Project name
project(SimpleOpenGLProject)

# Set policy CMP0072 to prefer GLVND
if (POLICY CMP0072)
  cmake_policy(SET CMP0072 NEW)
endif()

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the executable
add_executable(SimpleOpenGLProject main.cpp)

# Find OpenGL
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})

# Find GLFW
find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)
include_directories(${GLFW_INCLUDE_DIRS})

# Find GLEW
find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS})

# Link libraries
target_link_libraries(SimpleOpenGLProject ${OPENGL_LIBRARIES} ${GLFW_LIBRARIES} ${GLEW_LIBRARIES})
