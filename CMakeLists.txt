cmake_minimum_required(VERSION 2.8.0 FATAL_ERROR)
project(SpinnakerExample)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

find_package(Spinnaker REQUIRED)

add_subdirectory(src)
