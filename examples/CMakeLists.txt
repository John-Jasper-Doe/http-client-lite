cmake_minimum_required(VERSION 3.3 FATAL_ERROR)
project(examples)

function(add_exec CPP_FILE_NAME)
  add_executable(${PROJECT_NAME}_${CPP_FILE_NAME} ${CPP_FILE_NAME}.cpp)
  target_link_libraries(${PROJECT_NAME}_${CPP_FILE_NAME} PRIVATE http_client_lite)
endfunction(add_exec)

add_exec(simple_request)
add_exec(sandbox)

