# Test executables
add_executable(test_basic test_basic.cpp)

# Link with gmpxx_mkII interface library
target_link_libraries(test_basic PRIVATE gmpxx_mkII)

# Add test
add_test(NAME test_basic COMMAND test_basic)

# Set test properties
set_tests_properties(test_basic PROPERTIES
    TIMEOUT 30
    PASS_REGULAR_EXPRESSION "All tests passed"
)
