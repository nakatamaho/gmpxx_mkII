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

#if !defined(GMPXX_MKII_INSTRUMENT_WRAPPERS)
#error "test_mpz_addmul_alloc_count requires GMPXX_MKII_INSTRUMENT_WRAPPERS"
#endif

#if !defined(GMPXX_MKII_TEST_FUSION_COUNTERS)
#error "test_mpz_addmul_alloc_count requires GMPXX_MKII_TEST_FUSION_COUNTERS"
#endif

namespace {

std::uint64_t mpz_count() {
    return gmpxx_detail::mpz_ctor_count.load(std::memory_order_relaxed);
}

std::uint64_t addmul_count() {
    return gmpxx_detail::test_mpz_addmul_fused_count.load(
        std::memory_order_relaxed);
}

std::uint64_t submul_count() {
    return gmpxx_detail::test_mpz_submul_fused_count.load(
        std::memory_order_relaxed);
}

std::uint64_t addmul_ui_count() {
    return gmpxx_detail::test_mpz_addmul_ui_fused_count.load(
        std::memory_order_relaxed);
}

std::uint64_t submul_ui_count() {
    return gmpxx_detail::test_mpz_submul_ui_fused_count.load(
        std::memory_order_relaxed);
}

void reset_all() {
    gmpxx_detail::reset_wrapper_counters();
    gmpxx_detail::reset_mpz_addmul_fusion_counters();
}

void check_direct_mpz_fusion() {
    mpz_class a("1000");
    mpz_class b("12345678901234567890");
    mpz_class c("-98765432109876543210");

    reset_all();
    a += b * c;
    assert(mpz_count() == 0);
    assert(addmul_count() == 1);
    assert(submul_count() == 0);

    reset_all();
    a -= b * c;
    assert(mpz_count() == 0);
    assert(addmul_count() == 0);
    assert(submul_count() == 1);
}

void check_small_scalar_fusion() {
    mpz_class a("1000");
    mpz_class b("12345678901234567890");

    reset_all();
    a += b * 5LL;
    assert(mpz_count() == 0);
    assert(addmul_ui_count() == 1);
    assert(addmul_count() == 0);

    reset_all();
    a -= b * 5LL;
    assert(mpz_count() == 0);
    assert(submul_ui_count() == 1);
    assert(submul_count() == 0);
}

void check_large_scalar_width_path() {
    mpz_class a("1000");
    mpz_class b("12345678901234567890");
    constexpr std::uint64_t large = std::numeric_limits<std::uint64_t>::max();

    reset_all();
    a += b * large;
    if (gmpxx_detail::fits_in_ulong(large)) {
        assert(mpz_count() == 0);
        assert(addmul_ui_count() == 1);
        assert(addmul_count() == 0);
    } else {
        assert(mpz_count() == 1);
        assert(addmul_ui_count() == 0);
        assert(addmul_count() == 1);
    }

    reset_all();
    a -= large * b;
    if (gmpxx_detail::fits_in_ulong(large)) {
        assert(mpz_count() == 0);
        assert(submul_ui_count() == 1);
        assert(submul_count() == 0);
    } else {
        assert(mpz_count() == 1);
        assert(submul_ui_count() == 0);
        assert(submul_count() == 1);
    }
}

void check_generic_path_still_uses_tmp() {
    mpz_class a("1000");
    mpz_class b("12");
    mpz_class c("13");
    mpz_class d("14");

    reset_all();
    a += (b + c) * d;
    assert(mpz_count() == 1);
    assert(addmul_count() == 0);
    assert(submul_count() == 0);
    assert(addmul_ui_count() == 0);
    assert(submul_ui_count() == 0);
}

}  // namespace

int main() {
    check_direct_mpz_fusion();
    check_small_scalar_fusion();
    check_large_scalar_width_path();
    check_generic_path_still_uses_tmp();
    return 0;
}
