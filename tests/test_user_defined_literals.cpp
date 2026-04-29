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

#include "gmpxx_mkII.h"

#include <cassert>
#include <cstdint>

namespace {

void assert_mpf_equal(mpf_class const& lhs, mpf_class const& rhs) {
    assert(mpf_cmp(lhs.get_mpf_t(), rhs.get_mpf_t()) == 0);
}

void test_mpz_literals() {
    using namespace gmpxx::literals;

    mpz_class a = 42_mpz;
    mpz_class b = "123456789012345678901234567890"_mpz;

    assert(a == mpz_class(std::int64_t{42}));
    assert(b == mpz_class("123456789012345678901234567890", 10));
}

void test_mpq_literals() {
    using namespace gmpxx::literals;

    mpq_class a = 42_mpq;
    mpq_class b = "2/4"_mpq;

    assert(a == mpq_class(std::int64_t{42}));
    assert(b == mpq_class("1/2", 10));
}

void test_mpf_literals() {
    using namespace gmpxx::literals;

    mpf_class a = 1.25_mpf;
    mpf_class b = "1.25"_mpf;
    mpf_class c = 0.5_mpf;
    mpf_class d = "3.75"_mpf;

    assert_mpf_equal(a, b);
    assert_mpf_equal(c, mpf_class(0.5, c.get_prec()));
    assert_mpf_equal(d, mpf_class(3.75, d.get_prec()));
}

void test_string_literals_use_auto_base() {
    using namespace gmpxx::literals;

    gmpxx_defaults::set_default_base(16);

    mpz_class z = "0xff"_mpz;
    assert(z == mpz_class(std::int64_t{255}));

    mpq_class q = "0x10/0x20"_mpq;
    assert(q == mpq_class("1/2", 10));

    mpf_class f = "1.5"_mpf;
    assert(f == mpf_class("1.5", f.get_prec(), 10));

    mpz_class raw = 123_mpz;
    assert(raw == mpz_class(std::int64_t{123}));

    gmpxx_defaults::set_default_base(10);
}

}  // namespace

int main() {
    test_mpz_literals();
    test_mpq_literals();
    test_mpf_literals();
    test_string_literals_use_auto_base();
    return 0;
}
