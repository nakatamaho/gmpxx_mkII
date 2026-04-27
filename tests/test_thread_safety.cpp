// SPDX-License-Identifier: BSD-2-Clause

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
            std::uint64_t requested = gmpxx_mkII_detail::thread_default_prec();
            mpf_class x;
            std::uint64_t actual = static_cast<std::uint64_t>(x.get_prec());
            std::uint64_t expected = gmpxx_mkII_detail::effective_mpf_prec(requested);
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
        std::uint64_t mkII_req = gmpxx_mkII_detail::thread_default_prec();
        mp_bitcnt_t bogus = gmpxx_mkII_detail::checked_mp_bitcnt(
            gmpxx_mkII_detail::effective_mpf_prec(mkII_req) + GMP_NUMB_BITS);
        mpf_set_default_prec(bogus);

        mpf_class x;
        std::uint64_t actual = static_cast<std::uint64_t>(x.get_prec());
        std::uint64_t expected = gmpxx_mkII_detail::effective_mpf_prec(mkII_req);
        assert(actual == expected);
        assert(actual != static_cast<std::uint64_t>(bogus));
    });
    t.join();
    mpf_set_default_prec(original_gmp_global);
}

void test_set_before_spawn() {
    constexpr std::uint64_t requested = 777;
    gmpxx_defaults::set_initial_default_prec(requested);

    std::uint64_t expected = gmpxx_mkII_detail::effective_mpf_prec(requested);
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
