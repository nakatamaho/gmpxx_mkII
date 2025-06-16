// Include C++ standard headers first
#include <cassert>
#include <iostream>
#include <string>
#include <chrono>
#include <vector>

// Then include our header
#include <gmpxx_mkII.h>

using namespace gmpxx;

// Test framework macros
#define TEST_ASSERT(condition, message)                                \
    do {                                                               \
        if (!(condition)) {                                            \
            std::cerr << "ASSERTION FAILED: " << message << std::endl; \
            std::cerr << "  File: " << __FILE__ << std::endl;          \
            std::cerr << "  Line: " << __LINE__ << std::endl;          \
            std::exit(1);                                              \
        }                                                              \
    } while (0)

#define TEST_CASE(name)                                                               \
    void test_##name();                                                               \
    static bool register_##name = (test_cases.push_back({#name, test_##name}), true); \
    void test_##name()

// Test case registry
struct TestCase {
    std::string name;
    void (*function)();
};

std::vector<TestCase> test_cases;

// Utility function to measure execution time
template <typename F> double measure_time(F &&func) {
    auto start = std::chrono::high_resolution_clock::now();
    func();
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    return duration.count() / 1000.0; // Return milliseconds
}

// Test basic construction and assignment
TEST_CASE(construction_and_assignment) {
    // Default constructor
    mpz_class a;
    TEST_ASSERT(a.get_si() == 0, "Default constructor should initialize to 0");

    // Integer constructors
    mpz_class b(42);
    TEST_ASSERT(b.get_si() == 42, "Integer constructor failed");

    mpz_class c(-123);
    TEST_ASSERT(c.get_si() == -123, "Negative integer constructor failed");

    // String constructor
    mpz_class d("123456789012345678901234567890");
    TEST_ASSERT(d.get_str() == "123456789012345678901234567890", "String constructor failed");

    // Hex string constructor
    mpz_class e("DEADBEEF", 16);
    TEST_ASSERT(e.get_si() == 0xDEADBEEF, "Hex string constructor failed");

    // Copy constructor
    mpz_class f(d);
    TEST_ASSERT(f == d, "Copy constructor failed");

    // Assignment
    mpz_class g;
    g = b;
    TEST_ASSERT(g == b, "Assignment operator failed");

    std::cout << "✓ Construction and assignment tests passed" << std::endl;
}

// Test basic arithmetic operations
TEST_CASE(basic_arithmetic) {
    mpz_class a(100);
    mpz_class b(25);
    mpz_class c(4);

    // Addition
    mpz_class sum = a + b;
    TEST_ASSERT(sum.get_si() == 125, "Addition failed");

    // Subtraction
    mpz_class diff = a - b;
    TEST_ASSERT(diff.get_si() == 75, "Subtraction failed");

    // Multiplication
    mpz_class prod = a * b;
    TEST_ASSERT(prod.get_si() == 2500, "Multiplication failed");

    // Division
    mpz_class quot = a / c;
    TEST_ASSERT(quot.get_si() == 25, "Division failed");

    // Modulo
    mpz_class mod = a % b;
    TEST_ASSERT(mod.get_si() == 0, "Modulo failed");

    std::cout << "✓ Basic arithmetic tests passed" << std::endl;
}

// Test expression templates optimization
TEST_CASE(expression_templates) {
    mpz_class a(10);
    mpz_class b(20);
    mpz_class c(30);
    mpz_class d(40);

    // Complex expression that should be optimized
    mpz_class result = a * b + c * d - a / (b - a);

    // Manual calculation: 10*20 + 30*40 - 10/(20-10) = 200 + 1200 - 1 = 1399
    TEST_ASSERT(result.get_si() == 1399, "Complex expression evaluation failed");

    // Test chained operations
    mpz_class chained = a + b + c + d;
    TEST_ASSERT(chained.get_si() == 100, "Chained addition failed");

    // Test mixed operations
    mpz_class mixed = (a + b) * (c - d);
    TEST_ASSERT(mixed.get_si() == -300, "Mixed operations failed");

    std::cout << "✓ Expression template tests passed" << std::endl;
}

// Test compound assignment operators
TEST_CASE(compound_assignment) {
    mpz_class a(100);
    mpz_class b(25);

    // Test +=
    mpz_class temp = a;
    temp += b;
    TEST_ASSERT(temp.get_si() == 125, "Compound addition failed");

    // Test -=
    temp = a;
    temp -= b;
    TEST_ASSERT(temp.get_si() == 75, "Compound subtraction failed");

    // Test *=
    temp = a;
    temp *= b;
    TEST_ASSERT(temp.get_si() == 2500, "Compound multiplication failed");

    // Test /=
    temp = a;
    temp /= b;
    TEST_ASSERT(temp.get_si() == 4, "Compound division failed");

    // Test with expression templates
    temp = a;
    temp += b * mpz_class(2);
    TEST_ASSERT(temp.get_si() == 150, "Compound assignment with expression failed");

    std::cout << "✓ Compound assignment tests passed" << std::endl;
}

// Test comparison operators
TEST_CASE(comparison_operators) {
    mpz_class a(100);
    mpz_class b(200);
    mpz_class c(100);

    TEST_ASSERT(a == c, "Equality comparison failed");
    TEST_ASSERT(a != b, "Inequality comparison failed");
    TEST_ASSERT(a < b, "Less than comparison failed");
    TEST_ASSERT(a <= c, "Less than or equal comparison failed");
    TEST_ASSERT(b > a, "Greater than comparison failed");
    TEST_ASSERT(b >= a, "Greater than or equal comparison failed");

    std::cout << "✓ Comparison operator tests passed" << std::endl;
}

// Test unary operators
TEST_CASE(unary_operators) {
    mpz_class a(42);
    mpz_class b(-42);

    // Negation
    mpz_class neg_a = -a;
    TEST_ASSERT(neg_a.get_si() == -42, "Negation failed");

    mpz_class neg_b = -b;
    TEST_ASSERT(neg_b.get_si() == 42, "Double negation failed");

    // Absolute value
    mpz_class abs_a = abs(a);
    TEST_ASSERT(abs_a.get_si() == 42, "Absolute value of positive failed");

    mpz_class abs_b = abs(b);
    TEST_ASSERT(abs_b.get_si() == 42, "Absolute value of negative failed");

    std::cout << "✓ Unary operator tests passed" << std::endl;
}

// Test large number operations
TEST_CASE(large_numbers) {
    // Test with numbers larger than machine integers
    mpz_class big1("123456789012345678901234567890");
    mpz_class big2("987654321098765432109876543210");

    // Addition of large numbers
    mpz_class big_sum = big1 + big2;
    std::string expected_sum = "1111111110111111111011111111100";
    TEST_ASSERT(big_sum.get_str() == expected_sum, "Large number addition failed");

    // Multiplication of large numbers
    mpz_class big_prod = big1 * mpz_class(2L);
    std::string expected_prod = "246913578024691357802469135780";
    TEST_ASSERT(big_prod.get_str() == expected_prod, "Large number multiplication failed");

    std::cout << "✓ Large number tests passed" << std::endl;
}

// Test string conversion and I/O
TEST_CASE(string_conversion) {
    mpz_class a(255);

    // Decimal
    TEST_ASSERT(a.get_str(10) == "255", "Decimal string conversion failed");

    // Hexadecimal
    TEST_ASSERT(a.get_str(16) == "ff", "Hexadecimal string conversion failed");

    // Binary
    TEST_ASSERT(a.get_str(2) == "11111111", "Binary string conversion failed");

    // Octal
    TEST_ASSERT(a.get_str(8) == "377", "Octal string conversion failed");

    std::cout << "✓ String conversion tests passed" << std::endl;
}

// Performance test comparing expression templates vs traditional approach
TEST_CASE(performance_comparison) {
    const int iterations = 10000;
    mpz_class a("12345678901234567890");
    mpz_class b("98765432109876543210");
    mpz_class c("11111111111111111111");

    // Test expression template performance
    double et_time = measure_time([&]() {
        for (int i = 0; i < iterations; ++i) {
            mpz_class result = a * b + c * (a + b) - a / c;
            (void)result; // Prevent optimization
        }
    });

    // Test traditional approach performance
    double trad_time = measure_time([&]() {
        for (int i = 0; i < iterations; ++i) {
            mpz_class temp1 = a * b;
            mpz_class temp2 = a + b;
            mpz_class temp3 = c * temp2;
            mpz_class temp4 = a / c;
            mpz_class result = temp1 + temp3 - temp4;
            (void)result; // Prevent optimization
        }
    });

    std::cout << "✓ Performance test completed:" << std::endl;
    std::cout << "  Expression templates: " << et_time << " ms" << std::endl;
    std::cout << "  Traditional approach: " << trad_time << " ms" << std::endl;

    if (et_time < trad_time) {
        double speedup = trad_time / et_time;
        std::cout << "  Speedup: " << speedup << "x faster" << std::endl;
    }
}

// Test memory usage and leak detection
TEST_CASE(memory_management) {
    // This test creates many temporary objects to stress test memory management
    const int count = 1000;

    for (int i = 0; i < count; ++i) {
        mpz_class a(static_cast<long>(i));
        mpz_class b(static_cast<long>(i * 2));
        mpz_class c(static_cast<long>(i * 3));

        // Create complex expression with many temporaries
        mpz_class result = a * b + c - (a + b) * (c - a) + b / (a + mpz_class(1L));

        // Force evaluation and prevent optimization
        volatile long val = result.get_si();
        (void)val;
    }

    std::cout << "✓ Memory management test completed (check with valgrind for leaks)" << std::endl;
}

// Test edge cases
TEST_CASE(edge_cases) {
    // Zero operations
    mpz_class zero(0L);
    mpz_class one(1L);
    mpz_class neg_one(-1L);

    TEST_ASSERT(mpz_class(zero + zero).get_si() == 0, "Zero addition failed");
    TEST_ASSERT(mpz_class(zero * one).get_si() == 0, "Zero multiplication failed");
    TEST_ASSERT(mpz_class(one + zero).get_si() == 1, "Addition with zero failed");

    // Negative numbers
    TEST_ASSERT(mpz_class(neg_one * neg_one).get_si() == 1, "Negative multiplication failed");
    TEST_ASSERT(mpz_class(one + neg_one).get_si() == 0, "Adding negative failed");

    // Large negative numbers
    mpz_class big_neg("-123456789012345678901234567890");
    mpz_class abs_big = abs(big_neg);
    TEST_ASSERT(abs_big.get_str() == "123456789012345678901234567890", "Large negative abs failed");

    std::cout << "✓ Edge case tests passed" << std::endl;
}

// Main test runner
int main() {
    std::cout << "Running gmpxx_mkII test suite..." << std::endl;
    std::cout << "=================================" << std::endl;

    // Print version information
    gmpxx::print_version_info();
    std::cout << std::endl;

    int passed = 0;
    int failed = 0;

    for (const auto &test : test_cases) {
        try {
            std::cout << "\nRunning test: " << test.name << std::endl;
            test.function();
            passed++;
        } catch (const std::exception &e) {
            std::cerr << "✗ Test " << test.name << " failed with exception: " << e.what() << std::endl;
            failed++;
        } catch (...) {
            std::cerr << "✗ Test " << test.name << " failed with unknown exception" << std::endl;
            failed++;
        }
    }

    std::cout << "\n=================================" << std::endl;
    std::cout << "Test Results:" << std::endl;
    std::cout << "  Passed: " << passed << std::endl;
    std::cout << "  Failed: " << failed << std::endl;
    std::cout << "  Total:  " << (passed + failed) << std::endl;

    if (failed == 0) {
        std::cout << "\n🎉 All tests passed!" << std::endl;
        return 0;
    } else {
        std::cout << "\n❌ Some tests failed!" << std::endl;
        return 1;
    }
}
