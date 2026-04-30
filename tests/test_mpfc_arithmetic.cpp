// SPDX-License-Identifier: BSD-2-Clause

#include <cassert>
#include <cmath>
#include <complex>
#include <random>

#include <gmpxx_mkII.h>

namespace {

using complex_ref = std::complex<double>;

constexpr double complex_test_values[][2] = {
    {0.0, 0.0},
    {1.0, 0.0},
    {-1.0, 0.5},
    {2.25, -3.5},
    {-4.0, 1.75},
    {0.125, -0.25},
};

gmpxx::mpfc_class make_mpfc(double real_value, double imag_value) {
    return gmpxx::mpfc_class(gmpxx::mpf_class(real_value, 256),
                             gmpxx::mpf_class(imag_value, 256));
}

complex_ref to_ref(gmpxx::mpfc_class const& value) {
    return complex_ref(value.real().get_d(), value.imag().get_d());
}

void check_close(complex_ref expected, gmpxx::mpfc_class const& got) {
    complex_ref actual = to_ref(got);
    double scale = 1.0 + std::abs(expected);
    assert(std::abs(actual.real() - expected.real()) <= 1e-10 * scale);
    assert(std::abs(actual.imag() - expected.imag()) <= 1e-10 * scale);
}

void check_binary_arithmetic(complex_ref a_ref, complex_ref b_ref,
                             gmpxx::mpfc_class const& a,
                             gmpxx::mpfc_class const& b) {
    using gmpxx::mpfc_class;

    mpfc_class sum = a + b;
    check_close(a_ref + b_ref, sum);
    mpfc_class sum_compound = a;
    sum_compound += b;
    check_close(a_ref + b_ref, sum_compound);

    mpfc_class difference = a - b;
    check_close(a_ref - b_ref, difference);
    mpfc_class difference_compound = a;
    difference_compound -= b;
    check_close(a_ref - b_ref, difference_compound);

    mpfc_class product = a * b;
    check_close(a_ref * b_ref, product);
    mpfc_class product_compound = a;
    product_compound *= b;
    check_close(a_ref * b_ref, product_compound);

    constexpr double denom_min = 0.25;
    if (std::abs(b_ref) <= denom_min) {
        b_ref = complex_ref(1.25, 0.75);
        mpfc_class safe_b = make_mpfc(1.25, 0.75);
        mpfc_class quotient = a / safe_b;
        check_close(a_ref / b_ref, quotient);
        mpfc_class quotient_compound = a;
        quotient_compound /= safe_b;
        check_close(a_ref / b_ref, quotient_compound);
        return;
    }

    mpfc_class quotient = a / b;
    check_close(a_ref / b_ref, quotient);
    mpfc_class quotient_compound = a;
    quotient_compound /= b;
    check_close(a_ref / b_ref, quotient_compound);
}

void test_construction_and_accessors() {
    using namespace gmpxx;

    mpfc_class scalar_value(128);
    assert(scalar_value.real() == 128);
    assert(scalar_value.imag() == 0);

    mpfc_class scalar_pair(128, 64);
    assert(scalar_pair.real() == 128);
    assert(scalar_pair.imag() == 64);

    mpfc_class z(mpf_class(3, 192), mpf_class(-4, 128));

    assert(z.real() == mpf_class(3, 192));
    assert(z.imag() == mpf_class(-4, 128));
    assert(z.real().get_prec() == mpf_class(3, 192).get_prec());
    assert(z.imag().get_prec() == mpf_class(-4, 128).get_prec());
    assert(z.get_prec() == z.real().get_prec());

    z.real(mpf_class(5, 160));
    z.imag(mpf_class(6, 160));
    assert(z.real() == 5);
    assert(z.imag() == 6);
}

void test_lazy_arithmetic() {
    using namespace gmpxx;

    mpfc_class a(mpf_class(1, 160), mpf_class(2, 160));
    mpfc_class b(mpf_class(3, 224), mpf_class(-4, 224));

    mpfc_class sum = a + b;
    assert(sum.real() == 4);
    assert(sum.imag() == -2);
    assert(sum.get_prec() == b.get_prec());

    mpfc_class product = a * b;
    assert(product.real() == 11);
    assert(product.imag() == 2);
    assert(product.get_prec() == b.get_prec());

    mpfc_class chained = a + b * a - mpf_class(5, 192);
    assert(chained.real() == 7);
    assert(chained.imag() == 4);
}

void test_real_operands_and_division() {
    using namespace gmpxx;

    mpfc_class z(mpf_class(3, 192), mpf_class(4, 192));

    mpfc_class half = z / mpf_class(2, 192);
    assert(half.real() == mpf_class("1.5", 192));
    assert(half.imag() == 2);

    mpfc_class shifted = mpf_class(5, 192) + z;
    assert(shifted.real() == 8);
    assert(shifted.imag() == 4);

    mpfc_class one_plus_i(mpf_class(1, 192), mpf_class(1, 192));
    mpfc_class one_minus_i(mpf_class(1, 192), mpf_class(-1, 192));
    mpfc_class quotient = one_plus_i / one_minus_i;
    assert(quotient.real() == 0);
    assert(quotient.imag() == 1);
}

void test_assignment_preserves_destination_precision() {
    using namespace gmpxx;

    mpfc_class dst(mpf_class(0, 96), mpf_class(0, 128));
    mp_bitcnt_t real_prec = dst.real().get_prec();
    mp_bitcnt_t imag_prec = dst.imag().get_prec();

    mpfc_class lhs(mpf_class(1, 256), mpf_class(2, 256));
    mpfc_class rhs(mpf_class(3, 320), mpf_class(4, 320));

    dst = rhs;
    assert(dst.real() == 3);
    assert(dst.imag() == 4);
    assert(dst.real().get_prec() == real_prec);
    assert(dst.imag().get_prec() == imag_prec);

    dst = lhs + rhs;
    assert(dst.real() == 4);
    assert(dst.imag() == 6);
    assert(dst.real().get_prec() == real_prec);
    assert(dst.imag().get_prec() == imag_prec);

    dst *= mpf_class(2, 256);
    assert(dst.real() == 8);
    assert(dst.imag() == 12);
    assert(dst.real().get_prec() == real_prec);
    assert(dst.imag().get_prec() == imag_prec);
}

void test_conjugate_norm_and_abs() {
    using namespace gmpxx;

    mpfc_class z(mpf_class(3, 192), mpf_class(4, 192));
    mpfc_class c = conj(z);

    assert(c.real() == 3);
    assert(c.imag() == -4);
    assert(norm(z) == 25);
    assert(abs(z) == 5);
}

void test_comparison_and_free_helpers() {
    using namespace gmpxx;

    mpfc_class z(mpf_class(3, 192), mpf_class(4, 192));
    mpfc_class same(mpf_class(3, 256), mpf_class(4, 256));
    mpfc_class different(mpf_class(3, 192), mpf_class(-4, 192));
    mpfc_class real_value(mpf_class(3, 192), mpf_class(0, 192));

    assert(z == same);
    assert(z != different);
    assert(real_value == mpf_class(3, 192));
    assert(mpf_class(3, 192) == real_value);
    assert(z != mpf_class(3, 192));

    assert(real(z) == 3);
    assert(imag(z) == 4);
    assert(real(z + same) == 6);
    assert(imag(z + same) == 8);

    mpf_class angle = arg(z);
    assert(std::abs(angle.get_d() - std::atan2(4.0, 3.0)) < 1e-12);

    mpfc_class from_polar =
        polar(mpf_class(5, 192), mpf_class(std::atan2(4.0, 3.0), 192));
    check_close(complex_ref(3.0, 4.0), from_polar);
}

void test_arithmetic_smoke_against_std_complex() {
    using gmpxx::mpfc_class;

    for (auto const& lhs : complex_test_values) {
        for (auto const& rhs : complex_test_values) {
            complex_ref a_ref(lhs[0], lhs[1]);
            complex_ref b_ref(rhs[0], rhs[1]);
            mpfc_class a = make_mpfc(lhs[0], lhs[1]);
            mpfc_class b = make_mpfc(rhs[0], rhs[1]);
            check_binary_arithmetic(a_ref, b_ref, a, b);
        }
    }

    std::mt19937_64 rng(0x9e3779b97f4a7c15ULL);
    std::uniform_real_distribution<double> dist(-4.0, 4.0);
    for (int iter = 0; iter < 200; ++iter) {
        double ar = dist(rng);
        double ai = dist(rng);
        double br = dist(rng);
        double bi = dist(rng);

        complex_ref a_ref(ar, ai);
        complex_ref b_ref(br, bi);
        mpfc_class a = make_mpfc(ar, ai);
        mpfc_class b = make_mpfc(br, bi);
        check_binary_arithmetic(a_ref, b_ref, a, b);
    }
}

}  // namespace

int main() {
    test_construction_and_accessors();
    test_lazy_arithmetic();
    test_real_operands_and_division();
    test_assignment_preserves_destination_precision();
    test_conjugate_norm_and_abs();
    test_comparison_and_free_helpers();
    test_arithmetic_smoke_against_std_complex();
    return 0;
}
