cmake_minimum_required(VERSION 3.10)

# Project name
project(SpinningCubeProject)

# Set policy CMP0072 to prefer GLVND
if (POLICY CMP0072)
  cmake_policy(SET CMP0072 NEW)
endif()

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the executable
add_executable(SpinningCubeProject main.cpp)
add_executable(SpinningCubeProject_2 main_no_distortion.cpp)

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

# Find Eigen
find_package(Eigen3 REQUIRED)
include_directories(${EIGEN3_INCLUDE_DIR})

# Link libraries
target_link_libraries(SpinningCubeProject ${OPENGL_LIBRARIES} ${GLFW_LIBRARIES} ${GLEW_LIBRARIES})
target_link_libraries(SpinningCubeProject_2 ${OPENGL_LIBRARIES} ${GLFW_LIBRARIES} ${GLEW_LIBRARIES})
