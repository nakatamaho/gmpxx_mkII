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
add_gmpxx_mkii_test(test_construction_copy test_construction_copy.cpp)
add_gmpxx_mkii_test(test_mpf_math_functions test_mpf_math_functions.cpp)
add_gmpxx_mkii_test(test_mpf_transcendent_functions
    test_mpf_transcendent_functions.cpp)
add_gmpxx_mkii_test(test_mpf_extended_transcendent_functions
    test_mpf_extended_transcendent_functions.cpp)
add_gmpxx_mkii_test(test_type_conversions test_type_conversions.cpp)
add_gmpxx_mkii_test(test_numeric_equivalence test_numeric_equivalence.cpp)
add_gmpxx_mkii_test(test_alloc_count test_alloc_count.cpp)
add_gmpxx_mkii_test(test_alias_safety test_alias_safety.cpp)
add_gmpxx_mkii_test(test_thread_safety test_thread_safety.cpp)
add_gmpxx_mkii_test(test_scalar_arithmetic test_scalar_arithmetic.cpp)
add_gmpxx_mkii_test(test_scalar_alloc_count test_scalar_alloc_count.cpp)
add_gmpxx_mkii_test(test_compound_assign test_compound_assign.cpp)
add_gmpxx_mkii_test(test_long_width_dispatch test_long_width_dispatch.cpp)
add_gmpxx_mkii_test(test_long_width_dispatch_llp64 test_long_width_dispatch.cpp)
add_gmpxx_mkii_test(test_precision_policy test_precision_policy.cpp)
add_gmpxx_mkii_test(test_unary_minus_simplification test_unary_minus_simplification.cpp)
add_gmpxx_mkii_test(test_power_of_two_fusion test_power_of_two_fusion.cpp)
add_gmpxx_mkii_test(test_mpz_arithmetic test_mpz_arithmetic.cpp)
add_gmpxx_mkii_test(test_mpq_arithmetic test_mpq_arithmetic.cpp)
add_gmpxx_mkii_test(test_mixed_type_arithmetic test_mixed_type_arithmetic.cpp)
add_gmpxx_mkii_test(test_mpz_mpq_alloc_count test_mpz_mpq_alloc_count.cpp)
add_gmpxx_mkii_test(test_mpz_addmul_fusion test_mpz_addmul_fusion.cpp)
add_gmpxx_mkii_test(test_mpz_addmul_alloc_count test_mpz_addmul_alloc_count.cpp)
add_gmpxx_mkii_test(test_mpz_addmul_alloc_count_llp64
    test_mpz_addmul_alloc_count.cpp)
add_gmpxx_mkii_test(test_comparisons test_comparisons.cpp)
add_gmpxx_mkii_test(test_io_and_strings test_io_and_strings.cpp)
add_gmpxx_mkii_test(test_user_defined_literals test_user_defined_literals.cpp)
add_gmpxx_mkii_test(test_defaults_policy test_defaults_policy.cpp)
add_gmpxx_mkii_test(test_random test_random.cpp)
add_gmpxx_mkii_test(test_gmpxx_mkII test_gmpxx_mkII.cpp)

target_link_libraries(test_thread_safety PRIVATE Threads::Threads)
target_compile_definitions(test_long_width_dispatch_llp64
    PRIVATE GMPXX_MKII_TEST_LLP64_PATH)
target_compile_definitions(test_mpz_mpq_alloc_count
    PRIVATE GMPXX_MKII_INSTRUMENT_WRAPPERS)
target_compile_definitions(test_mpz_addmul_fusion
    PRIVATE GMPXX_MKII_TEST_FUSION_COUNTERS)
target_compile_definitions(test_mpz_addmul_alloc_count
    PRIVATE GMPXX_MKII_INSTRUMENT_WRAPPERS
            GMPXX_MKII_TEST_FUSION_COUNTERS)
target_compile_definitions(test_mpz_addmul_alloc_count_llp64
    PRIVATE GMPXX_MKII_INSTRUMENT_WRAPPERS
            GMPXX_MKII_TEST_FUSION_COUNTERS
            GMPXX_MKII_TEST_LLP64_PATH)

set_tests_properties(test_alloc_count PROPERTIES RUN_SERIAL TRUE LABELS alloc-count)
set_tests_properties(test_scalar_alloc_count PROPERTIES RUN_SERIAL TRUE LABELS alloc-count)
set_tests_properties(test_mpz_mpq_alloc_count PROPERTIES RUN_SERIAL TRUE LABELS alloc-count)
set_tests_properties(test_mpz_addmul_alloc_count PROPERTIES RUN_SERIAL TRUE LABELS alloc-count)
set_tests_properties(test_mpz_addmul_alloc_count_llp64 PROPERTIES RUN_SERIAL TRUE LABELS alloc-count)
set_tests_properties(test_io_and_strings PROPERTIES RUN_SERIAL TRUE LABELS alloc-count)

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/package_smoke/run_package_smoke.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/run_package_smoke.cmake"
    @ONLY)
add_test(NAME test_package_config
         COMMAND "${CMAKE_COMMAND}" -P
                 "${CMAKE_CURRENT_BINARY_DIR}/run_package_smoke.cmake")
set_tests_properties(test_package_config PROPERTIES RUN_SERIAL TRUE LABELS package)
