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
#include <limits>

namespace {

void assert_equal(mpz_class const& got, mpz_t const ref) {
    assert(mpz_cmp(got.get_mpz_t(), ref) == 0);
}

void check_binary(mpz_class const& a, mpz_class const& b) {
    mpz_t ref;
    mpz_init(ref);

    mpz_add(ref, a.get_mpz_t(), b.get_mpz_t());
    assert_equal(mpz_class(a + b), ref);

    mpz_sub(ref, a.get_mpz_t(), b.get_mpz_t());
    assert_equal(mpz_class(a - b), ref);

    mpz_mul(ref, a.get_mpz_t(), b.get_mpz_t());
    assert_equal(mpz_class(a * b), ref);

    if (mpz_sgn(b.get_mpz_t()) != 0) {
        mpz_tdiv_q(ref, a.get_mpz_t(), b.get_mpz_t());
        assert_equal(mpz_class(a / b), ref);

        mpz_class mod = a;
        mod %= b;
        mpz_tdiv_r(ref, a.get_mpz_t(), b.get_mpz_t());
        assert_equal(mod, ref);
        assert_equal(a % b, ref);
    }

    mpz_clear(ref);
}

void check_scalar(mpz_class const& a) {
    mpz_t ref;
    mpz_init(ref);

    mpz_add_ui(ref, a.get_mpz_t(), 5);
    assert_equal(mpz_class(a + 5LL), ref);
    assert_equal(mpz_class(5LL + a), ref);

    mpz_sub_ui(ref, a.get_mpz_t(), 5);
    assert_equal(mpz_class(a - 5LL), ref);

    mpz_ui_sub(ref, 5, a.get_mpz_t());
    assert_equal(mpz_class(5LL - a), ref);

    mpz_mul_ui(ref, a.get_mpz_t(), 7);
    assert_equal(mpz_class(a * 7u), ref);
    assert_equal(mpz_class(7u * a), ref);

    mpz_tdiv_q_ui(ref, a.get_mpz_t(), 3);
    assert_equal(mpz_class(a / 3u), ref);

    mpz_tdiv_r_ui(ref, a.get_mpz_t(), 3);
    assert_equal(a % 3u, ref);

    mpz_clear(ref);
}

}  // namespace

int main() {
    mpz_class zero(std::int64_t{0});
    mpz_class one(std::int64_t{1});
    mpz_class neg_one(std::int64_t{-1});
    mpz_class max_i(std::numeric_limits<std::int64_t>::max());
    mpz_class min_i(std::numeric_limits<std::int64_t>::min());
    mpz_class max_u(std::numeric_limits<std::uint64_t>::max());
    mpz_class huge("1267650600228229401496703205376"); // 2^100
    mpz_class neg_huge("-100000000000000000000000000000000000001");

    check_binary(zero, one);
    check_binary(one, neg_one);
    check_binary(max_i, neg_one);
    check_binary(min_i, one);
    check_binary(max_u, huge);
    check_binary(neg_huge, max_i);

    check_scalar(huge);
    check_scalar(neg_huge);

    {
        mpz_class a("100");
        mpz_class b("7");
        mpz_class c("-3");
        mpz_class got = (a + b) * c;
        mpz_t tmp;
        mpz_t ref;
        mpz_init(tmp);
        mpz_init(ref);
        mpz_add(tmp, a.get_mpz_t(), b.get_mpz_t());
        mpz_mul(ref, tmp, c.get_mpz_t());
        assert_equal(got, ref);
        mpz_clear(ref);
        mpz_clear(tmp);
    }

    {
        mpz_class a("100");
        mpz_class b("7");
        mpz_class ref = a;
        ref += b;
        a = a + b;
        assert(mpz_cmp(a.get_mpz_t(), ref.get_mpz_t()) == 0);
    }

    {
        mpz_class a("12");
        mpz_class b("5");
        mpz_class c("2");
        mpz_t ref;
        mpz_init_set(ref, a.get_mpz_t());
        mpz_t tmp;
        mpz_init(tmp);
        mpz_add(tmp, b.get_mpz_t(), c.get_mpz_t());
        mpz_add(ref, ref, tmp);
        a += b + c;
        assert_equal(a, ref);
        mpz_clear(tmp);
        mpz_clear(ref);
    }

    assert(mpz_class(-huge).sgn() < 0);
    assert(mpz_class(+huge).sgn() > 0);

    return 0;
}
