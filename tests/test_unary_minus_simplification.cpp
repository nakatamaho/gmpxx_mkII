// SPDX-License-Identifier: BSD-2-Clause

#include "gmpxx_mkII.h"

#include <cassert>
#include <type_traits>

namespace {

void assert_equal(mpf_class const& lhs, mpf_class const& rhs) {
    assert(mpf_cmp(lhs.get_mpf_t(), rhs.get_mpf_t()) == 0);
}

}  // namespace

int main() {
    static_assert(gmpxx_expr<decltype(-(-std::declval<mpf_class const&>()))>);
    static_assert(std::is_same_v<
                  decltype(-(-std::declval<mpf_class const&>())),
                  unary_expr<pos_op, mpf_class>>);

    mpf_class a("1.25", 256);
    mpf_class b("2.5", 512);

    {
        mpf_class r1 = -(-a);
        mpf_class r2 = a;
        assert_equal(r1, r2);
    }

    {
        mpf_class r3 = -(-(a + b));
        mpf_class r4 = a + b;
        assert_equal(r3, r4);
        assert(r3.get_prec() == r4.get_prec());
    }

    {
        mpf_class r("0", 128);
        mp_bitcnt_t old_prec = r.get_prec();

        r = -(-(a + b));

        mpf_class ref(old_prec);
        mpf_add(ref.get_mpf_t(), a.get_mpf_t(), b.get_mpf_t());

        assert(r.get_prec() == old_prec);
        assert_equal(r, ref);
    }

    {
        mpf_class r("1.0", 128);
        mp_bitcnt_t old_prec = r.get_prec();

        mpf_class ref(old_prec);
        mpf_set(ref.get_mpf_t(), r.get_mpf_t());
        mpf_add(ref.get_mpf_t(), ref.get_mpf_t(), a.get_mpf_t());

        r += -(-a);

        assert(r.get_prec() == old_prec);
        assert_equal(r, ref);
    }

    return 0;
}
