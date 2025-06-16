cmake_minimum_required(VERSION 3.12)
project(gmpxx_mkII VERSION 2.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compiler flags to avoid GMP/C++ standard library conflicts
add_compile_definitions(__STDC_LIMIT_MACROS __STDC_CONSTANT_MACROS)

# Find GMP (prefer custom build if available)
# Check multiple possible locations
set(GMP_SEARCH_PATHS
    "/home/docker/i/GMP-6.3.0"
    "${CMAKE_SOURCE_DIR}/i/GMP-6.3.0"
    "/home/docker/gmpxx_mkII/i/GMP-6.3.0"
    "$ENV{HOME}/i/GMP-6.3.0"
)

set(CUSTOM_GMP_PATH "")
foreach(path ${GMP_SEARCH_PATHS})
    if(EXISTS "${path}")
        set(CUSTOM_GMP_PATH "${path}")
        break()
    endif()
endforeach()

if(CUSTOM_GMP_PATH)
    message(STATUS "Using custom GMP: ${CUSTOM_GMP_PATH}")
    # Manually set paths for custom GMP
    set(GMP_INCLUDE_DIRS "${CUSTOM_GMP_PATH}/include")
    set(GMP_LIBRARY_DIRS "${CUSTOM_GMP_PATH}/lib")
    set(GMP_CFLAGS "-I${CUSTOM_GMP_PATH}/include")
    set(GMP_FOUND TRUE)
    # Set full library paths
    set(GMP_LIBRARIES "${CUSTOM_GMP_PATH}/lib/libgmp.so;${CUSTOM_GMP_PATH}/lib/libgmpxx.so")
else()
    message(STATUS "Custom GMP not found, using system GMP")
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GMP REQUIRED gmp gmpxx)
endif()

# Debug: Print GMP variables
message(STATUS "GMP_FOUND: ${GMP_FOUND}")
message(STATUS "GMP_INCLUDE_DIRS: ${GMP_INCLUDE_DIRS}")
message(STATUS "GMP_CFLAGS: ${GMP_CFLAGS}")
message(STATUS "GMP_LIBRARIES: ${GMP_LIBRARIES}")
message(STATUS "GMP_LIBRARY_DIRS: ${GMP_LIBRARY_DIRS}")

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
    message(STATUS "Git commit hash: ${GIT_COMMIT_HASH}")
else()
    set(GIT_COMMIT_HASH "unknown")
    message(STATUS "Git not found or not a git repository, using: ${GIT_COMMIT_HASH}")
endif()

# Configure header file
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/gmpxx_mkII.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/gmpxx_mkII.h"
    @ONLY
)

# Print configured values for debugging
message(STATUS "Configured header with:")
message(STATUS "  VERSION: ${PROJECT_VERSION}")
message(STATUS "  COMMIT_HASH: ${GIT_COMMIT_HASH}")

# Header-only library
add_library(gmpxx_mkII INTERFACE)
target_include_directories(gmpxx_mkII INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<INSTALL_INTERFACE:include>
)

# Apply GMP settings properly (use full paths to avoid directory issues)
target_compile_options(gmpxx_mkII INTERFACE ${GMP_CFLAGS})
target_link_libraries(gmpxx_mkII INTERFACE ${GMP_LIBRARIES})

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
