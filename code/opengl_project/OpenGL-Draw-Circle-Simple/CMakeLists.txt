cmake_minimum_required(VERSION 3.10)

# Set the project name
project(OpenGLDemo)

# Set policy CMP0072 to prefer GLVND if available
if (POLICY CMP0072)
  cmake_policy(SET CMP0072 NEW)
endif()

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find packages
find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 REQUIRED CONFIG)
find_package(GLEW REQUIRED)

# Include GLM manually
include_directories(${CMAKE_SOURCE_DIR}/include)

# Add the executable
add_executable(OpenGLDemo main.cpp)

# Link libraries
target_link_libraries(OpenGLDemo ${OPENGL_LIBRARIES} glfw GLEW)
