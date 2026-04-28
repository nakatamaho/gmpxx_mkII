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
#include <concepts>
#include <cstdint>
#include <stdexcept>
#include <type_traits>

namespace {

mpz_class z(std::int64_t value) {
    return mpz_class(value);
}

void assert_mpf_equal(mpf_class const& lhs, mpf_class const& rhs) {
    assert(mpf_cmp(lhs.get_mpf_t(), rhs.get_mpf_t()) == 0);
}

void test_compile_time_surface() {
    static_assert(std::is_default_constructible_v<gmp_randclass>);
    static_assert(std::is_constructible_v<
                  gmp_randclass, void (*)(gmp_randstate_t)>);
    static_assert(!std::is_copy_constructible_v<gmp_randclass>);
    static_assert(!std::is_copy_assignable_v<gmp_randclass>);
    static_assert(!std::is_move_constructible_v<gmp_randclass>);
    static_assert(!std::is_move_assignable_v<gmp_randclass>);
    static_assert(std::is_same_v<gmpxx::gmp_randclass, gmp_randclass>);
    static_assert(std::is_same_v<gmpxx::random_mpf_expr,
                                 random_mpf_expr>);
    static_assert(gmpxx_expr<random_mpf_expr>);
    static_assert(std::same_as<random_mpf_expr::result_type, mpf_class>);
}

void test_deterministic_seed_with_ui() {
    gmp_randclass r1(gmp_randinit_default);
    gmp_randclass r2(gmp_randinit_default);

    r1.seed(12345ul);
    r2.seed(12345ul);

    mpz_class z1 = r1.get_z_bits(static_cast<mp_bitcnt_t>(192));
    mpz_class z2 = r2.get_z_bits(static_cast<mp_bitcnt_t>(192));
    assert(z1 == z2);

    mpf_class f1 = r1.get_f(static_cast<mp_bitcnt_t>(256));
    mpf_class f2 = r2.get_f(static_cast<mp_bitcnt_t>(256));
    assert(f1.get_prec() == gmpxx_detail::effective_mpf_prec(256));
    assert(f2.get_prec() == f1.get_prec());
    assert_mpf_equal(f1, f2);
    assert(f1 >= mpf_class(0.0, f1.get_prec()));
    assert(f1 < mpf_class(1.0, f1.get_prec()));
}

void test_deterministic_seed_with_mpz() {
    gmp_randclass r1;
    gmp_randclass r2;
    mpz_class seed("123456789012345678901234567890");

    r1.seed(seed);
    r2.seed(seed);

    assert(r1.get_z_bits(static_cast<mp_bitcnt_t>(160)) ==
           r2.get_z_bits(static_cast<mp_bitcnt_t>(160)));
}

void test_z_range() {
    gmp_randclass r(gmp_randinit_mt);
    r.seed(7ul);

    mpz_class limit("1000000000000000000000000000000");
    for (int i = 0; i < 16; ++i) {
        mpz_class value = r.get_z_range(limit);
        assert(value >= z(0));
        assert(value < limit);
    }
}

void test_z_bits_from_mpz() {
    gmp_randclass r;
    r.seed(99ul);

    mpz_class bits = z(96);
    mpz_class value = r.get_z_bits(bits);
    assert(value >= z(0));

    bool threw = false;
    try {
        (void)r.get_z_bits(z(-1));
    } catch (std::invalid_argument const&) {
        threw = true;
    }
    assert(threw);
}

void test_f_precision_forms() {
    gmp_randclass r1;
    gmp_randclass r2;
    r1.seed(17ul);
    r2.seed(17ul);

    mpf_class prototype("0", static_cast<mp_bitcnt_t>(384));
    mpf_class by_object = r1.get_f(prototype);
    mpf_class by_prec = r2.get_f(prototype.get_prec());

    assert(by_object.get_prec() == prototype.get_prec());
    assert(by_prec.get_prec() == prototype.get_prec());
    assert_mpf_equal(by_object, by_prec);

    mpf_class by_default = r1.get_f();
    assert(by_default.get_prec() == gmpxx_defaults::get_default_prec());

    gmp_randclass r3;
    gmp_randclass r4;
    r3.seed(23ul);
    r4.seed(23ul);

    mpf_class assigned("0", static_cast<mp_bitcnt_t>(448));
    mpf_class expected = r4.get_f(assigned.get_prec());
    assigned = r3.get_f();

    assert(assigned.get_prec() == expected.get_prec());
    assert_mpf_equal(assigned, expected);
}

void test_lc_constructors() {
    gmp_randclass by_size(gmp_randinit_lc_2exp_size,
                          static_cast<mp_bitcnt_t>(32));
    by_size.seed(3ul);
    assert(by_size.get_z_bits(static_cast<mp_bitcnt_t>(32)) >= z(0));

    mpz_class a = z(5);
    gmp_randclass by_params(gmp_randinit_lc_2exp, a, 1ul,
                            static_cast<mp_bitcnt_t>(32));
    by_params.seed(3ul);
    assert(by_params.get_z_bits(static_cast<mp_bitcnt_t>(32)) >= z(0));

    gmp_randclass obsolete(GMP_RAND_ALG_LC, static_cast<mp_bitcnt_t>(32));
    obsolete.seed(3ul);
    assert(obsolete.get_z_bits(static_cast<mp_bitcnt_t>(32)) >= z(0));

    bool threw = false;
    try {
        gmp_randclass too_large(gmp_randinit_lc_2exp_size,
                                static_cast<mp_bitcnt_t>(129));
        (void)too_large;
    } catch (std::length_error const&) {
        threw = true;
    }
    assert(threw);
}

}  // namespace

int main() {
    test_compile_time_surface();
    test_deterministic_seed_with_ui();
    test_deterministic_seed_with_mpz();
    test_z_range();
    test_z_bits_from_mpz();
    test_f_precision_forms();
    test_lc_constructors();
    return 0;
}
