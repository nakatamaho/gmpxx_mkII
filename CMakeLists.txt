cmake_minimum_required(VERSION 3.12)
project(gmpxx_mkII VERSION 2.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find GMP (prefer custom build if available)
set(CUSTOM_GMP_PATH "${CMAKE_SOURCE_DIR}/i/GMP-6.3.0")
if(EXISTS "${CUSTOM_GMP_PATH}")
    list(PREPEND CMAKE_PREFIX_PATH "${CUSTOM_GMP_PATH}")
    message(STATUS "Using custom GMP: ${CUSTOM_GMP_PATH}")
endif()

find_package(PkgConfig REQUIRED)
pkg_check_modules(GMP REQUIRED gmp gmpxx)

# Get Git commit hash
find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
    execute_process(
        COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_COMMIT_HASH
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )
else()
    set(GIT_COMMIT_HASH "unknown")
endif()

# Configure header file
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/gmpxx_mkII.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/gmpxx_mkII.h"
    @ONLY
)

# Header-only library
add_library(gmpxx_mkII INTERFACE)
target_include_directories(gmpxx_mkII INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<INSTALL_INTERFACE:include>
)

# Link GMP libraries with full paths (avoids path issues)
target_link_libraries(gmpxx_mkII INTERFACE 
    ${GMP_LINK_LIBRARIES}
    ${GMP_LIBRARIES}
)
target_compile_options(gmpxx_mkII INTERFACE ${GMP_CFLAGS_OTHER})

# Optional examples and tests
option(BUILD_EXAMPLES "Build examples" OFF)
option(BUILD_TESTS "Build tests" OFF)

if(BUILD_EXAMPLES AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/examples")
    add_subdirectory(examples)
endif()

if(BUILD_TESTS AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests")
    enable_testing()
    add_subdirectory(tests)
endif()

# Installation
include(GNUInstallDirs)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/gmpxx_mkII.h"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(TARGETS gmpxx_mkII
    EXPORT gmpxx_mkII_targets
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(EXPORT gmpxx_mkII_targets
    FILE gmpxx_mkII_targets.cmake
    NAMESPACE gmpxx_mkII::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/gmpxx_mkII
)

# Package config
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/gmpxx_mkII_config_version.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

# Create simple config file
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/gmpxx_mkII_config.cmake" "
include(CMakeFindDependencyMacro)
find_dependency(PkgConfig)
if(PkgConfig_FOUND)
    pkg_check_modules(GMP REQUIRED gmp gmpxx)
endif()
include(\"\${CMAKE_CURRENT_LIST_DIR}/gmpxx_mkII_targets.cmake\")
")

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/gmpxx_mkII_config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/gmpxx_mkII_config_version.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/gmpxx_mkII
)
