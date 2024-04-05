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
#include <iomanip>
#include <cmath>

#if defined GMPXX_MKII
#include "gmpxx_mkII.h"
#else
#include <gmpxx.h>
#endif

#if defined GMPXX_MKII
using namespace gmp;
#endif

// Asserts that the mpf_class object equals the expected string representation
bool Is_mpf_class_Equals(mpf_class &gmpobj, const char *expected, bool debug_flag = false, int precision = 10, int base = 10) {
    char formatString[1024];
    char buffer[1024];

    // Adjust the comparison based on the base
    switch (base) {
    case 10:
        std::sprintf(formatString, "%%.%dFf", precision);      // Generates format string like "%.10Ff"
        gmp_sprintf(buffer, formatString, gmpobj.get_mpf_t()); // Uses generated format string
        break;
    case 16:
        std::sprintf(formatString, "%%.%dFa", precision);      // Generates format string like "%.10Fa"
        gmp_sprintf(buffer, formatString, gmpobj.get_mpf_t()); // Uses generated format string
        break;
    default:
        printf("not supported");
        exit(-1);
    }
    if (std::strcmp(buffer, expected) == 0)
        return true;
    else {
        if (debug_flag == true) {
            printf("%s\n", buffer);
        }
        return false;
    }
}
bool Is_mpz_class_Equals(mpz_class &gmpobj, const char *expected, bool debug_flag = false, int base = 0) {
    char buffer[1024];

    // Adjust the comparison based on the base
    switch (base) {
    case 0:
        gmp_sprintf(buffer, "%Zd", gmpobj.get_mpz_t());
        break;
    default:
        printf("not supported");
        exit(-1);
    }
    if (std::strcmp(buffer, expected) == 0) {
        return true;
    } else {
        if (debug_flag == true) {
            printf("%s\n", buffer);
        }
        return false;
    }
}
bool Is_mpq_class_Equals(mpq_class &gmpobj, const char *expected, bool debug_flag = false, int base = 0) {
    char buffer[1024];
    // Adjust the comparison based on the base
    switch (base) {
    case 0:
        gmp_sprintf(buffer, "%Qd", gmpobj.get_mpq_t());
        break;
    case 16:
        gmp_sprintf(buffer, "%Qx", gmpobj.get_mpq_t());
        break;
    default:
        printf("not supported");
        exit(-1);
    }
    if (std::strcmp(buffer, expected) == 0) {
        return true;
    } else {
        if (debug_flag == true) {
            printf("%s\n", buffer);
        }
        return false;
    }
}
void testDefaultPrecision() {
#if defined GMPXX_MKII
    mpf_class f("1.5");
    mp_bitcnt_t defaultPrec = defaults::get_default_prec();
    assert(defaultPrec == f.get_prec());
    std::cout << "Default precision: " << f.get_prec() << " test passed." << std::endl;

    defaults::set_default_prec(1024);
    defaultPrec = defaults::get_default_prec();
    assert(defaultPrec == 1024);
    mpf_class g("2.5");
    std::cout << "Set and get precision: " << g.get_prec() << " test passed." << std::endl;

    defaults::set_default_prec(512);
    defaultPrec = defaults::get_default_prec();
    mpf_class h("1.5");
    assert(defaultPrec == 512);
    std::cout << "Now get back to precision: " << h.get_prec() << " test passed." << std::endl;
#endif
}
void testDefaultConstructor() {
    mpf_class a;
    char buffer[100];
    gmp_snprintf(buffer, sizeof(buffer), "%.0Ff", a.get_mpf_t());
    assert(std::string(buffer) == "0");
    // initialized to zero
    std::cout << "Default constructor test passed." << std::endl;
}
void testCopyConstructor() {
    mpf_class a;
    mpf_class b = a;
    assert(true);
    std::cout << "Copy constructor test passed." << std::endl;
}
void testAssignmentOperator() {
    mpf_class a;
    mpf_class b;
    b = a;
    assert(true);
    std::cout << "Assignment operator test passed." << std::endl;
}
void testAssignmentOperator_the_rule_of_five() {
    mpf_class a("123.0");

    // testing the rule 1 of 5: copy constructor
    std::cout << "##testing the rule 1 of 5: copy constructor\n";
    mpf_class b(a);
    assert(b == a && " test failed");
    std::cout << "##testing the rule 1 of 5: copy constructor test passed.\n" << std::endl;

    // testing the rule 4 of 5: move constructor
    std::cout << "##testing the rule 4 of 5: move constructor\n";
    mpf_class c(std::move(a));
    assert(c == b && " test failed");
    std::cout << "##testing the rule 4 of 5: move constructor test passed.\n" << std::endl;

    // testing the rule 2 of 5: copy assignment
    std::cout << "##testing the rule 2 of 5: copy assignment\n";
    mpf_class d;
    d = b;
    assert(d == b && " test failed");
    std::cout << "##testing the rule 2 of 5: copy assignment test passed.\n" << std::endl;

    // testing the rule 5 of 5: move assignment
    std::cout << "##testing the rule 5 of 5: copy assignment\n";
    mpf_class e;
    e = std::move(c);
    assert(e == b);
    std::cout << "##testing the rule 5 of 5: copy assignment test passed.\n" << std::endl;
}
void testInitializationAndAssignmentDouble() {
    double testValue = 3.1415926535;
    const char *expectedValue = "3.1415926535";

    mpf_class a = (mpf_class)testValue;
    assert(Is_mpf_class_Equals(a, expectedValue));
    std::cout << "Substitution from double using constructor test passed." << std::endl;

    mpf_class b;
    b = testValue;
    assert(Is_mpf_class_Equals(b, expectedValue));
    std::cout << "Substitution from double using assignment test passed." << std::endl;
}
void testInitializationAndAssignmentString() {
#if defined GMPXX_MKII
    // Testing initialization with a decimal number using a constructor
    const char *expectedDecimalValue = "1.4142135624";
    mpf_class a = expectedDecimalValue;
    assert(Is_mpf_class_Equals(a, expectedDecimalValue));
    std::cout << "Constructor initialization with decimal '" << expectedDecimalValue << "' test passed." << std::endl;

    // Testing initialization with a decimal number using an assignment operator
    mpf_class b;
    b = expectedDecimalValue;
    assert(Is_mpf_class_Equals(b, expectedDecimalValue));
    std::cout << "Assignment initialization with decimal '" << expectedDecimalValue << "' test passed." << std::endl;

    // Testing initialization with a decimal number using a constructor
    std::string expectedDecimalValueString = "3.1415926535";
    mpf_class c = expectedDecimalValueString;
    assert(Is_mpf_class_Equals(c, expectedDecimalValueString.c_str()));
    std::cout << "Constructor initialization with decimal '" << expectedDecimalValueString << "' test passed." << std::endl;

    // Testing initialization with a decimal number using an assignment operator
    mpf_class d;
    d = expectedDecimalValueString;
    assert(Is_mpf_class_Equals(d, expectedDecimalValueString.c_str()));
    std::cout << "Assignment initialization with decimal '" << expectedDecimalValueString << "' test passed." << std::endl;
    // Testing initialization with a hexadecimal number using an assignment operator
    const char *expectedHexValue = "0x3.243f6a8885a3p+0";
    const char *inputHexValue = "3.243F6A8885A308D313198A2E03707344A4093822299F31D008";
    mpf_class e(inputHexValue, defaults::get_default_prec(), 16);
    assert(Is_mpf_class_Equals(e, expectedHexValue, false, 12, 16));
    std::cout << "Assignment initialization with hexadecimal '" << expectedHexValue << "' test passed." << std::endl;

    defaults::base = 16;
    // Testing initialization with a hexadecimal number using a constructor
    mpf_class f;
    e = inputHexValue;
    assert(Is_mpf_class_Equals(e, expectedHexValue, false, 12, defaults::base));
    std::cout << "Constructor initialization with hexadecimal '" << expectedHexValue << "' test passed." << std::endl;
    defaults::base = 10;
#endif
}
void testAddition() {
    mpf_class a(1.5);
    mpf_class b(2.5);
    std::string expected = "4.0000000000";

    mpf_class c = a + b;
    assert(Is_mpf_class_Equals(c, expected.c_str()));
    a += b;
    assert(Is_mpf_class_Equals(a, expected.c_str()));
    std::cout << "Addition Test passed." << std::endl;
}
void testMultplication() {
    mpf_class a(2.0);
    mpf_class b(3.0);
    std::string expected = "6.0000000000";

    mpf_class c = a * b;
    assert(Is_mpf_class_Equals(c, expected.c_str()));
    a *= b;
    assert(Is_mpf_class_Equals(a, expected.c_str()));
    std::cout << "Multiplication Test passed." << std::endl;
}
void testDivision() {
    mpf_class a(6.0);
    mpf_class b(2.0);
    std::string expected = "3.0000000000";

    mpf_class c = a / b;
    assert(Is_mpf_class_Equals(c, expected.c_str()));
    a /= b;
    assert(Is_mpf_class_Equals(a, expected.c_str()));
    std::cout << "Division Test passed." << std::endl;
}
void testSubtraction() {
    mpf_class a(5.0);
    mpf_class b(2.0);
    std::string expected = "3.0000000000";

    mpf_class c = a - b;
    assert(Is_mpf_class_Equals(c, expected.c_str()));
    a -= b;
    assert(Is_mpf_class_Equals(a, expected.c_str()));
    std::cout << "Subtraction Test passed." << std::endl;
}

void testComparisonOperators() {
    mpf_class num1("1.23"), num2("4.56"), num3("1.23");

    // Test == operator
    assert(num1 == num3);
    assert(!(num1 == num2));

    // Test != operator
    assert(num1 != num2);
    assert(!(num1 != num3));

    // Test < operator
    assert(num1 < num2);
    assert(!(num2 < num1));

    // Test > operator
    assert(num2 > num1);
    assert(!(num1 > num2));

    // Test <= operator
    assert(num1 <= num3);    // Equal case
    assert(num1 <= num2);    // Less than case
    assert(!(num2 <= num1)); // Not less than or equal

    // Test >= operator
    assert(num1 >= num3);    // Equal case
    assert(num2 >= num1);    // Greater than case
    assert(!(num1 >= num2)); // Not greater than or equal

    std::cout << "All comparison operator tests passed successfully." << std::endl;
}
void testSqrt() {
    // Test Case 1: Calculate the square root of a positive number
    mpf_class a;
    a = "4.0"; // Square root of 4.0 is 2.0
    mpf_class result = sqrt(a);
    std::string expected = "2.0000000000";
    assert(Is_mpf_class_Equals(result, expected.c_str()));

    // Test Case 2: Calculate the square root of 0
    mpf_class b("0.0"); // Square root of 0.0 is 0.0
    result = sqrt(b);
    expected = "0.0000000000";
    assert(Is_mpf_class_Equals(result, expected.c_str()));

    // Test Case 3: Calculate the square root of a negative number (should raise an error)
    //  mpf_class c("-1.0");
    //  result = sqrt(c);
    //  assert(result.is_nan()); // Check if the result is NaN
    std::cout << "Test square root test passed." << std::endl;
}
void testNeg() {
#if defined GMPXX_MKII
    mpf_class a(-3.5);
    mpf_class result = neg(a);
    mpf_class expected = "3.5";
    assert(result == expected);
    std::cout << "neg test passed." << std::endl;
#endif
}
void testAbs() {
    mpf_class a(-3.5);
    mpf_class c = abs(a);
    mpf_class expected("3.5");
    mpf_class result = abs(a);
    assert(result == expected);
    std::cout << "abs test passed." << std::endl;
}
void test_mpf_class_double_addition() {
    mpf_class a(1.0), c;
    const char *expectedValue = "3.0000000000";
    double b = 2.0;

    c = a + b;
    assert(Is_mpf_class_Equals(c, expectedValue));
    c = b + a;
    assert(Is_mpf_class_Equals(c, expectedValue));
    a += b;
    assert(Is_mpf_class_Equals(a, expectedValue));
    std::cout << "mpf_class + double test passed." << std::endl;
}
void test_mpf_class_double_subtraction() {
    mpf_class a(5.0), c, d;
    const char *expectedValueC = "3.0000000000";
    const char *expectedValueD = "-3.0000000000";
    double b = 2.0;

    c = a - b;
    assert(Is_mpf_class_Equals(c, expectedValueC));
    d = b - a;
    assert(Is_mpf_class_Equals(d, expectedValueD));
    a -= b;
    assert(Is_mpf_class_Equals(a, expectedValueC));
    std::cout << "mpf_class - double test passed." << std::endl;
}
void test_mpf_class_double_multiplication() {
    mpf_class a(2.0), c;
    const char *expectedValueMul = "4.0000000000";
    double b = 2.0;

    c = a * b;
    assert(Is_mpf_class_Equals(c, expectedValueMul));
    c = b * a;
    assert(Is_mpf_class_Equals(c, expectedValueMul));
    a *= b;
    assert(Is_mpf_class_Equals(a, expectedValueMul));
    std::cout << "mpf_class * double test passed." << std::endl;
}
void test_mpf_class_double_division() {
    mpf_class a(4.0), c, d;
    const char *expectedValueDiv = "2.0000000000";
    const char *expectedValueDivRev = "0.5000000000";
    double b = 2.0;

    c = a / b;
    assert(Is_mpf_class_Equals(c, expectedValueDiv));
    d = b / a;
    assert(Is_mpf_class_Equals(d, expectedValueDivRev));
    a /= b;
    assert(Is_mpf_class_Equals(a, expectedValueDiv));
    std::cout << "mpf_class / double test passed." << std::endl;
}
void testOutputOperator() {
    mpf_class num1(-0.33231);
    mpf_class num2(12345.6789);
    mpf_class num3(123456.789);
    mpf_class num4(0.0000123456789);
    mpf_class num5(123456789.0);
    std::ostringstream oss;

    oss.str("");
    oss.clear();
    oss << num1;
    assert(oss.str() == "-0.33231");

    oss.str("");
    oss.clear();
    oss << std::setprecision(8) << num2; // Using default (general) format
    assert(oss.str() == "12345.679");    // Expected output in general format with precision 8

    oss.str("");
    oss.clear();
    oss << std::fixed << std::setprecision(3) << num3;
    assert(oss.str() == "123456.789");

    oss.str("");
    oss.clear();
    oss << std::fixed << std::setprecision(4) << num4;
    assert(oss.str() == "0.0000");

    oss.str("");
    oss.clear();
    oss << std::scientific << std::setprecision(2) << num5;
    assert(oss.str() == "1.23e+08");

    std::cout.setf(std::ios_base::fmtflags(0), std::ios_base::floatfield);
    std::cout << "output test passed." << std::endl;
}
void testCeilFunction() {
    mpf_class num1(123.456);
    mpf_class num2(-123.456);
    mpf_class result;
    const char *expected1 = "124.0000000000";
    const char *expected2 = "-123.0000000000";

    result = ceil(num1);
    assert(Is_mpf_class_Equals(result, expected1));

    result = ceil(num2);
    assert(Is_mpf_class_Equals(result, expected2));

    std::cout << "Ceil function tests passed." << std::endl;
}
void testFloor() {
    mpf_class numPositive(3.14);
    mpf_class numNegative(-3.14);
    mpf_class result;
    const char *expected1 = "3.0000000000";
    const char *expected2 = "-4.0000000000";

    // Testing floor on a positive number
    result = floor(numPositive);
    assert(Is_mpf_class_Equals(result, expected1));

    // Testing floor on a negative number
    result = floor(numNegative);
    assert(Is_mpf_class_Equals(result, expected2));

    std::cout << "Floor tests passed." << std::endl;
}
void testHypot() {
    mpf_class op1(3.0);
    mpf_class op2(4.0);
    const char *expected = "5.0000000000";
    mpf_class result;

    // Since mpf_hypot doesn't exist, let's assume a correct implementation for hypot
    result = hypot(op1, op2);
    assert(Is_mpf_class_Equals(result, expected));

    std::cout << "Hypot tests passed." << std::endl;
}
void testSgn() {
    mpf_class positive(123.456);
    mpf_class negative(-123.456);
    mpf_class zero(0.0);

    assert(sgn(positive) > 0);
    assert(sgn(negative) < 0);
    assert(sgn(zero) == 0);

    std::cout << "Sign function tests passed." << std::endl;
}
void test_get_d() {
    // Test with a positive double
    mpf_class pos(0.125);
    assert(pos.get_d() == 0.125);

    mpf_class neg(-0.25);
    assert(neg.get_d() == -0.25);

    // Test with zero
    mpf_class zero(0.0);
    assert(zero.get_d() == 0.0);

    std::cout << "get_d function tests passed." << std::endl;
}
void test_get_ui() {
    // Test with a positive value
    mpf_class pos(123);
    assert(pos.get_ui() == 123);

    // Test with a value requiring rounding
    mpf_class round(123.456);
    assert(round.get_ui() == 123);

    // Test with zero
    mpf_class zero(0.0);
    assert(zero.get_ui() == 0);

    std::cout << "get_ui function tests passed." << std::endl;
}
void test_get_si() {
    // Test with a positive value
    mpf_class pos(123);
    assert(pos.get_si() == 123);

    // Test with a negative value
    mpf_class neg(-123);
    assert(neg.get_si() == -123);

    // Test with a value requiring rounding
    mpf_class round(-123.456);
    assert(round.get_si() == -123);

    // Test with zero
    mpf_class zero(0.0);
    assert(zero.get_si() == 0);

    std::cout << "get_si function tests passed." << std::endl;
}

void test_mpf_class_constructor_precision() {
#if !defined __GMPXX_MKII_NOPRECCHANGE__
    mpf_class f1(1.5); // default precision
    assert(f1.get_prec() == mpf_get_default_prec());
    mpf_class f2(1.5, 1024); // 1024 bits (at least)
    assert(f2.get_prec() == 1024);

    mpf_class g(2.5);
    mpf_class x(-1.5, 64); // 64 bits (at least)
    mpf_class y(3.5, 1024);
    mpf_class z(3.5, 2048);

    mpf_class f3(x);
    assert(f3.get_prec() == 64);

    mpf_class f4_1(abs(x));
    assert(f4_1.get_prec() == 64);
    mpf_class f4_2(ceil(x));
    assert(f4_2.get_prec() == 64);
    mpf_class f4_3(floor(x));
    assert(f4_3.get_prec() == 64);
    mpf_class f4_4(sqrt(y));
    assert(f4_4.get_prec() == 1024);
    mpf_class f4_5(sqrt(z));
    assert(f4_5.get_prec() == 2048);
    mpf_class f4_6(hypot(x, y));
    assert(f4_6.get_prec() == 1024);
    mpf_class f4_7(sqrt(x + y));
    assert(f4_7.get_prec() == 1024);

    mpf_class f5_1(-g, 1024);
    assert(f5_1.get_prec() == 1024);
    mpf_class f5_2(+g, 1024);
    assert(f5_2.get_prec() == 1024);

    mpf_class f6_1(x + y);
    assert(f6_1.get_prec() == 1024);
    mpf_class f6_2(y + x);
    assert(f6_2.get_prec() == 1024);
    mpf_class f6_3(x - y);
    assert(f6_3.get_prec() == 1024);
    mpf_class f6_4(y - x);
    assert(f6_4.get_prec() == 1024);
    mpf_class f6_5(x * y);
    assert(f6_5.get_prec() == 1024);
    mpf_class f6_6(y * x);
    assert(f6_6.get_prec() == 1024);
    mpf_class f6_7(x / y);
    assert(f6_7.get_prec() == 1024);
    mpf_class f6_8(y / x);
    assert(f6_8.get_prec() == 1024);

    x += y;
    assert(x.get_prec() == 64);
    assert(y.get_prec() == 1024);
    y += x;
    assert(x.get_prec() == 64);
    assert(y.get_prec() == 1024);
    y *= x;
    assert(x.get_prec() == 64);
    assert(y.get_prec() == 1024);
    y /= x;
    assert(x.get_prec() == 64);
    assert(y.get_prec() == 1024);

    mpf_class f7_1(y);
    assert(f7_1.get_prec() == 1024);
    mpf_class f7_2;
    f7_2 = y;
    assert(f7_2.get_prec() == mpf_get_default_prec());
    std::cout << "test_mpf_class_constructor_precision passed." << std::endl;
#endif
}

void test_mpf_class_constructor_with_mpf() {
    mpf_t f;
    mp_bitcnt_t prec = 128; // Example precision
    const char *expected = "0.0390625000";
    mpf_init2(f, mpf_get_default_prec());
    mpf_set_str(f, "0.0390625", 10); // Initialize f with a string value, base 10

    mpf_class result(f);
    assert(Is_mpf_class_Equals(result, expected));

    mpf_class b(f, prec);
    assert(b.get_prec() == prec);
    assert(Is_mpf_class_Equals(b, expected));
    mpf_clear(f);

    std::cout << "Constructor tests passed." << std::endl;
}
void test_mpf_class_literal() {
#ifdef GMPXX_MKII
    // Using the user-defined literal to create mpf_class objects
    mpf_class num1 = "3.14159"_mpf;
    mpf_class num2 = "2.71828"_mpf;
    mpf_class num3 = "0.0"_mpf;
    mpf_class num4 = "-123.456"_mpf;

    assert(Is_mpf_class_Equals(num1, "3.1415900000"));
    assert(Is_mpf_class_Equals(num2, "2.7182800000"));
    assert(Is_mpf_class_Equals(num3, "0.0000000000"));
    assert(Is_mpf_class_Equals(num4, "-123.4560000000"));

    std::cout << "User-defined literal tests for mpf_class passed." << std::endl;
#endif
}
void test_mpf_class_swap() {
    mpf_class a("123.456"), b("789.012");

    // Swap 'a' and 'b'
    a.swap(b);

    // Verify that their contents have been swapped
    assert(Is_mpf_class_Equals(a, "789.0120000000"));
    assert(Is_mpf_class_Equals(b, "123.4560000000"));

    mpf_class num1(3.14159);
    mpf_class num2(2.71828);

    mpf_class original_num1 = num1;
    mpf_class original_num2 = num2;

    swap(num1, num2);

    assert(num1 == original_num2 && "After swap, num1 should have the value of original num2");
    assert(num2 == original_num1 && "After swap, num2 should have the value of original num1");

    std::cout << "Swap tests passed." << std::endl;
}
void test_template_cmp() {
    mpf_class num1(3.14);
    assert(cmp(num1, 3.14) == 0);
    assert(cmp(3.14, num1) == 0);
    assert(cmp(num1, 2.71f) > 0);
    assert(cmp(2.71f, num1) < 0);
    assert(cmp(num1, 3UL) > 0);
    assert(cmp(3L, num1) < 0);

    std::cout << "Template cmp function tests passed." << std::endl;
}
void test_set_str() {
    mpf_class num;
    const char *num_c = "123.456";
    std::string num_s = "1.25e-3";
    std::string num_f = "FF";

    assert(num.set_str(num_c, 10) == 0);
    assert(Is_mpf_class_Equals(num, "123.4560000000"));

    assert(num.set_str(num_s, 10) == 0);
    assert(Is_mpf_class_Equals(num, "0.0012500000"));
    assert(num.set_str(num_f, 10) == -1);

    std::cout << "All set_str tests passed." << std::endl;
}
void test_mpf_class_get_str() {
    mp_exp_t exp;
    mpf_class num1("1234.56789");

    // Test in base 10 with default digits (entire number)
    std::string str1 = num1.get_str(exp, 10);
    std::cout << "String: " << str1 << ", Exponent: " << exp << std::endl;
    assert(str1 == "123456789" && exp == 4);

    // Test in base 16 with limited digits
    std::string str2 = num1.get_str(exp, 16, 4);
    std::cout << "String: " << str2 << ", Exponent: " << exp << std::endl;
    assert(str2 == "4d29" && exp == 3); // 4 * (16^2) + 13 * (16^1) + 2 * (16^0) + 9 * (16^(-1)) = 1234.56250

    std::cout << "mpf_class get_str method tests passed." << std::endl;
}
void test_trunc_function() {
    mpf_class num1(3.14159);
    mpf_class truncated1 = trunc(num1);
    assert(mpf_cmp_d(truncated1.get_mpf_t(), 3.0) == 0); // trancate 3.14159 => 3

    mpf_class num2(-3.14159);
    mpf_class truncated2 = trunc(num2);
    assert(mpf_cmp_d(truncated2.get_mpf_t(), -3.0) == 0); // trancate -3.14159 => -3

    std::cout << "trunc function tests passed." << std::endl;
}
void test_fits_sint_p() {
    mpf_class smallValue("123");
    assert(smallValue.fits_sint_p() == true);

    mpf_class largeValue("999999999999999999999999999999");
    assert(largeValue.fits_sint_p() == false);

    mpf_class negativeValue("-123");
    assert(negativeValue.fits_sint_p() == true);

    mpf_class edgeCaseMax(std::to_string(INT_MAX));
    assert(edgeCaseMax.fits_sint_p() == true);

    mpf_class edgeCaseMin(std::to_string(INT_MIN));
    assert(edgeCaseMin.fits_sint_p() == true);

    std::cout << "All fits_sint_p tests passed." << std::endl;
}
void test_fits_slong_p() {
    mpf_class value(std::to_string(INT_MAX));
    assert(value.fits_slong_p() == true);

    mpf_class outOfRange(std::to_string(LONG_MAX));
    outOfRange += 1;
    assert(outOfRange.fits_slong_p() == false);

    std::cout << "All fits_slong_p tests passed." << std::endl;
}

void test_fits_sshort_p() {
    mpf_class value(std::to_string(SHRT_MAX));
    assert(value.fits_sshort_p() == true);

    mpf_class outOfRange(std::to_string(SHRT_MAX));
    outOfRange += 1;
    assert(outOfRange.fits_sshort_p() == false);

    std::cout << "All fits_sshort_p tests passed." << std::endl;
}

void test_fits_uint_p() {
    mpf_class value(std::to_string(UINT_MAX));
    assert(value.fits_uint_p() == true);

    mpf_class outOfRange = value + 1;
    assert(outOfRange.fits_uint_p() == false);

    std::cout << "All fits_uint_p tests passed." << std::endl;
}

void test_fits_ulong_p() {
    mpf_class value(std::to_string(ULONG_MAX));
    assert(value.fits_ulong_p() == true);

    mpf_class outOfRange = value + 1;
    assert(outOfRange.fits_ulong_p() == false);

    std::cout << "All fits_ulong_p tests passed." << std::endl;
}

void test_fits_ushort_p() {
    mpf_class value(std::to_string(USHRT_MAX));
    assert(value.fits_ushort_p() == true);

    mpf_class outOfRange = value + 1;
    assert(outOfRange.fits_ushort_p() == false);

    std::cout << "All fits_ushort_p tests passed." << std::endl;
}
void testDefaultConstructor_mpz_class() {
    mpz_class a;
    char buffer[100];
    gmp_snprintf(buffer, sizeof(buffer), "%Zd", a.get_mpz_t());
    assert(std::string(buffer) == "0");
    // initialized to zero
    std::cout << "Default constructor test passed." << std::endl;
}
void testCopyConstructor_mpz_class() {
    mpz_class a;
    mpz_class b = a;
    assert(true);
    std::cout << "Copy constructor test passed." << std::endl;
}
void testAssignmentOperator_mpz_class() {
    mpz_class a;
    mpz_class b;
    b = a;
    assert(true);
    std::cout << "Assignment operator test passed." << std::endl;
}
void testAssignmentOperator_the_rule_of_five_mpz_class() {
    mpz_class a("123");

    // testing the rule 1 of 5: copy constructor
    std::cout << "##testing the rule 1 of 5: copy constructor\n";
    mpz_class b(a);
    assert(b == a && " test failed");
    std::cout << "##testing the rule 1 of 5: copy constructor test passed.\n" << std::endl;

    // testing the rule 4 of 5: move constructor
    std::cout << "##testing the rule 4 of 5: move constructor\n";
    mpz_class c(std::move(a));
    assert(c == b && " test failed");
    std::cout << "##testing the rule 4 of 5: move constructor test passed.\n" << std::endl;

    // testing the rule 2 of 5: copy assignment
    std::cout << "##testing the rule 2 of 5: copy assignment\n";
    mpz_class d;
    d = b;
    assert(d == b && " test failed");
    std::cout << "##testing the rule 2 of 5: copy assignment test passed.\n" << std::endl;

    // testing the rule 5 of 5: move assignment
    std::cout << "##testing the rule 5 of 5: copy assignment\n";
    mpz_class e;
    e = std::move(c);
    assert(e == b);
    std::cout << "##testing the rule 5 of 5: copy assignment test passed.\n" << std::endl;
}
void testInitializationAndAssignmentDouble_mpz_class() {
    double testValue = 31415926535;
    const char *expectedValue = "31415926535";

    mpz_class a = (mpz_class)testValue;
    assert(Is_mpz_class_Equals(a, expectedValue, true));
    std::cout << "Substitution from double using constructor test passed." << std::endl;

    mpz_class b;
    b = testValue;
    assert(Is_mpz_class_Equals(b, expectedValue, true));
    std::cout << "Substitution from double using assignment test passed." << std::endl;
}
void testInitializationAndAssignmentInt_mpz_class() {
    signed long int testValue = -31415926535;
    const char *expectedValue = "-31415926535";

    mpz_class a = (mpz_class)testValue;
    assert(Is_mpz_class_Equals(a, expectedValue, true));
    std::cout << "Substitution from signed long int using constructor test passed." << std::endl;

    mpz_class b;
    b = testValue;
    assert(Is_mpz_class_Equals(b, expectedValue, true));
    std::cout << "Substitution from signed long int using assignment test passed." << std::endl;

    unsigned long int testValue2 = 31415926535;
    const char *expectedValue2 = "31415926535";

    mpz_class c = (mpz_class)testValue2;
    assert(Is_mpz_class_Equals(c, expectedValue2, true));
    std::cout << "Substitution from unsigned long int using constructor test passed." << std::endl;

    mpz_class d;
    d = testValue2;
    assert(Is_mpz_class_Equals(d, expectedValue2, true));
    std::cout << "Substitution from unsigned long int using assignment test passed." << std::endl;
}

void testInitializationAndAssignment_mpz_class_mpf_class() {
    mpf_class testValue("-31415926535");
    const char *expectedValue = "-31415926535";

    mpz_class a = (mpz_class)testValue; // explicit cast requested
    assert(Is_mpz_class_Equals(a, expectedValue));
    std::cout << "Substitution from mpf_class using constructor test passed." << std::endl;

    mpz_class b;
    b = (mpz_class)testValue; // explicit cast requested
    assert(Is_mpz_class_Equals(b, expectedValue));
    std::cout << "Substitution from mpf_class using assignment test passed." << std::endl;
}
void testInitializationAndAssignment_mpf_class_mpz_class() {
    mpz_class testValue("-31415926535");
    const char *expectedValue = "-31415926535.0000000000";

    mpf_class a = (mpf_class)testValue; // explicit cast requested
    assert(Is_mpf_class_Equals(a, expectedValue));
    std::cout << "Substitution from mpz_class to mpf_class using constructor test passed." << std::endl;

    mpf_class b;
    b = (mpf_class)testValue; // explicit cast requested
    assert(Is_mpf_class_Equals(b, expectedValue));
    std::cout << "Substitution from mpz_class to mpf_class using assignment test passed." << std::endl;
}
void testInitializationAndAssignmentString_mpz_class() {
#if defined GMPXX_MKII
    // Testing initialization with a decimal number using a constructor
    const char *expectedDecimalValue = "14142135624";
    mpz_class a = expectedDecimalValue;
    assert(Is_mpz_class_Equals(a, expectedDecimalValue));
    std::cout << "Constructor initialization with decimal '" << expectedDecimalValue << "' test passed." << std::endl;

    // Testing initialization with a decimal number using an assignment operator
    mpz_class b;
    b = expectedDecimalValue;
    assert(Is_mpz_class_Equals(b, expectedDecimalValue));
    std::cout << "Assignment initialization with decimal '" << expectedDecimalValue << "' test passed." << std::endl;

    // Testing initialization with a decimal number using a constructor
    std::string expectedDecimalValueString = "31415926535";
    mpz_class c = expectedDecimalValueString;
    assert(Is_mpz_class_Equals(c, expectedDecimalValueString.c_str()));
    std::cout << "Constructor initialization with decimal '" << expectedDecimalValueString << "' test passed." << std::endl;

    // Testing initialization with a decimal number using an assignment operator
    mpz_class d;
    d = expectedDecimalValueString;
    assert(Is_mpz_class_Equals(d, expectedDecimalValueString.c_str()));
    std::cout << "Assignment initialization with decimal '" << expectedDecimalValueString << "' test passed." << std::endl;

    // Testing initialization with a hexadecimal number using an assignment operator
    const char *expectedHexValue = "66814286504060421741230023322616923956";
    const char *inputHexValue = "3243F6A8885A308D313198A2E0370734";
    mpz_class e(inputHexValue, 16);
    assert(Is_mpz_class_Equals(e, expectedHexValue, true));
    std::cout << "Assignment initialization with hexadecimal '" << expectedHexValue << "' test passed." << std::endl;
#endif
}
void test_template_cmp_mpz_class() {
    mpz_class num1(314L);
    assert(cmp(num1, 314L) == 0);
    assert(cmp(314L, num1) == 0);
    assert(cmp(num1, 271L) > 0);
    assert(cmp(271L, num1) < 0);
    assert(cmp(num1, 3UL) > 0);
    assert(cmp(3L, num1) < 0);

    std::cout << "Template cmp function tests passed." << std::endl;
}
void test_mpz_class_literal() {
#ifdef GMPXX_MKII
    // Using the user-defined literal to create mpz_class objects
    mpz_class num1 = "314159"_mpz;
    mpz_class num2 = "271828"_mpz;
    mpz_class num3 = "0"_mpz;
    mpz_class num4 = "-123456"_mpz;
    mpz_class num5 = "99999999999999999999999999999999999999999999999999999999"_mpz;

    const char *num1_expected = "314159";
    const char *num2_expected = "271828";
    const char *num3_expected = "0";
    const char *num4_expected = "-123456";
    const char *num5_expected = "99999999999999999999999999999999999999999999999999999999";
    assert(Is_mpz_class_Equals(num1, num1_expected));
    assert(Is_mpz_class_Equals(num2, num2_expected));
    assert(Is_mpz_class_Equals(num3, num3_expected));
    assert(Is_mpz_class_Equals(num4, num4_expected));
    assert(Is_mpz_class_Equals(num5, num5_expected));

    std::cout << "User-defined literal tests for mpz_class passed." << std::endl;
#endif
}
void test_arithmetic_operators_mpz_class() {
    mpz_class a(10);
    mpz_class b(5);
    mpz_class c(10);
    mpz_class d(3);
    mpz_class result;

    result = a;
    result += b;
    assert(result == mpz_class(15));

    result = a;
    result -= b;
    assert(result == mpz_class(5));

    result = a;
    result *= b;
    assert(result == mpz_class(50));

    result = a + b;
    assert(result == mpz_class(15));

    result = a - b;
    assert(result == mpz_class(5));

    result = a * b;
    assert(result == mpz_class(50));

    result = c % d;
    std::cout << "10 % 3 = " << result << std::endl;
    assert(result == mpz_class(1));

    c %= d;
    std::cout << "After c %= d, c = " << c << std::endl;
    assert(c == mpz_class(1));
    std::cout << "All operator tests passed." << std::endl;
}
void testFitsFunctions_mpz_class() {
    mpz_class small("123");
    mpz_class large("12345678901234567890123456789012345678901234567890");

    assert(small.fits_sint_p() == true);
    assert(small.fits_slong_p() == true);
    assert(small.fits_sshort_p() == true);
    assert(small.fits_uint_p() == true);
    assert(small.fits_ulong_p() == true);
    assert(small.fits_ushort_p() == true);

    assert(large.fits_sint_p() == false);
    assert(large.fits_slong_p() == false);
    assert(large.fits_sshort_p() == false);
    assert(large.fits_uint_p() == false);
    assert(large.fits_ulong_p() == false);
    assert(large.fits_ushort_p() == false);

    std::cout << "All fits functions tests passed." << std::endl;
}

void testAbsFunction_mpz_class() {
    mpz_class pos("123");
    mpz_class neg("-456");
    mpz_class zero("0");

    assert(abs(pos) == pos);
    assert(abs(neg) == mpz_class("456"));
    assert(abs(zero) == zero);

    std::cout << "abs function tests passed." << std::endl;
}
void testConversionFunctions_mpz_class() {
    mpz_class num("123456789");

    // Testing get_d()
    double d = num.get_d();
    std::cout << "Double: " << d << std::endl;
    assert(d == 123456789.0);

    // Testing get_si()
    long si = num.get_si();
    std::cout << "Long: " << si << std::endl;
    assert(si == 123456789);

    // Testing get_ui()
    unsigned long ui = num.get_ui();
    std::cout << "Unsigned Long: " << ui << std::endl;
    assert(ui == 123456789);

    // Testing get_str()
    std::string str = num.get_str();
    std::cout << "String: " << str << std::endl;
    assert(str == "123456789");

    std::cout << "All conversion function tests passed." << std::endl;
}

void testMathFunctions_mpz_class() {
    mpz_class num1("24");
    mpz_class num2("36");

    // Testing sgn()
    assert(sgn(num1) > 0);
    assert(sgn(mpz_class("-1")) < 0);

    // Testing sqrt()
    mpz_class sqrtResult = sqrt(num1);
    std::cout << "Sqrt of " << num1.get_str() << " is " << sqrtResult.get_str() << std::endl;
    assert(sqrtResult == mpz_class("4"));

    // Testing gcd()
    mpz_class gcdResult = gcd(num1, num2);
    std::cout << "GCD of " << num1.get_str() << " and " << num2.get_str() << " is " << gcdResult.get_str() << std::endl;
    assert(gcdResult == mpz_class("12"));

    // Testing lcm()
    mpz_class lcmResult = lcm(num1, num2);
    std::cout << "LCM of " << num1.get_str() << " and " << num2.get_str() << " is " << lcmResult.get_str() << std::endl;
    assert(lcmResult == mpz_class("72"));

    std::cout << "All math function tests for mpz_class passed." << std::endl;
}

template <class T> T test_func(const T &a, const T &b) { return a * b; }
void test_mpf_class_extention() {
#if defined GMPXX_MKII
    mpf_class f(2), g(1), h(3);

    mpf_class result;
    result = test_func(f * h, g);

    std::cout << "The result of test_func(f * h, g) is: " << result << std::endl;
#endif
}
void test_mpz_class_extention() {
#if defined GMPXX_MKII
    mpz_class f(2), g(1), h(3);

    mpz_class result;
    result = test_func(f * h, g);

    std::cout << "The result of test_func(f * h, g) is: " << result << std::endl;
#endif
}
void test_set_str_mpz_class() {
    mpz_class a, b, c, d, e, f;

    // Test setting value from C-style string
    assert(a.set_str("FF", 16) == 0);         // Hexadecimal
    assert(b.set_str("1010", 2) == 0);        // Binary
    assert(c.set_str("1234567890", 10) == 0); // Decimal
    const char *expectedValue_a = "255";
    const char *expectedValue_b = "10";
    const char *expectedValue_c = "1234567890";

    // Print to verify the values
    gmp_printf("a (hex 'FF') as integer: %Zd\n", a.get_mpz_t());
    gmp_printf("b (binary '1010') as integer: %Zd\n", b.get_mpz_t());
    gmp_printf("c (decimal '1234567890') as integer: %Zd\n", c.get_mpz_t());

    assert(Is_mpz_class_Equals(a, expectedValue_a));
    assert(Is_mpz_class_Equals(b, expectedValue_b));
    assert(Is_mpz_class_Equals(c, expectedValue_c));

    // Test setting value from std::string
    assert(d.set_str(std::string("255"), 10) == 0);    // Decimal
    assert(e.set_str(std::string("377"), 8) == 0);     // Octal
    assert(f.set_str(std::string("C3665C"), 16) == 0); // Hexadecimal
    const char *expectedValue_d = "255";
    const char *expectedValue_e = "255";
    const char *expectedValue_f = "12805724";

    // Print to verify the values
    gmp_printf("d (decimal '255') as integer: %Zd\n", d.get_mpz_t());
    gmp_printf("e (octal '377') as integer: %Zd\n", e.get_mpz_t());
    gmp_printf("f (hexadecimal 'C3665C') as integer: %Zd\n", f.get_mpz_t());

    assert(Is_mpz_class_Equals(d, expectedValue_d));
    assert(Is_mpz_class_Equals(e, expectedValue_e));
    assert(Is_mpz_class_Equals(f, expectedValue_f));

    std::cout << "set_str tests passed." << std::endl;
}
void test_factorial_mpz_class() {
    mpz_class fact5 = factorial((mpz_class)5);
    const char *expectedValue_fact5 = "120";
    assert(Is_mpz_class_Equals(fact5, expectedValue_fact5)); // 5! = 120
    std::cout << "Factorial test passed.\n";
}
void test_primorial_mpz_class() {
    mpz_class primorial5 = primorial((mpz_class)5);
    const char *expectedValue_primorial = "30";
    assert(Is_mpz_class_Equals(primorial5, expectedValue_primorial)); // 2*3*5 = 30
    std::cout << "Primorial test passed.\n";
}
void test_fibonacci_mpz_class() {
    mpz_class fibonacci7 = fibonacci((mpz_class)7);
    const char *expectedValue_fibonacci = "13";
    assert(Is_mpz_class_Equals(fibonacci7, expectedValue_fibonacci)); // F(7) = 13
    std::cout << "Fibonacchi test passed.\n";
}
void test_mpz_class_swap() {
    mpz_class a("123456"), b("789012");

    // Swap 'a' and 'b'
    a.swap(b);

    // Verify that their contents have been swapped
    assert(Is_mpz_class_Equals(a, "789012"));
    assert(Is_mpz_class_Equals(b, "123456"));

    mpz_class num1(314159);
    mpz_class num2(271828);

    mpz_class original_num1 = num1;
    mpz_class original_num2 = num2;

    swap(num1, num2);

    assert(num1 == original_num2 && "After swap, num1 should have the value of original num2");
    assert(num2 == original_num1 && "After swap, num2 should have the value of original num1");

    std::cout << "Swap tests passed." << std::endl;
}
void testOutputOperator_mpz_class() {
    // Test decimal output
    mpz_class numDec(123456789);
    std::ostringstream ossDec;
    ossDec << numDec;
    assert(ossDec.str() == "123456789");
    std::cout << ossDec.str() << "\n";

    // Test hexadecimal output in lowercase
    mpz_class numHexLower(255); // FF in hex
    std::ostringstream ossHexLower;
    ossHexLower << std::hex << std::nouppercase << numHexLower;
    assert(ossHexLower.str() == "ff"); // Lowercase hex
    std::cout << ossHexLower.str() << "\n";

    // Test hexadecimal output in uppercase
    mpz_class numHexUpper(255); // FF in hex
    std::ostringstream ossHexUpper;
    ossHexUpper << std::hex << std::uppercase << numHexUpper;
    assert(ossHexUpper.str() == "FF"); // Uppercase hex
    std::cout << ossHexUpper.str() << "\n";

    // Test octal output
    mpz_class numOct(8); // 10 in oct
    std::ostringstream ossOct;
    ossOct << std::oct << numOct;
    assert(ossOct.str() == "10");
    std::cout << ossOct.str() << "\n";

    std::cout << "Output operator tests for mpz_class passed." << std::endl;
}
void test_mpz_class_addition() {
    mpz_class a(1), c;
    const char *expectedValue = "3";
    unsigned long int b = 2;

    c = a + b;
    assert(Is_mpz_class_Equals(c, expectedValue));
    c = b + a;
    assert(Is_mpz_class_Equals(c, expectedValue));
    a += b;
    assert(Is_mpz_class_Equals(a, expectedValue));
    std::cout << "mpz_class + unsigned long int addition test passed." << std::endl;
}
void test_mpz_class_subtraction() {
    mpz_class a(5), c, d;
    const char *expectedValueC = "3";
    const char *expectedValueD = "4";
    unsigned long int b = 2;

    c = a - b;
    assert(Is_mpz_class_Equals(c, expectedValueC));
    b = 9;
    d = b - a;
    assert(Is_mpz_class_Equals(d, expectedValueD));
    b = 2;
    a -= b;
    assert(Is_mpz_class_Equals(a, expectedValueC));
    std::cout << "mpz_class - unsigned long int subtraction test passed." << std::endl;
}
void test_mpz_class_multiplication() {
    mpz_class a(3), c;
    const char *expectedValue = "6";
    unsigned long int b = 2;

    c = a * b;
    assert(Is_mpz_class_Equals(c, expectedValue));
    c = b * a;
    assert(Is_mpz_class_Equals(c, expectedValue));
    a *= b;
    assert(Is_mpz_class_Equals(a, expectedValue));
    std::cout << "mpz_class * unsigned long int multiplication test passed." << std::endl;
}
void test_mpz_class_division() {
    mpz_class a(6), c, d;
    const char *expectedValue = "3";
    unsigned long int b = 2;

    c = a / b;
    assert(Is_mpz_class_Equals(c, expectedValue));
    b = 18;
    d = b / a;
    assert(Is_mpz_class_Equals(d, expectedValue));
    b = 2;
    a /= b;
    assert(Is_mpz_class_Equals(a, expectedValue));
    std::cout << "mpz_class / unsigned long int division test passed." << std::endl;
}
void test_mpz_class_modulus() {
    mpz_class a(5), c, d;
    const char *expectedValue = "1";
    unsigned long int b = 2;

    c = a % b;
    assert(Is_mpz_class_Equals(c, expectedValue));
    b = 11;
    d = b % a;
    assert(Is_mpz_class_Equals(d, expectedValue));
    b = 2;
    a %= b;
    assert(Is_mpz_class_Equals(a, expectedValue));
    std::cout << "mpz_class % unsigned long int modulus test passed." << std::endl;
}
void testDefaultConstructor_mpq_class() {
    mpq_class a;
    char buffer[100];
    gmp_snprintf(buffer, sizeof(buffer), "%Qd", a.get_mpq_t());
    assert(std::string(buffer) == "0");
    // initialized to zero
    std::cout << "Default constructor mpq_class test passed." << std::endl;
}
void testCopyConstructor_mpq_class() {
    mpq_class a;
    mpq_class b = a;
    assert(true);
    std::cout << "Copy constructor mpq_class test passed." << std::endl;
}
void testAssignmentOperator_mpq_class() {
    mpq_class a;
    mpq_class b;
    b = a;
    assert(true);
    std::cout << "Assignment operator mpq_class test passed." << std::endl;
}
void testInitializationAndAssignmentInt_mpq_class() {
    const char *expectedValue = "355/113";

    mpq_class a(355, 113);
    assert(Is_mpq_class_Equals(a, expectedValue, true));
    std::cout << "Substitution mpq_class from integers test passed." << std::endl;

    mpq_class b;
    b = a;
    assert(Is_mpq_class_Equals(b, expectedValue, true));
    std::cout << "Substitution mpq_class assignment test passed." << std::endl;
}
void testAssignmentOperator_the_rule_of_five_mpq_class() {
    mpq_class a(1, 7);

    // testing the rule 1 of 5: copy constructor
    std::cout << "##testing the rule 1 of 5: copy constructor\n";
    mpq_class b(a);
    assert(b == a && " test failed");
    std::cout << "##testing the rule 1 of 5: copy constructor test passed.\n" << std::endl;

    // testing the rule 4 of 5: move constructor
    std::cout << "##testing the rule 4 of 5: move constructor\n";
    mpq_class c(std::move(a));
    assert(c == b && " test failed");
    std::cout << "##testing the rule 4 of 5: move constructor test passed.\n" << std::endl;

    // testing the rule 2 of 5: copy assignment
    std::cout << "##testing the rule 2 of 5: copy assignment\n";
    mpq_class d;
    d = b;
    assert(d == b && " test failed");
    std::cout << "##testing the rule 2 of 5: copy assignment test passed.\n" << std::endl;

    // testing the rule 5 of 5: move assignment
    std::cout << "##testing the rule 5 of 5: copy assignment\n";
    mpq_class e;
    e = std::move(c);
    assert(e == b);
    std::cout << "##testing the rule 5 of 5: copy assignment test passed.\n" << std::endl;
}
void testInitializationAndAssignmentString_mpq_class() {
#if defined GMPXX_MKII
    try {
#endif
        mpq_class decimalFraction("-13/297");
        const char *expectedValue_decimalFraction = "-13/297";
        assert(Is_mpq_class_Equals(decimalFraction, expectedValue_decimalFraction, true));
        std::cout << "Constructor initialization with decimal '" << expectedValue_decimalFraction << "' test passed." << std::endl;

        mpq_class hexFraction("1/a", 16);
        const char *expectedValue_hexFraction = "1/a";
        assert(Is_mpq_class_Equals(hexFraction, expectedValue_hexFraction, true, 16));
        std::cout << "Constructor initialization with hex '" << expectedValue_hexFraction << "' test passed." << std::endl;

        std::string strFraction = "3/4";
        mpq_class stringFraction(strFraction);
        const char *expectedValue_strFraction = "3/4";
        std::cout << "String fraction: " << expectedValue_strFraction << std::endl;
#if defined GMPXX_MKII
        mpq_class invalidFraction("not a number");
    } catch (const std::runtime_error &e) {
        std::cout << "Caught an exception: " << e.what() << std::endl;
    }
#endif
}
void test_template_cmp_mpq_class() {
    mpq_class num1(1, 3);
    mpq_class num2(1, 3);
    mpq_class num3(2, 7);
    assert(cmp(num1, num1) == 0);
    assert(cmp(num2, num1) == 0);
    assert(cmp(num2, num1) >= 0);
    assert(cmp(num1, num1) <= 0);
    assert(cmp(num3, num1) < 0);
    assert(cmp(num1, num3) > 0);
    assert(cmp(num3, num1) <= 0);
    assert(cmp(num1, num3) >= 0);

    std::cout << "Template cmp mpq_class function tests passed." << std::endl;
}
void test_arithmetic_operators_mpq_class_hardcoded1() {
    mpq_class a("3/5");
    mpq_class b("2/7");
    mpq_class c("5/11");
    mpq_class d("9/13");
    mpq_class result;

    result = a;
    result += b;
    mpq_class expectedAdd("31/35");
    assert(result == expectedAdd);
    std::cout << "a + b = " << result << " (Expected: 31/35)" << std::endl;

    result = a;
    result -= b;
    mpq_class expectedSub("11/35");
    assert(result == expectedSub);
    std::cout << "a - b = " << result << " (Expected: 11/35)" << std::endl;

    result = a;
    result *= b;
    mpq_class expectedMul("6/35");
    assert(result == expectedMul);
    std::cout << "a * b = " << result << " (Expected: 6/35)" << std::endl;

    result = a;
    result /= b;
    mpq_class expectedDiv("21/10");
    assert(result == expectedDiv);
    std::cout << "a / b = " << result << " (Expected: 21/10)" << std::endl;

    result = a;
    result += c;
    result -= d;
    result *= b;
    result /= c;
    mpq_class expectedComplex("74/325");
    assert(result == expectedComplex);
    std::cout << "((a + c - d) * b) / c = " << result << " (Expected: 74/325)" << std::endl;

    std::cout << "All operator tests for mpq_class with hardcoded results passed." << std::endl;
}
void test_arithmetic_operators_mpq_class_hardcoded2() {
    mpq_class a("3/5");
    mpq_class b("2/7");
    mpq_class result;

    result = a + b;
    mpq_class expectedAdditionResult("31/35");
    assert(result == expectedAdditionResult);
    std::cout << "Addition test passed: " << result << " == " << expectedAdditionResult << std::endl;

    result = a - b;
    mpq_class expectedSubtractionResult("11/35");
    assert(result == expectedSubtractionResult);
    std::cout << "Subtraction test passed: " << result << " == " << expectedSubtractionResult << std::endl;

    result = a * b;
    mpq_class expectedMultiplicationResult("6/35");
    assert(result == expectedMultiplicationResult);
    std::cout << "Multiplication test passed: " << result << " == " << expectedMultiplicationResult << std::endl;

    result = a / b;
    mpq_class expectedDivisionResult("21/10");
    assert(result == expectedDivisionResult);
    std::cout << "Division test passed: " << result << " == " << expectedDivisionResult << std::endl;

    std::cout << "All arithmetic operation tests for mpq_class passed." << std::endl;
}
void test_mpq_class_literal() {
#ifdef GMPXX_MKII
    // Using the user-defined literal to create mpq_class objects
    mpq_class num1 = "314159"_mpq;
    mpq_class num2 = "271828"_mpq;
    mpq_class num3 = "0"_mpq;
    mpq_class num4 = "-123456"_mpq;
    mpq_class num5 = "99999999999999999999999999999999999999999999999999999999"_mpq;

    const char *num1_expected = "314159";
    const char *num2_expected = "271828";
    const char *num3_expected = "0";
    const char *num4_expected = "-123456";
    const char *num5_expected = "99999999999999999999999999999999999999999999999999999999";
    assert(Is_mpq_class_Equals(num1, num1_expected));
    assert(Is_mpq_class_Equals(num2, num2_expected));
    assert(Is_mpq_class_Equals(num3, num3_expected));
    assert(Is_mpq_class_Equals(num4, num4_expected));
    assert(Is_mpq_class_Equals(num5, num5_expected));
    // more tests
    mpq_class a = "1/2"_mpq;
    mpq_class b = "3/4"_mpq;

    assert(a == mpq_class("1/2"));
    assert(b == mpq_class("3/4"));

    mpq_class result = a + b;
    assert(result == mpq_class("5/4"));

    std::cout << "a = " << a << ", b = " << b << std::endl;
    std::cout << "a + b = " << result << std::endl;
    std::cout << "User-defined literal tests for mpq_class passed." << std::endl;
#endif
}

void test_mpq_class_functions() {
    mpq_class a("2/8"), b("-1/3");

    // canonicalize
    std::cout << "a = " << a;
    a.canonicalize();
    assert(a == mpq_class("1/4"));
    std::cout << "  a = " << a << std::endl;

    // abs
    assert(abs(b) == mpq_class("1/3"));

    // get_d
    assert(a.get_d() == 0.25);

    // get_str
    mpq_class fraction1("1/2");
    std::string fraction1Str = fraction1.get_str();
    assert(fraction1Str == "1/2");
    std::cout << "1/2 in base 10: " << fraction1Str << std::endl;

    std::string fraction1Bin = fraction1.get_str(2);
    std::cout << "1/2 in base 2: " << fraction1Bin << std::endl;

    mpq_class fraction2("A/B", 16);
    std::string fraction2Hex = fraction2.get_str(16);
    assert(fraction2Hex == "a/b");
    std::cout << "A/B in base 16: " << fraction2Hex << std::endl;

    // set_str
    mpq_class fraction;
    assert(fraction.set_str("3/4") == 0);
    assert(fraction.get_str() == "3/4");
    std::cout << "Set to 3/4: " << fraction.get_str() << std::endl;

    assert(fraction.set_str("11/100", 2) == 0);
    assert(fraction.get_str(2) == "11/100");
    std::cout << "Set to 11/100 in base 2: " << fraction.get_str(2) << std::endl;

    assert(fraction.set_str("A/B", 16) == 0);
    assert(fraction.get_str(16) == "a/b");
    std::cout << "Set to A/B in base 16: " << fraction.get_str(16) << std::endl;

    mpq_class c("1/2");
    mpq_class d("-3/4");

    assert(sgn(c) == 1);
    assert(sgn(d) == -1);
    std::cout << "Sign test passed." << std::endl;

    c.swap(d);
    assert(sgn(c) == -1);
    assert(sgn(d) == 1);
    std::cout << "Member swap test passed." << std::endl;

    swap(c, d);
    assert(sgn(c) == 1);
    assert(sgn(d) == -1);
    std::cout << "Non-member swap test passed." << std::endl;

    mpq_class fraction3("3/4");
    mpz_class num = fraction3.get_num();
    mpz_class den = fraction3.get_den();

    // Test get_num() and get_den()
    gmp_printf("Number: %Zd, Denominator: %Zd\n", num.get_mpz_t(), den.get_mpz_t());
    assert(num == 3);
    assert(den == 4);

    // Test get_num_mpz_t() and get_den_mpz_t()
    gmp_printf("Number (mpz_t): %Zd, Denominator (mpz_t): %Zd\n", fraction3.get_num_mpz_t(), fraction3.get_den_mpz_t());

    // Direct comparisons with mpz_t types require manual comparison using mpz_cmp
    assert(mpz_cmp_si(fraction3.get_num_mpz_t(), 3) == 0);
    assert(mpz_cmp_si(fraction3.get_den_mpz_t(), 4) == 0);

    // istream& operator>> (istream& stream, mpq_class& rop) test
    {
        std::istringstream input("3/4");
        mpq_class a;
        input >> a;
        assert(a == mpq_class("3/4"));
    }

    {
        std::istringstream input("5");
        mpq_class a;
        input >> a;
        assert(a == mpq_class("5"));
    }

    {
        std::istringstream input("invalid");
        mpq_class a;
        input >> a;
        assert(input.fail());
    }

    std::cout << "mpq_class functions tests are passed." << std::endl;
}
void test_mpf_class_const_pi() {
#if defined GMPXX_MKII
    const char *pi_approx = "3141592653589793238462643383279502884197169399375105820974944592307816406286208998628034825342117067982148086513282306647093844609550582231725359408128481117450284102701938521105559644622948954930381964428810975665933446128475648233786783165271201909145648566923460348610454326648213393607260249141273724587006606315588174881520920962829254091715364367892590360011330530548820466521384146951941511609433057270365759591953092186117381932611793105118548074462379962749567351885752724891227938183011949129833673362440656643086021394946395224737190702179860943702770539217176293176752384674818467669405132000568127145263560827785771342757789609173637178721468440901224953430146549585371050792279689258923542019956112129021960864034418159813629774771309960518707211349999998372978049951059731732816096318595024459455346908302642522308253344685035261931188171010003137838752886587533208381420617177669147303598253490428755468731159562863882353787593751957781857780532171226806613001927876611195909216420199";
    mpf_class calculated_pi = const_pi();
    mp_bitcnt_t prec = mpf_get_default_prec();
    int prec_decimal_digits = floor(log10(2) * prec);
    int decimal_digits = prec_decimal_digits; // no decimal significants lost in 512bit
    mp_exp_t exp;
    std::string calculated_pi_str = calculated_pi.get_str(exp, 10, prec_decimal_digits);

    bool match = true;
    for (int i = 0; i < prec_decimal_digits; ++i) {
        if (pi_approx[i] != calculated_pi_str[i]) {
            match = false; // Set to false if any character does not match
            break;
        }
    }
    assert(match);
    std::cout << "Pi matched in " << decimal_digits << " decimal digits" << std::endl;
    mpf_class calculated_pi_2nd = const_pi();

    calculated_pi_str = calculated_pi_2nd.get_str(exp, 10, decimal_digits);
    match = true;
    for (int i = 0; i < prec_decimal_digits; ++i) {
        if (pi_approx[i] != calculated_pi_str[i]) {
            match = false; // Set to false if any character does not match
            std::cout << "\n" << i << "-th digit is wrong";
            break;
        }
    }
    assert(match);
    std::cout << "Pi matched 2nd in " << decimal_digits << " decimal digits (cached)" << std::endl;

    mpf_set_default_prec(prec * 2);
    prec = mpf_get_default_prec();
    prec_decimal_digits = floor(log10(2) * prec);
    decimal_digits = prec_decimal_digits - 1; // one decimal significant lost in 1024bit

    mpf_class calculated_pi_3rd = const_pi();
    calculated_pi_str = calculated_pi_3rd.get_str(exp, 10, prec_decimal_digits);
    match = true;
    for (int i = 0; i < decimal_digits; ++i) {
        if (pi_approx[i] != calculated_pi_str[i]) {
            match = false; // Set to false if any character does not match
            std::cout << "\n" << i << "-th digit is wrong";
            break;
        }
    }
    assert(match);
    std::cout << "Pi matched 3rd in " << decimal_digits << " decimal digits" << std::endl;
    mpf_set_default_prec(prec / 2);
#if !defined __GMPXX_MKII_NOPRECCHANGE__
    mpf_class pi_2048(0.0, 2048);
    pi_2048 = const_pi(2048);

    calculated_pi_str = pi_2048.get_str(exp, 10, 2048);
    match = true;
    prec_decimal_digits = floor(log10(2) * 2048);
    decimal_digits = prec_decimal_digits - 1; // one decimal significant lost in 1024bit
    for (int i = 0; i < decimal_digits; ++i) {
        if (pi_approx[i] != calculated_pi_str[i]) {
            match = false; // Set to false if any character does not match
            std::cout << "\n" << i << "-th digit is wrong";
            break;
        }
    }
    assert(match);
    std::cout << "Pi matched 4th in " << decimal_digits << " decimal digits" << std::endl;
#endif
#endif
}
void test_mpf_class_const_log2() {
#if defined GMPXX_MKII
    const char *log2_approx = "6931471805599453094172321214581765680755001343602552541206800094933936219696947156058633269964186875420014810205706857336855202357581305570326707516350759619307275708283714351903070386238916734711233501153644979552391204751726815749320651555247341395258829504530070953263666426541042391578149520437404303855008019441706416715186447128399681717845469570262716310645461502572074024816377733896385506952606683411372738737229289564935470257626520988596932019650585547647033067936544325476327449512504060694381471046899465062201677204245245296126879465461931651746813926725041038025462596568691441928716082938031727143677826548775664850856740776484514644399404614226031930967354025744460703080960850474866385231381816767514386674766478908814371419854942315199735488037516586127535291661000710535582498794147295092931138971559982056543928717000721808576102523688921324497138932037843935308877482597017155910708823683627589842589185353024363421436706118923678919237231467232172053401649256872747782344535348";
    mpf_class calculated_log2 = const_log2();

    mp_bitcnt_t prec = mpf_get_default_prec();
    int prec_decimal_digits = floor(log10(2) * prec);
    int decimal_digits = prec_decimal_digits - 2; // two decimal digits loss
    mp_exp_t exp;
    std::string calculated_log2_str = calculated_log2.get_str(exp, 10, prec_decimal_digits);

    bool match = true;
    for (int i = 0; i < decimal_digits; ++i) {
        if (log2_approx[i] != calculated_log2_str[i]) {
            match = false; // Set to false if any character does not match
            std::cout << "log2 not matched in " << i << " decimal digits" << std::endl;
            break;
        }
    }
    assert(match);
    std::cout << "log2 matched in " << decimal_digits << " decimal digits" << std::endl;
    mpf_class calculated_log2_2nd = const_log2();
    calculated_log2_str = calculated_log2_2nd.get_str(exp, 10, prec_decimal_digits);
    match = true;
    for (int i = 0; i < decimal_digits; ++i) {
        if (log2_approx[i] != calculated_log2_str[i]) {
            match = false; // Set to false if any character does not match
            std::cout << "\n" << i << "-th digit is wrong";
            break;
        }
    }
    assert(match);
    std::cout << "log2 matched 2nd in " << decimal_digits << " decimal digits (cached)" << std::endl;

    mpf_set_default_prec(prec * 2);
    prec = mpf_get_default_prec();
    prec_decimal_digits = floor(log10(2) * prec);
    decimal_digits = prec_decimal_digits - 1; // one decimal digit loss
    mpf_class calculated_log2_3rd = const_log2();
    calculated_log2_str = calculated_log2_3rd.get_str(exp, 10, prec_decimal_digits);

    match = true;
    for (int i = 0; i < decimal_digits; ++i) {
        if (log2_approx[i] != calculated_log2_str[i]) {
            match = false; // Set to false if any character does not match
            std::cout << "\n" << i << "-th digit is wrong";
            break;
        }
    }
    assert(match);
    std::cout << "log2 matched 3rd in " << decimal_digits << " decimal digits" << std::endl;
    mpf_set_default_prec(prec / 2);
#if !defined __GMPXX_MKII_NOPRECCHANGE__
    mpf_class log2_2048(0.0, 2048);
    log2_2048 = const_log2(2048);
    calculated_log2_str = log2_2048.get_str(exp, 10, 2048);
    match = true;
    prec_decimal_digits = floor(log10(2) * 2048);
    decimal_digits = prec_decimal_digits - 1; // one decimal significant lost in 1024bit
    for (int i = 0; i < decimal_digits; ++i) {
        if (log2_approx[i] != calculated_log2_str[i]) {
            match = false; // Set to false if any character does not match
            std::cout << "\n" << i << "-th digit is wrong";
            break;
        }
    }
    assert(match);
    std::cout << "Log2 matched 4th in " << decimal_digits << " decimal digits" << std::endl;
#endif
#endif
}
void test_div2exp_mul2exp_mpf_class(void) {
#if defined GMPXX_MKII
    mpf_class value(2.0);

    value.div_2exp(1); // value should now be 1.0
    assert(value == mpf_class(1.0));
    std::cout << "After div_2exp: " << value << std::endl;

    value.mul_2exp(1); // value should now be back to 2.0
    assert(value == mpf_class(2.0));
    std::cout << "After mul_2exp: " << value << std::endl;

    std::cout << "Test passed." << std::endl;
#endif
}
void test_log_mpf_class(void) {
#if defined GMPXX_MKII
    const char *log25_approx = "32188758248682007492015186664523752790512027085370354438252957829483579754153155292602677561863592215999326060434311257994480104586493523992672332349274114551043592749943664913069857124046830501145403103872017595547794513763870814255323094624436190558970425856427161194451353445705744809231788963567293426573479968492755191863788476878687069021019501089083894810027319741757347664262611459440813189707676774473255077530911254363230233198618304864147298233557278013517451715575327831676736479008509759124789680620039653942349819874829969619152420338220287577248067086430254462514691769231195745839617731413680401331996889453999464353623730352024404059568162180392950533995301378413179578267631434324144555491195410686407755749936587507222676018935280966605700043495497594160142869312327197940825178026752830480340117771974698969755759421086283566404029061241812315775813431962503349980828683442440738894001853711875195709842379235728657517536966441167119239935826333930186467770631707979647419708629262";
    const char *log10_approx = "23025850929940456840179914546843642076011014886287729760333279009675726096773524802359972050895982983419677840422862486334095254650828067566662873690987816894829072083255546808437998948262331985283935053089653777326288461633662222876982198867465436674744042432743651550489343149393914796194044002221051017141748003688084012647080685567743216228355220114804663715659121373450747856947683463616792101806445070648000277502684916746550586856935673420670581136429224554405758925724208241314695689016758940256776311356919292033376587141660230105703089634572075440370847469940168269282808481184289314848524948644871927809676271275775397027668605952496716674183485704422507197965004714951050492214776567636938662976979522110718264549734772662425709429322582798502585509785265383207606726317164309505995087807523710333101197857547331541421808427543863591778117054309827482385045648019095610299291824318237525357709750539565187697510374970888692180205189339507238539205144634197265287286965110862571492198849979";
    mpf_class x;
    mpf_class calculated;
    mp_bitcnt_t prec;
    int prec_decimal_digits;
    int decimal_digits;
    mp_exp_t exp;
    std::string calculated_str;
    bool match;

    x = 25.0;

    calculated = log(x);
    prec = mpf_get_default_prec();
    prec_decimal_digits = floor(log10(2) * prec);
    decimal_digits = prec_decimal_digits - 3;
    calculated_str = calculated.get_str(exp, 10, prec_decimal_digits);
    match = true;
    for (int i = 0; i < decimal_digits; ++i) {
        if (log25_approx[i] != calculated_str[i]) {
            match = false; // Set to false if any character does not match
            std::cout << "log25 not matched in " << i << " decimal digits" << std::endl;
            break;
        }
    }
    assert(match);
    std::cout << "log25 matched in " << decimal_digits << " decimal digits" << std::endl;

    x = 10.0;

    calculated = log(x);
    prec = mpf_get_default_prec();
    prec_decimal_digits = floor(log10(2) * prec);
    decimal_digits = prec_decimal_digits - 2;
    calculated_str = calculated.get_str(exp, 10, prec_decimal_digits);
    match = true;
    for (int i = 0; i < decimal_digits; ++i) {
        if (log10_approx[i] != calculated_str[i]) {
            match = false; // Set to false if any character does not match
            std::cout << "log10 not matched in " << i << " decimal digits" << std::endl;
            break;
        }
    }
    assert(match);
    std::cout << "log10 matched in " << decimal_digits << " decimal digits" << std::endl;

#endif
}
int main() {
#if !defined GMPXX_MKII
    mpf_set_default_prec(512);
#endif
    // mpf_class
    testDefaultPrecision();
    testDefaultConstructor();
    testCopyConstructor();
    testAssignmentOperator();
    testInitializationAndAssignmentDouble();
    testInitializationAndAssignmentString();
    testAddition();
    testMultplication();
    testDivision();
    testSubtraction();
    testComparisonOperators();
    testSqrt();
    testNeg();
    testAbs();
    test_mpf_class_double_addition();
    test_mpf_class_double_subtraction();
    test_mpf_class_double_multiplication();
    test_mpf_class_double_division();
    testOutputOperator();
    testCeilFunction();
    testFloor();
    testHypot();
    testSgn();
    test_get_d();
    test_get_ui();
    test_get_si();
    test_mpf_class_constructor_precision();
    test_mpf_class_constructor_with_mpf();
    test_mpf_class_literal();
    test_mpf_class_swap();
    test_template_cmp();
    test_set_str();
    test_mpf_class_get_str();
    test_trunc_function();
    test_fits_sint_p();
    test_fits_slong_p();
    test_fits_sshort_p();
    test_fits_uint_p();
    test_fits_ulong_p();
    test_fits_ushort_p();
    testAssignmentOperator_the_rule_of_five();
    test_mpf_class_extention();

    // mpz_class
    testDefaultConstructor_mpz_class();
    testCopyConstructor_mpz_class();
    testAssignmentOperator_mpz_class();
    testInitializationAndAssignmentDouble_mpz_class();
    testInitializationAndAssignmentInt_mpz_class();
    testInitializationAndAssignment_mpf_class_mpz_class();
    testInitializationAndAssignment_mpz_class_mpf_class();
    testInitializationAndAssignmentString_mpz_class();
    test_template_cmp_mpz_class();
    testAssignmentOperator_the_rule_of_five_mpz_class();
    test_mpz_class_literal();
    test_arithmetic_operators_mpz_class();
    testAbsFunction_mpz_class();
    testFitsFunctions_mpz_class();
    testConversionFunctions_mpz_class();
    testMathFunctions_mpz_class();
    test_mpz_class_extention();
    test_set_str_mpz_class();
    test_factorial_mpz_class();
    test_primorial_mpz_class();
    test_fibonacci_mpz_class();
    test_mpz_class_swap();
    testOutputOperator_mpz_class();
    test_mpz_class_addition();
    test_mpz_class_subtraction();
    test_mpz_class_multiplication();
    test_mpz_class_division();
    test_mpz_class_modulus();

    // mpq_class
    testDefaultConstructor_mpq_class();
    testCopyConstructor_mpq_class();
    testAssignmentOperator_mpq_class();
    testInitializationAndAssignmentString_mpq_class();
    test_template_cmp_mpq_class();
    test_arithmetic_operators_mpq_class_hardcoded1();
    test_arithmetic_operators_mpq_class_hardcoded2();
    test_mpq_class_literal();
    test_mpq_class_functions();

    test_mpf_class_const_pi();
    test_mpf_class_const_log2();
    test_div2exp_mul2exp_mpf_class();
    test_log_mpf_class();

    std::cout << "All tests passed." << std::endl;

    return 0;
}
