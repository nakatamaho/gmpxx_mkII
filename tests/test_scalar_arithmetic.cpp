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

#include <algorithm>
#include <cassert>
#include <cfloat>
#include <cstdio>
#include <cstdint>
#include <limits>
#include <type_traits>
#include <vector>

namespace {

mp_bitcnt_t test_tmp_prec_for_integer(mp_bitcnt_t dst_prec) {
    return std::max(dst_prec, static_cast<mp_bitcnt_t>(64));
}

mp_bitcnt_t test_tmp_prec_for_double(mp_bitcnt_t dst_prec) {
    return std::max(dst_prec, static_cast<mp_bitcnt_t>(DBL_MANT_DIG));
}

std::uint64_t abs_i64(std::int64_t v) {
    if (v == std::numeric_limits<std::int64_t>::min()) {
        return static_cast<std::uint64_t>(
                   std::numeric_limits<std::int64_t>::max()) +
               std::uint64_t{1};
    }
    return v < 0 ? static_cast<std::uint64_t>(-(v + 1)) + std::uint64_t{1}
                 : static_cast<std::uint64_t>(v);
}

void set_uint(mpf_t dst, std::uint64_t v, mp_bitcnt_t prec) {
    mpf_set_prec(dst, test_tmp_prec_for_integer(prec));
    char buf[24];
    std::snprintf(buf, sizeof(buf), "%llu",
                  static_cast<unsigned long long>(v));
    int rc = mpf_set_str(dst, buf, 10);
    assert(rc == 0);
}

void set_double(mpf_t dst, double v, mp_bitcnt_t prec) {
    mpf_set_prec(dst, test_tmp_prec_for_double(prec));
    mpf_set_d(dst, v);
}

void ref_apply(mpf_t ref, mpf_class const& a, std::uint64_t s, char op,
               bool scalar_left) {
    mp_bitcnt_t dst_prec = mpf_get_prec(ref);
    mpf_t tmp;
    mpf_init2(tmp, test_tmp_prec_for_integer(dst_prec));
    set_uint(tmp, s, dst_prec);
    if (op == '+') {
        mpf_add(ref, a.get_mpf_t(), tmp);
    } else if (op == '-') {
        scalar_left ? mpf_sub(ref, tmp, a.get_mpf_t())
                    : mpf_sub(ref, a.get_mpf_t(), tmp);
    } else if (op == '*') {
        mpf_mul(ref, a.get_mpf_t(), tmp);
    } else {
        scalar_left ? mpf_div(ref, tmp, a.get_mpf_t())
                    : mpf_div(ref, a.get_mpf_t(), tmp);
    }
    mpf_clear(tmp);
}

void ref_apply(mpf_t ref, mpf_class const& a, std::int64_t s, char op,
               bool scalar_left) {
    mp_bitcnt_t dst_prec = mpf_get_prec(ref);
    mpf_t tmp;
    mpf_init2(tmp, test_tmp_prec_for_integer(dst_prec));
    set_uint(tmp, abs_i64(s), dst_prec);
    if (s < 0) {
        mpf_neg(tmp, tmp);
    }
    if (op == '+') {
        mpf_add(ref, a.get_mpf_t(), tmp);
    } else if (op == '-') {
        scalar_left ? mpf_sub(ref, tmp, a.get_mpf_t())
                    : mpf_sub(ref, a.get_mpf_t(), tmp);
    } else if (op == '*') {
        mpf_mul(ref, a.get_mpf_t(), tmp);
    } else {
        scalar_left ? mpf_div(ref, tmp, a.get_mpf_t())
                    : mpf_div(ref, a.get_mpf_t(), tmp);
    }
    mpf_clear(tmp);
}

void ref_apply(mpf_t ref, mpf_class const& a, double s, char op,
               bool scalar_left) {
    mp_bitcnt_t dst_prec = mpf_get_prec(ref);
    mpf_t tmp;
    mpf_init2(tmp, test_tmp_prec_for_double(dst_prec));
    set_double(tmp, s, dst_prec);
    if (op == '+') {
        mpf_add(ref, a.get_mpf_t(), tmp);
    } else if (op == '-') {
        scalar_left ? mpf_sub(ref, tmp, a.get_mpf_t())
                    : mpf_sub(ref, a.get_mpf_t(), tmp);
    } else if (op == '*') {
        mpf_mul(ref, a.get_mpf_t(), tmp);
    } else {
        scalar_left ? mpf_div(ref, tmp, a.get_mpf_t())
                    : mpf_div(ref, a.get_mpf_t(), tmp);
    }
    mpf_clear(tmp);
}

template<class S>
void assert_matches(mpf_class const& got, mpf_class const& a, S scalar,
                    char op, bool scalar_left) {
    mpf_t ref;
    mpf_init2(ref, got.get_prec());
    using N = scalar_normalize_t<S>;
    ref_apply(ref, a, static_cast<N>(scalar), op, scalar_left);
    assert(mpf_cmp(got.get_mpf_t(), ref) == 0);
    mpf_clear(ref);
}

template<class S>
void check_scalar(mpf_class const& a, S scalar) {
    {
        mpf_class dst("0", 8);
        mp_bitcnt_t before_prec = dst.get_prec();
        dst = a + scalar;
        assert(dst.get_prec() == before_prec);
        assert_matches(dst, a, scalar, '+', false);
    }
    {
        mpf_class dst("0", 8);
        mp_bitcnt_t before_prec = dst.get_prec();
        dst = scalar + a;
        assert(dst.get_prec() == before_prec);
        assert_matches(dst, a, scalar, '+', true);
    }

    if (static_cast<scalar_normalize_t<S>>(scalar) != scalar_normalize_t<S>{0}) {
        assert_matches(mpf_class(a + scalar), a, scalar, '+', false);
        assert_matches(mpf_class(scalar + a), a, scalar, '+', true);
        assert_matches(mpf_class(a - scalar), a, scalar, '-', false);
        assert_matches(mpf_class(scalar - a), a, scalar, '-', true);
        assert_matches(mpf_class(a * scalar), a, scalar, '*', false);
        assert_matches(mpf_class(scalar * a), a, scalar, '*', true);
        assert_matches(mpf_class(a / scalar), a, scalar, '/', false);
        assert_matches(mpf_class(scalar / a), a, scalar, '/', true);
    } else {
        assert_matches(mpf_class(a + scalar), a, scalar, '+', false);
        assert_matches(mpf_class(scalar + a), a, scalar, '+', true);
        assert_matches(mpf_class(a - scalar), a, scalar, '-', false);
        assert_matches(mpf_class(scalar - a), a, scalar, '-', true);
        assert_matches(mpf_class(a * scalar), a, scalar, '*', false);
        assert_matches(mpf_class(scalar * a), a, scalar, '*', true);
    }
}

template<class S>
void check_expression_scalar(mpf_class const& a, mpf_class const& b, S scalar) {
    mpf_class got1 = (a + b) + scalar;
    mpf_class got2 = scalar + (a + b);
    mpf_class sum(got1.get_prec());
    mpf_add(sum.get_mpf_t(), a.get_mpf_t(), b.get_mpf_t());
    assert_matches(got1, sum, scalar, '+', false);
    assert_matches(got2, sum, scalar, '+', true);
}

void test_increment_decrement() {
    {
        mpz_class value = 3;
        mpz_class old = value++;
        assert(old == 3);
        assert(value == 4);
        assert(++value == 5);
        old = value--;
        assert(old == 5);
        assert(value == 4);
        assert(--value == 3);
    }
    {
        mpq_class value(3, 2);
        mpq_class old = value++;
        assert(old == mpq_class(3, 2));
        assert(value == mpq_class(5, 2));
        assert(++value == mpq_class(7, 2));
        old = value--;
        assert(old == mpq_class(7, 2));
        assert(value == mpq_class(5, 2));
        assert(--value == mpq_class(3, 2));
    }
    {
        mpf_class value(3.5, static_cast<mp_bitcnt_t>(128));
        mp_bitcnt_t prec = value.get_prec();
        mpf_class old = value++;
        assert(old.get_prec() == prec);
        assert(value.get_prec() == prec);
        assert(mpf_cmp_d(old.get_mpf_t(), 3.5) == 0);
        assert(mpf_cmp_d(value.get_mpf_t(), 4.5) == 0);
        assert((++value).get_prec() == prec);
        assert(mpf_cmp_d(value.get_mpf_t(), 5.5) == 0);
        old = value--;
        assert(old.get_prec() == prec);
        assert(value.get_prec() == prec);
        assert(mpf_cmp_d(old.get_mpf_t(), 5.5) == 0);
        assert(mpf_cmp_d(value.get_mpf_t(), 4.5) == 0);
        assert((--value).get_prec() == prec);
        assert(mpf_cmp_d(value.get_mpf_t(), 3.5) == 0);
    }
}

}  // namespace

int main() {
    test_increment_decrement();

    std::vector<mp_bitcnt_t> precs{8, 64, 256, 1024};
    for (mp_bitcnt_t prec : precs) {
        mpf_class a("1.75", prec);
        mpf_class b("-2.25", prec);

        check_scalar(a, int{0});
        check_scalar(a, int{1});
        check_scalar(a, int{-1});
        check_scalar(a, std::numeric_limits<int>::max());
        check_scalar(a, std::numeric_limits<int>::min());
        check_scalar(a, long{17});
        check_scalar(a, -19LL);
        check_scalar(a, std::numeric_limits<long long>::max());
        check_scalar(a, std::numeric_limits<std::int64_t>::min());
        check_scalar(a, unsigned{0});
        check_scalar(a, std::numeric_limits<unsigned>::max());
        check_scalar(a, static_cast<std::size_t>(123));
        check_scalar(a, std::numeric_limits<std::uint64_t>::max());
        check_scalar(a, 0.5f);
        check_scalar(a, -0.5);
        check_scalar(a, 1e-100);
        check_scalar(a, 1e+100);

        check_expression_scalar(a, b, 5LL);
        check_expression_scalar(a, b, 0.5);
    }

    return 0;
}
