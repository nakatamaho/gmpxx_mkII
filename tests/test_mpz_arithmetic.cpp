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
#include <new>
#include <stdexcept>

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

void check_shift_and_bitwise() {
    mpz_class a(0xcafe);
    mpz_class b(0xbeef);
    mpz_class c;
    c = a & b;
    assert(c == mpz_class(0x8aee));
    c = a | b;
    assert(c == mpz_class(0xfeff));
    c = a ^ b;
    assert(c == mpz_class(0x7411));
    c = a & -0xbeef;
    assert(c == mpz_class(0x4010));
    c = a | -0xbeef;
    assert(c == mpz_class(-0x3401));
    c = a ^ 48879.0;
    assert(c == mpz_class(0x7411));
    c = 0xcafeL & b;
    assert(c == mpz_class(0x8aee));
    c = 0xcafeL | b;
    assert(c == mpz_class(0xfeff));
    c = 0xcafeL ^ b;
    assert(c == mpz_class(0x7411));
    c = ~mpz_class(3);
    assert(c == mpz_class(-4));
    c = ~(a & b);
    assert(c == mpz_class(-0x8aef));

    mpz_class shifted = (-mpz_class(5)) << 2u;
    assert(shifted == mpz_class(-20));
    shifted = (mpz_class(5) * mpz_class(-4)) >> 3u;
    assert(shifted == mpz_class(-3));
    shifted = mpz_class(3);
    shifted <<= 4u;
    assert(shifted == mpz_class(48));
    shifted = mpz_class(-20);
    shifted >>= 3u;
    assert(shifted == mpz_class(-3));
}

void check_integer_helpers() {
    assert(gcd(mpz_class(6), mpz_class(8)) == mpz_class(2));
    assert(gcd(-mpz_class(6), mpz_class(8)) == mpz_class(2));
    assert(gcd(static_cast<long>(-6), mpz_class(5) + 3) == mpz_class(2));
    assert(lcm(mpz_class(6), mpz_class(8)) == mpz_class(24));
    assert(lcm(-mpz_class(6), -mpz_class(8)) == mpz_class(24));
    assert(lcm(-6.0, mpz_class(5) + 3) == mpz_class(24));

    assert(factorial(mpz_class(3)) == mpz_class(6));
    assert(factorial(mpz_class(5) - 1) == mpz_class(24));
    assert(mpz_class::factorial(mpz_class(3)) == mpz_class(6));
    assert(mpz_class::factorial(3.0f) == mpz_class(6));

    assert(primorial(mpz_class(5)) == mpz_class(30));
    assert(mpz_class::primorial(mpz_class(2) * 2) == mpz_class(6));
    assert(mpz_class::primorial(3UL) == mpz_class(6));

    assert(fibonacci(mpz_class(6)) == mpz_class(8));
    assert(mpz_class::fibonacci(mpz_class(2) * 2) == mpz_class(3));
    assert(mpz_class::fibonacci(3.0f) == mpz_class(2));
    assert(fibonacci(-mpz_class(6)) == mpz_class(-8));
    assert(mpz_class::fibonacci(-3) == mpz_class(2));

    bool threw_domain = false;
    try {
        (void)factorial(-mpz_class(3));
    } catch (std::domain_error const&) {
        threw_domain = true;
    }
    assert(threw_domain);

    threw_domain = false;
    try {
        (void)mpz_class::primorial(-5);
    } catch (std::domain_error const&) {
        threw_domain = true;
    }
    assert(threw_domain);

    bool threw_bad_alloc = false;
    try {
        (void)factorial(mpz_class(1) << 300);
    } catch (std::bad_alloc const&) {
        threw_bad_alloc = true;
    }
    assert(threw_bad_alloc);

    threw_bad_alloc = false;
    try {
        (void)fibonacci(mpz_class(1) << 300);
    } catch (std::bad_alloc const&) {
        threw_bad_alloc = true;
    }
    assert(threw_bad_alloc);
}

void check_nested_product_expression_shapes() {
    {
        mpz_class a(17), b(-5), c(9);
        mpz_class d;
        d = a + b * c;
        assert(d == mpz_class(-28));
        d = a - b * c;
        assert(d == mpz_class(62));
    }
    {
        mpz_class a(-14), b(6), c(-8);
        double d = 2.0;
        mpz_class e;
        e = a + b * (c + d);
        assert(e == mpz_class(-50));
        e = a - b * (c + d);
        assert(e == mpz_class(22));
    }
    {
        mpz_class a(23), b(-11);
        unsigned int c = 4, d = 7;
        mpz_class e;
        e = a + (b + c) * d;
        assert(e == mpz_class(-26));
        e = a - (b + c) * d;
        assert(e == mpz_class(72));
    }
    {
        mpz_class a(-31), b(12), c(-6);
        signed int d = -3, e = 10;
        mpz_class f;
        f = a + (b - d) * (c + e);
        assert(f == mpz_class(29));
        f = a - (b - d) * (c + e);
        assert(f == mpz_class(-91));
    }
    {
        mpz_class a(-7), b(-13), c(29);
        mpz_class d;
        d = a * b + c;
        assert(d == mpz_class(120));
        d = a * b - c;
        assert(d == mpz_class(62));
    }
    {
        mpz_class a(19), b(-17);
        double c = -3.0, d = 4.0;
        mpz_class e;
        e = c * (a + d) + b;
        assert(e == mpz_class(-86));
        e = c * (a + d) - b;
        assert(e == mpz_class(-52));
    }
    {
        mpz_class a(-4), b(15), c(-9);
        unsigned int d = 6, e = 20;
        mpz_class f;
        f = a * (b - d) + (c + e);
        assert(f == mpz_class(-25));
        f = a * (b - d) - (c + e);
        assert(f == mpz_class(-47));
    }
    {
        mpz_class a(8), b(-12), c(5);
        double d = -15.0, e = 3.0, f = -21.0;
        mpz_class g;
        g = (a + d) * (b - e) + (c + f);
        assert(g == mpz_class(89));
        g = (a + d) * (b - e) - (c + f);
        assert(g == mpz_class(121));
    }
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
    check_shift_and_bitwise();
    check_integer_helpers();
    check_nested_product_expression_shapes();

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
