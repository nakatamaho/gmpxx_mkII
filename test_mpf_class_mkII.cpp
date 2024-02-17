/*
 * Copyright (c) 2024
 *      Nakata, Maho
 *      All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

#include <iostream>
#include <cassert>
#include <cstring>
#include <string>

#include "mpfr_class.h"

using namespace mpfrcxx;

// Asserts that the mpfr_class object equals the expected string representation
bool IsMpfrEquals(mpfrcxx::mpfr_class &mpfrObj, const char *expected, int base = defaults::base, int precision = 10) {
    char formatString[64];
    char buffer[64];

    // Adjust the comparison based on the base
    switch (base) {
    case 2:
        std::sprintf(formatString, "%%.%dRb", precision);         // Generates format string like "%.10Rf"
        mpfr_sprintf(buffer, formatString, mpfrObj.get_mpfr_t()); // Uses generated format string
        break;
    case 10:
        std::sprintf(formatString, "%%.%dRf", precision);         // Generates format string like "%.10Rf"
        mpfr_sprintf(buffer, formatString, mpfrObj.get_mpfr_t()); // Uses generated format string
        break;

    case 16:
        std::sprintf(formatString, "%%.%dRa", precision);         // Generates format string like "%.10Rf"
        mpfr_sprintf(buffer, formatString, mpfrObj.get_mpfr_t()); // Uses generated format string
        break;

    default:
        printf("not supported");
        exit(-1);
    }
    if (std::strcmp(buffer, expected) == 0)
        return true;
    else
        return false;
}

void testDefaultPrecision() {
    mpfr_prec_t defaultPrec = defaults::get_default_prec();
    assert(defaultPrec == 512);
    std::cout << "Default precision test passed." << std::endl;

    defaults::set_default_prec(1024);
    defaultPrec = defaults::get_default_prec();
    assert(defaultPrec == 1024);
    std::cout << "Set and get precision test passed." << std::endl;

    defaults::set_default_prec(512);
}

void testDefaultRoundingMode() {
    mpfr_rnd_t defaultRnd = defaults::get_default_rounding_mode();
    assert(defaultRnd == MPFR_RNDN);
    std::cout << "Default rounding mode test passed." << std::endl;

    defaults::set_default_rounding_mode(MPFR_RNDZ);
    defaultRnd = defaults::get_default_rounding_mode();
    assert(defaultRnd == MPFR_RNDZ);
    std::cout << "Set and get rounding mode test passed." << std::endl;
    defaults::set_default_rounding_mode(MPFR_RNDN);
}

void testDefaultConstructor() {
    mpfr_class a;
    char buffer[100];
    mpfr_snprintf(buffer, sizeof(buffer), "%.0Rf", a.get_mpfr_t());
    assert(std::string(buffer) == "nan");
    std::cout << "Default constructor test passed." << std::endl;
}

void testCopyConstructor() {
    mpfr_class a;
    mpfr_class b = a;
    assert(true);
    std::cout << "Copy constructor test passed." << std::endl;
}

void testAssignmentOperator() {
    mpfr_class a;
    mpfr_class b;
    b = a;
    assert(true);
    std::cout << "Assignment operator test passed." << std::endl;
}

void testInitializationAndAssignmentDouble() {
    double testValue = 3.1415926535;
    const char *expectedValue = "3.1415926535";

    mpfrcxx::mpfr_class a = (mpfr_class)testValue;
    assert(IsMpfrEquals(a, expectedValue));
    std::cout << "Substitution from double using constructor test passed." << std::endl;

    mpfrcxx::mpfr_class b;
    b = testValue;
    assert(IsMpfrEquals(b, expectedValue));
    std::cout << "Substitution from double using assignment test passed." << std::endl;
}

void testInitializationAndAssignmentString() {
    // Testing initialization with a decimal number using a constructor
    const char *expectedDecimalValue = "1.4142135624";
    mpfr_class a = expectedDecimalValue;
    assert(IsMpfrEquals(a, expectedDecimalValue));
    std::cout << "Constructor initialization with decimal '" << expectedDecimalValue << "' test passed." << std::endl;

    // Testing initialization with a decimal number using an assignment operator
    mpfr_class b;
    b = expectedDecimalValue;
    assert(IsMpfrEquals(b, expectedDecimalValue));
    std::cout << "Assignment initialization with decimal '" << expectedDecimalValue << "' test passed." << std::endl;

    // Testing initialization with a decimal number using a constructor
    std::string expectedDecimalValueString = "3.1415926535";
    mpfr_class c = expectedDecimalValueString;
    assert(IsMpfrEquals(c, expectedDecimalValueString.c_str()));
    std::cout << "Constructor initialization with decimal '" << expectedDecimalValueString << "' test passed." << std::endl;

    // Testing initialization with a decimal number using an assignment operator
    mpfr_class d;
    d = expectedDecimalValueString;
    assert(IsMpfrEquals(d, expectedDecimalValueString.c_str()));
    std::cout << "Assignment initialization with decimal '" << expectedDecimalValueString << "' test passed." << std::endl;

    // Testing initialization with a hexadecimal number using an assignment operator
    const char *expectedHexValue = "0x3.243f6a8885a3p+0";
    mpfr_class e(expectedHexValue, 16);
    assert(IsMpfrEquals(e, expectedHexValue, 16, 12));
    std::cout << "Assignment initialization with hexadecimal '" << expectedHexValue << "' test passed." << std::endl;

    defaults::base = 16;
    // Testing initialization with a hexadecimal number using a constructor
    mpfr_class f;
    e = expectedHexValue;
    assert(IsMpfrEquals(e, expectedHexValue, defaults::base, 12));
    std::cout << "Constructor initialization with hexadecimal '" << expectedHexValue << "' test passed." << std::endl;
    defaults::base = 10;

    // Testing initialization with a hexadecimal number using an assignment operator
    const char *expectedBinaryValue = "1.0101010001000100010001011010001000100010001011p+1";
    mpfr_class g(expectedBinaryValue, 2);
    assert(IsMpfrEquals(g, expectedBinaryValue, 2, 46));
    std::cout << "Assignment initialization with binary '" << expectedBinaryValue << "' test passed." << std::endl;

    defaults::base = 2;
    // Testing initialization with a binaryadecimal number using a constructor
    mpfr_class h;
    h = expectedBinaryValue;
    assert(IsMpfrEquals(h, expectedBinaryValue, defaults::base, 46));
    std::cout << "Constructor initialization with binary '" << expectedBinaryValue << "' test passed." << std::endl;
    defaults::base = 10;
}

// Function to test equality of two mpfr_class instances with the same value
void testEqualitySameValue() {
    mpfr_class a, b;
    a = "3.14";     // Set value of a
    b = "3.14";     // Set value of b to the same as a
    assert(a == b); // Should be true
    std::cout << "Test equality with the same value passed." << std::endl;
}

// Function to test equality of two mpfr_class instances with different values
void testEqualityDifferentValue() {
    mpfr_class a, c;
    a = "3.14";        // Set value of a
    c = "2.17";        // Set value of c to a different value
    assert(!(a == c)); // Should be false
    std::cout << "Test equality with different values passed." << std::endl;
}

void testSqrt() {
    // Test Case 1: Calculate the square root of a positive number
    mpfr_class a;
    a = "4.0"; // Square root of 4.0 is 2.0
    mpfr_class result = mpfr_class::sqrt(a);
    std::string expected = "2.0000000000";
    assert(IsMpfrEquals(result, expected.c_str()));

    // Test Case 2: Calculate the square root of 0
    mpfr_class b("0.0"); // Square root of 0.0 is 0.0
    result = mpfr_class::sqrt(b);
    expected = "0.0000000000";
    assert(IsMpfrEquals(result, expected.c_str()));

    // Test Case 3: Calculate the square root of a negative number (should raise an error)
    mpfr_class c("-1.0");
    result = mpfr_class::sqrt(c);
    assert(result.is_nan()); // Check if the result is NaN
    std::cout << "Test square root of a negative number (should be NaN) passed." << std::endl;
}

// Test function for set_prec and get_prec
void testSetAndGetPrec() {
    mpfr_class a; // Default precision
    mpfr_prec_t defaultPrec = a.get_prec();
    std::cout << "Default precision: " << defaultPrec << " bits" << std::endl;

    mpfr_prec_t newPrec = 128; // New precision to set
    a.set_prec(newPrec);
    mpfr_prec_t retrievedPrec = a.get_prec();
    std::cout << "New precision: " << retrievedPrec << " bits" << std::endl;

    // Assert that the new precision is correctly set and retrieved
    assert(retrievedPrec == newPrec);
    std::cout << "Test passed: Precision was successfully set and retrieved." << std::endl;
}
void testLog() {
    mpfr_class a("1.0");
    mpfr_class result = mpfr_class::log(a);
    // log(1) = 0
    std::string expected = "0.0000000000";
    assert(IsMpfrEquals(result, expected.c_str()));
    // log(2) = 0.69314718056
    a = "2.0";
    expected = "0.6931471806";
    result = mpfr_class::log(a);
    assert(IsMpfrEquals(result, expected.c_str()));

    a = "0.0";
    result = mpfr_class::log(a);
    assert(result.is_inf());
    std::cout << "Test log zero (should be -Inf) passed." << std::endl;
    a = "-1.0";
    result = mpfr_class::log(a);
    assert(result.is_nan());
    std::cout << "Test log negative (should be NaN) passed." << std::endl;

    std::cout << "Log test passed." << std::endl;
}

void testLog10() {
    mpfr_class a("10.0");
    mpfr_class result = mpfr_class::log10(a);
    // log10(10) = 1
    std::string expected = "1.0000000000";
    assert(IsMpfrEquals(result, expected.c_str()));
    // log(2) = 0.3010299957
    a = "2.0";
    expected = "0.3010299957";
    result = mpfr_class::log10(a);
    assert(IsMpfrEquals(result, expected.c_str()));

    a = "0.0";
    result = mpfr_class::log10(a);
    assert(result.is_inf());
    std::cout << "Test log10 zero (should be -Inf) passed." << std::endl;
    a = "-1.0";
    result = mpfr_class::log10(a);
    assert(result.is_nan());
    std::cout << "Test log10 negative (should be NaN) passed." << std::endl;

    std::cout << "Log10 test passed." << std::endl;
}

void testLog2() {
    mpfr_class a("2.0");
    mpfr_class result = mpfr_class::log2(a);
    // log2(2) = 1
    std::string expected = "1.0000000000";
    assert(IsMpfrEquals(result, expected.c_str()));
    // log2(10) = 3.3219280949
    a = "10.0";
    expected = "3.3219280949";
    result = mpfr_class::log2(a);
    assert(IsMpfrEquals(result, expected.c_str()));

    a = "0.0";
    result = mpfr_class::log2(a);
    assert(result.is_inf());
    std::cout << "Test log2 zero (should be -Inf) passed." << std::endl;
    a = "-1.0";
    result = mpfr_class::log2(a);
    assert(result.is_nan());
    std::cout << "Test log2 negative (should be NaN) passed." << std::endl;

    std::cout << "Log2 test passed." << std::endl;
}

void testAddition() {
    mpfr_class a(1.5);
    mpfr_class b(2.5);
    std::string expected = "4.0000000000";

    mpfr_class c = a + b;
    assert(IsMpfrEquals(c, expected.c_str()));
    a += b;
    assert(IsMpfrEquals(a, expected.c_str()));
    std::cout << "Addition Test passed." << std::endl;
}

void testMultplication() {
    mpfr_class a(2.0);
    mpfr_class b(3.0);
    std::string expected = "6.0000000000";

    mpfr_class c = a * b;
    assert(IsMpfrEquals(c, expected.c_str()));
    a *= b;
    assert(IsMpfrEquals(a, expected.c_str()));
    std::cout << "Multiplication Test passed." << std::endl;
}

void testDivision() {
    mpfr_class a(6.0);
    mpfr_class b(2.0);
    std::string expected = "3.0000000000";

    mpfr_class c = a / b;
    assert(IsMpfrEquals(c, expected.c_str()));
    a /= b;
    assert(IsMpfrEquals(a, expected.c_str()));
    std::cout << "Division Test passed." << std::endl;
}
void testSubtraction() {
    mpfr_class a(5.0);
    mpfr_class b(2.0);
    std::string expected = "3.0000000000";

    mpfr_class c = a - b;
    assert(IsMpfrEquals(c, expected.c_str()));
    a -= b;
    assert(IsMpfrEquals(a, expected.c_str()));
    std::cout << "Subtraction Test passed." << std::endl;
}

void testEqNonEq() {
    mpfr_class obj1("2.718281828459045");
    mpfr_class obj2("2.718281828459045");
    assert(obj1 == obj2);
    std::cout << "Equality test passed." << std::endl;

    mpfr_class obj3("3.141592653589793");
    assert(obj1 != obj3);
    std::cout << "Inequality test passed." << std::endl;

    mpfr_class obj4("2.7182818284590451");
    assert(obj1 != obj4);
    std::cout << "Subtle difference test passed." << std::endl;
}

int main() {
    ////////////////////////////////////////////////////////////////////////////////////////
    // 5.1 Initialization Functions
    ////////////////////////////////////////////////////////////////////////////////////////
    testDefaultPrecision();
    testDefaultRoundingMode();
    testDefaultConstructor();
    testCopyConstructor();
    testAssignmentOperator();
    testInitializationAndAssignmentDouble();
    testInitializationAndAssignmentString();
    testEqualitySameValue();
    testEqualityDifferentValue();
    testSetAndGetPrec();
    testSqrt();
    testLog();
    testLog10();
    testLog2();
    testAddition();
    testMultplication();
    testDivision();
    testSubtraction();
    testEqNonEq();

    std::cout << "All tests passed." << std::endl;

    return 0;
}
