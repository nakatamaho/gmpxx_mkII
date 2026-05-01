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
 * Example 01: minimal mpf_class arithmetic.
 *
 * This file is intentionally not an algorithmic example.  It demonstrates
 * the smallest source-compatible shape expected from gmpxx_mkII: construct
 * two GMP-backed floating-point values at an explicit precision, build an
 * expression-template addition, and materialize the result as mpf_class.
 *
 * There is therefore no DOI-bearing first publication to cite here.  The
 * point of the example is library usage rather than a numerical method.
 */

#include "gmpxx_mkII.h"

#include <iostream>

int main() {
    gmpxx::mpf_class lhs("1.5", static_cast<mp_bitcnt_t>(128));
    gmpxx::mpf_class rhs("2.5", static_cast<mp_bitcnt_t>(128));

    gmpxx::mpf_class result = lhs + rhs;

    std::cout << "1.5 + 2.5 = " << result << '\n';
    return 0;
}
