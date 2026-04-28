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
#include <cstdint>
#include <concepts>
#include <type_traits>
#include <utility>

namespace {

std::uint64_t construction_prec(mpf_class const& a, mpf_class const& b) {
#if defined(GMPXX_MKII_NOPRECCHANGE)
    (void)a;
    (void)b;
    return gmpxx_detail::effective_mpf_prec(
        gmpxx_detail::thread_default_prec());
#else
    return std::max(static_cast<std::uint64_t>(a.get_prec()),
                    static_cast<std::uint64_t>(b.get_prec()));
#endif
}

void assert_same(mpf_class const& got, mpf_t ref) {
    assert(mpf_cmp(got.get_mpf_t(), ref) == 0);
}

}  // namespace

int main() {
    static_assert(std::same_as<decltype(std::declval<mpf_class&>().set_prec(
                                    std::declval<mp_bitcnt_t>())),
                               void>);
    static_assert(std::same_as<decltype(std::declval<mpf_class&>().set_prec_raw(
                                    std::declval<mp_bitcnt_t>())),
                               void>);

    mpf_class a("1.0", 64);
    mpf_class b("2.0", 1024);

    {
        mpf_class r = a + b;
        assert(static_cast<std::uint64_t>(r.get_prec()) ==
               construction_prec(a, b));
    }

    {
        mpf_class r("0.0", 128);
        mp_bitcnt_t old_prec = r.get_prec();

        r = a + b;

        mpf_t ref;
        mpf_init2(ref, old_prec);
        mpf_add(ref, a.get_mpf_t(), b.get_mpf_t());

        assert(r.get_prec() == old_prec);
        assert_same(r, ref);

        mpf_clear(ref);
    }

    {
        auto e = (a + b).eval();
        assert(static_cast<std::uint64_t>(e.get_prec()) ==
               construction_prec(a, b));
    }

    {
        mpf_class r("1.0", 128);
        mp_bitcnt_t old_prec = r.get_prec();

        mpf_t ref;
        mpf_init2(ref, old_prec);
        mpf_add(ref, a.get_mpf_t(), b.get_mpf_t());
        mpf_add(ref, r.get_mpf_t(), ref);

        r += a + b;

        assert(r.get_prec() == old_prec);
        assert_same(r, ref);

        mpf_clear(ref);
    }

    {
        mpf_class value("1.234567890123456789", 256);
        mpf_t ref;
        mpf_init2(ref, value.get_prec());
        mpf_set(ref, value.get_mpf_t());

        value.set_prec(64);
        mpf_set_prec(ref, 64);

        assert(value.get_prec() == mpf_get_prec(ref));
        assert_same(value, ref);

        mpf_clear(ref);
    }

    {
        mpf_class value("1.25", 256);
        const mp_bitcnt_t allocated_prec = value.get_prec();

        value.set_prec_raw(64);
        assert(value.get_prec() == gmpxx_detail::effective_mpf_prec(64));
        value = "1.5";
        mpf_class expected("1.5", value.get_prec());
        assert(mpf_cmp(value.get_mpf_t(), expected.get_mpf_t()) == 0);

        value.set_prec_raw(allocated_prec);
        assert(value.get_prec() == allocated_prec);
    }

    return 0;
}
