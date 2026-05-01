/*
 * Copyright (c) 2026
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

/*
 * Example 02: direct square-root evaluation.
 *
 * This is a small API example for gmpxx::sqrt(mpf_class).  It uses the
 * library overload so that overload resolution cannot fall back to
 * std::sqrt(double), which would silently truncate the computation to
 * IEEE-754 double precision.  The example is meant to show the direct
 * high-precision call pattern, not to present a new square-root algorithm.
 *
 * No DOI-bearing first publication is cited for this file because the
 * demonstrated operation is a wrapper API call around GMP floating-point
 * arithmetic.
 */

#include "gmpxx_mkII.h"

#include <iomanip>
#include <iostream>

int main() {
    constexpr mp_bitcnt_t precision = 256;

    gmpxx::mpf_class two("2.0", precision);
    gmpxx::mpf_class result = gmpxx::sqrt(two);

    std::cout << std::setprecision(50)
              << "sqrt(2) = " << result << '\n';
    return 0;
}
