cmake_minimum_required(VERSION 3.7 FATAL_ERROR)
project(examples)

include_directories(${CMAKE_SOURCE_DIR}/include)
add_executable(${PROJECT_NAME}_simple_request simple_request.cxx)
