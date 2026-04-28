// SPDX-License-Identifier: BSD-2-Clause

#include "gmpxx_mkII.h"

#include <cassert>
#include <cstdint>
#include <sstream>
#include <stdexcept>
#include <thread>

namespace {

void test_precision_getters() {
    std::uint64_t old_initial = gmpxx_defaults::get_initial_default_prec();
    assert(old_initial != 0);

    gmpxx_defaults::set_initial_default_prec(768);
    assert(gmpxx_defaults::get_initial_default_prec() == 768);

    gmpxx_defaults::set_initial_default_prec(0);
    assert(gmpxx_defaults::get_initial_default_prec() == 768);

    mpf_class local_default;
    assert(gmpxx_defaults::get_default_prec() == local_default.get_prec());

    gmpxx_defaults::set_initial_default_prec(old_initial);
}

void test_precision_thread_snapshots() {
    gmpxx_defaults::set_initial_default_prec(1024);

    std::thread fresh([] {
        mpf_class x;
        assert(x.get_prec() ==
               gmpxx_mkII_detail::effective_mpf_prec(1024));
        assert(gmpxx_defaults::get_default_prec() == x.get_prec());
    });
    fresh.join();

    std::thread frozen([] {
        mpf_class before;
        mp_bitcnt_t old_prec = before.get_prec();

        gmpxx_defaults::set_initial_default_prec(2048);

        mpf_class after;
        assert(after.get_prec() == old_prec);
        assert(gmpxx_defaults::get_default_prec() == old_prec);
    });
    frozen.join();
}

void test_precision_independent_from_gmp_global() {
    mp_bitcnt_t original = mpf_get_default_prec();
    mpf_set_default_prec(64);

    std::thread t([] {
        gmpxx_defaults::set_initial_default_prec(1536);
        mpf_class x;
        assert(x.get_prec() ==
               gmpxx_mkII_detail::effective_mpf_prec(1536));
        assert(x.get_prec() != static_cast<mp_bitcnt_t>(64));
    });
    t.join();

    mpf_set_default_prec(original);
}

void test_default_base() {
    assert(gmpxx_defaults::get_default_base() == 10);

    gmpxx_defaults::set_default_base(16);
    assert(gmpxx_defaults::get_default_base() == 16);

    mpz_class z("ff");
    assert(z == mpz_class(std::int64_t{255}));
    assert(z.get_str() == "ff");

    mpq_class q("10/20");
    assert(q == mpq_class("1/2", 10));
    assert(q.get_str() == "1/2");

    mpz_class assigned;
    assert(assigned.set_str("100") == 0);
    assert(assigned == mpz_class(std::int64_t{256}));

    std::ostringstream dec;
    dec << std::dec << z;
    assert(dec.str() == "255");

    std::ostringstream hex;
    hex << std::hex << z;
    assert(hex.str() == "ff");

    bool threw = false;
    try {
        gmpxx_defaults::set_default_base(1);
    } catch (std::invalid_argument const&) {
        threw = true;
    }
    assert(threw);
    assert(gmpxx_defaults::get_default_base() == 16);

    std::thread t([] {
        assert(gmpxx_defaults::get_default_base() == 10);
        gmpxx_defaults::set_default_base(8);
        assert(gmpxx_defaults::get_default_base() == 8);
    });
    t.join();
    assert(gmpxx_defaults::get_default_base() == 16);

    gmpxx_defaults::set_default_base(10);
}

}  // namespace

int main() {
    test_precision_getters();
    test_precision_thread_snapshots();
    test_precision_independent_from_gmp_global();
    test_default_base();
    return 0;
}
