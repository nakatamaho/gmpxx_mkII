// SPDX-License-Identifier: BSD-2-Clause

#include "gmpxx_mkII.h"

#include <algorithm>
#include <cassert>
#include <cfloat>
#include <cstdint>
#include <type_traits>

namespace {

void assert_mpf_equal(mpf_class const& got, mpf_t ref) {
    assert(mpf_cmp(got.get_mpf_t(), ref) == 0);
}

void assert_mpq_equal(mpq_class const& got, mpq_t ref) {
    assert(mpq_equal(got.get_mpq_t(), ref) != 0);
}

}  // namespace

int main() {
    static_assert(std::is_same_v<decltype((std::declval<mpf_class const&>() +
                                           std::declval<mpz_class const&>()).eval()),
                                 mpf_class>);
    static_assert(std::is_same_v<decltype((std::declval<mpz_class const&>() +
                                           std::declval<mpq_class const&>()).eval()),
                                 mpq_class>);
    static_assert(std::is_same_v<decltype((std::declval<mpz_class const&>() +
                                           0.5).eval()),
                                 mpf_class>);

    mpf_class f("1.25", 64);
    mpz_class z("123456789012345678901234567890");
    mpq_class q("22/7");

    {
        mpf_class got = f + z;
        mpf_t ref;
        mpf_t tmp;
        mpf_init2(ref, got.get_prec());
        mpf_init2(tmp, got.get_prec());
        mpf_set_z(tmp, z.get_mpz_t());
        mpf_add(ref, f.get_mpf_t(), tmp);
        assert_mpf_equal(got, ref);
#if defined(GMPXX_MKII_NOPRECCHANGE)
        assert(static_cast<std::uint64_t>(got.get_prec()) ==
               gmpxx_detail::effective_mpf_prec(
                   gmpxx_detail::thread_default_prec()));
#else
        assert(static_cast<std::uint64_t>(got.get_prec()) ==
               static_cast<std::uint64_t>(f.get_prec()));
#endif
        mpf_clear(tmp);
        mpf_clear(ref);
    }

    {
        mpf_class got = q * f;
        mpf_t ref;
        mpf_t tmp;
        mpf_init2(ref, got.get_prec());
        mpf_init2(tmp, got.get_prec());
        mpf_set_q(tmp, q.get_mpq_t());
        mpf_mul(ref, tmp, f.get_mpf_t());
        assert_mpf_equal(got, ref);
        mpf_clear(tmp);
        mpf_clear(ref);
    }

    {
        mpq_class got = z + q;
        mpq_t ref;
        mpq_t tmp;
        mpq_init(ref);
        mpq_init(tmp);
        mpq_set_z(tmp, z.get_mpz_t());
        mpq_add(ref, tmp, q.get_mpq_t());
        assert_mpq_equal(got, ref);
        mpq_clear(tmp);
        mpq_clear(ref);
    }

    {
        mpf_class got = (f + z) * 0.5;
        mpf_t ref;
        mpf_t tmp;
        mpf_t half;
        mpf_init2(ref, got.get_prec());
        mpf_init2(tmp, got.get_prec());
        mpf_init2(half, std::max(got.get_prec(),
                                 static_cast<mp_bitcnt_t>(DBL_MANT_DIG)));
        mpf_set_z(tmp, z.get_mpz_t());
        mpf_add(tmp, f.get_mpf_t(), tmp);
        mpf_set_d(half, 0.5);
        mpf_mul(ref, tmp, half);
        assert_mpf_equal(got, ref);
        mpf_clear(half);
        mpf_clear(tmp);
        mpf_clear(ref);
    }

    {
        mpf_class r("0", 128);
        mp_bitcnt_t old_prec = r.get_prec();
        r = f + z + q;
        assert(r.get_prec() == old_prec);
    }

    return 0;
}
