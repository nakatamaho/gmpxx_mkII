# MPZ Test executables
add_executable(test_mpz_basic test_mpz_basic.cpp)
# Link with gmpxx_mkII interface library
target_link_libraries(test_mpz_basic PRIVATE gmpxx_mkII)
# Add test
add_test(NAME test_mpz_basic COMMAND test_mpz_basic)
# Set test properties
set_tests_properties(test_mpz_basic PROPERTIES
    TIMEOUT 30
    PASS_REGULAR_EXPRESSION "All MPZ tests passed"
)

# MPQ Test executables
add_executable(test_mpq_basic test_mpq_basic.cpp)
# Link with gmpxx_mkII interface library
target_link_libraries(test_mpq_basic PRIVATE gmpxx_mkII)
# Add test
add_test(NAME test_mpq_basic COMMAND test_mpq_basic)
# Set test properties
set_tests_properties(test_mpq_basic PROPERTIES
    TIMEOUT 30
    PASS_REGULAR_EXPRESSION "All MPQ tests passed"
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
