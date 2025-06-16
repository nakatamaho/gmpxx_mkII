// Include C++ standard headers first
#include <cassert>
#include <iostream>
#include <string>
#include <chrono>
#include <vector>
#include <cmath>
#include <iomanip>

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

// Utility function for floating point comparison with tolerance
bool almost_equal(const mpf_class& a, const mpf_class& b, const mpf_class& tolerance = mpf_class("1e-50")) {
    return abs(a - b) <= tolerance;
}

bool almost_equal(const mpf_class& a, double b, double tolerance = 1e-10) {
    return std::abs(a.get_d() - b) <= tolerance;
}

// Test basic construction and assignment
TEST_CASE(construction_and_assignment) {
    // Default constructor
    mpf_class a;
    TEST_ASSERT(a.get_d() == 0.0, "Default constructor should initialize to 0.0");

    // Double constructors
    mpf_class b(3.14159);
    TEST_ASSERT(almost_equal(b, 3.14159), "Double constructor failed");

    mpf_class c(-2.71828);
    TEST_ASSERT(almost_equal(c, -2.71828), "Negative double constructor failed");

    // Integer constructors
    mpf_class d(42);
    TEST_ASSERT(almost_equal(d, 42.0), "Integer constructor failed");

    // String constructor
    mpf_class e("3.141592653589793238462643383279502884197169399375105820974944592307816406286208998628034825342117067");
    TEST_ASSERT(e.get_str(10, 0, 50).find("3.141592653589793238462643383279502884197169399375105820974944592307816406286208998628034825342117067") != std::string::npos, 
                "High precision string constructor failed");

    // Precision constructor
    mpf_class f(3.14159, 256);
    TEST_ASSERT(f.get_prec() == 256, "Precision constructor failed");

    // Copy constructor
    mpf_class g(e);
    TEST_ASSERT(almost_equal(g, e), "Copy constructor failed");

    // Assignment
    mpf_class h;
    h = b;
    TEST_ASSERT(almost_equal(h, b), "Assignment operator failed");

    std::cout << "✓ Construction and assignment tests passed" << std::endl;
}

// Test basic arithmetic operations
TEST_CASE(basic_arithmetic) {
    mpf_class a(100.5);
    mpf_class b(25.25);
    mpf_class c(4.0);

    // Addition
    mpf_class sum = a + b;
    TEST_ASSERT(almost_equal(sum, 125.75), "Addition failed");

    // Subtraction
    mpf_class diff = a - b;
    TEST_ASSERT(almost_equal(diff, 75.25), "Subtraction failed");

    // Multiplication
    mpf_class prod = a * b;
    TEST_ASSERT(almost_equal(prod, 2537.625), "Multiplication failed");

    // Division
    mpf_class quot = a / c;
    TEST_ASSERT(almost_equal(quot, 25.125), "Division failed");

    // Mixed operations
    mpf_class mixed = (a + b) / c;
    TEST_ASSERT(almost_equal(mixed, 31.4375), "Mixed operations failed");

    std::cout << "✓ Basic arithmetic tests passed" << std::endl;
}

// Test expression templates optimization
TEST_CASE(expression_templates) {
    mpf_class a(10.5);
    mpf_class b(20.25);
    mpf_class c(30.75);
    mpf_class d(40.125);

    // Complex expression that should be optimized
    mpf_class result = a * b + c * d - a / (b - a);
    
    // Manual calculation: 10.5*20.25 + 30.75*40.125 - 10.5/(20.25-10.5)
    // = 212.625 + 1233.84375 - 1.076923...
    // = 1445.392826...
    double expected = 10.5 * 20.25 + 30.75 * 40.125 - 10.5 / (20.25 - 10.5);
    TEST_ASSERT(almost_equal(result, expected, 1e-6), "Complex expression evaluation failed");

    // Test chained operations
    mpf_class chained = a + b + c + d;
    TEST_ASSERT(almost_equal(chained, 101.625), "Chained addition failed");

    // Test mixed operations with precedence
    mpf_class mixed = (a + b) * (c - d);
    double expected_mixed = (10.5 + 20.25) * (30.75 - 40.125);
    TEST_ASSERT(almost_equal(mixed, expected_mixed), "Mixed operations with precedence failed");

    std::cout << "✓ Expression template tests passed" << std::endl;
}

// Test compound assignment operators
TEST_CASE(compound_assignment) {
    mpf_class a(100.5);
    mpf_class b(25.25);

    // Test +=
    mpf_class temp = a;
    temp += b;
    TEST_ASSERT(almost_equal(temp, 125.75), "Compound addition failed");

    // Test -=
    temp = a;
    temp -= b;
    TEST_ASSERT(almost_equal(temp, 75.25), "Compound subtraction failed");

    // Test *=
    temp = a;
    temp *= b;
    TEST_ASSERT(almost_equal(temp, 2537.625), "Compound multiplication failed");

    // Test /=
    temp = a;
    temp /= b;
    TEST_ASSERT(almost_equal(temp, 100.5 / 25.25), "Compound division failed");

    // Test with expression templates
    temp = a;
    temp += b * mpf_class(2.0);
    TEST_ASSERT(almost_equal(temp, 151.0), "Compound assignment with expression failed");

    std::cout << "✓ Compound assignment tests passed" << std::endl;
}

// Test comparison operators
TEST_CASE(comparison_operators) {
    mpf_class a(100.5);
    mpf_class b(200.25);
    mpf_class c(100.5);
    mpf_class d(100.50000000000001); // Very close but not exactly equal

    TEST_ASSERT(a == c, "Equality comparison failed");
    TEST_ASSERT(a != b, "Inequality comparison failed");
    TEST_ASSERT(a < b, "Less than comparison failed");
    TEST_ASSERT(a <= c, "Less than or equal comparison failed");
    TEST_ASSERT(b > a, "Greater than comparison failed");
    TEST_ASSERT(b >= a, "Greater than or equal comparison failed");

    // Test with high precision
    mpf_class precise1("3.141592653589793238462643383279502884197169399375105820974944592307816406286208998628034825342117067");
    mpf_class precise2("3.141592653589793238462643383279502884197169399375105820974944592307816406286208998628034825342117068");
    TEST_ASSERT(precise1 != precise2, "High precision comparison failed");

    std::cout << "✓ Comparison operator tests passed" << std::endl;
}

// Test unary operators
TEST_CASE(unary_operators) {
    mpf_class a(42.5);
    mpf_class b(-42.5);

    // Negation
    mpf_class neg_a = -a;
    TEST_ASSERT(almost_equal(neg_a, -42.5), "Negation failed");

    mpf_class neg_b = -b;
    TEST_ASSERT(almost_equal(neg_b, 42.5), "Double negation failed");

    // Absolute value
    mpf_class abs_a = abs(a);
    TEST_ASSERT(almost_equal(abs_a, 42.5), "Absolute value of positive failed");

    mpf_class abs_b = abs(b);
    TEST_ASSERT(almost_equal(abs_b, 42.5), "Absolute value of negative failed");

    std::cout << "✓ Unary operator tests passed" << std::endl;
}

// Test precision management
TEST_CASE(precision_management) {
    // Test default precision
    mp_bitcnt_t default_prec = mpf_get_default_prec();
    mpf_class a(3.14159);
    TEST_ASSERT(a.get_prec() == default_prec, "Default precision not applied");

    // Test custom precision
    mpf_class b(3.14159, 256);
    TEST_ASSERT(b.get_prec() == 256, "Custom precision not set");

    // Test precision in operations
    mpf_class c(2.71828, 512);
    mpf_class result = b + c;
    TEST_ASSERT(result.get_prec() == 512, "Precision not properly propagated");

    // Test very high precision
    mpf_class high_prec("3.141592653589793238462643383279502884197169399375105820974944592307816406286208998628034825342117067", 1024);
    TEST_ASSERT(high_prec.get_prec() == 1024, "High precision not set");

    std::cout << "✓ Precision management tests passed" << std::endl;
}

// Test mathematical functions
TEST_CASE(mathematical_functions) {
    mpf_class x(2.0);
    mpf_class y(0.5);

    // Square root
    mpf_class sqrt_x = sqrt(x);
    TEST_ASSERT(almost_equal(sqrt_x, std::sqrt(2.0), 1e-10), "Square root failed");

    // Powers
    mpf_class pow_result = pow(x, y);
    TEST_ASSERT(almost_equal(pow_result, std::pow(2.0, 0.5), 1e-10), "Power function failed");

    // Trigonometric functions
    mpf_class angle(1.0);
    mpf_class sin_result = sin(angle);
    TEST_ASSERT(almost_equal(sin_result, std::sin(1.0), 1e-10), "Sine function failed");

    mpf_class cos_result = cos(angle);
    TEST_ASSERT(almost_equal(cos_result, std::cos(1.0), 1e-10), "Cosine function failed");

    // Logarithms
    mpf_class log_result = log(x);
    TEST_ASSERT(almost_equal(log_result, std::log(2.0), 1e-10), "Natural logarithm failed");

    // Exponential
    mpf_class exp_result = exp(mpf_class(1.0));
    TEST_ASSERT(almost_equal(exp_result, std::exp(1.0), 1e-10), "Exponential function failed");

    // Test mathematical constants
    mpf_class pi = const_pi();
    TEST_ASSERT(almost_equal(pi, M_PI, 1e-10), "Pi constant failed");

    std::cout << "✓ Mathematical functions tests passed" << std::endl;
}

// Test string conversion and formatting
TEST_CASE(string_conversion) {
    mpf_class a(255.75);

    // Test basic string conversion
    std::string str = a.get_str(10, 0, 10);
    TEST_ASSERT(str.find("255.75") != std::string::npos, "Basic string conversion failed");

    // Test scientific notation
    mpf_class large(1.23456e20);
    std::string sci_str = large.get_str(10, 0, 5);
    TEST_ASSERT(!sci_str.empty(), "Scientific notation conversion failed");

    // Test high precision string
    mpf_class precise("3.141592653589793238462643383279502884197169399375105820974944592307816406286208998628034825342117067", 1024);
    std::string prec_str = precise.get_str(10, 0, 50);
    TEST_ASSERT(prec_str.find("3.141592653589793238462643383279502884197169399375105820974944592307816406286208998628034825342117067") != std::string::npos, 
                "High precision string conversion failed");

    std::cout << "✓ String conversion tests passed" << std::endl;
}

// Test high precision calculations
TEST_CASE(high_precision_calculations) {
    // Set high precision for this test
    mp_bitcnt_t old_prec = mpf_get_default_prec();
    mpf_set_default_prec(1024);

    // Calculate pi using Machin's formula: pi = 16*arctan(1/5) - 4*arctan(1/239)
    mpf_class pi_calc = 16 * atan(mpf_class(1.0) / mpf_class(5.0)) - 4 * atan(mpf_class(1.0) / mpf_class(239.0));
    mpf_class pi_const = const_pi();
    
    // Should be accurate to many decimal places
    mpf_class diff = abs(pi_calc - pi_const);
    TEST_ASSERT(diff < mpf_class("1e-100"), "High precision pi calculation failed");

    // Test Euler's identity: e^(i*pi) + 1 = 0
    // Since we don't have complex numbers, test e^pi vs pi^e
    mpf_class e_const = exp(mpf_class(1.0));
    mpf_class e_to_pi = pow(e_const, pi_const);
    mpf_class pi_to_e = pow(pi_const, e_const);
    
    // These should be different values
    TEST_ASSERT(abs(e_to_pi - pi_to_e) > mpf_class("1.0"), "e^pi vs pi^e comparison failed");

    // Restore original precision
    mpf_set_default_prec(old_prec);

    std::cout << "✓ High precision calculation tests passed" << std::endl;
}

// Performance test comparing expression templates vs traditional approach
TEST_CASE(performance_comparison) {
    const int iterations = 1000;
    mpf_class a("1.23456789");
    mpf_class b("9.87654321");
    mpf_class c("5.55555555");

    // Test expression template performance
    double et_time = measure_time([&]() {
        for (int i = 0; i < iterations; ++i) {
            mpf_class result = a * b + c * sqrt(a + b) - log(a) / exp(c);
            (void)result; // Prevent optimization
        }
    });

    // Test traditional approach performance
    double trad_time = measure_time([&]() {
        for (int i = 0; i < iterations; ++i) {
            mpf_class temp1 = a * b;
            mpf_class temp2 = a + b;
            mpf_class temp3 = sqrt(temp2);
            mpf_class temp4 = c * temp3;
            mpf_class temp5 = log(a);
            mpf_class temp6 = exp(c);
            mpf_class temp7 = temp5 / temp6;
            mpf_class result = temp1 + temp4 - temp7;
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
        mpf_class a(static_cast<double>(i) + 0.5);
        mpf_class b(static_cast<double>(i) * 2.5);
        mpf_class c(static_cast<double>(i) * 3.14159);

        // Create complex expression with many temporaries
        mpf_class result = sqrt(a * b) + log(c) - exp(a / (b + mpf_class(1.0))) + pow(c, mpf_class(0.5));

        // Force evaluation and prevent optimization
        volatile double val = result.get_d();
        (void)val;
    }

    std::cout << "✓ Memory management test completed (check with valgrind for leaks)" << std::endl;
}

// Test edge cases and special values
TEST_CASE(edge_cases) {
    // Zero operations
    mpf_class zero(0.0);
    mpf_class one(1.0);
    mpf_class neg_one(-1.0);

    TEST_ASSERT(almost_equal(zero + zero, 0.0), "Zero addition failed");
    TEST_ASSERT(almost_equal(zero * one, 0.0), "Zero multiplication failed");
    TEST_ASSERT(almost_equal(one + zero, 1.0), "Addition with zero failed");

    // Negative numbers
    TEST_ASSERT(almost_equal(neg_one * neg_one, 1.0), "Negative multiplication failed");
    TEST_ASSERT(almost_equal(one + neg_one, 0.0), "Adding negative failed");

    // Very small numbers
    mpf_class tiny("1e-100");
    mpf_class tiny_result = tiny + tiny;
    TEST_ASSERT(almost_equal(tiny_result, mpf_class("2e-100")), "Very small number addition failed");

    // Very large numbers
    mpf_class huge("1e100");
    mpf_class huge_result = huge * mpf_class(2.0);
    TEST_ASSERT(almost_equal(huge_result, mpf_class("2e100")), "Very large number multiplication failed");

    // Division by very small number
    mpf_class div_result = one / tiny;
    TEST_ASSERT(almost_equal(div_result, mpf_class("1e100")), "Division by very small number failed");

    // Mathematical function edge cases
    TEST_ASSERT(almost_equal(sqrt(zero), 0.0), "Square root of zero failed");
    TEST_ASSERT(almost_equal(sqrt(one), 1.0), "Square root of one failed");
    TEST_ASSERT(almost_equal(log(one), 0.0, 1e-50), "Log of one failed");
    TEST_ASSERT(almost_equal(exp(zero), 1.0), "Exp of zero failed");

    std::cout << "✓ Edge case tests passed" << std::endl;
}

// Test numerical stability
TEST_CASE(numerical_stability) {
    // Test for catastrophic cancellation
    mpf_class large("1e20");
    mpf_class small("1.0");
    mpf_class sum = large + small;
    mpf_class diff = sum - large;
    
    // Should recover the small value
    TEST_ASSERT(almost_equal(diff, small), "Catastrophic cancellation test failed");

    // Test precision in iterative calculation
    mpf_class sum_iter(0.0);
    mpf_class increment("0.1");
    const int iterations = 10;
    
    for (int i = 0; i < iterations; ++i) {
        sum_iter += increment;
    }
    
    mpf_class expected = mpf_class(iterations) * increment;
    TEST_ASSERT(almost_equal(sum_iter, expected), "Iterative calculation precision failed");

    // Test associativity in floating point
    mpf_class a("0.1");
    mpf_class b("0.2");
    mpf_class c("0.3");
    
    mpf_class left_assoc = (a + b) + c;
    mpf_class right_assoc = a + (b + c);
    
    // Should be equal within reasonable precision
    TEST_ASSERT(almost_equal(left_assoc, right_assoc), "Associativity test failed");

    std::cout << "✓ Numerical stability tests passed" << std::endl;
}

// Main test runner
int main() {
    std::cout << "Running gmpxx_mkII MPF test suite..." << std::endl;
    std::cout << "====================================" << std::endl;

    // Print version information
    gmpxx::print_version_info();
    std::cout << std::endl;

    // Set a reasonable default precision for testing
    mp_bitcnt_t original_prec = mpf_get_default_prec();
    mpf_set_default_prec(512);

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

    // Restore original precision
    mpf_set_default_prec(original_prec);

    std::cout << "\n====================================" << std::endl;
    std::cout << "Test Results:" << std::endl;
    std::cout << "  Passed: " << passed << std::endl;
    std::cout << "  Failed: " << failed << std::endl;
    std::cout << "  Total:  " << (passed + failed) << std::endl;

    if (failed == 0) {
        std::cout << "\n🎉 All MPF tests passed!" << std::endl;
        return 0;
    } else {
        std::cout << "\n❌ Some MPF tests failed!" << std::endl;
        return 1;
    }
}
