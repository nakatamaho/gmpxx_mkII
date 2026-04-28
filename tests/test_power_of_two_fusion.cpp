/*
 * Copyright (c) 2008-2026
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

void assert_equal(mpf_class const& got, mpf_t ref) {
    assert(mpf_cmp(got.get_mpf_t(), ref) == 0);
}

void ref_mul_2exp(mpf_t ref, mpf_class const& a, mp_bitcnt_t exp,
                  bool negative) {
    mpf_mul_2exp(ref, a.get_mpf_t(), exp);
    if (negative) {
        mpf_neg(ref, ref);
    }
}

void ref_div_2exp(mpf_t ref, mpf_class const& a, mp_bitcnt_t exp,
                  bool negative) {
    mpf_div_2exp(ref, a.get_mpf_t(), exp);
    if (negative) {
        mpf_neg(ref, ref);
    }
}

void ref_mul_ui(mpf_t ref, mpf_class const& a, unsigned long value) {
    mpf_mul_ui(ref, a.get_mpf_t(), value);
}

void ref_div_ui(mpf_t ref, mpf_class const& a, unsigned long value) {
    mpf_div_ui(ref, a.get_mpf_t(), value);
}

void check_constructed_mul2(mpf_class const& got, mpf_class const& a,
                            mp_bitcnt_t exp, bool negative = false) {
    mpf_t ref;
    mpf_init2(ref, got.get_prec());
    ref_mul_2exp(ref, a, exp, negative);
    assert_equal(got, ref);
    mpf_clear(ref);
}

void check_constructed_div2(mpf_class const& got, mpf_class const& a,
                            mp_bitcnt_t exp, bool negative = false) {
    mpf_t ref;
    mpf_init2(ref, got.get_prec());
    ref_div_2exp(ref, a, exp, negative);
    assert_equal(got, ref);
    mpf_clear(ref);
}

}  // namespace

int main() {
    mpf_class a("3.75", 256);

    check_constructed_mul2(a * std::uint64_t{1}, a, 0);
    check_constructed_mul2(a * std::uint64_t{2}, a, 1);
    check_constructed_mul2(a * std::uint64_t{8}, a, 3);
    check_constructed_mul2(std::uint64_t{8} * a, a, 3);
    check_constructed_div2(a / std::uint64_t{8}, a, 3);

    check_constructed_mul2(a * std::int64_t{-8}, a, 3, true);
    check_constructed_mul2(std::int64_t{-8} * a, a, 3, true);
    check_constructed_div2(a / std::int64_t{-8}, a, 3, true);

    check_constructed_mul2(a * std::numeric_limits<std::int64_t>::min(),
                           a, 63, true);
    check_constructed_div2(a / std::numeric_limits<std::int64_t>::min(),
                           a, 63, true);

    {
        mpf_class got = a * std::uint64_t{0};
        mpf_t ref;
        mpf_init2(ref, got.get_prec());
        ref_mul_ui(ref, a, 0);
        assert_equal(got, ref);
        mpf_clear(ref);
    }

    {
        mpf_class got = a * std::uint64_t{3};
        mpf_t ref;
        mpf_init2(ref, got.get_prec());
        ref_mul_ui(ref, a, 3);
        assert_equal(got, ref);
        mpf_clear(ref);
    }

    {
        mpf_class got = a / std::uint64_t{3};
        mpf_t ref;
        mpf_init2(ref, got.get_prec());
        ref_div_ui(ref, a, 3);
        assert_equal(got, ref);
        mpf_clear(ref);
    }

    {
        mpf_class got = std::uint64_t{8} / a;
        mpf_t ref;
        mpf_init2(ref, got.get_prec());
        mpf_ui_div(ref, 8, a.get_mpf_t());
        assert_equal(got, ref);
        mpf_clear(ref);
    }

    {
        mpf_class r("1.5", 128);
        mp_bitcnt_t old_prec = r.get_prec();
        r = a * std::uint64_t{8};

        mpf_t ref;
        mpf_init2(ref, old_prec);
        ref_mul_2exp(ref, a, 3, false);

        assert(r.get_prec() == old_prec);
        assert_equal(r, ref);
        mpf_clear(ref);
    }

    {
        mpf_class r("1.5", 128);
        mp_bitcnt_t old_prec = r.get_prec();
        mpf_t ref;
        mpf_init2(ref, old_prec);

        mpf_set(ref, r.get_mpf_t());
        mpf_mul_2exp(ref, ref, 4);
        r *= std::uint64_t{16};
        assert(r.get_prec() == old_prec);
        assert_equal(r, ref);

        mpf_set(ref, r.get_mpf_t());
        mpf_div_2exp(ref, ref, 4);
        r /= std::uint64_t{16};
        assert(r.get_prec() == old_prec);
        assert_equal(r, ref);

        mpf_clear(ref);
    }

    return 0;
}
