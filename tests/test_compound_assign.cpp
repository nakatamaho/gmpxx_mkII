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

namespace {

std::uint64_t abs_i64(std::int64_t v) {
    if (v == std::numeric_limits<std::int64_t>::min()) {
        return static_cast<std::uint64_t>(
                   std::numeric_limits<std::int64_t>::max()) +
               std::uint64_t{1};
    }
    return v < 0 ? static_cast<std::uint64_t>(-(v + 1)) + std::uint64_t{1}
                 : static_cast<std::uint64_t>(v);
}

void set_uint(mpf_t dst, std::uint64_t v, mp_bitcnt_t dst_prec) {
    mpf_set_prec(dst, std::max(dst_prec, static_cast<mp_bitcnt_t>(64)));
    char buf[24];
    std::snprintf(buf, sizeof(buf), "%llu",
                  static_cast<unsigned long long>(v));
    int rc = mpf_set_str(dst, buf, 10);
    assert(rc == 0);
}

void set_double(mpf_t dst, double v, mp_bitcnt_t dst_prec) {
    mpf_set_prec(dst, std::max(dst_prec,
                               static_cast<mp_bitcnt_t>(DBL_MANT_DIG)));
    mpf_set_d(dst, v);
}

template<class S>
void set_scalar(mpf_t dst, S scalar, mp_bitcnt_t dst_prec) {
    using N = scalar_normalize_t<S>;
    if constexpr (std::is_same_v<N, std::uint64_t>) {
        set_uint(dst, static_cast<N>(scalar), dst_prec);
    } else if constexpr (std::is_same_v<N, std::int64_t>) {
        N v = static_cast<N>(scalar);
        set_uint(dst, abs_i64(v), dst_prec);
        if (v < 0) {
            mpf_neg(dst, dst);
        }
    } else {
        set_double(dst, static_cast<N>(scalar), dst_prec);
    }
}

void apply_ref(mpf_t lhs, mpf_t rhs, char op) {
    if (op == '+') {
        mpf_add(lhs, lhs, rhs);
    } else if (op == '-') {
        mpf_sub(lhs, lhs, rhs);
    } else if (op == '*') {
        mpf_mul(lhs, lhs, rhs);
    } else {
        mpf_div(lhs, lhs, rhs);
    }
}

void assert_equal(mpf_class const& got, mpf_t ref, mp_bitcnt_t before_prec) {
    assert(got.get_prec() == before_prec);
    assert(mpf_cmp(got.get_mpf_t(), ref) == 0);
}

template<class S>
void check_scalar(char op, S scalar) {
    mpf_class a("7.5", 256);
    mp_bitcnt_t before = a.get_prec();
    mpf_t ref, rhs;
    mpf_init2(ref, before);
    mpf_init2(rhs, before);
    mpf_set(ref, a.get_mpf_t());
    set_scalar(rhs, scalar, before);
    apply_ref(ref, rhs, op);

    if (op == '+') {
        a += scalar;
    } else if (op == '-') {
        a -= scalar;
    } else if (op == '*') {
        a *= scalar;
    } else {
        a /= scalar;
    }
    assert_equal(a, ref, before);
    mpf_clear(rhs);
    mpf_clear(ref);
}

void check_mpf_rhs(char op) {
    mpf_class a("7.5", 256);
    mpf_class b("2.5", 1024);
    mp_bitcnt_t before = a.get_prec();
    mpf_t ref;
    mpf_init2(ref, before);
    mpf_set(ref, a.get_mpf_t());
    apply_ref(ref, b.get_mpf_t(), op);
    if (op == '+') {
        a += b;
    } else if (op == '-') {
        a -= b;
    } else if (op == '*') {
        a *= b;
    } else {
        a /= b;
    }
    assert_equal(a, ref, before);
    mpf_clear(ref);
}

void check_self_alias() {
    {
        mpf_class a("3.5", 256);
        mp_bitcnt_t before = a.get_prec();
        mpf_t ref;
        mpf_init2(ref, before);
        mpf_add(ref, a.get_mpf_t(), a.get_mpf_t());
        a += a;
        assert_equal(a, ref, before);
        mpf_clear(ref);
    }
    {
        mpf_class a("3.5", 256);
        mp_bitcnt_t before = a.get_prec();
        mpf_t ref;
        mpf_init2(ref, before);
        mpf_sub(ref, a.get_mpf_t(), a.get_mpf_t());
        a -= a;
        assert_equal(a, ref, before);
        mpf_clear(ref);
    }
    {
        mpf_class a("3.5", 256);
        mp_bitcnt_t before = a.get_prec();
        mpf_t ref;
        mpf_init2(ref, before);
        mpf_mul(ref, a.get_mpf_t(), a.get_mpf_t());
        a *= a;
        assert_equal(a, ref, before);
        mpf_clear(ref);
    }
    {
        mpf_class a("3.5", 256);
        mp_bitcnt_t before = a.get_prec();
        mpf_t ref;
        mpf_init2(ref, before);
        mpf_div(ref, a.get_mpf_t(), a.get_mpf_t());
        a /= a;
        assert_equal(a, ref, before);
        mpf_clear(ref);
    }
}

void check_expression_rhs() {
    {
        mpf_class a("3.5", 256);
        mpf_class b("1.25", 1024);
        mp_bitcnt_t before = a.get_prec();
        mpf_t rhs, ref;
        mpf_init2(rhs, before);
        mpf_init2(ref, before);
        mpf_add(rhs, a.get_mpf_t(), b.get_mpf_t());
        mpf_set(ref, a.get_mpf_t());
        mpf_add(ref, ref, rhs);
        a += a + b;
        assert_equal(a, ref, before);
        mpf_clear(ref);
        mpf_clear(rhs);
    }
    {
        mpf_class a("3.5", 256);
        mpf_class b("1.25", 1024);
        mp_bitcnt_t before = a.get_prec();
        mpf_t lhs, rhs, ref;
        mpf_init2(lhs, before);
        mpf_init2(rhs, before);
        mpf_init2(ref, before);
        mpf_add(lhs, a.get_mpf_t(), b.get_mpf_t());
        mpf_mul(rhs, lhs, a.get_mpf_t());
        mpf_set(ref, a.get_mpf_t());
        mpf_mul(ref, ref, rhs);
        a *= (a + b) * a;
        assert_equal(a, ref, before);
        mpf_clear(ref);
        mpf_clear(rhs);
        mpf_clear(lhs);
    }
}

}  // namespace

int main() {
    for (char op : {'+', '-', '*', '/'}) {
        check_mpf_rhs(op);
        check_scalar(op, 5LL);
        check_scalar(op, 2.0);
        check_scalar(op, std::numeric_limits<std::int64_t>::min());
        check_scalar(op, std::numeric_limits<std::int64_t>::max());
        check_scalar(op, std::numeric_limits<std::uint64_t>::max());
    }
    check_scalar('+', 0);
    check_scalar('-', 0);
    check_scalar('*', 0);
    check_self_alias();
    check_expression_rhs();
    return 0;
}
