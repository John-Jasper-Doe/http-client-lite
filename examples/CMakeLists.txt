cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
project(examples)

add_executable(${PROJECT_NAME}_simple_request simple_request.cpp)
target_link_libraries(${PROJECT_NAME}_simple_request PRIVATE http_client_lite)
