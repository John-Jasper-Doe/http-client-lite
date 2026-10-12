# amalgamation.cmake
#
# Optional single-header amalgamation (HTTP_CLIENT_LITE_AMALGAMATE): the
# generated httpclientlite.hpp is offered to the consumers of the target
# and regenerated only when one of the source headers or the script itself
# changes. Must be included after cmake/library.cmake (needs the header
# list) and before cmake/install.cmake (installs AMALGAMATED_HEADER).
# --------------------------
##
# Copyright 2021... by Maxim Gusev
#
# https://github.com/John-Jasper-Doe/http-client-lite
#
# Distributed under the MIT License.
# (See accompanying file LICENSE or copy at https://mit-license.org/)


if (NOT HTTP_CLIENT_LITE_AMALGAMATE)
  return ()
endif (NOT HTTP_CLIENT_LITE_AMALGAMATE)

message (STATUS "Configuring the single-header amalgamation ...")

set (AMALGAMATED_HEADER ${CMAKE_BINARY_DIR}/generated/httpclientlite.hpp)
add_custom_command (
  OUTPUT ${AMALGAMATED_HEADER}
  COMMAND ${CMAKE_COMMAND}
    "-DHEADER_DIR=${CMAKE_CURRENT_SOURCE_DIR}/include"
    "-DOUTPUT_FILE=${AMALGAMATED_HEADER}"
    -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/amalgamate.cmake
  DEPENDS ${${PROJECT_NAME}_HEADERS}
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/amalgamate.cmake
  COMMENT
    "Amalgamating the headers into httpclientlite.hpp"
  VERBATIM
)
add_custom_target (amalgamate ALL DEPENDS ${AMALGAMATED_HEADER})
target_include_directories (${PROJECT_NAME} INTERFACE
  "$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/generated>"
)
message (STATUS "- Generated header: ${AMALGAMATED_HEADER}")
