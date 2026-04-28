// SPDX-License-Identifier: BSD-2-Clause

#include "gmpxx_mkII.h"

#include <cassert>
#include <cstdint>

namespace {

void assert_mpf_equal(mpf_class const& lhs, mpf_class const& rhs) {
    assert(mpf_cmp(lhs.get_mpf_t(), rhs.get_mpf_t()) == 0);
}

void test_mpz_literals() {
    using namespace gmpxx::literals;

    mpz_class a = 42_mpz;
    mpz_class b = "123456789012345678901234567890"_mpz;

    assert(a == mpz_class(std::int64_t{42}));
    assert(b == mpz_class("123456789012345678901234567890", 10));
}

void test_mpq_literals() {
    using namespace gmpxx::literals;

    mpq_class a = 42_mpq;
    mpq_class b = "2/4"_mpq;

    assert(a == mpq_class(std::int64_t{42}));
    assert(b == mpq_class("1/2", 10));
}

void test_mpf_literals() {
    using namespace gmpxx::literals;

    mpf_class a = 1.25_mpf;
    mpf_class b = "1.25"_mpf;
    mpf_class c = 0.5_mpf;
    mpf_class d = "3.75"_mpf;

    assert_mpf_equal(a, b);
    assert_mpf_equal(c, mpf_class(0.5, c.get_prec()));
    assert_mpf_equal(d, mpf_class(3.75, d.get_prec()));
}

void test_string_literals_use_default_base() {
    using namespace gmpxx::literals;

    gmpxx_defaults::set_default_base(16);

    mpz_class z = "ff"_mpz;
    assert(z == mpz_class(std::int64_t{255}));

    mpq_class q = "10/20"_mpq;
    assert(q == mpq_class("1/2", 10));

    mpf_class f = "1.8"_mpf;
    assert(f == mpf_class("1.5", f.get_prec(), 10));

    mpz_class raw = 123_mpz;
    assert(raw == mpz_class(std::int64_t{123}));

    gmpxx_defaults::set_default_base(10);
}

}  // namespace

int main() {
    test_mpz_literals();
    test_mpq_literals();
    test_mpf_literals();
    test_string_literals_use_default_base();
    return 0;
}
