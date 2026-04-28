// SPDX-License-Identifier: BSD-2-Clause

#include "gmpxx_mkII.h"

#include <atomic>
#include <cassert>
#include <cstdint>
#include <iostream>

#if !defined(GMPXX_MKII_INSTRUMENT_WRAPPERS)
#error "test_mpz_mpq_alloc_count requires GMPXX_MKII_INSTRUMENT_WRAPPERS"
#endif

namespace {

std::uint64_t mpf_count() {
    return gmpxx_mkII_detail::mpf_ctor_count.load(std::memory_order_relaxed);
}

std::uint64_t mpz_count() {
    return gmpxx_mkII_detail::mpz_ctor_count.load(std::memory_order_relaxed);
}

std::uint64_t mpq_count() {
    return gmpxx_mkII_detail::mpq_ctor_count.load(std::memory_order_relaxed);
}

void reset() {
    gmpxx_mkII_detail::reset_wrapper_counters();
}

}  // namespace

int main() {
    mpz_class za("10");
    mpz_class zb("20");
    mpz_class zc("30");
    mpz_class zdst;

    reset();
    zdst = za + zb;
    assert(mpz_count() == 0);
    assert(mpq_count() == 0);
    assert(mpf_count() == 0);

    reset();
    zdst = za + zb + zc;
    std::cout << "DIAG: wrapper mpz ctor count for `za+zb+zc` = "
              << mpz_count() << "\n";
    assert(mpz_count() == 0);

    mpf_class fa("1.5", 256);
    mpf_class fdst(static_cast<mp_bitcnt_t>(256));
    reset();
    fdst = fa + za;
    assert(mpf_count() == 1);

    mpq_class qa("1/3");
    reset();
    fdst = fa + qa;
    assert(mpf_count() == 1);

    reset();
    fdst = za + 0.5;
    assert(mpf_count() == 2);

    mpq_class qb("2/5");
    mpq_class qdst;
    reset();
    qdst = qa * qb;
    assert(mpq_count() == 0);
    assert(mpz_count() == 0);
    assert(mpf_count() == 0);

    reset();
    za += zb;
    assert(mpz_count() == 0);

    reset();
    za += zb + zc;
    assert(mpz_count() == 1);

    return 0;
}
