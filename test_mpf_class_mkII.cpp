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

#include "mpf_class_mkII.h"

using namespace gmp;

// Asserts that the mpf_class object equals the expected string representation
bool IsGMPEquals(mpf_class &gmpobj, const char *expected, int base = defaults::base, int precision = 10) {
    char formatString[64];
    char buffer[64];

    // Adjust the comparison based on the base
    switch (base) {
    case 2:
        std::sprintf(formatString, "%%.%dRb", precision);      // Generates format string like "%.10Rf"
        gmp_sprintf(buffer, formatString, gmpobj.get_mpf_t()); // Uses generated format string
        break;
    case 10:
        std::sprintf(formatString, "%%.%dRf", precision);      // Generates format string like "%.10Rf"
        gmp_sprintf(buffer, formatString, gmpobj.get_mpf_t()); // Uses generated format string
        break;

    case 16:
        std::sprintf(formatString, "%%.%dRa", precision);      // Generates format string like "%.10Rf"
        gmp_sprintf(buffer, formatString, gmpobj.get_mpf_t()); // Uses generated format string
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
    mp_bitcnt_t defaultPrec = defaults::get_default_prec();
    assert(defaultPrec == 512);
    std::cout << "Default precision test passed." << std::endl;

    defaults::set_default_prec(1024);
    defaultPrec = defaults::get_default_prec();
    assert(defaultPrec == 1024);
    std::cout << "Set and get precision test passed." << std::endl;

    defaults::set_default_prec(512);
}

void testDefaultConstructor() {
    mpf_class a;
    char buffer[100];
    gmp_snprintf(buffer, sizeof(buffer), "%.0Rf", a.get_mpf_t());
    assert(std::string(buffer) == "nan");
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

void testInitializationAndAssignmentDouble() {
    double testValue = 3.1415926535;
    const char *expectedValue = "3.1415926535";

    gmp::mpf_class a = (mpf_class)testValue;
    assert(IsGMPEquals(a, expectedValue));
    std::cout << "Substitution from double using constructor test passed." << std::endl;

    gmp::mpf_class b;
    b = testValue;
    assert(IsGMPEquals(b, expectedValue));
    std::cout << "Substitution from double using assignment test passed." << std::endl;
}

int main() {
    ////////////////////////////////////////////////////////////////////////////////////////
    // 5.1 Initialization Functions
    ////////////////////////////////////////////////////////////////////////////////////////
    testDefaultPrecision();
    testDefaultConstructor();
    testCopyConstructor();
    std::cout << "All tests passed." << std::endl;

    return 0;
}
