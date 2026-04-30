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

}  // namespace

int main() {
    test_primary_functions_against_std_complex();
    test_inverse_functions_against_std_complex();
    test_real_axis_and_expression_inputs();
    test_pow_against_std_complex();
    return 0;
}
