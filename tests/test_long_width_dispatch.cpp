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
#include <cstdio>
#include <cstdint>
#include <limits>

namespace {

void set_uint(mpf_t dst, std::uint64_t v) {
    char buf[24];
    std::snprintf(buf, sizeof(buf), "%llu",
                  static_cast<unsigned long long>(v));
    int rc = mpf_set_str(dst, buf, 10);
    assert(rc == 0);
}

void assert_add_uint(mpf_class const& a, std::uint64_t value) {
    mpf_class dst(static_cast<mp_bitcnt_t>(256));
    dst = a + value;
    mpf_t tmp, ref;
    mpf_init2(tmp, dst.get_prec());
    mpf_init2(ref, dst.get_prec());
    set_uint(tmp, value);
    mpf_add(ref, a.get_mpf_t(), tmp);
    assert(mpf_cmp(dst.get_mpf_t(), ref) == 0);
    mpf_clear(ref);
    mpf_clear(tmp);
}

void assert_add_i64(mpf_class const& a, std::int64_t value) {
    mpf_class dst(static_cast<mp_bitcnt_t>(256));
    dst = a + value;
    mpf_t tmp, ref;
    mpf_init2(tmp, dst.get_prec());
    mpf_init2(ref, dst.get_prec());
    std::uint64_t mag = value == std::numeric_limits<std::int64_t>::min()
        ? static_cast<std::uint64_t>(std::numeric_limits<std::int64_t>::max()) +
              std::uint64_t{1}
        : (value < 0 ? static_cast<std::uint64_t>(-(value + 1)) + std::uint64_t{1}
                     : static_cast<std::uint64_t>(value));
    set_uint(tmp, mag);
    if (value < 0) {
        mpf_neg(tmp, tmp);
    }
    mpf_add(ref, a.get_mpf_t(), tmp);
    assert(mpf_cmp(dst.get_mpf_t(), ref) == 0);
    mpf_clear(ref);
    mpf_clear(tmp);
}

}  // namespace

int main() {
#if defined(GMPXX_MKII_TEST_LLP64_PATH)
    static_assert(!gmpxx_detail::ulong_fits_uint64);
    assert(!gmpxx_detail::fits_in_ulong(std::uint64_t{1} << 50));
#else
    static_assert(gmpxx_detail::ulong_fits_uint64 ==
                  (sizeof(unsigned long) >= sizeof(std::uint64_t)));
#endif

    mpf_class a("1.5", 256);
    assert_add_uint(a, std::uint64_t{42});
    assert_add_uint(a, std::uint64_t{1} << 50);
    assert_add_i64(mpf_class("0", 256), std::numeric_limits<std::int64_t>::min());
    return 0;
}
