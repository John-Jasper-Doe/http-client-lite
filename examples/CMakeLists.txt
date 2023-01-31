cmake_minimum_required (VERSION 3.3 FATAL_ERROR)
project (examples)

function (add_exec CPP_FILE_NAME)
  add_executable (${PROJECT_NAME}_${CPP_FILE_NAME} ${CPP_FILE_NAME}.cpp)
  target_link_libraries (${PROJECT_NAME}_${CPP_FILE_NAME} PRIVATE http_client_lite)
  set_target_properties (${PROJECT_NAME}_${CPP_FILE_NAME} PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY}")
endfunction (add_exec)

add_exec(simple_request)
add_exec(sandbox)

