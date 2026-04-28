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
#include <type_traits>

namespace {

void assert_equal(mpf_class const& lhs, mpf_class const& rhs) {
    assert(mpf_cmp(lhs.get_mpf_t(), rhs.get_mpf_t()) == 0);
}

}  // namespace

int main() {
    static_assert(gmpxx_expr<decltype(-(-std::declval<mpf_class const&>()))>);
    static_assert(std::is_same_v<
                  decltype(-(-std::declval<mpf_class const&>())),
                  unary_expr<pos_op, mpf_class>>);

    mpf_class a("1.25", 256);
    mpf_class b("2.5", 512);

    {
        mpf_class r1 = -(-a);
        mpf_class r2 = a;
        assert_equal(r1, r2);
    }

    {
        mpf_class r3 = -(-(a + b));
        mpf_class r4 = a + b;
        assert_equal(r3, r4);
        assert(r3.get_prec() == r4.get_prec());
    }

    {
        mpf_class r("0", 128);
        mp_bitcnt_t old_prec = r.get_prec();

        r = -(-(a + b));

        mpf_class ref(0.0, old_prec);
        mpf_add(ref.get_mpf_t(), a.get_mpf_t(), b.get_mpf_t());

        assert(r.get_prec() == old_prec);
        assert_equal(r, ref);
    }

    {
        mpf_class r("1.0", 128);
        mp_bitcnt_t old_prec = r.get_prec();

        mpf_class ref(0.0, old_prec);
        mpf_set(ref.get_mpf_t(), r.get_mpf_t());
        mpf_add(ref.get_mpf_t(), ref.get_mpf_t(), a.get_mpf_t());

        r += -(-a);

        assert(r.get_prec() == old_prec);
        assert_equal(r, ref);
    }

    return 0;
}
