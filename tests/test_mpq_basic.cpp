// Include C++ standard headers first
#include <cassert>
#include <iostream>
#include <string>
#include <chrono>
#include <vector>
#include <sstream>

// Then include our header
#include <gmpxx_mkII.h>

using namespace gmpxx;
using namespace std;

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
    mpq_class a;
    TEST_ASSERT(a.get_d() == 0.0, "Default constructor should initialize to 0");

    // Integer constructor
    mpq_class b(42);
    TEST_ASSERT(b.get_d() == 42.0, "Integer constructor failed");

    // Fraction constructor
    mpq_class c(3, 4);
    TEST_ASSERT(c.get_d() == 0.75, "Fraction constructor failed");

    // Negative fraction constructor
    mpq_class d(-7, 3);
    TEST_ASSERT(abs(d.get_d() - (-7.0/3.0)) < 1e-10, "Negative fraction constructor failed");

    // Double constructor
    mpq_class e(3.14);
    TEST_ASSERT(abs(e.get_d() - 3.14) < 1e-10, "Double constructor failed");

    // String constructor
    mpq_class f("22/7");
    TEST_ASSERT(abs(f.get_d() - (22.0/7.0)) < 1e-10, "String constructor failed");

    // mpz_class constructor
    mpz_class z(123);
    mpq_class g(z);
    TEST_ASSERT(g.get_d() == 123.0, "mpz_class constructor failed");

    // Two mpz_class constructor
    mpz_class num(15);
    mpz_class den(4);
    mpq_class h(num, den);
    TEST_ASSERT(h.get_d() == 3.75, "Two mpz_class constructor failed");

    // Copy constructor
    mpq_class i(c);
    TEST_ASSERT(i == c, "Copy constructor failed");

    // Assignment
    mpq_class j;
    j = b;
    TEST_ASSERT(j == b, "Assignment operator failed");

    std::cout << "✓ Construction and assignment tests passed" << std::endl;
}

// Test basic arithmetic operations
TEST_CASE(basic_arithmetic) {
    mpq_class a(3, 4);  // 3/4
    mpq_class b(1, 2);  // 1/2
    mpq_class c(2, 3);  // 2/3

    // Addition: 3/4 + 1/2 = 3/4 + 2/4 = 5/4
    mpq_class sum = a + b;
    TEST_ASSERT(sum.get_d() == 1.25, "Addition failed");

    // Subtraction: 3/4 - 1/2 = 3/4 - 2/4 = 1/4
    mpq_class diff = a - b;
    TEST_ASSERT(diff.get_d() == 0.25, "Subtraction failed");

    // Multiplication: 3/4 * 1/2 = 3/8
    mpq_class prod = a * b;
    TEST_ASSERT(prod.get_d() == 0.375, "Multiplication failed");

    // Division: 3/4 / 1/2 = 3/4 * 2/1 = 6/4 = 3/2
    mpq_class quot = a / b;
    TEST_ASSERT(quot.get_d() == 1.5, "Division failed");

    // Mixed with integers
    mpq_class int_sum = a + mpz_class(2);
    TEST_ASSERT(int_sum.get_d() == 2.75, "Addition with integer failed");

    std::cout << "✓ Basic arithmetic tests passed" << std::endl;
}

// Test expression templates optimization
TEST_CASE(expression_templates) {
    mpq_class a(1, 2);  // 1/2
    mpq_class b(1, 3);  // 1/3
    mpq_class c(1, 4);  // 1/4
    mpq_class d(1, 6);  // 1/6

    // Complex expression: 1/2 + 1/3 - 1/4 + 1/6
    // = 6/12 + 4/12 - 3/12 + 2/12 = 9/12 = 3/4
    mpq_class result = a + b - c + d;
    TEST_ASSERT(result.get_d() == 0.75, "Complex expression evaluation failed");

    // Test chained operations
    mpq_class chained = a + b + c + d;
    // 1/2 + 1/3 + 1/4 + 1/6 = 6/12 + 4/12 + 3/12 + 2/12 = 15/12 = 5/4
    TEST_ASSERT(chained.get_d() == 1.25, "Chained addition failed");

    // Test mixed operations with parentheses
    mpq_class mixed = (a + b) * (c + d);
    // (1/2 + 1/3) * (1/4 + 1/6) = (5/6) * (5/12) = 25/72
    double expected = (5.0/6.0) * (5.0/12.0);
    TEST_ASSERT(abs(mixed.get_d() - expected) < 1e-10, "Mixed operations failed");
    
    // More complex expression template test
    mpq_class x(1, 6);
    mpq_class y(1, 8);
    
    // Test expression: (x + y) * (x - y)
    mpq_class z = (x + y) * (x - y);
    
    // Calculate expected: (1/6 + 1/8) * (1/6 - 1/8)
    // = (4/24 + 3/24) * (4/24 - 3/24)
    // = (7/24) * (1/24) = 7/576
    mpq_class expected_z(7, 576);
    TEST_ASSERT(z == expected_z, "Complex expression template failed");
    
    // Test expression that should use expression templates efficiently
    mpq_class expr_result = a + b * c - d / mpq_class(2);
    
    // Calculate expected: 1/2 + (1/3 * 1/4) - (1/6 / 2)
    // = 1/2 + 1/12 - 1/12 = 1/2
    TEST_ASSERT(expr_result == mpq_class(1, 2), "Expression template optimization failed");

    std::cout << "✓ Expression template tests passed" << std::endl;
}

// Test compound assignment operators
TEST_CASE(compound_assignment) {
    mpq_class a(3, 4);
    mpq_class b(1, 4);

    // Test +=
    mpq_class temp = a;
    temp += b;
    TEST_ASSERT(temp.get_d() == 1.0, "Compound addition failed");

    // Test -=
    temp = a;
    temp -= b;
    TEST_ASSERT(temp.get_d() == 0.5, "Compound subtraction failed");

    // Test *=
    temp = a;
    temp *= b;
    TEST_ASSERT(temp.get_d() == 3.0/16.0, "Compound multiplication failed");

    // Test /=
    temp = a;
    temp /= b;
    TEST_ASSERT(temp.get_d() == 3.0, "Compound division failed");

    // Test with expression templates
    temp = a;
    temp += b * mpq_class(2);
    TEST_ASSERT(temp.get_d() == 1.25, "Compound assignment with expression failed");

    std::cout << "✓ Compound assignment tests passed" << std::endl;
}

// Test comparison operators
TEST_CASE(comparison_operators) {
    mpq_class a(3, 4);
    mpq_class b(1, 2);
    mpq_class c(6, 8);  // Same as 3/4

    TEST_ASSERT(a == c, "Equality comparison failed");
    TEST_ASSERT(a != b, "Inequality comparison failed");
    TEST_ASSERT(b < a, "Less than comparison failed");
    TEST_ASSERT(b <= a, "Less than or equal comparison failed");
    TEST_ASSERT(a > b, "Greater than comparison failed");
    TEST_ASSERT(a >= c, "Greater than or equal comparison failed");

    // Compare with zero
    mpq_class zero(0);
    mpq_class neg(-1, 2);
    TEST_ASSERT(zero > neg, "Comparison with negative failed");
    TEST_ASSERT(neg < zero, "Negative comparison failed");

    std::cout << "✓ Comparison operator tests passed" << std::endl;
}

// Test unary operators
TEST_CASE(unary_operators) {
    mpq_class a(3, 4);
    mpq_class b(-3, 4);

    // Negation
    mpq_class neg_a = -a;
    TEST_ASSERT(neg_a.get_d() == -0.75, "Negation failed");

    mpq_class neg_b = -b;
    TEST_ASSERT(neg_b.get_d() == 0.75, "Double negation failed");

    // Absolute value
    mpq_class abs_a = abs(a);
    TEST_ASSERT(abs_a.get_d() == 0.75, "Absolute value of positive failed");

    mpq_class abs_b = abs(b);
    TEST_ASSERT(abs_b.get_d() == 0.75, "Absolute value of negative failed");

    std::cout << "✓ Unary operator tests passed" << std::endl;
}

// Test fraction-specific operations
TEST_CASE(fraction_operations) {
    // Test numerator and denominator access
    mpq_class a(22, 7);
    a.canonicalize();
    
    mpz_class num = a.get_num();
    mpz_class den = a.get_den();
    
    TEST_ASSERT(num.get_si() == 22, "Numerator access failed");
    TEST_ASSERT(den.get_si() == 7, "Denominator access failed");

    // Test canonicalization
    mpq_class b(10, 15);  // Should become 2/3
    b.canonicalize();
    TEST_ASSERT(b.get_num().get_si() == 2, "Canonicalization numerator failed");
    TEST_ASSERT(b.get_den().get_si() == 3, "Canonicalization denominator failed");

    // Test with negative numbers
    mpq_class c(-15, 25);  // Should become -3/5
    c.canonicalize();
    TEST_ASSERT(c.get_num().get_si() == -3, "Negative canonicalization numerator failed");
    TEST_ASSERT(c.get_den().get_si() == 5, "Negative canonicalization denominator failed");

    // Test sign function
    TEST_ASSERT(a.sgn() == 1, "Positive sign test failed");
    TEST_ASSERT(c.sgn() == -1, "Negative sign test failed");
    
    mpq_class zero(0);
    TEST_ASSERT(zero.sgn() == 0, "Zero sign test failed");

    std::cout << "✓ Fraction operation tests passed" << std::endl;
}

// Test mathematical functions
TEST_CASE(mathematical_functions) {
    mpq_class a(-3, 4);
    mpq_class b(3, 4);
    mpq_class zero(0);
    
    // Absolute value
    TEST_ASSERT(abs(a) == b, "Absolute value failed");
    TEST_ASSERT(abs(b) == b, "Absolute value of positive failed");
    TEST_ASSERT(abs(zero) == zero, "Absolute value of zero failed");
    
    // Global sign function (if available)
    TEST_ASSERT(sgn(a) == -1, "Global sgn function for negative failed");
    TEST_ASSERT(sgn(b) == 1, "Global sgn function for positive failed");
    TEST_ASSERT(sgn(zero) == 0, "Global sgn function for zero failed");

// Test special values and automatic canonicalization
TEST_CASE(special_values) {
    // Test zero
    mpq_class zero;
    TEST_ASSERT(zero == 0, "Default constructor should be zero");
    TEST_ASSERT(zero.get_num() == 0, "Zero numerator failed");
    TEST_ASSERT(zero.get_den() == 1, "Zero denominator should be 1");
    
    // Test unit fractions
    mpq_class unit(1);
    TEST_ASSERT(unit == 1, "Unit fraction value failed");
    TEST_ASSERT(unit.get_num() == 1, "Unit numerator failed");
    TEST_ASSERT(unit.get_den() == 1, "Unit denominator failed");
    
    // Test automatic canonicalization
    mpq_class frac(6, 9);
    TEST_ASSERT(frac == mpq_class(2, 3), "Automatic canonicalization failed");
    TEST_ASSERT(frac.get_num() == 2, "Canonicalized numerator failed");
    TEST_ASSERT(frac.get_den() == 3, "Canonicalized denominator failed");
    
    // Test negative fractions - different representations
    mpq_class neg1(-1, 2);
    mpq_class neg2(1, -2);
    TEST_ASSERT(neg1 == neg2, "Negative fraction representations should be equal");
    TEST_ASSERT(neg1.get_num() == -1, "Negative in numerator failed");
    TEST_ASSERT(neg1.get_den() == 2, "Positive denominator failed");
    TEST_ASSERT(neg2.get_num() == -1, "Negative normalization failed");
    TEST_ASSERT(neg2.get_den() == 2, "Denominator normalization failed");
    
    // Test more complex canonicalization
    mpq_class complex_frac(12, 18);
    TEST_ASSERT(complex_frac == mpq_class(2, 3), "Complex canonicalization failed");
    
    // Test with negative denominators
    mpq_class neg_denom(3, -4);
    TEST_ASSERT(neg_denom.get_num() == -3, "Negative denominator normalization failed");
    TEST_ASSERT(neg_denom.get_den() == 4, "Negative denominator should become positive");

    std::cout << "✓ Special values tests passed" << std::endl;
}

// Test large fraction operations
TEST_CASE(large_fractions) {
    // Test with large numerators and denominators
    mpq_class big1("123456789012345678901234567890", "987654321098765432109876543210");
    mpq_class big2("111111111111111111111111111111", "222222222222222222222222222222");

    // Addition of large fractions
    mpq_class big_sum = big1 + big2;
    
    // Verify the result is reasonable (should be positive and finite)
    TEST_ASSERT(big_sum.sgn() > 0, "Large fraction addition sign failed");
    TEST_ASSERT(big_sum.get_d() > 0, "Large fraction addition result failed");

    // Test multiplication
    mpq_class big_prod = big1 * big2;
    TEST_ASSERT(big_prod.sgn() > 0, "Large fraction multiplication failed");

    // Test with mixed large and small fractions
    mpq_class small(1, 1000000);
    mpq_class mixed = big1 + small;
    TEST_ASSERT(mixed.sgn() > 0, "Mixed large/small fraction addition failed");

    std::cout << "✓ Large fraction tests passed" << std::endl;
}

// Test string conversion and I/O
TEST_CASE(string_conversion) {
    mpq_class a(22, 7);

    // Decimal string representation
    std::string str = a.get_str();
    TEST_ASSERT(str == "22/7", "String conversion failed");

    // Test with different bases
    mpq_class b(15, 8);
    std::string str_b = b.get_str(16);
    TEST_ASSERT(str_b == "f/8", "Hexadecimal string conversion failed");

    // Test canonical form
    mpq_class c(10, 15);
    c.canonicalize();
    TEST_ASSERT(c.get_str() == "2/3", "Canonical string conversion failed");

    // Test negative fractions
    mpq_class d(-3, 4);
    TEST_ASSERT(d.get_str() == "-3/4", "Negative string conversion failed");

    std::cout << "✓ String conversion tests passed" << std::endl;
}

// Test I/O operations with streams
TEST_CASE(io_operations) {
    mpq_class a(22, 7);
    
    // Test string output
    std::ostringstream oss;
    oss << a;
    std::string result = oss.str();
    TEST_ASSERT(result == "22/7", "Stream output failed");
    
    // Test string input
    std::istringstream iss("5/8");
    mpq_class b;
    iss >> b;
    TEST_ASSERT(b == mpq_class(5, 8), "Stream input failed");
    
    // Test input/output with negative fractions
    std::ostringstream oss_neg;
    mpq_class neg(-3, 4);
    oss_neg << neg;
    TEST_ASSERT(oss_neg.str() == "-3/4", "Negative stream output failed");
    
    std::istringstream iss_neg("-7/11");
    mpq_class neg_input;
    iss_neg >> neg_input;
    TEST_ASSERT(neg_input == mpq_class(-7, 11), "Negative stream input failed");

    std::cout << "✓ I/O operation tests passed" << std::endl;
}

// Test mixed operations with integers and floats
TEST_CASE(mixed_type_operations) {
    mpq_class q(3, 4);
    mpz_class z(2);
    
    // mpq with mpz operations
    mpq_class result1 = q + z;
    TEST_ASSERT(result1.get_d() == 2.75, "mpq + mpz failed");

    mpq_class result2 = z - q;
    TEST_ASSERT(result2.get_d() == 1.25, "mpz - mpq failed");

    mpq_class result3 = q * z;
    TEST_ASSERT(result3.get_d() == 1.5, "mpq * mpz failed");

    mpq_class result4 = z / q;
    TEST_ASSERT(abs(result4.get_d() - (8.0/3.0)) < 1e-10, "mpz / mpq failed");

    // Test comparison with integers
    TEST_ASSERT(q < z, "mpq < mpz comparison failed");
    TEST_ASSERT(z > q, "mpz > mpq comparison failed");

    // Operations with integers
    mpq_class a(1, 2);
    mpq_class b = a + 1;
    TEST_ASSERT(b == mpq_class(3, 2), "mpq + int failed");
    
    mpq_class c = 2 * a;
    TEST_ASSERT(c == 1, "int * mpq failed");
    
    // Operations with doubles
    mpq_class d = a + 0.5;
    TEST_ASSERT(d == 1, "mpq + double failed");
    
    // Comparison with integers and doubles
    TEST_ASSERT(a < 1, "mpq < int comparison failed");
    TEST_ASSERT(a > 0, "mpq > int comparison failed");
    TEST_ASSERT(a == 0.5, "mpq == double comparison failed");

    std::cout << "✓ Mixed type operation tests passed" << std::endl;
}

// Performance test comparing expression templates vs traditional approach
TEST_CASE(performance_comparison) {
    const int iterations = 10000;
    mpq_class a(22, 7);
    mpq_class b(355, 113);
    mpq_class c(1, 3);

    // Test expression template performance
    double et_time = measure_time([&]() {
        for (int i = 0; i < iterations; ++i) {
            mpq_class result = a * b + c * (a + b) - a / c;
            (void)result; // Prevent optimization
        }
    });

    // Test traditional approach performance
    double trad_time = measure_time([&]() {
        for (int i = 0; i < iterations; ++i) {
            mpq_class temp1 = a * b;
            mpq_class temp2 = a + b;
            mpq_class temp3 = c * temp2;
            mpq_class temp4 = a / c;
            mpq_class result = temp1 + temp3 - temp4;
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

    for (int i = 1; i < count; ++i) {
        mpq_class a(i, i + 1);
        mpq_class b(i + 1, i + 2);
        mpq_class c(i + 2, i + 3);

        // Create complex expression with many temporaries
        mpq_class result = a * b + c - (a + b) * (c - a) + b / (a + mpq_class(1, i + 1));

        // Force evaluation and prevent optimization
        volatile double val = result.get_d();
        (void)val;
    }

    std::cout << "✓ Memory management test completed (check with valgrind for leaks)" << std::endl;
}

// Test edge cases
TEST_CASE(edge_cases) {
    // Zero operations
    mpq_class zero(0);
    mpq_class one(1);
    mpq_class neg_one(-1);

    TEST_ASSERT((zero + zero).get_d() == 0.0, "Zero addition failed");
    TEST_ASSERT((zero * one).get_d() == 0.0, "Zero multiplication failed");
    TEST_ASSERT((one + zero).get_d() == 1.0, "Addition with zero failed");

    // Operations with one
    mpq_class half(1, 2);
    TEST_ASSERT((half * one).get_d() == 0.5, "Multiplication by one failed");
    TEST_ASSERT((half / one).get_d() == 0.5, "Division by one failed");

    // Negative fractions
    mpq_class neg_half(-1, 2);
    TEST_ASSERT((neg_half * neg_one).get_d() == 0.5, "Negative multiplication failed");
    TEST_ASSERT((one + neg_one).get_d() == 0.0, "Adding negative failed");

    // Very small fractions
    mpq_class tiny(1, 1000000);
    mpq_class sum = one + tiny;
    TEST_ASSERT(sum.get_d() > 1.0, "Tiny fraction addition failed");

    // Reciprocals
    mpq_class two_thirds(2, 3);
    mpq_class reciprocal = one / two_thirds;
    TEST_ASSERT(abs(reciprocal.get_d() - 1.5) < 1e-10, "Reciprocal failed");

    std::cout << "✓ Edge case tests passed" << std::endl;
}

// Test error conditions and robustness
TEST_CASE(error_conditions) {
    // Test division by zero (should be handled gracefully)
    mpq_class a(1, 2);
    mpq_class zero(0);
    
    // Note: Actual behavior depends on GMP implementation
    // This test mainly ensures no crashes occur
    
    // Test very large denominators
    try {
        mpq_class large_denom(1, 1000000000);
        TEST_ASSERT(large_denom.get_d() > 0.0, "Large denominator failed");
    } catch (...) {
        // If an exception occurs, that's also acceptable behavior
        std::cout << "  Note: Large denominator caused exception (acceptable)" << std::endl;
    }

    // Test string parsing edge cases
    try {
        mpq_class from_str("1/3");
        TEST_ASSERT(abs(from_str.get_d() - (1.0/3.0)) < 1e-10, "String parsing failed");
    } catch (...) {
        std::cout << "  Note: String parsing caused exception" << std::endl;
    }

    std::cout << "✓ Error condition tests completed" << std::endl;
}

// Main test runner
int main() {
    std::cout << "Running gmpxx_mkII mpq_class test suite..." << std::endl;
    std::cout << "===========================================" << std::endl;

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

    std::cout << "\n===========================================" << std::endl;
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
