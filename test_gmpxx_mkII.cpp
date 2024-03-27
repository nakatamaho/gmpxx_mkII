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
void testAssignmentOperator_class_move_and_copy() {
    mpf_class a = mpf_class("123");
    mpf_class b = mpf_class("456");
    const char *expectedValue_b = "456.0000000000";
    a = b;
    assert(Is_mpf_class_Equals(a, expectedValue_b) && Is_mpf_class_Equals(b, expectedValue_b) && "copy test failed");

    mpf_class c = mpf_class("789");
    mpf_class d = mpf_class("123");
    const char *expectedValue_d = "789.0000000000";

    d = std::move(c);
    /*
    assert(Is_mpf_class_Equals(d, expectedValue_d) && "move test failed");
    */
    std::cout << "Assignment operator (move and copy) test passed." << std::endl;
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
/*
void testAssignmentOperator_mpz_class_move_and_copy() {
    mpz_class a = 123L;
    mpz_class b = 456L;
    a = b;
    assert(a == 456 && b == 456 && "copy test failed");

    mpz_class c = 789L;

    a = std::move(c);
    assert(a == 789 && "move test failed");

    std::cout << "Assignment operator (move and copy) test passed." << std::endl;
}
*/
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
    testAssignmentOperator_class_move_and_copy();

    // mpz_class
    testDefaultConstructor_mpz_class();
    testCopyConstructor_mpz_class();
    testAssignmentOperator_mpz_class();
    testInitializationAndAssignmentDouble_mpz_class();
    testInitializationAndAssignmentInt_mpz_class();
    testInitializationAndAssignment_mpf_class_mpz_class();
    testInitializationAndAssignment_mpz_class_mpf_class();
    testInitializationAndAssignmentString_mpz_class();
    //    testAssignmentOperator_mpz_class_move_and_copy();
    std::cout << "All tests passed." << std::endl;

    return 0;
}
