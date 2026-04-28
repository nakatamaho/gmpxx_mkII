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
#include <type_traits>
#include <utility>

namespace {

static_assert(std::same_as<decltype(std::declval<mpz_class const&>() ==
                                    std::declval<mpz_class const&>()), bool>);
static_assert(std::same_as<decltype(std::declval<mpq_class const&>() <
                                    std::declval<mpz_class const&>()), bool>);
static_assert(std::same_as<decltype(std::declval<mpf_class const&>() >=
                                    std::declval<mpq_class const&>()), bool>);

static_assert(requires(mpz_class z1, mpz_class z2, mpz_class z3) {
    { (z1 + z2) == z3 } -> std::same_as<bool>;
});
static_assert(requires(mpq_class q, mpz_class z) {
    { (q + z) < q } -> std::same_as<bool>;
});
static_assert(requires(mpf_class f, mpz_class z) {
    { (f + z) >= f } -> std::same_as<bool>;
});
static_assert(!comparison_pair<int, int>);
static_assert(comparison_pair<mpz_class, int>);
static_assert(comparison_pair<int, mpz_class>);

template<class L, class R>
void check_consistency(L const& lhs, R const& rhs) {
    int r = cmp(lhs, rhs);
    assert((lhs == rhs) == (r == 0));
    assert((lhs != rhs) == (r != 0));
    assert((lhs < rhs) == (r < 0));
    assert((lhs <= rhs) == (r <= 0));
    assert((lhs > rhs) == (r > 0));
    assert((lhs >= rhs) == (r >= 0));
}

void test_exact_comparisons() {
    mpz_class z0(std::int64_t{0});
    mpz_class z1(std::int64_t{1});
    mpz_class z2(std::int64_t{2});
    mpz_class z7(std::int64_t{7});
    mpz_class zn(std::int64_t{-1});

    mpq_class q1("1/2");
    mpq_class q2("2/4");
    mpq_class q3("-3/7");
    mpq_class q72("7/2");

    assert(q1 == q2);
    assert(q3 < q1);
    assert(z0 < q1);
    assert(z1 > q1);
    assert(zn < q3);
    assert(cmp(z7, z2) > 0);
    assert(cmp(z7, q72) > 0);
    assert(cmp(z2, q72) < 0);
    assert(cmp(mpq_class("6/4"), mpq_class("3/2")) == 0);

    mpz_class big1("1234567890123456789012345678901234567890");
    mpz_class big2("1234567890123456789012345678901234567891");
    assert(big1 < big2);
    assert(big2 > big1);
    assert(big1 != big2);

    mpq_class rb1("123456789012345678901234567890/7");
    mpq_class rb2("123456789012345678901234567891/7");
    assert(rb1 < rb2);
    check_consistency(z1, z2);
    check_consistency(q1, q2);
    check_consistency(z1, q1);
    check_consistency(q1, z1);
}

void test_mpf_comparisons() {
    mpf_class f0(0.0);
    mpf_class f1(1.0);
    mpf_class fn(-1.0);

    assert(f0 == 0.0);
    assert(f1 > f0);
    assert(fn < f0);
    assert(f1 != fn);

    mpf_class f(1.5);
    mpz_class z1(std::int64_t{1});
    mpz_class z2(std::int64_t{2});
    mpq_class q("3/2");
    assert(f > z1);
    assert(f < z2);
    assert(f == q);

    check_consistency(f0, f1);
    check_consistency(f, z1);
    check_consistency(z1, f);
    check_consistency(f, q);
    check_consistency(q, f);
}

void test_expression_comparisons() {
    mpz_class a(std::int64_t{2});
    mpz_class b(std::int64_t{3});
    mpz_class c(std::int64_t{5});
    assert((a + b) == c);
    assert((a * b) > c);

    mpq_class q1("1/2");
    mpq_class q2("1/3");
    mpq_class q3("5/6");
    assert((q1 + q2) == q3);

    mpf_class f1(1.25);
    mpf_class f2(2.5);
    assert((f1 + f1) == f2);

    mpz_class z(std::int64_t{2});
    mpq_class q("1/2");
    mpf_class f(2.5);
    assert((z + q) == f);
    assert((f - q) == z);

    check_consistency(a + b, c);
    check_consistency(q1 + q2, q3);
    check_consistency(f1 + f1, f2);
    check_consistency(z + q, f);
}

void test_scalar_comparisons() {
    mpz_class z(std::int64_t{5});
    assert(z == 5);
    assert(5 == z);
    assert(z < 6);
    assert(6 > z);

    mpq_class q("5/2");
    assert(q > 2);
    assert(2 < q);
    assert(q == 2.5);

    mpf_class f(5.5);
    assert(f == 5.5);
    assert(5.5 == f);
    assert(f > 5);
    assert(5 < f);

    std::int64_t min_i = std::numeric_limits<std::int64_t>::min();
    std::int64_t max_i = std::numeric_limits<std::int64_t>::max();
    std::uint64_t max_u = std::numeric_limits<std::uint64_t>::max();

    mpz_class min_z(min_i);
    mpz_class max_z(max_i);
    mpz_class max_u_z(max_u);
    assert(min_z == min_i);
    assert(min_i == min_z);
    assert(max_z == max_i);
    assert(max_i == max_z);
    assert(max_u_z == max_u);
    assert(max_u == max_u_z);

    check_consistency(z, 5);
    check_consistency(5, z);
    check_consistency(q, 2.5);
    check_consistency(2.5, q);
    check_consistency(f, 5.5);
    check_consistency(5.5, f);
}

void test_division_semantics() {
    mpz_class seven(std::int64_t{7});
    mpz_class two(std::int64_t{2});

    assert((seven / two) == mpz_class(std::int64_t{3}));
    assert((seven / two) != mpq_class("7/2"));
    assert((mpq_class(std::int64_t{7}) / two) == mpq_class("7/2"));
}

void test_precision_policy_interaction() {
    mpf_class a("1.25", static_cast<mp_bitcnt_t>(64));
    mpf_class b("2.5", static_cast<mp_bitcnt_t>(512));
    mpf_class evaluated = (a + b).eval();

    assert((a + b) == evaluated);
    assert(cmp(a + b, evaluated) == 0);
}

}  // namespace

int main() {
    test_exact_comparisons();
    test_mpf_comparisons();
    test_expression_comparisons();
    test_scalar_comparisons();
    test_division_semantics();
    test_precision_policy_interaction();
    return 0;
}
