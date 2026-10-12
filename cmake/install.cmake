# install.cmake
#
# Installation rules and the CMake package config
# --------------------------------------------------
#
# Installs the headers, the documentation and the CMake package files, so a
# consumer can pick the library up either from the include path or with
#
#   find_package (http_client_lite CONFIG REQUIRED)
#   target_link_libraries (<app> PRIVATE http_client_lite::http_client_lite)
#
# GNUInstallDirs is included by the root CMakeLists.txt (the target needs
# CMAKE_INSTALL_INCLUDEDIR at creation time).

message (STATUS "Configuring the installation layout ...")
message (STATUS "- Install prefix: ${CMAKE_INSTALL_PREFIX}")

# Numeric version for the compatibility file: the build metadata after '+'
# (git hash etc.) is dropped; a non-numeric version (git unavailable) falls
# back to 0.0.0.
set (HTTP_CLIENT_LITE_VERSION_NUM "0.0.0")
if (HTTP_CLIENT_LITE_VERSION MATCHES "^([0-9]+(\\.[0-9]+)*)")
  set (HTTP_CLIENT_LITE_VERSION_NUM "${CMAKE_MATCH_1}")
endif (HTTP_CLIENT_LITE_VERSION MATCHES "^([0-9]+(\\.[0-9]+)*)")
message (STATUS "- Package version: ${HTTP_CLIENT_LITE_VERSION_NUM}")

include (CMakePackageConfigHelpers)

configure_package_config_file (
  ${CMAKE_CURRENT_LIST_DIR}/http_client_liteConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/cmake/http_client_liteConfig.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)
write_basic_package_version_file (
  ${CMAKE_CURRENT_BINARY_DIR}/cmake/http_client_liteConfigVersion.cmake
  VERSION ${HTTP_CLIENT_LITE_VERSION_NUM}
  COMPATIBILITY SameMajorVersion
  ARCH_INDEPENDENT
)

# Headers: include/httpclientlite/** -> <prefix>/include/httpclientlite/**
install (DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Documentation: LICENSE and README -> <prefix>/share/doc/<project>/
install (FILES LICENSE README.md DESTINATION ${CMAKE_INSTALL_DOCDIR})
message (STATUS "- Documentation: ${CMAKE_INSTALL_DOCDIR}")

# The amalgamated single header (generated at build time) -> <prefix>/include/
if (AMALGAMATED_HEADER)
  install (FILES ${AMALGAMATED_HEADER} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
  message (STATUS "- Single header: ${CMAKE_INSTALL_INCLUDEDIR}/httpclientlite.hpp")
endif (AMALGAMATED_HEADER)

# CMake package: the exported target plus the config/version files
set (INSTALL_CMAKEDIR ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})
install (TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}Targets)
install (FILES
  ${CMAKE_CURRENT_BINARY_DIR}/cmake/${PROJECT_NAME}Config.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/cmake/${PROJECT_NAME}ConfigVersion.cmake
  DESTINATION ${INSTALL_CMAKEDIR}
)
install (EXPORT ${PROJECT_NAME}Targets
  NAMESPACE ${PROJECT_NAME}::
  DESTINATION ${INSTALL_CMAKEDIR}
)
message (STATUS "- CMake package: ${INSTALL_CMAKEDIR}")
