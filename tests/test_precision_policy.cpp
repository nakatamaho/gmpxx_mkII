// SPDX-License-Identifier: BSD-2-Clause

#include "gmpxx_mkII.h"

#include <algorithm>
#include <cassert>
#include <cstdint>

namespace {

std::uint64_t construction_prec(mpf_class const& a, mpf_class const& b) {
#if defined(GMPXX_MKII_NOPRECCHANGE)
    (void)a;
    (void)b;
    return gmpxx_mkII_detail::effective_mpf_prec(
        gmpxx_mkII_detail::thread_default_prec());
#else
    return std::max(static_cast<std::uint64_t>(a.get_prec()),
                    static_cast<std::uint64_t>(b.get_prec()));
#endif
}

void assert_same(mpf_class const& got, mpf_t ref) {
    assert(mpf_cmp(got.get_mpf_t(), ref) == 0);
}

}  // namespace

int main() {
    mpf_class a("1.0", 64);
    mpf_class b("2.0", 1024);

    {
        mpf_class r = a + b;
        assert(static_cast<std::uint64_t>(r.get_prec()) ==
               construction_prec(a, b));
    }

    {
        mpf_class r("0.0", 128);
        mp_bitcnt_t old_prec = r.get_prec();

        r = a + b;

        mpf_t ref;
        mpf_init2(ref, old_prec);
        mpf_add(ref, a.get_mpf_t(), b.get_mpf_t());

        assert(r.get_prec() == old_prec);
        assert_same(r, ref);

        mpf_clear(ref);
    }

    {
        auto e = (a + b).eval();
        assert(static_cast<std::uint64_t>(e.get_prec()) ==
               construction_prec(a, b));
    }

    {
        mpf_class r("1.0", 128);
        mp_bitcnt_t old_prec = r.get_prec();

        mpf_t ref;
        mpf_init2(ref, old_prec);
        mpf_add(ref, a.get_mpf_t(), b.get_mpf_t());
        mpf_add(ref, r.get_mpf_t(), ref);

        r += a + b;

        assert(r.get_prec() == old_prec);
        assert_same(r, ref);

        mpf_clear(ref);
    }

    return 0;
}
