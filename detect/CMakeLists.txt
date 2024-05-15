# License: Apache 2.0. See LICENSE file in root directory.
# Copyright(c) 2019 Intel Corporation. All Rights Reserved.
cmake_minimum_required(VERSION 3.1.0)

project(crater)

# Find librealsense2 installed package
find_package(realsense2 REQUIRED)
find_package(OpenCV REQUIRED)
# Enable C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# Add the application sources to the target
add_executable(${PROJECT_NAME} depthMap.cpp)
add_executable(aiden ~/aiden.cpp)
add_executable(sample sample.cpp)
# Link librealsense2 to the target
target_link_libraries(${PROJECT_NAME} ${realsense2_LIBRARY} ${OpenCV_LIBRARIES})
target_link_libraries(sample ${realsense2_LIBRARY} ${OpenCV_LIBRARIES})
target_link_libraries(aiden ${realsense2_LIBRARY} ${OpenCV_LIBRARIES})
