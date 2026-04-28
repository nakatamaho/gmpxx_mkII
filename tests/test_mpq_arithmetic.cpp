// SPDX-License-Identifier: BSD-2-Clause

#include "gmpxx_mkII.h"

#include <cassert>
#include <cstdint>
#include <limits>

namespace {

void assert_equal(mpq_class const& got, mpq_t const ref) {
    assert(mpq_equal(got.get_mpq_t(), ref) != 0);
}

void check_binary(mpq_class const& a, mpq_class const& b) {
    mpq_t ref;
    mpq_init(ref);

    mpq_add(ref, a.get_mpq_t(), b.get_mpq_t());
    assert_equal(mpq_class(a + b), ref);

    mpq_sub(ref, a.get_mpq_t(), b.get_mpq_t());
    assert_equal(mpq_class(a - b), ref);

    mpq_mul(ref, a.get_mpq_t(), b.get_mpq_t());
    assert_equal(mpq_class(a * b), ref);

    if (mpq_sgn(b.get_mpq_t()) != 0) {
        mpq_div(ref, a.get_mpq_t(), b.get_mpq_t());
        assert_equal(mpq_class(a / b), ref);
    }

    mpq_clear(ref);
}

void check_scalar(mpq_class const& a) {
    mpq_t ref;
    mpq_t scalar;
    mpq_init(ref);
    mpq_init(scalar);
    mpq_set_si(scalar, 5, 1);

    mpq_add(ref, a.get_mpq_t(), scalar);
    assert_equal(mpq_class(a + 5LL), ref);
    assert_equal(mpq_class(5LL + a), ref);

    mpq_sub(ref, a.get_mpq_t(), scalar);
    assert_equal(mpq_class(a - 5LL), ref);

    mpq_sub(ref, scalar, a.get_mpq_t());
    assert_equal(mpq_class(5LL - a), ref);

    mpq_mul(ref, a.get_mpq_t(), scalar);
    assert_equal(mpq_class(a * 5LL), ref);
    assert_equal(mpq_class(5LL * a), ref);

    mpq_div(ref, a.get_mpq_t(), scalar);
    assert_equal(mpq_class(a / 5LL), ref);

    mpq_clear(scalar);
    mpq_clear(ref);
}

}  // namespace

int main() {
    mpq_class zero(std::int64_t{0});
    mpq_class one(std::int64_t{1});
    mpq_class neg_one(std::int64_t{-1});
    mpq_class third("1/3");
    mpq_class pi_approx("22/7");
    mpq_class large(mpz_class(std::numeric_limits<std::int64_t>::max()),
                    mpz_class(std::int64_t{17}));
    mpq_class neg_large(mpz_class(std::numeric_limits<std::int64_t>::min()),
                        mpz_class(std::int64_t{19}));

    check_binary(zero, one);
    check_binary(one, neg_one);
    check_binary(third, pi_approx);
    check_binary(large, neg_large);

    check_scalar(third);
    check_scalar(neg_large);

    {
        mpq_class x("3/6");
        mpz_class num = x.get_num();
        mpz_class den = x.get_den();
        assert(mpz_cmp_si(num.get_mpz_t(), 1) == 0);
        assert(mpz_cmp_si(den.get_mpz_t(), 2) == 0);
    }

    {
        mpq_class a("1/3");
        mpq_class b("2/5");
        mpq_class c("-7/11");
        mpq_class got = (a + b) * c;
        mpq_t tmp;
        mpq_t ref;
        mpq_init(tmp);
        mpq_init(ref);
        mpq_add(tmp, a.get_mpq_t(), b.get_mpq_t());
        mpq_mul(ref, tmp, c.get_mpq_t());
        assert_equal(got, ref);
        mpq_clear(ref);
        mpq_clear(tmp);
    }

    {
        mpq_class a("1/3");
        mpq_class b("2/5");
        mpq_class c("7/11");
        mpq_t ref;
        mpq_t tmp;
        mpq_init(ref);
        mpq_init(tmp);
        mpq_set(ref, a.get_mpq_t());
        mpq_add(tmp, b.get_mpq_t(), c.get_mpq_t());
        mpq_add(ref, ref, tmp);
        a += b + c;
        assert_equal(a, ref);
        mpq_clear(tmp);
        mpq_clear(ref);
    }

    assert(mpq_class(-third).sgn() < 0);
    assert(mpq_class(+third).sgn() > 0);

    return 0;
}
