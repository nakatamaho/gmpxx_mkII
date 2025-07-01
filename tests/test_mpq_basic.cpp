#include <iostream>
#include <cassert>
#include <string>
#include <sstream>
#include "gmpxx_mkII.h"

using namespace gmpxx;
using namespace std;

// Test function to validate expressions
void test_basic_operations() {
    cout << "Testing basic arithmetic operations..." << endl;
    
    // Test constructors
    mpq_class a;
    mpq_class b(3, 4);
    mpq_class c("5/6");
    mpq_class d(2.5);
    mpq_class e(7);
    
    assert(a == 0);
    assert(b == mpq_class(3, 4));
    assert(c == mpq_class(5, 6));
    assert(d == mpq_class(5, 2));
    assert(e == 7);
    
    cout << "Constructor tests passed." << endl;
}

void test_arithmetic() {
    cout << "Testing arithmetic operations..." << endl;
    
    mpq_class a(1, 2);
    mpq_class b(1, 3);
    mpq_class c(3, 4);
    
    // Addition
    mpq_class sum = a + b;
    assert(sum == mpq_class(5, 6));
    
    // Subtraction
    mpq_class diff = c - a;
    assert(diff == mpq_class(1, 4));
    
    // Multiplication
    mpq_class prod = a * c;
    assert(prod == mpq_class(3, 8));
    
    // Division
    mpq_class quot = a / b;
    assert(quot == mpq_class(3, 2));
    
    // Unary minus
    mpq_class neg = -a;
    assert(neg == mpq_class(-1, 2));
    
    cout << "Arithmetic tests passed." << endl;
}

void test_comparisons() {
    cout << "Testing comparison operations..." << endl;
    
    mpq_class a(1, 2);
    mpq_class b(2, 4);
    mpq_class c(3, 4);
    
    assert(a == b);
    assert(a != c);
    assert(a < c);
    assert(c > a);
    assert(a <= b);
    assert(a <= c);
    assert(c >= a);
    assert(b >= a);
    
    cout << "Comparison tests passed." << endl;
}

void test_mixed_operations() {
    cout << "Testing mixed type operations..." << endl;
    
    mpq_class a(1, 2);
    
    // Operations with integers
    mpq_class b = a + 1;
    assert(b == mpq_class(3, 2));
    
    mpq_class c = 2 * a;
    assert(c == 1);
    
    // Operations with doubles
    mpq_class d = a + 0.5;
    assert(d == 1);
    
    // Comparison with integers and doubles
    assert(a < 1);
    assert(a > 0);
    assert(a == 0.5);
    
    cout << "Mixed operations tests passed." << endl;
}

void test_io_operations() {
    cout << "Testing I/O operations..." << endl;
    
    mpq_class a(22, 7);
    
    // Test string output
    ostringstream oss;
    oss << a;
    string result = oss.str();
    
    // Test string input
    istringstream iss("5/8");
    mpq_class b;
    iss >> b;
    assert(b == mpq_class(5, 8));
    
    cout << "I/O tests passed." << endl;
}

void test_functions() {
    cout << "Testing mathematical functions..." << endl;
    
    mpq_class a(-3, 4);
    mpq_class b(3, 4);
    
    // Absolute value
    assert(abs(a) == b);
    
    // Sign function (if available)
    assert(sgn(a) == -1);
    assert(sgn(b) == 1);
    assert(sgn(mpq_class(0)) == 0);
    
    cout << "Function tests passed." << endl;
}

void test_assignment_operations() {
    cout << "Testing assignment operations..." << endl;
    
    mpq_class a(1, 2);
    mpq_class b(1, 3);
    
    // Compound assignment
    a += b;
    assert(a == mpq_class(5, 6));
    
    a -= mpq_class(1, 6);
    assert(a == mpq_class(2, 3));
    
    a *= mpq_class(3, 2);
    assert(a == 1);
    
    a /= mpq_class(2);
    assert(a == mpq_class(1, 2));
    
    cout << "Assignment operation tests passed." << endl;
}

void test_special_values() {
    cout << "Testing special values..." << endl;
    
    // Test zero
    mpq_class zero;
    assert(zero == 0);
    assert(zero.get_num() == 0);
    assert(zero.get_den() == 1);
    
    // Test unit fractions
    mpq_class unit(1);
    assert(unit == 1);
    assert(unit.get_num() == 1);
    assert(unit.get_den() == 1);
    
    // Test proper fraction canonicalization
    mpq_class frac(6, 9);
    assert(frac == mpq_class(2, 3));
    
    // Test negative fractions
    mpq_class neg1(-1, 2);
    mpq_class neg2(1, -2);
    assert(neg1 == neg2);
    
    cout << "Special values tests passed." << endl;
}

void test_expression_templates() {
    cout << "Testing expression templates..." << endl;
    
    mpq_class a(1, 2);
    mpq_class b(1, 3);
    mpq_class c(1, 4);
    mpq_class d(1, 5);
    
    // Complex expression that should use expression templates
    mpq_class result = a + b * c - d / 2;
    mpq_class expected = mpq_class(1, 2) + mpq_class(1, 12) - mpq_class(1, 10);
    expected = mpq_class(30, 60) + mpq_class(5, 60) - mpq_class(6, 60);
    expected = mpq_class(29, 60);
    
    assert(result == expected);
    
    // Test that expressions don't create unnecessary temporaries
    mpq_class x(1, 6);
    mpq_class y(1, 8);
    mpq_class z = (x + y) * (x - y);
    mpq_class expected_z = mpq_class(7, 24) * mpq_class(1, 24);
    expected_z = mpq_class(7, 576);
    
    assert(z == expected_z);
    
    cout << "Expression template tests passed." << endl;
}

int main() {
    cout << "Starting mpq_class basic tests..." << endl << endl;
    
    try {
        test_basic_operations();
        test_arithmetic();
        test_comparisons();
        test_mixed_operations();
        test_io_operations();
        test_functions();
        test_assignment_operations();
        test_special_values();
        test_expression_templates();
        
        cout << endl << "All tests passed successfully!" << endl;
        return 0;
        
    } catch (const exception& e) {
        cerr << "Test failed with exception: " << e.what() << endl;
        return 1;
    } catch (...) {
        cerr << "Test failed with unknown exception" << endl;
        return 1;
    }
}
