cmake_minimum_required(VERSION 3.16.0)

project(GrabKeyRootProject VERSION 0.2.0)

if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
   SET(CMAKE_BUILD_TYPE Release)
endif(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)

add_subdirectory("GrabKeyLib")
add_subdirectory("Examples")