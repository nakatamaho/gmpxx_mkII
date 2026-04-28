# SPDX-License-Identifier: BSD-2-Clause

cmake_minimum_required(VERSION 3.20)

project(gmpxx_mkII_package_smoke LANGUAGES CXX)

find_package(gmpxx_mkII CONFIG REQUIRED)

add_executable(package_smoke main.cpp)
target_link_libraries(package_smoke PRIVATE gmpxx_mkII::gmpxx_mkII)

enable_testing()
add_test(NAME package_smoke COMMAND package_smoke)
