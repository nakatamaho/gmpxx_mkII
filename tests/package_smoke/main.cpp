// SPDX-License-Identifier: BSD-2-Clause

#include <gmpxx_mkII.h>

int main() {
    using namespace gmpxx_mkII;
    using namespace gmpxx_mkII::literals;

    mpz_class z = "100000000000000000000"_mpz + 1_mpz;
    mpq_class q = "2/4"_mpq + "1/2"_mpq;
    mpf_class f = 1.25_mpf + "2.5"_mpf;

    return (z.sgn() > 0 && q.sgn() > 0 && f > mpf_class(0.0)) ? 0 : 1;
}
