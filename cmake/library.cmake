# library.cmake
#
# The interface (header-only) library target: header list for the IDEs,
# include directories for the build tree and for the installed package.
# --------------------------
##
# Copyright 2021... by Maxim Gusev
#
# https://github.com/John-Jasper-Doe/http-client-lite
#
# Distributed under the MIT License.
# (See accompanying file LICENSE or copy at https://mit-license.org/)


message (STATUS "Configuring the library target ...")

file (GLOB_RECURSE ${PROJECT_NAME}_HEADERS
  ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h*
)

# Interface library
add_library (${PROJECT_NAME} INTERFACE)
# The same name with the package namespace: the build-tree consumers use
# the target exactly like the installed ones (find_package).
add_library (${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

# The headers are listed as INTERFACE sources for IDE visibility only: the
# BUILD_INTERFACE wrapper keeps them out of the installed export file, which
# rejects source-directory paths.
foreach (${PROJECT_NAME}_HEADER IN LISTS ${PROJECT_NAME}_HEADERS)
  target_sources (
    ${PROJECT_NAME}
    INTERFACE $<BUILD_INTERFACE:${${PROJECT_NAME}_HEADER}>
  )
endforeach (${PROJECT_NAME}_HEADER IN LISTS ${PROJECT_NAME}_HEADERS)

target_include_directories (
  ${PROJECT_NAME}
  INTERFACE
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)
