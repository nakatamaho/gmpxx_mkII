cmake_minimum_required(VERSION 3.20)

project(gmpxx_mkII VERSION 2.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(GMPXX_MKII_BUILD_TESTS "Build tests" ON)
option(GMPXX_MKII_NOPRECCHANGE
       "Use thread-local default_prec only (P3) instead of operand max (P2)" OFF)

find_library(GMP_LIBRARY NAMES gmp libgmp REQUIRED HINTS ${GMP_ROOT})
find_path(GMP_INCLUDE_DIR NAMES gmp.h REQUIRED HINTS ${GMP_ROOT})
include(GNUInstallDirs)

if(NOT TARGET GMP::GMP)
    add_library(GMP::GMP UNKNOWN IMPORTED)
    set_target_properties(GMP::GMP PROPERTIES
        IMPORTED_LOCATION "${GMP_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${GMP_INCLUDE_DIR}")
endif()

execute_process(
    COMMAND git rev-parse --short HEAD
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    OUTPUT_VARIABLE GIT_COMMIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET)
if(NOT GIT_COMMIT_HASH)
    set(GIT_COMMIT_HASH "unknown")
endif()

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/include/gmpxx_mkII.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/gmpxx_mkII.h"
    @ONLY)

add_library(gmpxx_mkII INTERFACE)
add_library(gmpxx_mkII::gmpxx_mkII ALIAS gmpxx_mkII)
target_compile_features(gmpxx_mkII INTERFACE cxx_std_20)
target_include_directories(gmpxx_mkII INTERFACE
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")
target_link_libraries(gmpxx_mkII INTERFACE GMP::GMP)

if(GMPXX_MKII_NOPRECCHANGE)
    target_compile_definitions(gmpxx_mkII INTERFACE GMPXX_MKII_NOPRECCHANGE)
endif()

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/gmpxx_mkII.h"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
install(TARGETS gmpxx_mkII
        EXPORT gmpxx_mkIITargets
        INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
install(EXPORT gmpxx_mkIITargets
        NAMESPACE gmpxx_mkII::
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/gmpxx_mkII")
# A full gmpxx_mkIIConfig.cmake package is deferred to Phase 5.

include(CTest)
if(GMPXX_MKII_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
