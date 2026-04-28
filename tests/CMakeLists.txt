# SPDX-License-Identifier: BSD-2-Clause

find_package(Threads REQUIRED)

function(add_gmpxx_mkii_test name)
    add_executable(${name} ${ARGN})
    target_link_libraries(${name} PRIVATE gmpxx_mkII::gmpxx_mkII)
    if(MSVC)
        target_compile_options(${name} PRIVATE /W4)
    else()
        target_compile_options(${name} PRIVATE -Wall -Wextra -Wpedantic)
    endif()
    add_test(NAME ${name} COMMAND ${name})
endfunction()

add_gmpxx_mkii_test(test_abi_fingerprint test_abi_fingerprint.cpp)
add_gmpxx_mkii_test(test_numeric_equivalence test_numeric_equivalence.cpp)
add_gmpxx_mkii_test(test_alloc_count test_alloc_count.cpp)
add_gmpxx_mkii_test(test_alias_safety test_alias_safety.cpp)
add_gmpxx_mkii_test(test_thread_safety test_thread_safety.cpp)
add_gmpxx_mkii_test(test_scalar_arithmetic test_scalar_arithmetic.cpp)
add_gmpxx_mkii_test(test_scalar_alloc_count test_scalar_alloc_count.cpp)
add_gmpxx_mkii_test(test_compound_assign test_compound_assign.cpp)
add_gmpxx_mkii_test(test_long_width_dispatch test_long_width_dispatch.cpp)
add_gmpxx_mkii_test(test_long_width_dispatch_llp64 test_long_width_dispatch.cpp)

target_link_libraries(test_thread_safety PRIVATE Threads::Threads)
target_compile_definitions(test_long_width_dispatch_llp64
    PRIVATE GMPXX_MKII_TEST_LLP64_PATH)

set_tests_properties(test_alloc_count PROPERTIES RUN_SERIAL TRUE LABELS alloc-count)
set_tests_properties(test_scalar_alloc_count PROPERTIES RUN_SERIAL TRUE LABELS alloc-count)
