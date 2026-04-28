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
               gmpxx_detail::effective_mpf_prec(1024));
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
               gmpxx_detail::effective_mpf_prec(1536));
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
