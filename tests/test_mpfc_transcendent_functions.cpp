// SPDX-License-Identifier: BSD-2-Clause

#include <cassert>
#include <cmath>
#include <complex>
#include <vector>

#include <gmpxx_mkII.h>

namespace {

using complex_ref = std::complex<double>;

gmpxx::mpfc_class make_mpfc(double real_value, double imag_value) {
    return gmpxx::mpfc_class(gmpxx::mpf_class(real_value, 256),
                             gmpxx::mpf_class(imag_value, 256));
}

gmpxx::mpfc_class make_mpfc(gmpxx::mpf_class const& real_value,
                            gmpxx::mpf_class const& imag_value) {
    return gmpxx::mpfc_class(real_value, imag_value);
}

gmpxx::mpf_class branch_epsilon(mp_bitcnt_t precision) {
    mp_bitcnt_t epsilon_bits = precision / 2;
    if (epsilon_bits < 32) {
        epsilon_bits = 32;
    }

    gmpxx::mpf_class epsilon(1, precision);
    mpf_div_2exp(epsilon.get_mpf_t(), epsilon.get_mpf_t(), epsilon_bits);
    return epsilon;
}

gmpxx::mpf_class branch_tolerance(mp_bitcnt_t precision) {
    mp_bitcnt_t tolerance_bits = precision / 4;
    if (tolerance_bits < 16) {
        tolerance_bits = 16;
    }

    gmpxx::mpf_class tolerance(1, precision);
    mpf_div_2exp(tolerance.get_mpf_t(), tolerance.get_mpf_t(),
                 tolerance_bits);
    return tolerance;
}

complex_ref to_ref(gmpxx::mpfc_class const& value) {
    return complex_ref(value.real().get_d(), value.imag().get_d());
}

void check_close(char const* label, complex_ref expected,
                 gmpxx::mpfc_class const& got, double tolerance = 1e-8) {
    (void)label;
    complex_ref actual = to_ref(got);
    double scale = 1.0 + std::abs(expected);
    assert(std::abs(actual.real() - expected.real()) <= tolerance * scale);
    assert(std::abs(actual.imag() - expected.imag()) <= tolerance * scale);
}

void check_close_mpf(gmpxx::mpf_class const& expected,
                     gmpxx::mpf_class const& got,
                     gmpxx::mpf_class const& tolerance) {
    assert(gmpxx::abs(got - expected) <= tolerance);
}

void check_close_mpfc(gmpxx::mpfc_class const& expected,
                      gmpxx::mpfc_class const& got,
                      gmpxx::mpf_class const& tolerance) {
    check_close_mpf(expected.real(), got.real(), tolerance);
    check_close_mpf(expected.imag(), got.imag(), tolerance);
}

void test_primary_functions_against_std_complex() {
    using namespace gmpxx;

    std::vector<complex_ref> values = {
        complex_ref(0.25, -0.5),
        complex_ref(-0.75, 0.25),
        complex_ref(1.25, 0.5),
        complex_ref(-1.5, -0.75),
    };

    for (complex_ref ref : values) {
        mpfc_class z = make_mpfc(ref.real(), ref.imag());

        check_close("sqrt", std::sqrt(ref), sqrt(z));
        check_close("exp", std::exp(ref), exp(z));
        check_close("log", std::log(ref), log(z));
        check_close("sin", std::sin(ref), sin(z));
        check_close("cos", std::cos(ref), cos(z));
        check_close("tan", std::tan(ref), tan(z));
        check_close("sinh", std::sinh(ref), sinh(z));
        check_close("cosh", std::cosh(ref), cosh(z));
        check_close("tanh", std::tanh(ref), tanh(z));
    }
}

void test_inverse_functions_against_std_complex() {
    using namespace gmpxx;

    std::vector<complex_ref> values = {
        complex_ref(0.25, -0.5),
        complex_ref(-0.35, 0.25),
        complex_ref(0.75, 0.4),
        complex_ref(-0.8, -0.3),
    };

    for (complex_ref ref : values) {
        mpfc_class z = make_mpfc(ref.real(), ref.imag());

        check_close("asin", std::asin(ref), asin(z));
        check_close("acos", std::acos(ref), acos(z));
        check_close("atan", std::atan(ref), atan(z));
        check_close("asinh", std::asinh(ref), asinh(z));
        check_close("acosh", std::acosh(ref), acosh(z));
        check_close("atanh", std::atanh(ref), atanh(z));
    }
}

void test_real_axis_and_expression_inputs() {
    using namespace gmpxx;

    mpfc_class z(mpf_class("0.5", 256), mpf_class("0.25", 256));
    mpfc_class w(mpf_class("-0.125", 192), mpf_class("0.375", 192));

    mpfc_class composed = sin(z + w);
    check_close("sin expression",
                std::sin(complex_ref(0.375, 0.625)), composed);

    mpfc_class real_value(mpf_class("0.5", 256), mpf_class(0, 256));
    mpfc_class real_exp = exp(real_value);
    assert(real_exp.imag() == 0);
    assert(real_exp.real().get_prec() == 256);

    mpfc_class negative_real(mpf_class("-4", 256), mpf_class(0, 256));
    mpfc_class principal_root = sqrt(negative_real);
    assert(principal_root.real() == 0);
    assert(principal_root.imag() == 2);
}

void test_pow_against_std_complex() {
    using namespace gmpxx;

    complex_ref ref(0.75, -0.5);
    mpfc_class z = make_mpfc(ref.real(), ref.imag());

    check_close("pow unsigned", std::pow(ref, 5), pow(z, 5U));
    check_close("pow signed", std::pow(ref, 5), pow(z, 5));
    check_close("pow zero", complex_ref(1.0, 0.0), pow(z, 0));
    check_close("pow negative", std::pow(ref, -3), pow(z, -3));

    mpf_class real_exponent("1.75", 256);
    check_close("pow real exponent", std::pow(ref, 1.75),
                pow(z, real_exponent));

    complex_ref exponent_ref(-0.25, 0.5);
    mpfc_class exponent =
        make_mpfc(exponent_ref.real(), exponent_ref.imag());
    check_close("pow complex exponent", std::pow(ref, exponent_ref),
                pow(z, exponent));

    mpfc_class w = make_mpfc(0.125, 0.25);
    check_close("pow expression base", std::pow(ref + complex_ref(0.125, 0.25), 3),
                pow(z + w, 3));
    check_close("pow expression exponent",
                std::pow(ref, exponent_ref + complex_ref(0.125, 0.25)),
                pow(z, exponent + w));
}

void test_branch_cuts_against_std_complex() {
    using namespace gmpxx;

    constexpr double eps = 1e-12;

    mpf_class minus_four("-4", 256);
    mpf_class minus_two("-2", 256);
    mpf_class minus_one("-1", 256);
    mpf_class half_value("0.5", 256);
    mpf_class two("2", 256);
    mpf_class smoke_epsilon("1e-12", 256);
    mpf_class smoke_neg_epsilon = -smoke_epsilon;

    struct case_type {
        char const* label;
        complex_ref input;
        mpfc_class value;
    };

    case_type negative_axis[] = {
        {"negative axis from above", complex_ref(-4.0, eps),
         make_mpfc(minus_four, smoke_epsilon)},
        {"negative axis from below", complex_ref(-4.0, -eps),
         make_mpfc(minus_four, smoke_neg_epsilon)},
    };

    for (case_type const& c : negative_axis) {
        check_close(c.label, std::sqrt(c.input), sqrt(c.value));
        check_close(c.label, std::log(c.input), log(c.value));
    }
    assert(sqrt(negative_axis[0].value).imag() > 0);
    assert(sqrt(negative_axis[1].value).imag() < 0);
    assert(log(negative_axis[0].value).imag() > 0);
    assert(log(negative_axis[1].value).imag() < 0);

    case_type outer_real_axis[] = {
        {"outer real axis from above", complex_ref(2.0, eps),
         make_mpfc(two, smoke_epsilon)},
        {"outer real axis from below", complex_ref(2.0, -eps),
         make_mpfc(two, smoke_neg_epsilon)},
        {"negative outer real axis from above", complex_ref(-2.0, eps),
         make_mpfc(minus_two, smoke_epsilon)},
        {"negative outer real axis from below", complex_ref(-2.0, -eps),
         make_mpfc(minus_two, smoke_neg_epsilon)},
    };

    for (case_type const& c : outer_real_axis) {
        check_close(c.label, std::asin(c.input), asin(c.value));
        check_close(c.label, std::acos(c.input), acos(c.value));
    }

    check_close("atanh upper cut", std::atanh(complex_ref(2.0, eps)),
                atanh(make_mpfc(two, smoke_epsilon)));
    check_close("atanh lower cut", std::atanh(complex_ref(2.0, -eps)),
                atanh(make_mpfc(two, smoke_neg_epsilon)));
    assert(atanh(make_mpfc(two, smoke_epsilon)).imag() > 0);
    assert(atanh(make_mpfc(two, smoke_neg_epsilon)).imag() < 0);

    check_close("acosh upper cut", std::acosh(complex_ref(0.5, eps)),
                acosh(make_mpfc(half_value, smoke_epsilon)));
    check_close("acosh lower cut", std::acosh(complex_ref(0.5, -eps)),
                acosh(make_mpfc(half_value, smoke_neg_epsilon)));
    assert(acosh(make_mpfc(half_value, smoke_epsilon)).imag() > 0);
    assert(acosh(make_mpfc(half_value, smoke_neg_epsilon)).imag() < 0);

    check_close("pow upper cut", std::pow(complex_ref(-1.0, eps), 0.5),
                pow(make_mpfc(minus_one, smoke_epsilon), half_value));
    check_close("pow lower cut", std::pow(complex_ref(-1.0, -eps), 0.5),
                pow(make_mpfc(minus_one, smoke_neg_epsilon), half_value));
    assert(pow(make_mpfc(minus_one, smoke_epsilon), half_value).imag() > 0);
    assert(pow(make_mpfc(minus_one, smoke_neg_epsilon), half_value).imag() < 0);
}

void test_branch_cut_identities_with_mpf() {
    using namespace gmpxx;

    mp_bitcnt_t precision = gmpxx_defaults::get_default_prec();
    mpf_class epsilon = branch_epsilon(precision);
    mpf_class neg_epsilon = -epsilon;
    mpf_class tolerance = branch_tolerance(precision);
    mpf_class pi = const_pi(precision);

    mpf_class minus_four(-4, precision);
    mpf_class minus_one(-1, precision);
    mpf_class half_value("0.5", precision);
    mpf_class two(2, precision);
    mpf_class half_real("0.5", precision);

    mpfc_class negative_above = make_mpfc(minus_four, epsilon);
    mpfc_class negative_below = make_mpfc(minus_four, neg_epsilon);

    mpfc_class sqrt_above = sqrt(negative_above);
    mpfc_class sqrt_below = sqrt(negative_below);
    assert(sqrt_above.imag() > 0);
    assert(sqrt_below.imag() < 0);
    check_close_mpfc(negative_above, mpfc_class(sqrt_above * sqrt_above),
                     tolerance);
    check_close_mpfc(negative_below, mpfc_class(sqrt_below * sqrt_below),
                     tolerance);

    mpfc_class log_above = log(negative_above);
    mpfc_class log_below = log(negative_below);
    assert(log_above.imag() > 0);
    assert(log_below.imag() < 0);
    check_close_mpf(log(gmpxx::abs(minus_four)), log_above.real(),
                    tolerance);
    check_close_mpf(pi, log_above.imag(), tolerance);
    check_close_mpf(-pi, log_below.imag(), tolerance);

    mpfc_class atanh_above = atanh(make_mpfc(two, epsilon));
    mpfc_class atanh_below = atanh(make_mpfc(two, neg_epsilon));
    assert(atanh_above.imag() > 0);
    assert(atanh_below.imag() < 0);
    check_close_mpfc(make_mpfc(two, epsilon), tanh(atanh_above), tolerance);
    check_close_mpfc(make_mpfc(two, neg_epsilon), tanh(atanh_below),
                     tolerance);

    mpfc_class acosh_above = acosh(make_mpfc(half_real, epsilon));
    mpfc_class acosh_below = acosh(make_mpfc(half_real, neg_epsilon));
    assert(acosh_above.imag() > 0);
    assert(acosh_below.imag() < 0);
    check_close_mpfc(make_mpfc(half_real, epsilon), cosh(acosh_above),
                     tolerance);
    check_close_mpfc(make_mpfc(half_real, neg_epsilon), cosh(acosh_below),
                     tolerance);

    mpfc_class pow_above = pow(make_mpfc(minus_one, epsilon), half_value);
    mpfc_class pow_below = pow(make_mpfc(minus_one, neg_epsilon), half_value);
    assert(pow_above.imag() > 0);
    assert(pow_below.imag() < 0);
    check_close_mpfc(make_mpfc(minus_one, epsilon),
                     mpfc_class(pow_above * pow_above), tolerance);
    check_close_mpfc(make_mpfc(minus_one, neg_epsilon),
                     mpfc_class(pow_below * pow_below), tolerance);

    mpfc_class asin_above = asin(make_mpfc(two, epsilon));
    mpfc_class acos_above = acos(make_mpfc(two, epsilon));
    mpfc_class asin_below = asin(make_mpfc(two, neg_epsilon));
    mpfc_class acos_below = acos(make_mpfc(two, neg_epsilon));
    assert(asin_above.imag() > 0);
    assert(acos_above.imag() < 0);
    assert(asin_below.imag() < 0);
    assert(acos_below.imag() > 0);
    check_close_mpfc(make_mpfc(two, epsilon), sin(asin_above), tolerance);
    check_close_mpfc(make_mpfc(two, neg_epsilon), sin(asin_below),
                     tolerance);
    check_close_mpfc(make_mpfc(two, epsilon), cos(acos_above), tolerance);
    check_close_mpfc(make_mpfc(two, neg_epsilon), cos(acos_below),
                     tolerance);
}

}  // namespace

int main() {
    test_primary_functions_against_std_complex();
    test_inverse_functions_against_std_complex();
    test_real_axis_and_expression_inputs();
    test_pow_against_std_complex();
    test_branch_cuts_against_std_complex();
    test_branch_cut_identities_with_mpf();
    return 0;
}
