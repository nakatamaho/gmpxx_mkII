# Test executables
add_executable(test_mpz_basic test_mpz_basic.cpp)
# Link with gmpxx_mkII interface library
target_link_libraries(test_mpz_basic PRIVATE gmpxx_mkII)
# Add test
add_test(NAME test_basic COMMAND test_basic)
# Set test properties
set_tests_properties(test_basic PROPERTIES
    TIMEOUT 30
    PASS_REGULAR_EXPRESSION "All tests passed"
)

# MPF Test executable
add_executable(test_mpf_basic test_mpf_basic.cpp)
# Link with gmpxx_mkII interface library
target_link_libraries(test_mpf_basic PRIVATE gmpxx_mkII)
# Add test
add_test(NAME test_mpf_basic COMMAND test_mpf_basic)
# Set test properties
set_tests_properties(test_mpf_basic PROPERTIES
    TIMEOUT 60
    PASS_REGULAR_EXPRESSION "All MPF tests passed"
)
