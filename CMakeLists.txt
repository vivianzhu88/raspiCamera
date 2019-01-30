# cmake needs this line
cmake_minimum_required(VERSION 3.1)

# Enable C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# Define project name
project(opencv_example_project)

# Find OpenCV, you may need to set OpenCV_DIR variable
# to the absolute path to the directory containing OpenCVConfig.cmake file
# via the command line or GUI
find_package(OpenCV REQUIRED)
find_package(Threads REQUIRED)

# If the package has been found, several variables will
# be set, you can find the full list with descriptions
# in the OpenCVConfig.cmake file.
# Print some message showing some of them
message(STATUS "OpenCV library status:")
message(STATUS "    config: ${OpenCV_DIR}")
message(STATUS "    version: ${OpenCV_VERSION}")
message(STATUS "    libraries: ${OpenCV_LIBS}")
message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")
message(STATUS "PThread library status:")
message(STATUS "    version: ${Threads_VERSION}")
message(STATUS "    libraries: ${Threads_LIBS}")

# Declare the executable target built from your sources
add_executable(opencv_cam_server cameraServer.cpp)
add_executable(opencv_cam_client cameraClient.cpp)

# Link your application with OpenCV libraries
target_link_libraries(opencv_cam_server ${OpenCV_LIBS} ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(opencv_cam_client ${OpenCV_LIBS} ${CMAKE_THREAD_LIBS_INIT})
