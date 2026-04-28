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

#include <atomic>
#include <cassert>
#include <cstdint>
#include <thread>
#include <vector>

void test_independent_lazy_init() {
    std::atomic<int> mismatches{0};
    std::vector<std::thread> threads;
    for (int i = 0; i < 8; ++i) {
        threads.emplace_back([&] {
            std::uint64_t requested = gmpxx_detail::thread_default_prec();
            mpf_class x;
            std::uint64_t actual = static_cast<std::uint64_t>(x.get_prec());
            std::uint64_t expected = gmpxx_detail::effective_mpf_prec(requested);
            if (actual != expected) {
                ++mismatches;
            }
        });
    }
    for (auto& t : threads) {
        t.join();
    }
    assert(mismatches == 0);
}

void test_isolation_from_gmp_global() {
    mp_bitcnt_t original_gmp_global = mpf_get_default_prec();

    std::thread t([] {
        std::uint64_t mkII_req = gmpxx_detail::thread_default_prec();
        mp_bitcnt_t bogus = gmpxx_detail::checked_mp_bitcnt(
            gmpxx_detail::effective_mpf_prec(mkII_req) + GMP_NUMB_BITS);
        mpf_set_default_prec(bogus);

        mpf_class x;
        std::uint64_t actual = static_cast<std::uint64_t>(x.get_prec());
        std::uint64_t expected = gmpxx_detail::effective_mpf_prec(mkII_req);
        assert(actual == expected);
        assert(actual != static_cast<std::uint64_t>(bogus));
    });
    t.join();
    mpf_set_default_prec(original_gmp_global);
}

void test_set_before_spawn() {
    constexpr std::uint64_t requested = 777;
    gmpxx_defaults::set_initial_default_prec(requested);

    std::uint64_t expected = gmpxx_detail::effective_mpf_prec(requested);
    std::atomic<int> mismatches{0};
    std::vector<std::thread> threads;
    for (int i = 0; i < 4; ++i) {
        threads.emplace_back([&] {
            mpf_class x;
            std::uint64_t actual = static_cast<std::uint64_t>(x.get_prec());
            if (actual != expected) {
                ++mismatches;
            }
        });
    }
    for (auto& t : threads) {
        t.join();
    }
    assert(mismatches == 0);
}

void test_snapshot_semantics() {
    std::thread t([] {
        mpf_class x1;
        std::uint64_t v1 = static_cast<std::uint64_t>(x1.get_prec());

        gmpxx_defaults::set_initial_default_prec(v1 + 999);

        mpf_class x2;
        std::uint64_t v2 = static_cast<std::uint64_t>(x2.get_prec());

        assert(v2 == v1);
    });
    t.join();
}

int main() {
    test_independent_lazy_init();
    test_isolation_from_gmp_global();
    test_set_before_spawn();
    test_snapshot_semantics();
    return 0;
}
