/*
 * Copyright (c) 2024, 2025
 *      Nakata, Maho
 *      All rights reserved.
 *
 *
 * The test_gmpxx_mkII.cpp is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at your
 * option) any later version.
 *
 * The test_gmpxx_mkII.cpp is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with the test_gmpxx_mkII.cpp; see the file LICENSE.  If not, see
 * http://www.gnu.org/licenses/ or write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include <iostream>
#include <cassert>
#include <cstring>
#include <string>
#include <iomanip>
#include <cmath>
#include <vector>
#include <cstdint>

#if defined USE_ORIGINAL_GMPXX
#include <gmpxx.h>
#else
#include "gmpxx_mkII.h"
#if !defined ___GMPXX_STRICT_COMPATIBILITY___
using namespace gmpxx;
#endif
#endif

std::string to_hex_sci(const mpf_class &val) {
    if (val == 0)
        return "0.0@+00"; // Handle zero separately

    // Get the double representation of the value
    double d = val.get_d();
    int exponent;
    double fraction = frexp(d, &exponent); // fraction * 2^exponent = d

    // Adjust the exponent to base 16
    exponent = (exponent - 1) / 4 + 1;
    fraction *= std::pow(2, (exponent * 4 - 4)); // Adjust fraction for base 16

    // Construct the scientific notation string
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "%.1f@+%02d", fraction, exponent);

    // Convert to a string and replace decimal point to hexadecimal point
    std::string result = buffer;
    size_t pos = result.find('.');
    if (pos != std::string::npos) {
        result[pos] = 'p'; // Use 'p' to denote hex power
    }
    return result;
}
std::string insertDecimalPoint(const std::string &str, signed long int exp) {
    std::string result;
    std::string number = str;
    bool isNegative = false;

    if (str[0] == '-') {
        isNegative = true;
        number = str.substr(1);
    }

    if (exp == 0) {
        result = "0." + number;
    } else {
        if (exp >= (signed long int)number.length()) {
            result = number;
        } else {
            if (exp >= 0)
                result = number.substr(0, exp) + "." + number.substr(exp);
            else
                result = "0." + std::string(std::abs(exp), '0') + number;
        }
    }

    if (isNegative) {
        result = "-" + result;
    }

    return result;
}
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
#if !defined ___GMPXX_STRICT_COMPATIBILITY___ && !defined USE_ORIGINAL_GMPXX
    mpf_class f("1.5");
    mp_bitcnt_t defaultPrec = gmpxx_defaults::get_default_prec();
    assert(defaultPrec == f.get_prec());
    std::cout << "Default precision: " << f.get_prec() << " test passed." << std::endl;

    gmpxx_defaults::set_default_prec(1024);
    defaultPrec = gmpxx_defaults::get_default_prec();
    assert(defaultPrec == 1024);
    mpf_class g("2.5");
    std::cout << "Set and get precision: " << g.get_prec() << " test passed." << std::endl;

    gmpxx_defaults::set_default_prec(512);
    defaultPrec = gmpxx_defaults::get_default_prec();
    mpf_class h("1.5");
    assert(defaultPrec == 512);
    std::cout << "Now get back to precision: " << h.get_prec() << " test passed." << std::endl;
    std::cout << "testDefaultPrecision passed" << std::endl;
#endif
}
void testDefaultConstructor() {
    mpf_class a;
    char buffer[100];
    gmp_snprintf(buffer, sizeof(buffer), "%.0Ff", a.get_mpf_t());
    assert(std::string(buffer) == "0");
    // initialized to zero
    std::cout << "testDefaultConstructor passed." << std::endl;
}
void testCopyConstructor() {
    mpf_class a;
    mpf_class b = a;
    assert(true);
    std::cout << "testCopyConstructor passed." << std::endl;
}
void testAssignmentOperator() {
    mpf_class a;
    mpf_class b;
    b = a;
    assert(true);
    std::cout << "testAssignmentOperator passed." << std::endl;
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
    std::cout << "testAssignmentOperator_the_rule_of_five passed" << std::endl;
}
void testInitializationAndAssignmentDouble() {
    double testValue = 3.1415926535;
    const char *expectedValue = "3.1415926535";

    mpf_class a = (mpf_class)testValue;
    assert(Is_mpf_class_Equals(a, expectedValue));

    mpf_class b;
    b = testValue;
    assert(Is_mpf_class_Equals(b, expectedValue));
    std::cout << "testInitializationAndAssignmentDouble passed" << std::endl;
}
void testInitializationAndAssignmentString() {
#if !defined USE_ORIGINAL_GMPXX
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
    mpf_class e(inputHexValue, gmpxx_defaults::get_default_prec(), 16);
    assert(Is_mpf_class_Equals(e, expectedHexValue, false, 12, 16));
    std::cout << "Assignment initialization with hexadecimal '" << expectedHexValue << "' test passed." << std::endl;

    gmpxx_defaults::base = 16;
    // Testing initialization with a hexadecimal number using a constructor
    mpf_class f;
    e = inputHexValue;
    assert(Is_mpf_class_Equals(e, expectedHexValue, false, 12, gmpxx_defaults::base));
    std::cout << "Constructor initialization with hexadecimal '" << expectedHexValue << "' test passed." << std::endl;
    gmpxx_defaults::base = 10;
    std::cout << "testInitializationAndAssignmentString passed" << std::endl;
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
    std::cout << "testAddition passed." << std::endl;
}
void testMultplication() {
    mpf_class a(2.0);
    mpf_class b(3.0);
    std::string expected = "6.0000000000";

    mpf_class c = a * b;
    assert(Is_mpf_class_Equals(c, expected.c_str()));
    a *= b;
    assert(Is_mpf_class_Equals(a, expected.c_str()));
    std::cout << "testMultplication passed." << std::endl;
}
void testDivision() {
    mpf_class a(6.0);
    mpf_class b(2.0);
    std::string expected = "3.0000000000";

    mpf_class c = a / b;
    assert(Is_mpf_class_Equals(c, expected.c_str()));
    a /= b;
    assert(Is_mpf_class_Equals(a, expected.c_str()));
    std::cout << "testDivision passed." << std::endl;
}
void testSubtraction() {
    mpf_class a(5.0);
    mpf_class b(2.0);
    std::string expected = "3.0000000000";

    mpf_class c = a - b;
    assert(Is_mpf_class_Equals(c, expected.c_str()));
    a -= b;
    assert(Is_mpf_class_Equals(a, expected.c_str()));
    std::cout << "testSubtraction passed." << std::endl;
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

    std::cout << "testComparisonOperators passed." << std::endl;
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
    std::cout << "testSqrt passed." << std::endl;
}
void testNeg() {
#if !defined USE_ORIGINAL_GMPXX
    mpf_class a(-3.5);
    mpf_class result = neg(a);
    mpf_class expected = "3.5";
    assert(result == expected);
    std::cout << "testNeg passed." << std::endl;
#endif
}
void testAbs() {
    mpf_class a(-3.5);
    mpf_class c = abs(a);
    mpf_class expected("3.5");
    mpf_class result = abs(a);
    assert(result == expected);
    std::cout << "testAbs passed." << std::endl;
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
    std::cout << "test_mpf_class_double_addition passed." << std::endl;
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
    std::cout << "test_mpf_class_double_subtraction passed." << std::endl;
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
    std::cout << "test_mpf_class_double_multiplication passed." << std::endl;
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
    std::cout << "test_mpf_class_double_division passed." << std::endl;
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
    std::cout << "testOutputOperator passed." << std::endl;
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

    std::cout << "testCeilFunction passed." << std::endl;
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

    std::cout << "testFloor passed." << std::endl;
}
void testHypot() {
    mpf_class op1(3.0);
    mpf_class op2(4.0);
    const char *expected = "5.0000000000";
    mpf_class result;

    // Since mpf_hypot doesn't exist, let's assume a correct implementation for hypot
    result = hypot(op1, op2);
    assert(Is_mpf_class_Equals(result, expected));

    std::cout << "testHypot passed." << std::endl;
}
void testSgn() {
    mpf_class positive(123.456);
    mpf_class negative(-123.456);
    mpf_class zero(0.0);

    assert(sgn(positive) > 0);
    assert(sgn(negative) < 0);
    assert(sgn(zero) == 0);

    std::cout << "testSgn passed." << std::endl;
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

    std::cout << "test_get_d passed." << std::endl;
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

    std::cout << "test_get_ui passed." << std::endl;
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

    std::cout << "test_get_si passed." << std::endl;
}
void test_mpf_class_constructor_precision() {
#if !defined ___GMPXX_MKII_NOPRECCHANGE___
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

    std::cout << "test_mpf_class_constructor_with_mpf passed." << std::endl;
}
void test_mpf_class_literal() {
    // Using the user-defined literal to create mpf_class objects
    mpf_class num1 = 3.14159_mpf;
    mpf_class num2 = 2.71828_mpf;
    mpf_class num3 = 0.0_mpf;
    mpf_class num4 = -123.456_mpf;

    assert(Is_mpf_class_Equals(num1, "3.1415900000"));
    assert(Is_mpf_class_Equals(num2, "2.7182800000"));
    assert(Is_mpf_class_Equals(num3, "0.0000000000"));
    assert(Is_mpf_class_Equals(num4, "-123.4560000000"));

#if defined GMPXX_MKII
    mpf_class num5 = "2.23606"_mpf;
    mpf_class num6 = "1.73205"_mpf;
    mpf_class num7 = "8888.0"_mpf;
    mpf_class num8 = "-456.123"_mpf;

    assert(Is_mpf_class_Equals(num5, "2.2360600000"));
    assert(Is_mpf_class_Equals(num6, "1.7320500000"));
    assert(Is_mpf_class_Equals(num7, "8888.0000000000"));
    assert(Is_mpf_class_Equals(num8, "-456.1230000000"));
#endif
    std::cout << "test_mpf_class_literal passed." << std::endl;
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

    std::cout << "test_mpf_class_swap passed." << std::endl;
}
void test_template_cmp() {
    mpf_class num1(3.14);
    assert(cmp(num1, 3.14) == 0);
    assert(cmp(3.14, num1) == 0);
    assert(cmp(num1, 2.71f) > 0);
    assert(cmp(2.71f, num1) < 0);
    assert(cmp(num1, 3UL) > 0);
    assert(cmp(3L, num1) < 0);

    std::cout << "test_template_cmp passed." << std::endl;
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

    std::cout << "test_set_str passed." << std::endl;
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

    std::cout << "test_mpf_class_get_str passed." << std::endl;
}
void test_trunc_function() {
    mpf_class num1(3.14159);
    mpf_class truncated1 = trunc(num1);
    assert(mpf_cmp_d(truncated1.get_mpf_t(), 3.0) == 0); // trancate 3.14159 => 3

    mpf_class num2(-3.14159);
    mpf_class truncated2 = trunc(num2);
    assert(mpf_cmp_d(truncated2.get_mpf_t(), -3.0) == 0); // trancate -3.14159 => -3

    std::cout << "test_trunc_function passed." << std::endl;
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

    std::cout << "test_fits_sint_p passed." << std::endl;
}
void test_fits_slong_p() {
    mpf_class value(std::to_string(INT_MAX));
    assert(value.fits_slong_p() == true);

    mpf_class outOfRange(std::to_string(LONG_MAX));
    outOfRange += 1;
    assert(outOfRange.fits_slong_p() == false);

    std::cout << "test_fits_slong_p passed." << std::endl;
}
void test_fits_sshort_p() {
    mpf_class value(std::to_string(SHRT_MAX));
    assert(value.fits_sshort_p() == true);

    mpf_class outOfRange(std::to_string(SHRT_MAX));
    outOfRange += 1;
    assert(outOfRange.fits_sshort_p() == false);

    std::cout << "test_fits_sshort_p passed." << std::endl;
}
void test_fits_uint_p() {
    mpf_class value(std::to_string(UINT_MAX));
    assert(value.fits_uint_p() == true);

    mpf_class outOfRange = value + 1;
    assert(outOfRange.fits_uint_p() == false);

    std::cout << "test_fits_uint_p passed." << std::endl;
}
void test_fits_ulong_p() {
    mpf_class value(std::to_string(ULONG_MAX));
    assert(value.fits_ulong_p() == true);

    mpf_class outOfRange = value + 1;
    assert(outOfRange.fits_ulong_p() == false);

    std::cout << "test_fits_ulong_p passed." << std::endl;
}
void test_fits_ushort_p() {
    mpf_class value(std::to_string(USHRT_MAX));
    assert(value.fits_ushort_p() == true);

    mpf_class outOfRange = value + 1;
    assert(outOfRange.fits_ushort_p() == false);

    std::cout << "test_fits_ushort_p passed." << std::endl;
}
void testDefaultConstructor_mpz_class() {
    mpz_class a;
    char buffer[100];
    gmp_snprintf(buffer, sizeof(buffer), "%Zd", a.get_mpz_t());
    assert(std::string(buffer) == "0");
    // initialized to zero
    std::cout << "testDefaultConstructor_mpz_class passed." << std::endl;
}
void testCopyConstructor_mpz_class() {
    mpz_class a;
    mpz_class b = a;
    assert(true);
    std::cout << "testCopyConstructor_mpz_class passed." << std::endl;
}
void testAssignmentOperator_mpz_class() {
    mpz_class a;
    mpz_class b;
    b = a;
    assert(true);
    std::cout << "testAssignmentOperator_mpz_class passed." << std::endl;
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
    std::cout << "testAssignmentOperator_the_rule_of_five_mpz_class passed." << std::endl;
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
    std::cout << "testInitializationAndAssignmentDouble_mpz_class passed." << std::endl;
}
// deprecated, must be replaced with the following tests.
//     test_int64_t_uint64_t_constructor();
//     test_int32_t_uint32_t_constructor();
//     test_int64_t_uint64_t_int32_t_uint32_t_assignment();
void testInitializationAndAssignment_int64_t_uint64_t_mpz_class() {
#if !defined USE_ORIGINAL_GMPXX
    int64_t testValue = INT64_C(-9223372036854775807);
    const char *expectedValue = "-9223372036854775807";

    mpz_class a = (mpz_class)testValue;
    assert(Is_mpz_class_Equals(a, expectedValue, true));
    std::cout << "Substitution from signed long int using constructor test passed." << std::endl;

    mpz_class b;
    b = testValue;
    assert(Is_mpz_class_Equals(b, expectedValue, true));
    std::cout << "Substitution from signed long int using assignment test passed." << std::endl;

    uint64_t testValue2 = UINT64_C(18446744073709551614);
    const char *expectedValue2 = "18446744073709551614";

    mpz_class c = (mpz_class)testValue2;
    assert(Is_mpz_class_Equals(c, expectedValue2, true));
    std::cout << "Substitution from unsigned long int using constructor test passed." << std::endl;

    mpz_class d;
    d = testValue2;
    assert(Is_mpz_class_Equals(d, expectedValue2, true));
    std::cout << "Substitution from int64_t using assignment test passed." << std::endl;
    std::cout << "testInitializationAndAssignment_int64_t_uint64_t_mpz_class passed." << std::endl;
#endif
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
    std::cout << "testInitializationAndAssignment_mpz_class_mpf_class passed." << std::endl;
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
    std::cout << "testInitializationAndAssignment_mpf_class_mpz_class passed." << std::endl;
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
    std::cout << "testInitializationAndAssignmentString_mpz_class passed." << std::endl;
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

    std::cout << "test_template_cmp_mpz_class passed." << std::endl;
}
void test_mpz_class_literal() {
    // Using the user-defined literal to create mpz_class objects
    mpz_class num1 = 8801_mpz;
    mpz_class num2 = 6809_mpz;
    mpz_class num3 = 0_mpz;
    mpz_class num4 = -123456789_mpz;
    mpz_class num5 = 4294967295_mpz;

    const char *num1_expected = "8801";
    const char *num2_expected = "6809";
    const char *num3_expected = "0";
    const char *num4_expected = "-123456789";
    const char *num5_expected = "4294967295";
    assert(Is_mpz_class_Equals(num1, num1_expected));
    assert(Is_mpz_class_Equals(num2, num2_expected));
    assert(Is_mpz_class_Equals(num3, num3_expected));
    assert(Is_mpz_class_Equals(num4, num4_expected));
    assert(Is_mpz_class_Equals(num5, num5_expected));
#if defined GMPXX_MKII
    mpz_class num6 = "314159"_mpz;
    mpz_class num7 = "271828"_mpz;
    mpz_class num8 = "0"_mpz;
    mpz_class num9 = "-123456"_mpz;
    mpz_class num10 = "99999999999999999999999999999999999999999999999999999999"_mpz;

    const char *num6_expected = "314159";
    const char *num7_expected = "271828";
    const char *num8_expected = "0";
    const char *num9_expected = "-123456";
    const char *num10_expected = "99999999999999999999999999999999999999999999999999999999";
    assert(Is_mpz_class_Equals(num6, num6_expected));
    assert(Is_mpz_class_Equals(num7, num7_expected));
    assert(Is_mpz_class_Equals(num8, num8_expected));
    assert(Is_mpz_class_Equals(num9, num9_expected));
    assert(Is_mpz_class_Equals(num10, num10_expected));
#endif
    std::cout << "test_mpz_class_literal passed." << std::endl;
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
    std::cout << "test_arithmetic_operators_mpz_class passed." << std::endl;
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

    std::cout << "testFitsFunctions_mpz_class passed." << std::endl;
}

void testAbsFunction_mpz_class() {
    mpz_class pos("123");
    mpz_class neg("-456");
    mpz_class zero("0");

    assert(abs(pos) == pos);
    assert(abs(neg) == mpz_class("456"));
    assert(abs(zero) == zero);

    std::cout << "testAbsFunction_mpz_class passed." << std::endl;
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

    std::cout << "testConversionFunctions_mpz_class passed." << std::endl;
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

    std::cout << "testMathFunctions_mpz_class passed." << std::endl;
}
template <class T> T test_func(const T &a, const T &b) { return a * b; }
void test_mpf_class_extention() {
#if !defined USE_ORIGINAL_GMPXX
    mpf_class f(2), g(1), h(3);

    mpf_class result;
    result = test_func(f * h, g);

    std::cout << "The result of test_func(f * h, g) is: " << result << std::endl;
#endif
}
void test_mpz_class_extention() {
#if !defined USE_ORIGINAL_GMPXX
    mpz_class f(2), g(1), h(3);

    mpz_class result;
    result = test_func(f * h, g);

    std::cout << "The result of test_func(f * h, g) is: " << result << std::endl;
    std::cout << "test_mpz_class_extention passed." << std::endl;
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

    std::cout << "test_set_str_mpz_class tests passed." << std::endl;
}
void test_factorial_mpz_class() {
    mpz_class fact5 = factorial((mpz_class)5);
    const char *expectedValue_fact5 = "120";
    assert(Is_mpz_class_Equals(fact5, expectedValue_fact5)); // 5! = 120
    std::cout << "test_factorial_mpz_class passed.\n";
}
void test_primorial_mpz_class() {
    mpz_class primorial5 = primorial((mpz_class)5);
    const char *expectedValue_primorial = "30";
    assert(Is_mpz_class_Equals(primorial5, expectedValue_primorial)); // 2*3*5 = 30
    std::cout << "test_primorial_mpz_class passed.\n";
}
void test_fibonacci_mpz_class() {
    mpz_class fibonacci7 = fibonacci((mpz_class)7);
    const char *expectedValue_fibonacci = "13";
    assert(Is_mpz_class_Equals(fibonacci7, expectedValue_fibonacci)); // F(7) = 13
    std::cout << "test_fibonacci_mpz_class passed.\n";
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

    std::cout << "test_mpz_class_swap passed." << std::endl;
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

    std::cout << "testOutputOperator_mpz_class passed." << std::endl;
}
void test_mpz_class_addition() {
    {
        mpz_class a(1), c;
        uint64_t b = 2;
        const char *expectedValue = "3";

        c = a + b;
        assert(Is_mpz_class_Equals(c, expectedValue));
        c = b + a;
        assert(Is_mpz_class_Equals(c, expectedValue));
        a += b;
        assert(Is_mpz_class_Equals(a, expectedValue));
    }
    {
        mpz_class a(-1), c;
        signed long int b = 2;
        const char *expectedValue = "1";

        c = a + b;
        assert(Is_mpz_class_Equals(c, expectedValue, true));
        c = b + a;
        assert(Is_mpz_class_Equals(c, expectedValue));
        a += b;
        assert(Is_mpz_class_Equals(a, expectedValue));
    }
    {
        mpz_class a(1), c;
        signed long int b = -2;
        const char *expectedValue = "-1";

        c = a + b;
        assert(Is_mpz_class_Equals(c, expectedValue));
        c = b + a;
        assert(Is_mpz_class_Equals(c, expectedValue));
        a += b;
        assert(Is_mpz_class_Equals(a, expectedValue));
    }
    {
        mpz_class a(1), c;
        unsigned int b = 2;
        const char *expectedValue = "3";

        c = a + b;
        assert(Is_mpz_class_Equals(c, expectedValue));
        c = b + a;
        assert(Is_mpz_class_Equals(c, expectedValue));
        a += b;
        assert(Is_mpz_class_Equals(a, expectedValue));
    }
    {
        mpz_class a(-1), c;
        signed int b = 2;
        const char *expectedValue = "1";

        c = a + b;
        assert(Is_mpz_class_Equals(c, expectedValue, true));
        c = b + a;
        assert(Is_mpz_class_Equals(c, expectedValue));
        a += b;
        assert(Is_mpz_class_Equals(a, expectedValue));
    }
    {
        mpz_class a(1), c;
        signed int b = -2;
        const char *expectedValue = "-1";

        c = a + b;
        assert(Is_mpz_class_Equals(c, expectedValue));
        c = b + a;
        assert(Is_mpz_class_Equals(c, expectedValue));
        a += b;
        assert(Is_mpz_class_Equals(a, expectedValue));
    }
    {
        mpz_class a(-1), c;
        double b = 2.0;
        const char *expectedValue = "1";

        c = a + b;
        assert(Is_mpz_class_Equals(c, expectedValue, true));
        c = b + a;
        assert(Is_mpz_class_Equals(c, expectedValue));
        a += b;
        assert(Is_mpz_class_Equals(a, expectedValue));
    }
    std::cout << "test_mpz_class_addition passed." << std::endl;
}
void test_mpz_class_subtraction() {
    {
        mpz_class a(1), c;
        uint64_t b = 2;
        const char *expectedValue = "-1";
        const char *expectedValue1 = "1";

        c = a - b;
        assert(Is_mpz_class_Equals(c, expectedValue));
        c = b - a;
        assert(Is_mpz_class_Equals(c, expectedValue1));
        a -= b;
        assert(Is_mpz_class_Equals(a, expectedValue));
    }
    {
        mpz_class a(-1), c;
        signed long int b = 2;
        const char *expectedValue = "-3";
        const char *expectedValue1 = "3";

        c = a - b;
        assert(Is_mpz_class_Equals(c, expectedValue));
        c = b - a;
        assert(Is_mpz_class_Equals(c, expectedValue1));
        a -= b;
        assert(Is_mpz_class_Equals(a, expectedValue));
    }
    {
        mpz_class a(1), c;
        signed long int b = -2;
        const char *expectedValue = "3";
        const char *expectedValue1 = "-3";

        c = a - b;
        assert(Is_mpz_class_Equals(c, expectedValue));
        c = b - a;
        assert(Is_mpz_class_Equals(c, expectedValue1));
        a -= b;
        assert(Is_mpz_class_Equals(a, expectedValue));
    }
    {
        mpz_class a(1), c;
        unsigned int b = 2;
        const char *expectedValue = "-1";
        const char *expectedValue1 = "1";

        c = a - b;
        assert(Is_mpz_class_Equals(c, expectedValue));
        c = b - a;
        assert(Is_mpz_class_Equals(c, expectedValue1));
        a -= b;
        assert(Is_mpz_class_Equals(a, expectedValue));
    }
    {
        mpz_class a(-1), c;
        signed int b = 2;
        const char *expectedValue = "-3";
        const char *expectedValue1 = "3";

        c = a - b;
        assert(Is_mpz_class_Equals(c, expectedValue));
        c = b - a;
        assert(Is_mpz_class_Equals(c, expectedValue1));
        a -= b;
        assert(Is_mpz_class_Equals(a, expectedValue));
    }
    {
        mpz_class a(1), c;
        signed int b = -2;
        const char *expectedValue = "3";
        const char *expectedValue1 = "-3";

        c = a - b;
        assert(Is_mpz_class_Equals(c, expectedValue));
        c = b - a;
        assert(Is_mpz_class_Equals(c, expectedValue1));
        a -= b;
        assert(Is_mpz_class_Equals(a, expectedValue));
    }
    {
        mpz_class a(-1), c;
        double b = 2.0;
        const char *expectedValue = "-3";
        const char *expectedValue1 = "3";

        c = a - b;
        assert(Is_mpz_class_Equals(c, expectedValue));
        c = b - a;
        assert(Is_mpz_class_Equals(c, expectedValue1));
        a -= b;
        assert(Is_mpz_class_Equals(a, expectedValue));
    }
    std::cout << "test_mpz_class_subtraction passed." << std::endl;
}
void test_mpz_class_multiplication() {
    {
        mpz_class a(3), c;
        const char *expectedValue = "6";
        uint64_t b = 2;

        c = a * b;
        assert(Is_mpz_class_Equals(c, expectedValue));
        c = b * a;
        assert(Is_mpz_class_Equals(c, expectedValue));
        a *= b;
        assert(Is_mpz_class_Equals(a, expectedValue));
    }
    {
        mpz_class a(3), c;
        const char *expectedValue = "6";
        signed long int b = 2;

        c = a * b;
        assert(Is_mpz_class_Equals(c, expectedValue));
        c = b * a;
        assert(Is_mpz_class_Equals(c, expectedValue));
        a *= b;
        assert(Is_mpz_class_Equals(a, expectedValue));
    }
    {
        mpz_class a(3), c;
        const char *expectedValue = "-6";
        signed long int b = -2;

        c = a * b;
        assert(Is_mpz_class_Equals(c, expectedValue));
        c = b * a;
        assert(Is_mpz_class_Equals(c, expectedValue));
        a *= b;
        assert(Is_mpz_class_Equals(a, expectedValue));
    }
    {
        mpz_class a(3), c;
        const char *expectedValue = "6";
        unsigned int b = 2;

        c = a * b;
        assert(Is_mpz_class_Equals(c, expectedValue));
        c = b * a;
        assert(Is_mpz_class_Equals(c, expectedValue));
        a *= b;
        assert(Is_mpz_class_Equals(a, expectedValue));
    }
    {
        mpz_class a(3), c;
        const char *expectedValue = "6";
        signed int b = 2;

        c = a * b;
        assert(Is_mpz_class_Equals(c, expectedValue));
        c = b * a;
        assert(Is_mpz_class_Equals(c, expectedValue));
        a *= b;
        assert(Is_mpz_class_Equals(a, expectedValue));
    }
    {
        mpz_class a(3), c;
        const char *expectedValue = "-6";
        signed int b = -2;

        c = a * b;
        assert(Is_mpz_class_Equals(c, expectedValue));
        c = b * a;
        assert(Is_mpz_class_Equals(c, expectedValue));
        a *= b;
        assert(Is_mpz_class_Equals(a, expectedValue));
    }
    {
        mpz_class a(3), c;
        const char *expectedValue = "-6";
        double b = -2.0;

        c = a * b;
        assert(Is_mpz_class_Equals(c, expectedValue));
        c = b * a;
        assert(Is_mpz_class_Equals(c, expectedValue));
        a *= b;
        assert(Is_mpz_class_Equals(a, expectedValue));
    }
    std::cout << "test_mpz_class_multiplication passed." << std::endl;
}
void test_mpz_class_division() {
    {
        mpz_class a(7), c, d;
        const char *expectedValue = "-3";
        const char *expectedValue1 = "-2";
        mpz_class b(-2);

        c = a / b;
        assert(Is_mpz_class_Equals(c, expectedValue));
        b = -18;
        d = b / a;
        assert(Is_mpz_class_Equals(d, expectedValue1));
        b = -2;
        a /= b;
        assert(Is_mpz_class_Equals(a, expectedValue));
    }
    {
        mpz_class a(6), c, d;
        const char *expectedValue = "3";
        uint64_t b = 2;

        c = a / b;
        assert(Is_mpz_class_Equals(c, expectedValue));
        b = 18;
        d = b / a;
        assert(Is_mpz_class_Equals(d, expectedValue));
        b = 2;
        a /= b;
        assert(Is_mpz_class_Equals(a, expectedValue));
    }
    {
        mpz_class a(6), c, d;
        const char *expectedValue = "3";
        signed long int b = 2;

        c = a / b;
        assert(Is_mpz_class_Equals(c, expectedValue));
        b = 18;
        d = b / a;
        assert(Is_mpz_class_Equals(d, expectedValue));
        b = 2;
        a /= b;
        assert(Is_mpz_class_Equals(a, expectedValue));
    }
    {
        mpz_class a(6), c, d;
        const char *expectedValue = "-3";
        signed long int b = -2;

        c = a / b;
        assert(Is_mpz_class_Equals(c, expectedValue));
        b = -18;
        d = b / a;
        assert(Is_mpz_class_Equals(d, expectedValue));
        b = -2;
        a /= b;
        assert(Is_mpz_class_Equals(a, expectedValue));
    }
    {
        mpz_class a(6), c, d;
        const char *expectedValue = "3";
        unsigned int b = 2;

        c = a / b;
        assert(Is_mpz_class_Equals(c, expectedValue));
        b = 18;
        d = b / a;
        assert(Is_mpz_class_Equals(d, expectedValue));
        b = 2;
        a /= b;
        assert(Is_mpz_class_Equals(a, expectedValue));
    }
    {
        mpz_class a(6), c, d;
        const char *expectedValue = "3";
        signed int b = 2;

        c = a / b;
        assert(Is_mpz_class_Equals(c, expectedValue));
        b = 18;
        d = b / a;
        assert(Is_mpz_class_Equals(d, expectedValue));
        b = 2;
        a /= b;
        assert(Is_mpz_class_Equals(a, expectedValue));
    }
    {
        mpz_class a(7), c, d;
        const char *expectedValue = "-3";
        const char *expectedValue1 = "-2";
        signed int b = -2;

        c = a / b;
        assert(Is_mpz_class_Equals(c, expectedValue));
        b = -18;
        d = b / a;
        assert(Is_mpz_class_Equals(d, expectedValue1));
        b = -2;
        a /= b;
        assert(Is_mpz_class_Equals(a, expectedValue));
    }
    {
        mpz_class a(7), c, d;
        const char *expectedValue = "-3";
        const char *expectedValue1 = "-2";
        double b = -2.0;
        c = a / b;
        assert(Is_mpz_class_Equals(c, expectedValue));
        b = -18;
        d = b / a;
        assert(Is_mpz_class_Equals(d, expectedValue1));
        b = -2;
        a /= b;
        assert(Is_mpz_class_Equals(a, expectedValue));
    }
    std::cout << "test_mpz_class_division passed." << std::endl;
}
void test_mpz_class_modulus() {
    {
        mpz_class a(5), c, d;
        const char *expectedValue = "1";
        mpz_class b(2);

        c = a % b;
        assert(Is_mpz_class_Equals(c, expectedValue));
        b = 11;
        d = b % a;
        assert(Is_mpz_class_Equals(d, expectedValue));
        b = 2;
        a %= b;
        assert(Is_mpz_class_Equals(a, expectedValue));
    }
    {
        mpz_class a(11), c, d;
        const char *expectedValue = "2";
        const char *expectedValue1 = "7";
        const char *expectedValue2 = "1";
        mpz_class b(-3);

        c = a % b;
        assert(Is_mpz_class_Equals(c, expectedValue));
        b = 7;
        d = b % a;
        assert(Is_mpz_class_Equals(d, expectedValue1));
        b = -2;
        a %= b;
        assert(Is_mpz_class_Equals(a, expectedValue2));
    }
    {
        mpz_class a(5), c, d;
        const char *expectedValue = "1";
        uint64_t b(2);

        c = a % b;
        assert(Is_mpz_class_Equals(c, expectedValue));
        b = 11;
        d = b % a;
        assert(Is_mpz_class_Equals(d, expectedValue));
        b = 2;
        a %= b;
        assert(Is_mpz_class_Equals(a, expectedValue));
    }
    {
        mpz_class a(5), c, d;
        const char *expectedValue = "1";
        signed long int b(2);
        c = a % b;
        assert(Is_mpz_class_Equals(c, expectedValue));
        b = 11;
        d = b % a;
        assert(Is_mpz_class_Equals(d, expectedValue));
        b = 2;
        a %= b;
        assert(Is_mpz_class_Equals(a, expectedValue));
    }
    {
        mpz_class a(11), c, d;
        const char *expectedValue = "2";
        const char *expectedValue1 = "7";
        const char *expectedValue2 = "1";
        signed long int b(-3);

        c = a % b;
        assert(Is_mpz_class_Equals(c, expectedValue));
        b = 7;
        d = b % a;
        assert(Is_mpz_class_Equals(d, expectedValue1));
        b = -2;
        a %= b;
        assert(Is_mpz_class_Equals(a, expectedValue2));
    }
    {
        mpz_class a(5), c, d;
        const char *expectedValue = "1";
        unsigned int b(2);

        c = a % b;
        assert(Is_mpz_class_Equals(c, expectedValue));
        b = 11;
        d = b % a;
        assert(Is_mpz_class_Equals(d, expectedValue));
        b = 2;
        a %= b;
        assert(Is_mpz_class_Equals(a, expectedValue));
    }
    {
        mpz_class a(5), c, d;
        const char *expectedValue = "1";
        signed int b(2);
        c = a % b;
        assert(Is_mpz_class_Equals(c, expectedValue));
        b = 11;
        d = b % a;
        assert(Is_mpz_class_Equals(d, expectedValue));
        b = 2;
        a %= b;
        assert(Is_mpz_class_Equals(a, expectedValue));
    }
    {
        mpz_class a(11), c, d;
        const char *expectedValue = "2";
        const char *expectedValue1 = "7";
        const char *expectedValue2 = "1";
        signed int b(-3);

        c = a % b;
        assert(Is_mpz_class_Equals(c, expectedValue));
        b = 7;
        d = b % a;
        assert(Is_mpz_class_Equals(d, expectedValue1));
        b = -2;
        a %= b;
        assert(Is_mpz_class_Equals(a, expectedValue2));
    }
    std::cout << "test_mpz_class_modulus passed." << std::endl;
}
void testDefaultConstructor_mpq_class() {
    mpq_class a;
    char buffer[100];
    gmp_snprintf(buffer, sizeof(buffer), "%Qd", a.get_mpq_t());
    assert(std::string(buffer) == "0");
    // initialized to zero
    std::cout << "testDefaultConstructor_mpq_class passed." << std::endl;
}
void testCopyConstructor_mpq_class() {
    mpq_class a;
    mpq_class b = a;
    assert(true);
    std::cout << "testCopyConstructor_mpq_class passed." << std::endl;
}
void testAssignmentOperator_mpq_class() {
    mpq_class a;
    mpq_class b;
    b = a;
    assert(true);
    std::cout << "testAssignmentOperator_mpq_class passed." << std::endl;
}
void testInitializationAndAssignmentInt_mpq_class() {
    const char *expectedValue = "355/113";

    mpq_class a(355, 113);
    assert(Is_mpq_class_Equals(a, expectedValue, true));
    std::cout << "Substitution mpq_class from integers test passed." << std::endl;

    mpq_class b;
    b = a;
    assert(Is_mpq_class_Equals(b, expectedValue, true));
    std::cout << "testInitializationAndAssignmentInt_mpq_class passed." << std::endl;
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
    std::cout << "testAssignmentOperator_the_rule_of_five_mpq_class passed." << std::endl;
}
void testInitializationAndAssignmentString_mpq_class() {
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

    std::cout << "testInitializationAndAssignmentString_mpq_class passed." << std::endl;
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

    std::cout << "test_template_cmp_mpq_class passed." << std::endl;
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

    std::cout << "test_arithmetic_operators_mpq_class_hardcoded1 passed." << std::endl;
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

    std::cout << "test_arithmetic_operators_mpq_class_hardcoded2 passed." << std::endl;
}
void test_mpq_class_literal() {
    mpq_class num1 = 223606_mpq;
    mpq_class num2 = 141421_mpq;
    mpq_class num3 = 0_mpq;
    mpq_class num4 = -9876543210_mpq;
    mpq_class num5 = 999999999999_mpq;

    const char *num1_expected = "223606";
    const char *num2_expected = "141421";
    const char *num3_expected = "0";
    const char *num4_expected = "-9876543210";
    const char *num5_expected = "999999999999";
    assert(Is_mpq_class_Equals(num1, num1_expected));
    assert(Is_mpq_class_Equals(num2, num2_expected));
    assert(Is_mpq_class_Equals(num3, num3_expected));
    assert(Is_mpq_class_Equals(num4, num4_expected));
    assert(Is_mpq_class_Equals(num5, num5_expected));
#if !defined USE_ORIGINAL_GMPXX && !defined ___GMPXX_STRICT_COMPATIBILITY___
    mpq_class num6 = "314159"_mpq;
    mpq_class num7 = "271828"_mpq;
    mpq_class num8 = "0"_mpq;
    mpq_class num9 = "-123456"_mpq;
    mpq_class num10 = "99999999999999999999999999999999999999999999999999999999"_mpq;

    const char *num6_expected = "314159";
    const char *num7_expected = "271828";
    const char *num8_expected = "0";
    const char *num9_expected = "-123456";
    const char *num10_expected = "99999999999999999999999999999999999999999999999999999999";
    assert(Is_mpq_class_Equals(num6, num6_expected));
    assert(Is_mpq_class_Equals(num7, num7_expected));
    assert(Is_mpq_class_Equals(num8, num8_expected));
    assert(Is_mpq_class_Equals(num9, num9_expected));
    assert(Is_mpq_class_Equals(num10, num10_expected));
    // more tests
    mpq_class a = "1/2"_mpq;
    mpq_class b = "3/4"_mpq;

    assert(a == mpq_class("1/2"));
    assert(b == mpq_class("3/4"));

    mpq_class result = a + b;
    assert(result == mpq_class("5/4"));

    std::cout << "a = " << a << ", b = " << b << std::endl;
    std::cout << "a + b = " << result << std::endl;
#endif
    std::cout << "test_mpq_class_literal passed." << std::endl;
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
    assert(fraction.set_str("3/4", 10) == 0);
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
#if !defined USE_ORIGINAL_GMPXX
    {
        std::istringstream input("invalid");
        mpq_class a;
        input >> a;
        assert(input.fail());
    }
#endif
    std::cout << "test_mpq_class_functions passed." << std::endl;
}
void test_mpz_class_comparison_int() {
    mpz_class a = 3;
    mpz_class b = 5;

    assert(a == 3);
    assert(!(a == 4));
    assert(b == 5);
    assert(!(b == 3));

    assert(3 == a);
    assert(5 != a);
    assert(5 == b);
    assert(3 != b);

    std::cout << "test_mpz_class_comparison_int tests passed successfully." << std::endl;
}
void test_mpf_class_const_pi() {
#if !defined USE_ORIGINAL_GMPXX
    // https://www.wolframalpha.com/input?i=N%5Bpi%2C+1000%5D
    const char *pi_approx = "3.141592653589793238462643383279502884197169399375105820974944592307816406286208998628034825342117067982148086513282306647093844609550582231725359408128481117450284102701938521105559644622948954930381964428810975665933446128475648233786783165271201909145648566923460348610454326648213393607260249141273724587006606315588174881520920962829254091715364367892590360011330530548820466521384146951941511609433057270365759591953092186117381932611793105118548074462379962749567351885752724891227938183011949129833673362440656643086021394946395224737190702179860943702770539217176293176752384674818467669405132000568127145263560827785771342757789609173637178721468440901224953430146549585371050792279689258923542019956112129021960864034418159813629774771309960518707211349999998372978049951059731732816096318595024459455346908302642522308253344685035261931188171010003137838752886587533208381420617177669147303598253490428755468731159562863882353787593751957781857780532171226806613001927876611195909216420199";
    mpf_class calculated_pi = const_pi();
    mp_bitcnt_t prec = mpf_get_default_prec();
    int decimal_digits = floor(std::log10(2) * prec);
    mp_exp_t exp;
    std::string _calculated_pi_str = calculated_pi.get_str(exp, 10, decimal_digits);
    std::string calculated_pi_str = insertDecimalPoint(_calculated_pi_str, exp);

    int i;
    for (i = 0; i < decimal_digits; ++i) {
        if (pi_approx[i] != calculated_pi_str[i]) {
            break;
        }
    }
    std::cout << "Pi matched in " << i - 1 << " decimal digits" << std::endl;
    assert(i - 1 > decimal_digits - 2 && "not accurate");

    mpf_class calculated_pi_2nd = const_pi();

    _calculated_pi_str = calculated_pi_2nd.get_str(exp, 10, decimal_digits);
    calculated_pi_str = insertDecimalPoint(_calculated_pi_str, exp);
    for (i = 0; i < decimal_digits; ++i) {
        if (pi_approx[i] != calculated_pi_str[i]) {
            break;
        }
    }
    std::cout << "Pi matched 2nd in " << i - 1 << " decimal digits (cached)" << std::endl;
    assert(i - 1 > decimal_digits - 2 && "not accurate");

    mpf_set_default_prec(prec * 2);
    prec = mpf_get_default_prec();
    decimal_digits = floor(std::log10(2) * prec);

    mpf_class calculated_pi_3rd = const_pi();

    _calculated_pi_str = calculated_pi_3rd.get_str(exp, 10, decimal_digits);
    calculated_pi_str = insertDecimalPoint(_calculated_pi_str, exp);

    for (i = 0; i < decimal_digits; ++i) {
        if (pi_approx[i] != calculated_pi_str[i]) {
            break;
        }
    }
    std::cout << "Pi matched 3rd in " << i - 1 << " decimal digits" << std::endl;
    assert(i - 1 > decimal_digits - 2 && "not accurate");
#if !defined ___GMPXX_MKII_NOPRECCHANGE___
    mpf_class pi_2048(0.0, 2048);
    pi_2048 = const_pi(2048);
    decimal_digits = floor(std::log10(2) * 2048);

    _calculated_pi_str = pi_2048.get_str(exp, 10, decimal_digits);
    calculated_pi_str = insertDecimalPoint(_calculated_pi_str, exp);

    for (i = 0; i < decimal_digits; ++i) {
        if (pi_approx[i] != calculated_pi_str[i]) {
            break;
        }
    }
    std::cout << "Pi matched 4th in " << i - 1 << " decimal digits" << std::endl;
    assert(i - 1 > decimal_digits - 2 && "not accurate");
#endif
    mpf_set_default_prec(prec / 2);
#endif
    std::cout << "test_mpf_class_const_pi passed." << std::endl;
}
void test_mpf_class_const_log2() {
#if !defined USE_ORIGINAL_GMPXX
    // https://www.wolframalpha.com/input?i=N%5Bln%282%29%2C+1000%5D
    const char *log2_approx = "0.6931471805599453094172321214581765680755001343602552541206800094933936219696947156058633269964186875420014810205706857336855202357581305570326707516350759619307275708283714351903070386238916734711233501153644979552391204751726815749320651555247341395258829504530070953263666426541042391578149520437404303855008019441706416715186447128399681717845469570262716310645461502572074024816377733896385506952606683411372738737229289564935470257626520988596932019650585547647033067936544325476327449512504060694381471046899465062201677204245245296126879465461931651746813926725041038025462596568691441928716082938031727143677826548775664850856740776484514644399404614226031930967354025744460703080960850474866385231381816767514386674766478908814371419854942315199735488037516586127535291661000710535582498794147295092931138971559982056543928717000721808576102523688921324497138932037843935308877482597017155910708823683627589842589185353024363421436706118923678919237231467232172053401649256872747782344535348";
    mpf_class calculated_log2 = const_log2();
    mp_bitcnt_t prec = mpf_get_default_prec();
    int decimal_digits = floor(std::log10(2) * prec);
    mp_exp_t exp;
    std::string _calculated_log2_str = calculated_log2.get_str(exp, 10, decimal_digits);
    std::string calculated_log2_str = insertDecimalPoint(_calculated_log2_str, exp);

    int i;
    for (i = 0; i < decimal_digits; ++i) {
        if (log2_approx[i] != calculated_log2_str[i]) {
            break;
        }
    }
    std::cout << "log2 matched in " << i - 1 << " decimal digits" << std::endl;
    assert(i - 1 > decimal_digits - 2 && "not accurate");

    mpf_class calculated_log2_2nd = const_log2();

    _calculated_log2_str = calculated_log2_2nd.get_str(exp, 10, decimal_digits);
    calculated_log2_str = insertDecimalPoint(_calculated_log2_str, exp);
    for (i = 0; i < decimal_digits; ++i) {
        if (log2_approx[i] != calculated_log2_str[i]) {
            break;
        }
    }
    std::cout << "log2 matched 2nd in " << i - 1 << " decimal digits (cached)" << std::endl;
    assert(i - 1 > decimal_digits - 2 && "not accurate");

    mpf_set_default_prec(prec * 2);
    prec = mpf_get_default_prec();
    decimal_digits = floor(std::log10(2) * prec);

    mpf_class calculated_log2_3rd = const_log2();

    _calculated_log2_str = calculated_log2_3rd.get_str(exp, 10, decimal_digits);
    calculated_log2_str = insertDecimalPoint(_calculated_log2_str, exp);

    for (i = 0; i < decimal_digits; ++i) {
        if (log2_approx[i] != calculated_log2_str[i]) {
            break;
        }
    }
    std::cout << "log2 matched 3rd in " << i - 1 << " decimal digits" << std::endl;
    assert(i - 1 > decimal_digits - 2 && "not accurate");
#if !defined ___GMPXX_MKII_NOPRECCHANGE___
    mpf_class log2_2048(0.0, 2048);
    log2_2048 = const_log2(2048);
    decimal_digits = floor(std::log10(2) * 2048);

    _calculated_log2_str = log2_2048.get_str(exp, 10, decimal_digits);
    calculated_log2_str = insertDecimalPoint(_calculated_log2_str, exp);

    for (i = 0; i < decimal_digits; ++i) {
        if (log2_approx[i] != calculated_log2_str[i]) {
            break;
        }
    }
    std::cout << "log2 matched 4th in " << i - 1 << " decimal digits" << std::endl;
    assert(i - 1 > decimal_digits - 2 && "not accurate");
#endif
    mpf_set_default_prec(prec / 2);
    std::cout << "test_mpf_class_const_log2 passed." << std::endl;
#endif
}
void test_div2exp_mul2exp_mpf_class(void) {
#if !defined USE_ORIGINAL_GMPXX
    mpf_class value(2.0);

    value.div_2exp(1); // value should now be 1.0
    assert(value == mpf_class(1.0));
    std::cout << "After div_2exp: " << value << std::endl;

    value.mul_2exp(1); // value should now be back to 2.0
    assert(value == mpf_class(2.0));
    std::cout << "After mul_2exp: " << value << std::endl;

    std::cout << "test_div2exp_mul2exp_mpf_class passed." << std::endl;
#endif
}
void test_log_mpf_class(void) {
#if !defined USE_ORIGINAL_GMPXX
    // https://www.wolframalpha.com/input?i=N%5Bln%2825%29%2C+1000%5D
    const char *log25_approx = "3.218875824868200749201518666452375279051202708537035443825295782948357975415315529260267756186359221599932606043431125799448010458649352399267233234927411455104359274994366491306985712404683050114540310387201759554779451376387081425532309462443619055897042585642716119445135344570574480923178896356729342657347996849275519186378847687868706902101950108908389481002731974175734766426261145944081318970767677447325507753091125436323023319861830486414729823355727801351745171557532783167673647900850975912478968062003965394234981987482996961915242033822028757724806708643025446251469176923119574583961773141368040133199688945399946435362373035202440405956816218039295053399530137841317957826763143432414455549119541068640775574993658750722267601893528096660570004349549759416014286931232719794082517802675283048034011777197469896975575942108628356640402906124181231577581343196250334998082868344244073889400185371187519570984237923572865751753696644116711923993582633393018646777063170797964741970862926";
    // https://www.wolframalpha.com/input?i=N%5Bln%2810%29%2C+1000%5D
    const char *log10_approx = "2.302585092994045684017991454684364207601101488628772976033327900967572609677352480235997205089598298341967784042286248633409525465082806756666287369098781689482907208325554680843799894826233198528393505308965377732628846163366222287698219886746543667474404243274365155048934314939391479619404400222105101714174800368808401264708068556774321622835522011480466371565912137345074785694768346361679210180644507064800027750268491674655058685693567342067058113642922455440575892572420824131469568901675894025677631135691929203337658714166023010570308963457207544037084746994016826928280848118428931484852494864487192780967627127577539702766860595249671667418348570442250719796500471495105049221477656763693866297697952211071826454973477266242570942932258279850258550978526538320760672631716430950599508780752371033310119785754733154142180842754386359177811705430982748238504564801909561029929182431823752535770975053956518769751037497088869218020518933950723853920514463419726528728696511086257149219884998";
    mpf_class x;
    mpf_class calculated;
    mp_bitcnt_t prec;
    mp_exp_t exp;
    int i;

    prec = mpf_get_default_prec();
    int decimal_digits = floor(std::log10(2) * prec);

    std::string _calculated_str;
    std::string calculated_str;

    x = 25.0;

    calculated = log(x);

    _calculated_str = calculated.get_str(exp, 10, decimal_digits);
    calculated_str = insertDecimalPoint(_calculated_str, exp);
    for (i = 0; i < decimal_digits; ++i) {
        if (log25_approx[i] != calculated_str[i]) {
            break;
        }
    }
    std::cout << "log25 matched in " << i - 1 << " decimal digits" << std::endl;
    assert(i - 1 > decimal_digits - 4 && "not accurate");

    x = 10.0;

    calculated = log(x);

    _calculated_str = calculated.get_str(exp, 10, decimal_digits);
    calculated_str = insertDecimalPoint(_calculated_str, exp);
    for (i = 0; i < decimal_digits; ++i) {
        if (log10_approx[i] != calculated_str[i]) {
            break;
        }
    }
    std::cout << "log10 matched in " << i - 1 << " decimal digits" << std::endl;
    assert(i - 1 > decimal_digits - 4 && "not accurate");
    std::cout << "test_log_mpf_class passed." << std::endl;
#endif
}
void test_exp_mpf_class(void) {
#if !defined USE_ORIGINAL_GMPXX && !defined ___GMPXX_STRICT_COMPATIBILITY___
    // https://www.wolframalpha.com/input?i=+N%5Be%2C1000%5D
    const char *exp_approx = "2.718281828459045235360287471352662497757247093699959574966967627724076630353547594571382178525166427427466391932003059921817413596629043572900334295260595630738132328627943490763233829880753195251019011573834187930702154089149934884167509244761460668082264800168477411853742345442437107539077744992069551702761838606261331384583000752044933826560297606737113200709328709127443747047230696977209310141692836819025515108657463772111252389784425056953696770785449969967946864454905987931636889230098793127736178215424999229576351482208269895193668033182528869398496465105820939239829488793320362509443117301238197068416140397019837679320683282376464804295311802328782509819455815301756717361332069811250996181881593041690351598888519345807273866738589422879228499892086805825749279610484198444363463244968487560233624827041978623209002160990235304369941849146314093431738143640546253152096183690888707016768396424378140592714563549061303107208510383750510115747704171898610687396965521267154688957035035";
    // https://www.wolframalpha.com/input?i=+N%5Be%5E3%2C1000%5D
    const char *exp3_approx = "20.08553692318766774092852965458171789698790783855415014437893422969884587809197373120449716025301770215360761585194900288181101247935350669023262178447725050394567710006607785181222904788438394025815253470935262298146553842455569773351510815011840475493383849784317767607091377286249178734939603782279371768713125406059755342664082603094866392021625866739118181021434425976486101447439682078592116814196837482236890943228820747794712380717751234795948355631291870717723980325570722406961326764308869756183595072231928690478084267326369088420142099426054055699851968620917767803763057071354303572712275059443501409536833666992145341488550268350504225115314225042524859653457692308488860569844450156316355139644793086595358952932153985658505063860564112464635460516266458066482742220056950190662889962457088076988990086516557075290361202314960209103836201522591300399393702712132993144270679882569941465256511446682047285382500173314712582742713173225842241594992766868376755311110926298901918538329586";
    const char *exp0_approx = "1";
    // https://www.wolframalpha.com/input?i=+N%5Be%5E%280.0625%29%2C1000%5D
    const char *exp00625_approx = "1.064494458917859429563390594642889673100725443649353301519307510635563936828166006334293435506876624375512982981261182073854356653897081255324575720943188390436516579202185434714440715132134605396058232067474793692958474821644936262673767848624063665456593887338700003015937389434206746843838616729064804088140299138749563088405906808770147979046656295077073574492365000978579685541941489690710617221139026348319104842132460253485358389734336660735291996276846584629530491420132035638022087055027220463016877512711521121230033485908289316183341433102014289269967049384216123733832289887197843025246799940899616117487513172985585131891545139987789162442335985449484239383601174909282335195436596482265804349407914785506864767507425055656406514213521603195374631462677119357592947359181457683956108174516473099004585754702479858852312018451907594104169398486875390930489570809549535442756179020982349645237795972932402795811776050608660066794023407524846811142261227354534066720530710136456909806707047";
    // https://www.wolframalpha.com/input?i=+N%5Be%5E%28-1%29%2C1000%5D
    const char *expm1_approx = "0.3678794411714423215955237701614608674458111310317678345078368016974614957448998033571472743459196437466273252768439952082469757927901290086266535894940987830921943673773381150486389911251456163449877199786844759579397473025498924954532393662079648105146475206122942230891649265666003650745772837055328537383881068047876119568298934544973507393185992166174330035699372082071022775180215849942337816907156676717623366082303761229156237572094700070405097334256775762525280303768861651570936537995427406370717878445419467490931306980560163702111389774228214017380232832465287291389004660986659512444097699851459164287803720202510224578732111059537776807437112206240005167965280975444780286486006838564200433684662484349386918262062518994821970992423425207510492093445285124486022451380986417421061219536368310078209224804653079806562854154786061793155705987170215999699188228265397927803747127438635156296714511943986702682452679716814389772141359579690542529103548859731078233269414118579235695949376986";
    // https://www.wolframalpha.com/input?i=+N%5Be%5E%28-10%29%2C1000%5D
    const char *expm10_approx = "0.00004539992976248485153559151556055061023791808886656496925907130565099942161430228165252500454594778232170805508968602849294519911724452038883718334770941456756099090921700736397018105950178390076296851778703090882436517154844872229365233241602050116826436030560494157010772997535440807940399423293213827078052004271049896035448616606683700920170757320883634467939051402688860388083294497677616203039090150324548764511431630330952049312537765766774006756454876738125287505690572275377628382171284143663732182087248552216443007813115036461879368959426358019462505567865040585338993403653413058286641193648376178025649554278964463839952664381022973460413385495914470289894669705903833618297934791927412492682326345369660009565588000880953721966615544120170188073150289699206773793774210280614727384360102801589161076119101602167880922506360875805338654984867101289668710215872879988148535546912033731423698940549209573536597828837940706553848348004339425871326393731931520087753512200072317764471807908310";

    mpf_class x;
    mpf_class calculated;
    mp_bitcnt_t prec;
    mp_exp_t _exp;
    int i;

    prec = mpf_get_default_prec();
    int decimal_digits = floor(std::log10(2) * prec);

    std::string _calculated_str;
    std::string calculated_str;

    x = "1.0";

    calculated = exp(x);

    _calculated_str = calculated.get_str(_exp, 10, decimal_digits);
    calculated_str = insertDecimalPoint(_calculated_str, _exp);
    for (i = 0; i < decimal_digits; ++i) {
        if (exp_approx[i] != calculated_str[i]) {
            break;
        }
    }
    std::cout << "exp matched in " << i - 1 << " decimal digits" << std::endl;
    assert(i - 1 > decimal_digits - 4 && "not accurate");

    x = 3.0;

    calculated = exp(x);

    _calculated_str = calculated.get_str(_exp, 10, decimal_digits);
    calculated_str = insertDecimalPoint(_calculated_str, _exp);
    for (i = 0; i < decimal_digits; ++i) {
        if (exp3_approx[i] != calculated_str[i]) {
            break;
        }
    }
    std::cout << "exp3 matched in " << i - 1 << " decimal digits" << std::endl;
    assert(i - 1 > decimal_digits - 4 && "not accurate");

    x = 0.0;

    calculated = exp(x);

    _calculated_str = calculated.get_str(_exp, 10, decimal_digits);
    calculated_str = insertDecimalPoint(_calculated_str, _exp);
    if (exp0_approx[0] == calculated_str[0]) {
        std::cout << "exp0 matched" << std::endl;
    } else {
        assert(false && "not accurate");
    }

    x = 0.0625;

    calculated = exp(x);

    _calculated_str = calculated.get_str(_exp, 10, decimal_digits);
    calculated_str = insertDecimalPoint(_calculated_str, _exp);
    for (i = 0; i < decimal_digits; ++i) {
        if (exp00625_approx[i] != calculated_str[i]) {
            break;
        }
    }
    std::cout << "exp00625 matched in " << i - 1 << " decimal digits" << std::endl;
    assert(i - 1 > decimal_digits - 4 && "not accurate");

    x = -1.0;

    calculated = exp(x);

    _calculated_str = calculated.get_str(_exp, 10, decimal_digits);
    calculated_str = insertDecimalPoint(_calculated_str, _exp);
    for (i = 0; i < decimal_digits; ++i) {
        if (expm1_approx[i] != calculated_str[i]) {
            break;
        }
    }
    std::cout << "expm1 matched in " << i - 1 << " decimal digits" << std::endl;
    assert(i - 1 > decimal_digits - 4 && "not accurate");

    x = -10.0;

    calculated = exp(x);
    _calculated_str = calculated.get_str(_exp, 10, decimal_digits);
    calculated_str = insertDecimalPoint(_calculated_str, _exp);

    for (i = 0; i < decimal_digits; ++i) {
        if (expm10_approx[i] != calculated_str[i]) {
            break;
        }
    }
    std::cout << "expm10 matched in " << i - 1 << " decimal digits" << std::endl;
    assert(i - 1 > decimal_digits - 4 && "not accurate");
    std::cout << "test_exp_mpf_class passed." << std::endl;
#endif
}
void test_casts() {
    // mpf_class -> mpz_class
    mpf_class a0("4.5");
    mpz_class b0;
    const char *expectedValue0 = "4";
    b0 = mpz_class(a0);
    assert(Is_mpz_class_Equals(b0, expectedValue0));

    // mpz_class -> mpf_class
    mpf_class a1;
    mpz_class b1(3000);
    const char *expectedValue1 = "3000.0000000000";
    a1 = mpf_class(b1);
    assert(Is_mpf_class_Equals(a1, expectedValue1));

    // mpq_class -> mpf_class
    mpf_class a2;
    mpq_class b2("3000/13");
    const char *expectedValue2 = "230.7692307692";
    a2 = mpf_class(b2);
    assert(Is_mpf_class_Equals(a2, expectedValue2, true));

    // mpz_class -> mpq_class
    mpz_class a3("3153");
    mpq_class b3;
    const char *expectedValue3 = "3153";
    b3 = mpq_class(a3);
    assert(Is_mpq_class_Equals(b3, expectedValue3, true));

    // mpq_class -> mpz_class
    mpz_class a4;
    mpq_class b4("3000/13");
    const char *expectedValue4 = "230";
    a4 = mpz_class(b4);
    assert(Is_mpz_class_Equals(a4, expectedValue4, true));

    // mpf_class -> mpq_class
    mpf_class a5("4.5");
    mpq_class b5;
    const char *expectedValue5 = "9/2";
    b5 = mpq_class(a5);
    assert(Is_mpq_class_Equals(b5, expectedValue5));

    std::cout << "test_casts passed." << std::endl;
}
void test_precisions_mixed() {
#if !defined ___GMPXX_MKII_NOPRECCHANGE___
    {
        mp_exp_t _exp = 1024;
        mpf_class a(10, _exp);
        mpz_class b("1234567890");
        a = b;
        assert(a == 1234567890);
        assert(a.get_prec() == 1024);
    }
    {
        mpf_class a(10);
        mpz_class b(3);
        mpf_class c(3, 1024);
        assert(a.get_prec() == 512);
        a = b + c;
        assert(a == 6);
        assert((b + c).get_prec() == 1024);
        assert(a.get_prec() == 512);
    }
    {
        mpf_class a(10);
        mpz_class b(5);
        mpf_class c(3, 1024);
        assert(a.get_prec() == 512);
        a = c + b;
        assert(a == 8);
        assert((c + b).get_prec() == 1024);
        assert(a.get_prec() == 512);
    }
    {
        mpf_class a(10);
        mpz_class b(3);
        mpf_class c(3, 1024);
        assert(a.get_prec() == 512);
        a = b - c;
        assert(a == 0);
        assert((b - c).get_prec() == 1024);
        assert(a.get_prec() == 512);
    }
    {
        mpf_class a(10);
        mpz_class b(5);
        mpf_class c(3, 1024);
        assert(a.get_prec() == 512);
        a = c - b;
        assert(a == -2);
        assert((c - b).get_prec() == 1024);
        assert(a.get_prec() == 512);
    }
    std::cout << "test_precisions_mixed passed." << std::endl;
#endif
}
void test_misc() {
    {
        mpz_class a(-11);
        mpz_class b(3);
        mpz_class c;
        c = a % b;
        std::cout << c << std::endl;
    }
    {
        int a = -11;
        int b = 3;
        int c;
        c = a % b;
        std::cout << c << std::endl;
    }
    {
        mpf_class a(-0.33231);
        std::cout << a << std::endl;
    }
    {
        mpf_class a(-0.33231);
        std::cout << a << std::endl;
    }
    {
        mpf_class a;
        a = 0;
        std::cout << std::hex << std::scientific << "a " << a << std::endl;
        a = -0.00313119;
        std::cout << std::hex << std::scientific << "a " << a << std::endl;
        a = 1;
        std::cout << std::hex << std::scientific << "a " << a << std::endl;
        a = 12300.133;
        std::cout << std::hex << std::scientific << "a " << a << std::endl;
        a = 12300133;
        std::cout << std::hex << std::scientific << "a " << a << std::endl;
    }
    {
        mpf_class a;
        a = 0;
        std::cout << std::hex << std::scientific << std::setprecision(3) << std::setw(0) << "a " << a << std::endl;
        a = 3.14159;
        std::cout << std::hex << std::scientific << std::setprecision(3) << std::setw(0) << "a " << a << std::endl;
        a = -19930.9;
        std::cout << std::hex << std::scientific << std::setprecision(3) << std::setw(0) << "a " << a << std::endl;
    }
    {
        mpf_class a;
        a = -0.5;
        std::cout << std::hex << std::scientific << std::showbase << std::setprecision(3) << std::setw(0) << "a " << a << std::endl;
        a = 123.000005;
        std::cout << std::hex << std::scientific << std::showbase << std::setprecision(3) << std::setw(0) << "a " << a << std::endl;
    }
    {
        mpf_class a;
        a = 0.0000000599999;
        std::cout << std::dec << std::scientific << std::setprecision(8) << std::setw(0) << "a " << a << std::endl;
    }
    {
        mpf_class a;
        a = 12300.133;
        std::cout << std::dec << std::scientific << std::setprecision(8) << std::setw(0) << "a " << a << std::endl;
    }
    {
        const int small_prec = 64, medium_prec = 128, large_prec = 192, very_large_prec = 256;
        mpf_set_default_prec(medium_prec);
        mpf_class f(3.0, small_prec);
        mpf_class g(1 / f, very_large_prec);
        mpf_class h(1.0, medium_prec);
        mpf_class i(1.0, large_prec);

        h = 1 / f;
        i = 1 / f;
        gmp_printf("%.78Ff\n", f.get_mpf_t());
        gmp_printf("%.78Ff\n", g.get_mpf_t());
        gmp_printf("%.78Ff\n", h.get_mpf_t());
        gmp_printf("%.78Ff\n", i.get_mpf_t());
        mpf_set_default_prec(512);
    }
    {
        gmp_randclass r1(gmp_randinit_default);
        std::cout << "Using gmp_randinit_default:" << std::endl;
        for (int i = 0; i < 5; i++) {
            mpf_class f(0, 512);
            f = r1.get_f(512);
            gmp_printf("%.78Ff\n", f.get_mpf_t());
        }
    }
    {
        mpz_class a(2862933555777941757);
        unsigned long c = 0, m2exp = 32;
        gmp_randclass r2(gmp_randinit_lc_2exp, a, c, m2exp);
        std::cout << "Using gmp_randinit_lc_2exp:" << std::endl;
        for (int i = 0; i < 5; i++) {
            mpf_class f(0, 512);
            f = r2.get_f(512);
            gmp_printf("%.78Ff\n", f.get_mpf_t());
        }
    }
    {
        unsigned long m2exp_size = 64;
        gmp_randclass r3(gmp_randinit_lc_2exp_size, m2exp_size);
        std::cout << "Using gmp_randinit_lc_2exp_size:" << std::endl;
        for (int i = 0; i < 5; i++) {
            mpf_class f(0, 512);
            f = r3.get_f(512);
            gmp_printf("%.78Ff\n", f.get_mpf_t());
        }
    }
    {
        gmp_randclass r1(gmp_randinit_mt);
        r1.seed(1);
        std::cout << "Using gmp_randinit_mt:" << std::endl;
        for (int i = 0; i < 10; i++) {
            mpf_class f(0, 512);
            f = r1.get_f(512);
            gmp_printf("%.78Ff\n", f.get_mpf_t());
        }
    }
    {
        gmp_randclass randGen(gmp_randinit_default);
        mpz_class seedValue(123);
        randGen.seed(seedValue);
        std::cout << "Random mpz_class numbers with 256 bits:" << std::endl;
        for (int i = 0; i < 5; i++) {
            mpz_class randomNum;
            randomNum = randGen.get_z_bits(256);
            gmp_printf("%Zd\n", randomNum.get_mpz_t());
        }
    }
    {
        gmp_randclass randGen(gmp_randinit_default);
        mpz_class seedValue(123);
        randGen.seed(seedValue);
        mpz_class range(256);
        std::cout << "Random mpz_class numbers in the range 0 to 255:" << std::endl;
        for (int i = 0; i < 5; i++) {
            mpz_class randomNum;
            randomNum = randGen.get_z_range(range);
            gmp_printf("%Zd\n", randomNum.get_mpz_t());
        }
    }
    ////
    {
        // A clear difference between the original macro-based implementation and my C++17 implementation
        // the original implementation behaves like 1/f (or 1/3) is evaluated in the infinite precision!
        // whereas ours evaluate 1/f in small_prec then we substitute to g in the very_large_prec.
        std::cout << "A clear example of the difference between the original gmpxx and my new implementation 1:" << std::endl;
        const int small_prec = 64, medium_prec = 128, very_large_prec = 256;
        mpf_set_default_prec(medium_prec);
        mpf_class f(3.0, small_prec);
        mpf_class g(1 / f, very_large_prec);
        gmp_printf("%.78Ff\n", f.get_mpf_t());
        gmp_printf("%.78Ff\n", g.get_mpf_t());
        mpf_set_default_prec(512);
    }
#if defined ___GMPXX_MKII_NOPRECCHANGE___ || defined USE_ORIGINAL_GMPXX
    {
        // Another clear difference between the original macro-based implementation and my C++17 implementation
        // r2.get_f() knows the precision of f in the original implementation, whereas my implementation does not.
        // we disable the xx.get_f() form, use xx.get_f(prec) instead and please explicitly state the precision.
        std::cout << "A clear example of the difference between the original gmpxx and my new implementation 2:" << std::endl;
        const int medium_prec = 128, large_prec = 512;
        mpf_set_default_prec(medium_prec);
        gmp_randclass r1(gmp_randinit_default);
        std::cout << "Using gmp_randinit_default:" << std::endl;
        for (int i = 0; i < 5; i++) {
            mpf_class f(0, large_prec);
            f = r1.get_f();
            gmp_printf("%.78Ff\n", f.get_mpf_t());
        }
        mpf_set_default_prec(512);
    }
#endif
}
void test_reminder() {
#if !defined USE_ORIGINAL_GMPXX
    std::vector<mpf_class> x_values = {mpf_class("10.5"), mpf_class("23.7"), mpf_class("5.3"), mpf_class("-15.8"), mpf_class("-7.6")};
    std::vector<mpf_class> y_values = {mpf_class("3.2"), mpf_class("4.5"), mpf_class("2.1"), mpf_class("6.1"), mpf_class("2.3")};
    mpf_class epsilon;
    epsilon.set_epsilon();

    for (int i = 0; i < 5; ++i) {
        mpf_class x = x_values[i];
        mpf_class y = y_values[i];
        if (y == 0) {
            std::cerr << "Error: Division by zero is not allowed for pair (" << x << ", " << y << ")" << std::endl;
            continue;
        }
        mpz_class quotient;
        mpf_class remainder = mpf_remainder(x, y, &quotient);
        mpf_class reconstructed_x = quotient * y + remainder;
        std::cout << "gmp_remainder(" << x << ", " << y << ") = " << remainder << ", quotient = " << quotient << ", remainder = " << remainder << ", reconstructed x = " << reconstructed_x << std::endl;
        assert(abs(x - reconstructed_x) < epsilon * 2.0 && "Check failed: reconstructed_x does not match original x");
    }
    std::cout << "test_reminder passed." << std::endl;
#endif
}

void test_cos() {
#if !defined USE_ORIGINAL_GMPXX
#ifdef NOT_COMPILE
    {
        for (int i = -30; i < 30; i++) {
            mpf_class x = mpf_class(i) / 5.0;
            mpf_class cosx = cos(mpf_class(x));
            std::cout << "x= " << std::setprecision(3) << x << ", y= " << std::setprecision(30) << cosx << std::endl;
        }
    }
#endif
    mpf_class x, y;
    {
        // https://www.wolframalpha.com/input?i=N%5Bcos%280.5%29%2C+1000%5D
        const char *cos0_5_approx = "0.8775825618903727161162815826038296519916451971097440529976108683159507632742139474057941840846822583554784005931090539934138279768332802667997561209502240155876291568785907234769393109896167396770144089976491285702134682183845438183933161688075406608111594034898319080526243422936798388210395344326097106933964804754464858190431523680783473541872989979620421073859870269534823243661795004902278414374593654144306238924803124411026516533210735858010801473973258173140240814461006638241372126952313583616860650624696095502592639473404558019911232873000111486767965193594297422439260707842035420054065847425228652644042751722138136221632486859686075089614370046702635364136115397636475756940310434770995888642780666339487590926986075912927819550614661607038331262366723492135736630948348605515723591765073735112642941795552487556516006978246830409779665064511236454552565685430132101697642386610565289247418901856196110813195247014727854776665745365108923118032985425467729661657598871073292535299826370";
        mp_bitcnt_t prec = mpf_get_default_prec();
        int decimal_digits = floor(std::log10(2) * prec);
        mp_exp_t exp;

        mpf_class calculated_cos0_5;
        calculated_cos0_5 = cos(mpf_class(0.5));
        std::cout << "cos(0.5) " << std::setprecision(decimal_digits) << calculated_cos0_5 << std::endl;
        std::string _cos0_5_pi_str = calculated_cos0_5.get_str(exp, 10, decimal_digits);
        std::string calculated_cos0_5_str = insertDecimalPoint(_cos0_5_pi_str, exp);

        int i;
        for (i = 0; i < decimal_digits; ++i) {
            if (cos0_5_approx[i] != calculated_cos0_5_str[i]) {
                break;
            }
        }
        std::cout << "cos(0.5) matched in " << i - 1 << " decimal digits" << std::endl;
        assert(i - 1 > decimal_digits - 2 && "not accurate");
    }
    {
        // https://www.wolframalpha.com/input?i=N%5Bcos%281%29%2C+1000%5D
        const char *cos1_approx = "0.5403023058681397174009366074429766037323104206179222276700972553811003947744717645179518560871830893435717311600300890978606337600216634564065122654173185847179711644744794942331179245513932543359435177567028925963757361543275496417544917751151312227301006313570782322367714015174689959366787306742276202450776374406758749816178427202164558511156329688905710812427293316986852471456894904342375433094423024093596239583182454728173664078071243433621748100322027129757882291764468359872699426491344391826569453515750762782513804991607306380317214450349861294883363356557799097930152879278840389800974548251049924537987740061453776371387833594234524168164283618828482374896327390556260912017589827502528599917438580692485584232217826858271088291564683006796875955130036108120336747472749181033673515093458888304203217596594052703934762502487370752661313369842416059710595606599978691384415574414466420012839398870926323453338868626299654709768054836830358211823411732418465771864116514294188326444690784";
        mp_bitcnt_t prec = mpf_get_default_prec();
        int decimal_digits = floor(std::log10(2) * prec);
        mp_exp_t exp;

        mpf_class calculated_cos1;
        calculated_cos1 = cos(mpf_class(1.0));
        std::cout << "cos(1.0) " << std::setprecision(decimal_digits) << calculated_cos1 << std::endl;
        std::string _cos1_pi_str = calculated_cos1.get_str(exp, 10, decimal_digits);
        std::string calculated_cos1_str = insertDecimalPoint(_cos1_pi_str, exp);
        int i;
        for (i = 0; i < decimal_digits; ++i) {
            if (cos1_approx[i] != calculated_cos1_str[i]) {
                break;
            }
        }
        std::cout << "cos(1.0) matched in " << i - 1 << " decimal digits" << std::endl;
        assert(i - 1 > decimal_digits - 2 && "not accurate");
    }
    std::cout << "test_cos passed." << std::endl;
#endif
}
void test_sin() {
#if !defined USE_ORIGINAL_GMPXX
#if defined NOT_COMPILE
    {
        for (int i = -30; i < 30; i++) {
            mpf_class x = mpf_class(i) / 5.0;
            mpf_class sinx = sin(mpf_class(x));
            std::cout << "x= " << std::setprecision(3) << x << ", y= " << std::setprecision(30) << sinx << std::endl;
        }
    }
#endif
    mpf_class x, y;
    {
        // https://www.wolframalpha.com/input?i=N%5Bsin%280.5%29%2C+1000%5D
        const char *sin0_5_approx = "0.4794255386042030002732879352155713880818033679406006751886166131255350002878148322096312746843482690861320910845057174178110937486099402827801539620461919246099572939322814005335463381880552285956701356998542336391210717207773801529798713771695151761807211496980737014747686970319870390009733954910298944341773311110967390393612416365348040191834631437628439264526015707128309276600679101753363116228761679573484037186681773033317987203406456734718299450682466361245546345327828936124477953660173546282046471782377689888164451282619784029173546615068368973314728739748878819020792879913842309550381758470503006764642826713620335251453987530901420484701772927288921230141786697128002651171760791938737965442084896430338944756682357287676259771462444700080783692821494199113874381055164647107208046281224742233561086832314463354777933737113643745496547901512272850722158212556276133568178117279952130008689159388955206479734490950297931352413777709150736057102650601524887458172621092489280129105543582";
        mp_bitcnt_t prec = mpf_get_default_prec();
        int decimal_digits = floor(std::log10(2) * prec);
        mp_exp_t exp;

        mpf_class calculated_sin0_5;
        calculated_sin0_5 = sin(mpf_class(0.5));
        std::cout << "sin(0.5) " << std::setprecision(decimal_digits) << calculated_sin0_5 << std::endl;
        std::string _sin0_5_pi_str = calculated_sin0_5.get_str(exp, 10, decimal_digits);
        std::string calculated_sin0_5_str = insertDecimalPoint(_sin0_5_pi_str, exp);

        int i;
        for (i = 0; i < decimal_digits; ++i) {
            if (sin0_5_approx[i] != calculated_sin0_5_str[i]) {
                break;
            }
        }
        std::cout << "sin(0.5) matched in " << i - 1 << " decimal digits" << std::endl;
        assert(i - 1 > decimal_digits - 2 && "not accurate");
    }
    {
        // https://www.wolframalpha.com/input?i=N%5Bsin%281%29%2C+1000%5D
        const char *sin1_approx = "0.8414709848078965066525023216302989996225630607983710656727517099919104043912396689486397435430526958543490379079206742932591189209918988811934103277292124094807919558267666069999077640119784087827325663474848028702986561570179624553948935729246701270864862810533820305613772182038684496677616742662390133827533979567642555654779639897648243286902756964291206300583036515230312782552898532648513981934521359709559620621721148144417810576010756741366480550089167266058041400780623930703718779562612888046360817345246563914202524041877634207492069520077133478098142790214526825566320823352154416091644209058929870224733844604489723713979912740819247250488554873119310350681908151532607457392911183319628215089734868811421452838229865125701667384074455192375614322129060592482739703681801585630905432667846431075312638121732567019856011068360289018950194215161665519179145172004668659597169107219780588540646001994013701405309580855205280525317113323054616383636018169947971500485150793983830395678167948";
        mp_bitcnt_t prec = mpf_get_default_prec();
        int decimal_digits = floor(std::log10(2) * prec);
        mp_exp_t exp;

        mpf_class calculated_sin1;
        calculated_sin1 = sin(mpf_class(1.0));
        std::cout << "sin(1.0) " << std::setprecision(decimal_digits) << calculated_sin1 << std::endl;
        std::string _sin1_pi_str = calculated_sin1.get_str(exp, 10, decimal_digits);
        std::string calculated_sin1_str = insertDecimalPoint(_sin1_pi_str, exp);
        int i;
        for (i = 0; i < decimal_digits; ++i) {
            if (sin1_approx[i] != calculated_sin1_str[i]) {
                break;
            }
        }
        std::cout << "sin(1.0) matched in " << i - 1 << " decimal digits" << std::endl;
        assert(i - 1 > decimal_digits - 2 && "not accurate");
    }
    std::cout << "test_sin passed." << std::endl;
#endif
}
void test_tan() {
#if !defined USE_ORIGINAL_GMPXX
#if defined NOT_COMPILE
    {
        for (int i = -30; i < 30; i++) {
            mpf_class x = mpf_class(i) / 5.0;
            mpf_class tanx = tan(mpf_class(x));
            std::cout << "x= " << std::setprecision(3) << x << ", y= " << std::setprecision(30) << tanx << std::endl;
        }
    }
#endif
    mpf_class x, y;
    {
        // https://www.wolframalpha.com/input?i=N%5Btan%280.5%29%2C+1000%5D
        const char *tan0_5_approx = "0.5463024898437905132551794657802853832975517201797912461640913859329075105180258157151806482706562185891048626002641142654932300911684028432173909299109142166369407437884742689574104012579117568787459997245089182122377508438391608137482993661734164513777158644131400892401894149314486480586500519674351342574977872908415221085467241970331446790527880716668468871483440752106506547415663369501828548780097109488509644488410491341080212223685971108678647644974269581372181607864349110751585226185856614603467598636106553813732244409778930262124949071667636788930185584623355658134842716791328409314446174562329551552070316686434408136052389587984698409202849349009643066268950773663202221068275927031381196543664392557080629562225725119638821553592032726798458097956096400150348186778846893612401538691472385317543262280556405533102005058478897757145127790585979379172843437129877265959041252742836433680072490985845406240568070134924579794028452986043961828074137389768264847958290962242928561501704126";
        mp_bitcnt_t prec = mpf_get_default_prec();
        int decimal_digits = floor(std::log10(2) * prec);
        mp_exp_t exp;

        mpf_class calculated_tan0_5;
        calculated_tan0_5 = tan(mpf_class(0.5));
        std::cout << "tan(0.5) " << std::setprecision(decimal_digits) << calculated_tan0_5 << std::endl;
        std::string _tan0_5_pi_str = calculated_tan0_5.get_str(exp, 10, decimal_digits);
        std::string calculated_tan0_5_str = insertDecimalPoint(_tan0_5_pi_str, exp);

        int i;
        for (i = 0; i < decimal_digits; ++i) {
            if (tan0_5_approx[i] != calculated_tan0_5_str[i]) {
                break;
            }
        }
        std::cout << "tan(0.5) matched in " << i - 1 << " decimal digits" << std::endl;
        assert(i - 1 > decimal_digits - 2 && "not accurate");
    }
    {
        // https://www.wolframalpha.com/input?i=N%5Btan%281%29%2C+1000%5D
        const char *tan1_approx = "1.5574077246549022305069748074583601730872507723815200383839466056988613971517272895550999652022429838046338214117481666133235546181245589376060716845489044392935860431671479080368246132747069555973416406107755352473025067968505070413523851449176214816275700278860224507720140161857721306739416643223690166756717950962610882330224852131148350591629692587616111732650100459456348215643353850532597808631549824289061892210758848878924221385209053656104434472362861925446578778273514850773908880700372159070308791432517058209136269619534377478940166007456316157881695681614614361866567292286330977096769203716916017263778512867226037524402851587319279078273733858578334130049543217104676193264364900030445702442217484459252160014668706377785595486573990024721785049795633301199461883344398640238026977359498288636108028120171311163152373742096531116813199937760466003565845884281985736727654467894198460022996521512946514081597313869091002793578754979210974245391495265566277558487047690210";
        mp_bitcnt_t prec = mpf_get_default_prec();
        int decimal_digits = floor(std::log10(2) * prec);
        mp_exp_t exp;

        mpf_class calculated_tan1;
        calculated_tan1 = tan(mpf_class(1.0));
        std::cout << "tan(1.0) " << std::setprecision(decimal_digits) << calculated_tan1 << std::endl;
        std::string _tan1_pi_str = calculated_tan1.get_str(exp, 10, decimal_digits);
        std::string calculated_tan1_str = insertDecimalPoint(_tan1_pi_str, exp);
        int i;
        for (i = 0; i < decimal_digits; ++i) {
            if (tan1_approx[i] != calculated_tan1_str[i]) {
                break;
            }
        }
        std::cout << "tan(1.0) matched in " << i - 1 << " decimal digits" << std::endl;
        assert(i - 1 > decimal_digits - 2 && "not accurate");
    }
    std::cout << "test_tan passed." << std::endl;
#endif
}
void test_pow() {
#if !defined USE_ORIGINAL_GMPXX
    mpf_class x, y;
    {
        // https://www.wolframalpha.com/input?i=pow%282%2C+2.99999%29+1000digits
        const char *pow_2_2_99999 = "7.999944548417735965910427321986411590007003904442038132291488188120284506417926019743038697930781942441212229015203753055344604976719775541683274905822542716936089681513541528837265481722285077607538153879413387225048204293036909488663811240936528271425291679800538627720103914741570455389376763940229057088623125292205425270204038905619401904883147870046493715813006170791297500130251733542472010041662127518095136090114582166464630242041080880975172430797223628223898529931359640362260853203432352649887875158060342773995939685901151579363692908402746666666748311996208218492519141180158201869281408817716733853149381362037054419229586500431491878844041117779706588023958357277050346488042204230461407896718031702820917123867880977686266359926436606171955679614321055797488324855187277043286688300169280429893727319379050862663860827278308035863797048647035299912083757777233725824367768200345551041949270566480992479741772425344915064321722195943794277488495858988225882588458082977161835843493174";
        mp_bitcnt_t prec = mpf_get_default_prec();
        int decimal_digits = floor(std::log10(2) * prec);
        mp_exp_t exp;

        mpf_class calculated_pow_2_2_99999;
        calculated_pow_2_2_99999 = pow(mpf_class(2), mpf_class("2.99999"));
        std::cout << "pow(2,2.99999) " << std::setprecision(decimal_digits) << calculated_pow_2_2_99999 << std::endl;
        std::string pow_2_2_99999_str = calculated_pow_2_2_99999.get_str(exp, 10, decimal_digits);
        std::string calculated_pow_2_2_99999_str = insertDecimalPoint(pow_2_2_99999_str, exp);

        int i;
        for (i = 0; i < decimal_digits; ++i) {
            if (pow_2_2_99999[i] != calculated_pow_2_2_99999_str[i]) {
                break;
            }
        }
        std::cout << "pow(2,2.99999) matched in " << i - 1 << " decimal digits" << std::endl;
        assert(i - 1 > decimal_digits - 5 && "not accurate");
    }
    {
        // https://www.wolframalpha.com/input?i2d=true&i=pow%5C%2840%295%5C%2844%29+0.5%5C%2841%29+1000digits
        const char *pow_5_0_5 = "2.236067977499789696409173668731276235440618359611525724270897245410520925637804899414414408378782274969508176150773783504253267724447073863586360121533452708866778173191879165811276645322639856580535761350417533785003423392414064442086432539097252592627228876299517402440681611775908909498492371390729728898482088641542689894099131693577019748678884425089754132956183176921499977424801530434115035957668332512498815178139408000562420855243542235556106306342820234093331982933959746352271201341749614202635904737885504389687061135660045757139956595566956917564578221952500060539231234005009286764875529722056766253666074485853505262330678494633422242317637277026632407680104443315825733505893098136226343198686471946989970180818952426445962034522141192232912598196325811104170495807048120403455994943506855551855572512388641655010262436312571024449618789424682903404474716115455723201737676590460918529575603577984398054155380779064393639723028756062999482213852177348592453515121046345555040707227872";
        mp_bitcnt_t prec = mpf_get_default_prec();
        int decimal_digits = floor(std::log10(2) * prec);
        mp_exp_t exp;

        mpf_class calculated_pow_5_0_5;
        calculated_pow_5_0_5 = pow(mpf_class(5), mpf_class(0.5));
        std::cout << "pow(5, 0.5) " << std::setprecision(decimal_digits) << calculated_pow_5_0_5 << std::endl;
        std::string pow_5_0_5_str = calculated_pow_5_0_5.get_str(exp, 10, decimal_digits);
        std::string calculated_pow_5_0_5_str = insertDecimalPoint(pow_5_0_5_str, exp);

        int i;
        for (i = 0; i < decimal_digits; ++i) {
            if (pow_5_0_5[i] != calculated_pow_5_0_5_str[i]) {
                break;
            }
        }
        std::cout << "pow(5, 0.5) matched in " << i - 1 << " decimal digits" << std::endl;
        assert(i - 1 > decimal_digits - 5 && "not accurate");
    }
    {
        // https://www.wolframalpha.com/input?i2d=true&i=pow%5C%2840%298%5C%2844%29+-3.5%5C%2841%29+1000digits
        const char *pow_8__3_5 = "0.0006905339660024878167976995723680166399265975953989004263558006533157873429990757025636657882459187366870184796049376450317015801053958677796113874497558280426822376298474148402127929445806410068655134131394335798857276731125961240306291588958207466790085704732570724163644147709178528338027320563515574644791975323885785300979592847941173803978250733826952938133962769630554354770563253782243205560777150326660013178734448391328260694904561756948576625668547956936418276912093108950258865325043882994312932174144830319813063583170998186370146556066758189177663512771316928553541855579338597175712305478885974083328723633830851790293680858834933774866499902495795042220779825502978806083351564055898604587352186665352072789373477203974538699276036152463423451025501436097448060195554559623519289071469662289282580956721059111700682100283435378331898924404456446520713197885314872584072756083907300093320434297530140884510793849415708533842374447907573325738126912393421332724491560831128968862069574449";
        mp_bitcnt_t prec = mpf_get_default_prec();
        int decimal_digits = floor(std::log10(2) * prec);
        mp_exp_t exp;

        mpf_class calculated_pow_8__3_5;
        calculated_pow_8__3_5 = pow(mpf_class(8), mpf_class(-3.5));
        std::cout << "pow(8, -3.5) " << std::setprecision(decimal_digits) << calculated_pow_8__3_5 << std::endl;
        std::string pow_8__3_5_str = calculated_pow_8__3_5.get_str(exp, 10, decimal_digits);
        std::string calculated_pow_8__3_5_str = insertDecimalPoint(pow_8__3_5_str, exp);

        int i;
        for (i = 0; i < decimal_digits; ++i) {
            if (pow_8__3_5[i] != calculated_pow_8__3_5_str[i]) {
                break;
            }
        }
        std::cout << "pow(8, -3.5) matched in " << i - 1 << " decimal digits" << std::endl;
        assert(i - 1 > decimal_digits - 5 && "not accurate");
    }
    std::cout << "test_pow passed." << std::endl;
#endif
}
void test_log2() {
#if !defined USE_ORIGINAL_GMPXX
    mpf_class x, y;
    {
        // https://www.wolframalpha.com/input?i=N%5Blog2%280.5%29%2C+1000%5D
        const char *log2_7_99_approx = "2.998195503153252084684237906062267448232092121426001922089762125213648072392263489504996947139992619474166188136624561745331054122922599092268564364043504602610915693733879191899719177705156349069079002325462765489400159767719231028852240346478326499819301206200359456213705564331606462731528510763995319549023440900671939161381342633868212614923489143366917167754665857473652704556348870409457576678800122309077075030146982206298183346737899758620501440865284344264402422477003345257543941354684027712503638803118633038190976965395920455337138091699508248638565470439496629648120636228305122297817102772855158558708447386137483336148002512701875479954581960327710848237616827187586771165281346426792908371122351828058711125358443206679381715460881022202860259886849238715741328713916215658193520122044561580622587135794301571795822935957653218537880096782471869407502495007680276382295985073865126128668637665581938258605446939585553940054209081772890413199750562198325125278918871994052248064342783";
        mp_bitcnt_t prec = mpf_get_default_prec();
        int decimal_digits = floor(std::log10(2) * prec);
        mp_exp_t exp;

        mpf_class calculated_log2_7_99;
        calculated_log2_7_99 = log2(mpf_class("7.99"));
        std::cout << "log2(7.99) " << std::setprecision(decimal_digits) << calculated_log2_7_99 << std::endl;
        std::string _log2_7_99_pi_str = calculated_log2_7_99.get_str(exp, 10, decimal_digits);
        std::string calculated_log2_7_99_str = insertDecimalPoint(_log2_7_99_pi_str, exp);

        int i;
        for (i = 0; i < decimal_digits; ++i) {
            if (log2_7_99_approx[i] != calculated_log2_7_99_str[i]) {
                break;
            }
        }
        std::cout << "log2(7.99) matched in " << i - 1 << " decimal digits" << std::endl;
        assert(i - 1 > decimal_digits - 4 && "not accurate");
    }
    std::cout << "test_log2 passed." << std::endl;
#endif
}
void test_log10() {
#if !defined USE_ORIGINAL_GMPXX
    mpf_class x, y;
    {
        // https://www.wolframalpha.com/input?i=N%5Blog10%280.5%29%2C+1000%5D
        const char *log10_5_approx = "0.6989700043360188047862611052755069732318101185378914586895725388728918107255754905130727478818138279593155228085690046209052321188664764940003076662953044249354970357458065973381802656883970564988160971018214173828455604681380709536461153004797606891503875374595997366874053785211541526817173273160176738034572064923686824516490728610350530822142310819492099924004519121845402854149680351223738775077091708818090485010028283801395223234999321794820874426713713316579995970794901629154277751045057024378502927553402913863103907780905172387856085034717648321735076851959722537567558366884612617406961169606193667838697609481194178680843145383070946984948680730146215115812816799342464305316070282578679890941031091494143753590127816031233514601437648387226973610721217391501633189696915685844391860563823254511433365754618762660675775304056509397879554957031725393115214538843152315893562020499534030082254342459135981535920543470455658922591706000254599262782983198051109445143089305996245883100365842";
        mp_bitcnt_t prec = mpf_get_default_prec();
        int decimal_digits = floor(std::log10(2) * prec);
        mp_exp_t exp;

        mpf_class calculated_log10_5;
        calculated_log10_5 = log10(mpf_class(5));
        std::cout << "log10(5) " << std::setprecision(decimal_digits) << calculated_log10_5 << std::endl;
        std::string _log10_5_pi_str = calculated_log10_5.get_str(exp, 10, decimal_digits);
        std::string calculated_log10_5_str = insertDecimalPoint(_log10_5_pi_str, exp);

        int i;
        for (i = 0; i < decimal_digits; ++i) {
            if (log10_5_approx[i] != calculated_log10_5_str[i]) {
                break;
            }
        }
        std::cout << "log10(5) matched in " << i - 1 << " decimal digits" << std::endl;
        assert(i - 1 > decimal_digits - 4 && "not accurate");
    }
    std::cout << "test_log10 passed." << std::endl;
#endif
}
void test_atan() {
#if !defined USE_ORIGINAL_GMPXX
#if defined NOT_COMPILE
    {
        for (int i = -30; i < 30; i++) {
            mpf_class x = mpf_class(i) / 10.0;
            mpf_class atanx = atan(mpf_class(x));
            std::cout << "x= " << std::setprecision(3) << x << ", y= " << std::setprecision(30) << atanx << std::endl;
        }
    }
#endif
    {
        // https://www.wolframalpha.com/input?i=N%5Batan%280.5%29%2C+1000%5D
        mpf_class x, y;
        const char *atan_5_approx = "1.373400766945015860861271926444961148650999595899700808969783355912874233164860713581319584633770489878445925182949793219090442417981051760115114208847937288672931304349404758996773693692921814139459112933741098152347770223655231004468230252904201614861500929679550565887562332271559392837057559521749046762564874099862917847341175300778135076105742013090200227268797891423570152023906457675037773553314203967627958693306088655177143482397898654057266514861506185235555840523550790212772584443761839273946290168976498051786958859715775524585010035843455538759926506226276897555173179656899166812371308495189818726605064188403477748764848005080581992432446914621825271354769817716078857258708588002718630110717481894156441027335846838799134077349441220333394698508351484660918345020648861553112581320374608541418345600320069084261642509786643191517722197990887771076309362260530376857622760238111531554883589288281932978712489860212555762429288507306839076989999857868709373960067746719575405158835807";
        mp_bitcnt_t prec = mpf_get_default_prec();
        int decimal_digits = floor(std::log10(2) * prec);
        mp_exp_t exp;

        mpf_class calculated_atan_5;
        calculated_atan_5 = atan(mpf_class(5));
        std::cout << "atan(5) " << std::setprecision(decimal_digits) << calculated_atan_5 << std::endl;
        std::string _atan_5_pi_str = calculated_atan_5.get_str(exp, 10, decimal_digits);
        std::string calculated_atan_5_str = insertDecimalPoint(_atan_5_pi_str, exp);

        int i;
        for (i = 0; i < decimal_digits; ++i) {
            if (atan_5_approx[i] != calculated_atan_5_str[i]) {
                break;
            }
        }
        std::cout << "atan(5) matched in " << i - 1 << " decimal digits" << std::endl;
        assert(i - 1 > decimal_digits - 5 && "not accurate");
    }
    {
        mpf_class x, y;
        // https://www.wolframalpha.com/input?i=N%5Batan%280.5%29%2C+1000%5D
        const char *atan_0_5_approx = "0.4636476090008061162142562314612144020285370542861202638109330887201978641657417053006002839848878925565298522511908375135058181816250111554715305699441056207193362661648801015325027559879258055168538891674782372865387939180125171994840139558381851150950216333064938721546097320785555572086014632275652426730521804574640086974505838973638964890026486877853780128236331217164578146836900993340528882486244562388119090158949767997197011496776001645006253016812125609335304134939663012931924274840293161119492061620844159372361273166876981687027593189510333973325929038512892545945922463215609783638009537499320948607339491864325160274827930450373317725546504996086757706227544162850222737237119744733669773185106940138112699577792562748256600962116726748115272827225207225972684215710195877562091701557768709866542668903449351805472890053707838124212854794303024367845264669937683808877190412767311593748061628833032028804465239589618924130515270876726439400070443923542442569122697771151892771722644634";
        mp_bitcnt_t prec = mpf_get_default_prec();
        int decimal_digits = floor(std::log10(2) * prec);
        mp_exp_t exp;

        mpf_class calculated_atan_0_5;
        calculated_atan_0_5 = atan(mpf_class(0.5));
        std::cout << "atan(0.5) " << std::setprecision(decimal_digits) << calculated_atan_0_5 << std::endl;
        std::string _atan_0_5_pi_str = calculated_atan_0_5.get_str(exp, 10, decimal_digits);
        std::string calculated_atan_0_5_str = insertDecimalPoint(_atan_0_5_pi_str, exp);

        int i;
        for (i = 0; i < decimal_digits; ++i) {
            if (atan_0_5_approx[i] != calculated_atan_0_5_str[i]) {
                break;
            }
        }
        std::cout << "atan(0.5) matched in " << i - 1 << " decimal digits" << std::endl;
        assert(i - 1 > decimal_digits - 5 && "not accurate");
    }
    std::cout << "test_atan passed." << std::endl;
#endif
}
void test_atan2() {
#if !defined USE_ORIGINAL_GMPXX
    {
        const char *pi_str = "3.141592653589793238462643383279502884197169399375105820974944592307816406286208998628034825342117067982148086513282306647093844609550582231725359408128481117450284102701938521105559644622948954930381964428810975665933446128475648233786783165271201909145648566923460348610454326648213393607260249141273724587006606315588174881520920962829254091715364367892590360011330530548820466521384146951941511609433057270365759591953092186117381932611793105118548074462379962749567351885752724891227938183011949129833673362440656643086021394946395224737190702179860943702770539217176293176752384674818467669405132000568127145263560827785771342757789609173637178721468440901224953430146549585371050792279689258923542019956112129021960864034418159813629774771309960518707211349999998372978049951059731732816096318595024459455346908302642522308253344685035261931188171010003137838752886587533208381420617177669147303598253490428755468731159562863882353787593751957781857780532171226806613001927876611195909216420199";
        const char *pi_2_str = "1.570796326794896619231321691639751442098584699687552910487472296153908203143104499314017412671058533991074043256641153323546922304775291115862679704064240558725142051350969260552779822311474477465190982214405487832966723064237824116893391582635600954572824283461730174305227163324106696803630124570636862293503303157794087440760460481414627045857682183946295180005665265274410233260692073475970755804716528635182879795976546093058690966305896552559274037231189981374783675942876362445613969091505974564916836681220328321543010697473197612368595351089930471851385269608588146588376192337409233834702566000284063572631780413892885671378894804586818589360734220450612476715073274792685525396139844629461771009978056064510980432017209079906814887385654980259353605674999999186489024975529865866408048159297512229727673454151321261154126672342517630965594085505001568919376443293766604190710308588834573651799126745214377734365579781431941176893796875978890928890266085613403306500963938305597954608210099";
        const char *pi_m2_str = "-1.570796326794896619231321691639751442098584699687552910487472296153908203143104499314017412671058533991074043256641153323546922304775291115862679704064240558725142051350969260552779822311474477465190982214405487832966723064237824116893391582635600954572824283461730174305227163324106696803630124570636862293503303157794087440760460481414627045857682183946295180005665265274410233260692073475970755804716528635182879795976546093058690966305896552559274037231189981374783675942876362445613969091505974564916836681220328321543010697473197612368595351089930471851385269608588146588376192337409233834702566000284063572631780413892885671378894804586818589360734220450612476715073274792685525396139844629461771009978056064510980432017209079906814887385654980259353605674999999186489024975529865866408048159297512229727673454151321261154126672342517630965594085505001568919376443293766604190710308588834573651799126745214377734365579781431941176893796875978890928890266085613403306500963938305597954608210099";
        const char *pi_4_str = "0.7853981633974483096156608458198757210492923498437764552437361480769541015715522496570087063355292669955370216283205766617734611523876455579313398520321202793625710256754846302763899111557372387325954911072027439164833615321189120584466957913178004772864121417308650871526135816620533484018150622853184311467516515788970437203802302407073135229288410919731475900028326326372051166303460367379853779023582643175914398979882730465293454831529482762796370186155949906873918379714381812228069845457529872824584183406101641607715053487365988061842976755449652359256926348042940732941880961687046169173512830001420317863158902069464428356894474022934092946803671102253062383575366373963427626980699223147308855049890280322554902160086045399534074436928274901296768028374999995932445124877649329332040240796487561148638367270756606305770633361712588154827970427525007844596882216468833020953551542944172868258995633726071888671827898907159705884468984379894454644451330428067016532504819691527989773041050497";
        const char *pi_m4_str = "-0.7853981633974483096156608458198757210492923498437764552437361480769541015715522496570087063355292669955370216283205766617734611523876455579313398520321202793625710256754846302763899111557372387325954911072027439164833615321189120584466957913178004772864121417308650871526135816620533484018150622853184311467516515788970437203802302407073135229288410919731475900028326326372051166303460367379853779023582643175914398979882730465293454831529482762796370186155949906873918379714381812228069845457529872824584183406101641607715053487365988061842976755449652359256926348042940732941880961687046169173512830001420317863158902069464428356894474022934092946803671102253062383575366373963427626980699223147308855049890280322554902160086045399534074436928274901296768028374999995932445124877649329332040240796487561148638367270756606305770633361712588154827970427525007844596882216468833020953551542944172868258995633726071888671827898907159705884468984379894454644451330428067016532504819691527989773041050497";
        const char *pi_3_4_str = "2.356194490192344928846982537459627163147877049531329365731208444230862304714656748971026119006587800986611064884961729985320383457162936673794019556096360838087713077026453890829169733467211716197786473321608231749450084596356736175340087373953401431859236425192595261457840744986160045205445186855955293440254954736691131161140690722121940568786523275919442770008497897911615349891038110213956133707074792952774319693964819139588036449458844828838911055846784972062175513914314543668420953637258961847375255021830492482314516046209796418552893026634895707777077904412882219882564288506113850752053849000426095358947670620839328507068342206880227884041101330675918715072609912189028288094209766944192656514967084096766470648025813619860222331078482470389030408512499998779733537463294798799612072238946268344591510181226981891731190008513776446448391128257502353379064664940649906286065462883251860477698690117821566601548369672147911765340695313968336393335399128420104959751445907458396931912315149";
        const char *pi_m3_4_str = "-2.356194490192344928846982537459627163147877049531329365731208444230862304714656748971026119006587800986611064884961729985320383457162936673794019556096360838087713077026453890829169733467211716197786473321608231749450084596356736175340087373953401431859236425192595261457840744986160045205445186855955293440254954736691131161140690722121940568786523275919442770008497897911615349891038110213956133707074792952774319693964819139588036449458844828838911055846784972062175513914314543668420953637258961847375255021830492482314516046209796418552893026634895707777077904412882219882564288506113850752053849000426095358947670620839328507068342206880227884041101330675918715072609912189028288094209766944192656514967084096766470648025813619860222331078482470389030408512499998779733537463294798799612072238946268344591510181226981891731190008513776446448391128257502353379064664940649906286065462883251860477698690117821566601548369672147911765340695313968336393335399128420104959751445907458396931912315149";
        const char *arctan_1_2str = "1.107148717794090503017065460178537040070047645401432646676539207433710338977362794013417128686170641434544191005450315810041104123150279960391149134120134938005805785186089159020277066323548671948337093046927250546427929146225306917409377626797415839477802650155236302150617431245551139595028661343071619620451122700330078743309876584050730556855033496160917167182032143557952418577001974141917867556092072396370970780081569293338989816628296388058648735549977420441253262448910061152421541607476658452967630519135912384306883380785499443665836032138897074518792365757298891993783946015848255470901612250351968711897831227460369643896101759549486816806083720841936706092318858507663251672427870156094793691467362050699710474237952805081154791173982305447826322952479276589220603404510278110198878003520641243073406563806386080606837666971733818544308606074699132134849976299998223302991267312103414276992963861911174853919055822470048763842269788306246988883221693259159049588694161190408677435945636";
        const char *arctan_1_m2str = "-1.107148717794090503017065460178537040070047645401432646676539207433710338977362794013417128686170641434544191005450315810041104123150279960391149134120134938005805785186089159020277066323548671948337093046927250546427929146225306917409377626797415839477802650155236302150617431245551139595028661343071619620451122700330078743309876584050730556855033496160917167182032143557952418577001974141917867556092072396370970780081569293338989816628296388058648735549977420441253262448910061152421541607476658452967630519135912384306883380785499443665836032138897074518792365757298891993783946015848255470901612250351968711897831227460369643896101759549486816806083720841936706092318858507663251672427870156094793691467362050699710474237952805081154791173982305447826322952479276589220603404510278110198878003520641243073406563806386080606837666971733818544308606074699132134849976299998223302991267312103414276992963861911174853919055822470048763842269788306246988883221693259159049588694161190408677435945636";
        const char *arctan_m1_2str = "2.034443935795702735445577923100965844127121753973673174298405384874106067308846204614617696655946426547603895507831990837052740486400302271334210274008346179444478317515849362085282578299400282982044871381883725119505516982250341316377405538473786069667845916768224046459836895402662254012231587798202104966555483615258096138211044378778523534860330871731673192829298386990868047944382172810023644053340984873994788811871522892778392115983496717059899338912402542308314089436842663738806396575535290676866042843304744258779138014160895781071354670040963869183978173459877401182968438658970212198503519750216158433365729600325401698861687849624150361915384720059288247337827691077707799119851819102828748328488750078322250389796465354732474983597327655070880888397520721783757446546549453622617218315074383216381940344496256441701415677713301443386879564935304005703902910287534985078429349865565733026605289628517580614812103740393833589945323963651534868897310477967647563413233715420787231780474563";
        const char *arctan_m2_m1str = "-2.034443935795702735445577923100965844127121753973673174298405384874106067308846204614617696655946426547603895507831990837052740486400302271334210274008346179444478317515849362085282578299400282982044871381883725119505516982250341316377405538473786069667845916768224046459836895402662254012231587798202104966555483615258096138211044378778523534860330871731673192829298386990868047944382172810023644053340984873994788811871522892778392115983496717059899338912402542308314089436842663738806396575535290676866042843304744258779138014160895781071354670040963869183978173459877401182968438658970212198503519750216158433365729600325401698861687849624150361915384720059288247337827691077707799119851819102828748328488750078322250389796465354732474983597327655070880888397520721783757446546549453622617218315074383216381940344496256441701415677713301443386879564935304005703902910287534985078429349865565733026605289628517580614812103740393833589945323963651534868897310477967647563413233715420787231780474563";
        const char *zero_str = "0.0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";
        struct TestCase {
            mpf_class y;
            mpf_class x;
            const char *expected;
        };
        TestCase testCases[] = {
            {0.0, 0.0, zero_str}, {1.0, 0.0, pi_2_str}, {-1.0, 0.0, pi_m2_str}, {0.0, 1.0, zero_str}, {0.0, -1.0, pi_str}, {1.0, 1.0, pi_4_str}, {1.0, -1.0, pi_3_4_str}, {-1.0, 1.0, pi_m4_str}, {-1.0, -1.0, pi_m3_4_str}, {2.0, 1.0, arctan_1_2str}, {-2.0, 1.0, arctan_1_m2str}, {2.0, -1.0, arctan_m1_2str}, {-2.0, -1.0, arctan_m2_m1str},
        };
        for (size_t i = 0; i < sizeof(testCases) / sizeof(TestCase); ++i) {
            mp_bitcnt_t prec = mpf_get_default_prec();
            int decimal_digits = floor(std::log10(2) * prec);
            mp_exp_t exp;

            mpf_class calculated_atan2;
            calculated_atan2 = atan2(testCases[i].y, testCases[i].x);
            std::string expected_str = std::string(testCases[i].expected);
            std::string calculated_atan2_str;
            std::string _calculated_atan2_str;
            if (calculated_atan2 != 0.0) {
                _calculated_atan2_str = calculated_atan2.get_str(exp, 10, decimal_digits);
                calculated_atan2_str = insertDecimalPoint(_calculated_atan2_str, exp);
            } else
                calculated_atan2_str = zero_str;
            std::cout << "atan2(" << std::setprecision(2) << testCases[i].y << ", " << testCases[i].x << ")=" << std::setprecision(decimal_digits) << calculated_atan2_str << std::endl;
            int j;
            for (j = 0; j < decimal_digits; ++j) {
                if (expected_str[j] != calculated_atan2_str[j]) {
                    break;
                }
            }
            std::cout << "matched: " << j - 1 << std::endl;
            assert(j - 1 > decimal_digits - 5 && "not accurate");
        }
    }
    std::cout << "test_atan2 passed." << std::endl;
#endif
}

void test_int64_t_uint64_t_constructor() {
#if !defined USE_ORIGINAL_GMPXX
    {
        int64_t testValue = INT64_C(-0x6EDCBA9876543210);
        mpz_class value(testValue);

        std::string expected = std::to_string(testValue);

        std::string actual = value.get_str();

        std::cout << "Expected: " << expected << std::endl;
        std::cout << "Actual  : " << actual << std::endl;

        assert(actual == expected && "Comparison failed: The strings do not match.");
        std::cout << "Comparison passed: The strings match." << std::endl;
    }
    {
        uint64_t testValue = UINT64_C(0xFEDCBA9876543210);
        mpz_class value(testValue);

        std::string expected = std::to_string(testValue);

        std::string actual = value.get_str();

        std::cout << "Expected: " << expected << std::endl;
        std::cout << "Actual  : " << actual << std::endl;

        assert(actual == expected && "Comparison failed: The strings do not match.");
        std::cout << "Comparison passed: The strings match." << std::endl;
    }
#endif
}

void test_int32_t_uint32_t_constructor() {
#if !defined USE_ORIGINAL_GMPXX
    {
        int32_t testValue = INT32_C(-0x12345678);
        mpz_class value(testValue);

        std::string expected = std::to_string(testValue);
        std::string actual = value.get_str();

        std::cout << "Testing int32_t constructor:" << std::endl;
        std::cout << "Expected: " << expected << std::endl;
        std::cout << "Actual  : " << actual << std::endl;

        assert(actual == expected && "int32_t constructor test failed: The strings do not match.");
        std::cout << "int32_t constructor test passed!" << std::endl;
    }
    {
        uint32_t testValue = UINT32_C(0xFEDCBA98);
        mpz_class value(testValue);

        std::string expected = std::to_string(testValue);

        std::string actual = value.get_str();

        std::cout << "Testing uint32_t constructor:" << std::endl;
        std::cout << "Expected: " << expected << std::endl;
        std::cout << "Actual  : " << actual << std::endl;

        assert(actual == expected && "uint32_t constructor test failed: The strings do not match.");
        std::cout << "uint32_t constructor test passed!" << std::endl;
    }
#endif
}
void test_int64_t_uint64_t_int32_t_uint32_t_assignment() {
#if !defined USE_ORIGINAL_GMPXX
    {
        int64_t testValue = INT64_C(-0x7EDCBA9876543210);
        mpz_class value;
        value = testValue;

        std::string expected = std::to_string(testValue);
        std::string actual = value.get_str();

        std::cout << "Testing int64_t assignment:" << std::endl;
        std::cout << "Expected: " << expected << std::endl;
        std::cout << "Actual  : " << actual << std::endl;

        assert(actual == expected && "int64_t assignment test failed.");
        std::cout << "int64_t assignment test passed!" << std::endl;
    }
    {
        uint64_t testValue = UINT64_C(0xFEDCBA9876543210);
        mpz_class value;
        value = testValue;

        std::string expected = std::to_string(testValue);
        std::string actual = value.get_str();

        std::cout << "Testing uint64_t assignment:" << std::endl;
        std::cout << "Expected: " << expected << std::endl;
        std::cout << "Actual  : " << actual << std::endl;

        assert(actual == expected && "uint64_t assignment test failed.");
        std::cout << "uint64_t assignment test passed!" << std::endl;
    }
    {
        int32_t testValue = INT32_C(-0x7FFCBA98);
        mpz_class value;
        value = testValue;

        std::string expected = std::to_string(testValue);
        std::string actual = value.get_str();

        std::cout << "Testing int32_t assignment:" << std::endl;
        std::cout << "Expected: " << expected << std::endl;
        std::cout << "Actual  : " << actual << std::endl;

        assert(actual == expected && "int32_t assignment test failed.");
        std::cout << "int32_t assignment test passed!" << std::endl;
    }
    {
        uint32_t testValue = UINT32_C(0xFEDCBA98);
        mpz_class value;
        value = testValue;

        std::string expected = std::to_string(testValue);
        std::string actual = value.get_str();

        std::cout << "Testing uint32_t assignment:" << std::endl;
        std::cout << "Expected: " << expected << std::endl;
        std::cout << "Actual  : " << actual << std::endl;

        assert(actual == expected && "uint32_t assignment test failed.");
        std::cout << "uint32_t assignment test passed!" << std::endl;
    }
#endif
}
int main() {
#if defined USE_ORIGINAL_GMPXX
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
    testInitializationAndAssignment_int64_t_uint64_t_mpz_class();

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
    test_mpz_class_comparison_int();

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

    // mpf_class transcendental functions
    test_mpf_class_const_pi();
    test_mpf_class_const_log2();
    test_div2exp_mul2exp_mpf_class();
    test_log_mpf_class();
    test_exp_mpf_class();

    // mpf_class, mpz_class, mpq_class cast
    test_casts();

    // misc tests
    test_precisions_mixed();
    test_misc();

    //
    test_reminder();
    test_cos();
    test_sin();
    test_tan();
    test_pow();
    test_log2();
    test_log10();
    test_atan();
    test_atan2();

    test_int64_t_uint64_t_constructor();
    test_int32_t_uint32_t_constructor();
    test_int64_t_uint64_t_int32_t_uint32_t_assignment();

    std::cout << "All tests passed." << std::endl;

    return 0;
}
