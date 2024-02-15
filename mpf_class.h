/*
 * Copyright (c) 2024
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

#if __cplusplus < 201703L
#error "This class only runs on C++ 17 and later"
#endif

#define MPFR_WANT_FLOAT128

#include <mpfr.h>
#include <iostream>
#include <utility>

#define ___MPFR_CLASS_EXPLICIT___ explicit

namespace mpfrcxx {

class defaults {
  public:
    static mpfr_prec_t prec;
    static mpfr_rnd_t rnd;
    static int base;

    ////////////////////////////////////////////////////////////////////////////////////////
    // 5.1 Initialization Functions (part II)
    ////////////////////////////////////////////////////////////////////////////////////////
    static inline mpfr_prec_t get_default_prec() { return mpfr_get_default_prec(); }
    static void set_default_prec(mpfr_prec_t prec) { mpfr_set_default_prec(prec); }
    static inline mpfr_rnd_t get_default_rounding_mode() { return mpfr_get_default_rounding_mode(); }
    static void set_default_rounding_mode(mpfr_rnd_t r = MPFR_RNDN) { mpfr_set_default_rounding_mode(r); }
    static inline mpfr_prec_t get_default_base() { return base; }
    static void set_default_base(int _base) { base = _base; }
};

class mpfr_class {
  public:
    ////////////////////////////////////////////////////////////////////////////////////////
    // 5.1 Initialization Functions (part I)
    ////////////////////////////////////////////////////////////////////////////////////////
    mpfr_class() { mpfr_init(value); }
    ~mpfr_class() { mpfr_clear(value); }
    void set_prec(mpfr_prec_t prec) { mpfr_set_prec(value, prec); }
    mpfr_prec_t get_prec() const { return mpfr_get_prec(value); }

    ////////////////////////////////////////////////////////////////////////////////////////
    // 5.2 Assignment Functions
    ////////////////////////////////////////////////////////////////////////////////////////
    mpfr_class(const mpfr_class &other) {
        mpfr_init2(value, mpfr_get_prec(other.value));
        mpfr_set(value, other.value, defaults::rnd);
    }
    // Initialization using a constructor
    // move constructor
    mpfr_class(mpfr_class &&other) noexcept { mpfr_swap(value, other.value); }
    ___MPFR_CLASS_EXPLICIT___ mpfr_class(uintmax_t uj) noexcept {
        mpfr_init(value);
        mpfr_set_uj(value, uj, defaults::rnd);
    }
    ___MPFR_CLASS_EXPLICIT___ mpfr_class(intmax_t sj) noexcept {
        mpfr_init(value);
        mpfr_set_sj(value, sj, defaults::rnd);
    }
    ___MPFR_CLASS_EXPLICIT___ mpfr_class(float op) noexcept {
        mpfr_init(value);
        mpfr_set_flt(value, op, defaults::rnd);
    }
    ___MPFR_CLASS_EXPLICIT___ mpfr_class(double d) noexcept {
        mpfr_init(value);
        mpfr_set_d(value, d, defaults::rnd);
    }
    ___MPFR_CLASS_EXPLICIT___ mpfr_class(long double op) noexcept {
        mpfr_init(value);
        mpfr_set_ld(value, op, defaults::rnd);
    }
    ___MPFR_CLASS_EXPLICIT___ mpfr_class(_Float128 op) noexcept {
        mpfr_init(value);
        mpfr_set_float128(value, op, defaults::rnd);
    }
    ___MPFR_CLASS_EXPLICIT___ mpfr_class(const mpz_t op) noexcept {
        mpfr_init(value);
        mpfr_set_z(value, op, defaults::rnd);
    }
    ___MPFR_CLASS_EXPLICIT___ mpfr_class(const mpq_t op) noexcept {
        mpfr_init(value);
        mpfr_set_q(value, op, defaults::rnd);
    }
    ___MPFR_CLASS_EXPLICIT___ mpfr_class(const mpf_t op) noexcept {
        mpfr_init(value);
        mpfr_set_f(value, op, defaults::rnd);
    }
    mpfr_class(const char *str, int base = defaults::base, mpfr_rnd_t rnd = defaults::rnd) {
        mpfr_init(value);
        if (mpfr_set_str(value, str, base, rnd) != 0) {
            std::cerr << "Error initializing mpfr_t from const char*: " << str << std::endl;
            throw std::runtime_error("Failed to initialize mpfr_t with given string.");
        }
    }
    mpfr_class(const std::string &str, int base = defaults::base, mpfr_rnd_t rnd = defaults::rnd) {
        mpfr_init(value);
        if (mpfr_set_str(value, str.c_str(), base, rnd) != 0) {
            std::cerr << "Error initializing mpfr_t from std::string: " << str << std::endl;
            throw std::runtime_error("Failed to initialize mpfr_t with given string.");
        }
    }
    // Initialization using assignment operator
    // Copy-and-Swap Idiom; it does both the copy assignment operator and the move assignment operator.
    mpfr_class &operator=(mpfr_class other) noexcept {
        mpfr_swap(value, other.value);
        return *this;
    }
    mpfr_class &operator=(double d) noexcept {
        mpfr_set_d(value, d, defaults::rnd);
        return *this;
    }
    mpfr_class &operator=(const char *str) {
        if (mpfr_set_str(value, str, defaults::base, defaults::rnd) != 0) {
            std::cerr << "Error assigning mpfr_t from char:" << std::endl;
            throw std::runtime_error("Failed to initialize mpfr_t with given string.");
        }
        return *this;
    }
    mpfr_class &operator=(const std::string &str) {
        if (mpfr_set_str(value, str.c_str(), defaults::base, defaults::rnd) != 0) {
            std::cerr << "Error assigning mpfr_t from string: " << str << std::endl;
            throw std::runtime_error("Failed to initialize mpfr_t with given string.");
        }
        return *this;
    }
    ////////////////////////////////////////////////////////////////////////////////////////
    // 5.3 Combined Initialization and Assignment Functions
    ////////////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////////////
    // 5.4 Conversion Functions
    ////////////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////////////
    // 5.5 Arithmetic Functions
    ////////////////////////////////////////////////////////////////////////////////////////
    mpfr_class operator+(const mpfr_class &rhs) const {
        mpfr_class result;
        mpfr_add(result.value, value, rhs.value, defaults::rnd);
        return result;
    }
    mpfr_class &operator+=(const mpfr_class &rhs) {
        mpfr_add(value, value, rhs.value, defaults::rnd);
        return *this;
    }
    mpfr_class operator*(const mpfr_class &rhs) const {
        mpfr_class result;
        mpfr_mul(result.value, this->value, rhs.value, defaults::rnd);
        return result;
    }
    mpfr_class &operator*=(const mpfr_class &rhs) {
        mpfr_mul(value, value, rhs.value, defaults::rnd);
        return *this;
    }
    mpfr_class operator-(const mpfr_class &rhs) const {
        mpfr_class result;
        mpfr_sub(result.value, this->value, rhs.value, defaults::rnd);
        return result;
    }
    mpfr_class &operator-=(const mpfr_class &rhs) {
        mpfr_sub(value, value, rhs.value, defaults::rnd);
        return *this;
    }
    mpfr_class operator/(const mpfr_class &rhs) const {
        mpfr_class result;
        mpfr_div(result.value, this->value, rhs.value, defaults::rnd);
        return result;
    }
    mpfr_class &operator/=(const mpfr_class &rhs) {
        mpfr_div(value, value, rhs.value, defaults::rnd);
        return *this;
    }
    static mpfr_class sqrt(const mpfr_class &a) {
        mpfr_class result;
        mpfr_sqrt(result.value, a.value, defaults::rnd);
        return result;
    }
    ////////////////////////////////////////////////////////////////////////////////////////
    // 5.6 Comparison Functions
    ////////////////////////////////////////////////////////////////////////////////////////
    friend inline bool operator==(const mpfr_class &lhs, const mpfr_class &rhs) { return mpfr_cmp(lhs.value, rhs.value) == 0; }
    friend inline bool operator!=(const mpfr_class &lhs, const mpfr_class &rhs) { return mpfr_cmp(lhs.value, rhs.value) != 0; }

    ////////////////////////////////////////////////////////////////////////////////////////
    // 5.7 Transcendental Functions
    ////////////////////////////////////////////////////////////////////////////////////////
    static mpfr_class log(const mpfr_class &a) {
        mpfr_class result;
        mpfr_log(result.value, a.value, defaults::rnd);
        return result;
    }
    static mpfr_class log_ui(unsigned long int op) {
        mpfr_class result;
        mpfr_log_ui(result.value, op, defaults::rnd);
        return result;
    }
    static mpfr_class log2(const mpfr_class &a) {
        mpfr_class result;
        mpfr_log2(result.value, a.value, defaults::rnd);
        return result;
    }
    static mpfr_class log10(const mpfr_class &a) {
        mpfr_class result;
        mpfr_log10(result.value, a.value, defaults::rnd);
        return result;
    }
    static mpfr_class log1p(const mpfr_class &a) {
        mpfr_class result;
        mpfr_log1p(result.value, a.value, defaults::rnd);
        return result;
    }
    static mpfr_class log2p1(const mpfr_class &a) {
        mpfr_class result;
        mpfr_log2p1(result.value, a.value, defaults::rnd);
        return result;
    }
    static mpfr_class log10p1(const mpfr_class &a) {
        mpfr_class result;
        mpfr_log10p1(result.value, a.value, defaults::rnd);
        return result;
    }
    static mpfr_class exp(const mpfr_class &a) {
        mpfr_class result;
        mpfr_exp(result.value, a.value, defaults::rnd);
        return result;
    }
    static mpfr_class exp2(const mpfr_class &a) {
        mpfr_class result;
        mpfr_exp2(result.value, a.value, defaults::rnd);
        return result;
    }
    static mpfr_class exp10(const mpfr_class &a) {
        mpfr_class result;
        mpfr_exp10(result.value, a.value, defaults::rnd);
        return result;
    }
    static mpfr_class expm1(const mpfr_class &a) {
        mpfr_class result;
        mpfr_expm1(result.value, a.value, defaults::rnd);
        return result;
    }
    static mpfr_class exp2m1(const mpfr_class &a) {
        mpfr_class result;
        mpfr_exp2m1(result.value, a.value, defaults::rnd);
        return result;
    }
    static mpfr_class exp10m1(const mpfr_class &a) {
        mpfr_class result;
        mpfr_exp10m1(result.value, a.value, defaults::rnd);
        return result;
    }
    static mpfr_class pow(const mpfr_class &base, const mpfr_class &exponent) {
        mpfr_class result;
        mpfr_pow(result.value, base.value, exponent.value, defaults::rnd);
        return result;
    }
    static mpfr_class powr(const mpfr_class &base, const mpfr_class &exponent) {
        mpfr_class result;
        mpfr_powr(result.value, base.value, exponent.value, defaults::rnd);
        return result;
    }
    static mpfr_class pow_ui(const mpfr_class &base, unsigned long int exponent) {
        mpfr_class result;
        mpfr_pow_ui(result.value, base.value, exponent, defaults::rnd);
        return result;
    }
    static mpfr_class pow_si(const mpfr_class &base, long int exponent) {
        mpfr_class result;
        mpfr_pow_si(result.value, base.value, exponent, defaults::rnd);
        return result;
    }
    static mpfr_class pow_uj(const mpfr_class &base, uintmax_t exponent) {
        mpfr_class result;
        mpfr_pow_uj(result.value, base.value, exponent, defaults::rnd);
        return result;
    }
    static mpfr_class pow_sj(const mpfr_class &base, intmax_t exponent) {
        mpfr_class result;
        mpfr_pow_sj(result.value, base.value, exponent, defaults::rnd);
        return result;
    }
    static mpfr_class pown(const mpfr_class &base, intmax_t exponent) {
        mpfr_class result;
        mpfr_pown(result.value, base.value, exponent, defaults::rnd);
        return result;
    }
    static mpfr_class pow_z(const mpfr_class &base, const mpz_t exponent) {
        mpfr_class result;
        mpfr_pow_z(result.value, base.value, exponent, defaults::rnd);
        return result;
    }
    static mpfr_class ui_pow_ui(unsigned long int base, unsigned long int exponent) {
        mpfr_class result;
        mpfr_ui_pow_ui(result.value, base, exponent, defaults::rnd);
        return result;
    }
    static mpfr_class ui_pow(unsigned long int base, const mpfr_class &exponent) {
        mpfr_class result;
        mpfr_ui_pow(result.value, base, exponent.value, defaults::rnd);
        return result;
    }
    static mpfr_class compound_si(const mpfr_class &base, long int n) {
        mpfr_class result;
        mpfr_compound_si(result.value, base.value, n, defaults::rnd);
        return result;
    }
    static mpfr_class cos(const mpfr_class &a) {
        mpfr_class result;
        mpfr_cos(result.value, a.value, defaults::rnd);
        return result;
    }
    static mpfr_class sin(const mpfr_class &a) {
        mpfr_class result;
        mpfr_sin(result.value, a.value, defaults::rnd);
        return result;
    }
    static mpfr_class tan(const mpfr_class &a) {
        mpfr_class result;
        mpfr_tan(result.value, a.value, defaults::rnd);
        return result;
    }
    static mpfr_class cosu(const mpfr_class &a, unsigned long int u) {
        mpfr_class result;
        mpfr_cosu(result.value, a.value, u, defaults::rnd);
        return result;
    }
    static mpfr_class sinu(const mpfr_class &a, unsigned long int u) {
        mpfr_class result;
        mpfr_sinu(result.value, a.value, u, defaults::rnd);
        return result;
    }
    static mpfr_class tanu(const mpfr_class &a, unsigned long int u) {
        mpfr_class result;
        mpfr_tanu(result.value, a.value, u, defaults::rnd);
        return result;
    }
    static mpfr_class cospi(const mpfr_class &a) {
        mpfr_class result;
        mpfr_cospi(result.value, a.value, defaults::rnd);
        return result;
    }
    static mpfr_class sinpi(const mpfr_class &a) {
        mpfr_class result;
        mpfr_sinpi(result.value, a.value, defaults::rnd);
        return result;
    }
    static mpfr_class tanpi(const mpfr_class &a) {
        mpfr_class result;
        mpfr_tanpi(result.value, a.value, defaults::rnd);
        return result;
    }
    static void sin_cos(mpfr_class &sin_result, mpfr_class &cos_result, const mpfr_class &a) { mpfr_sin_cos(sin_result.value, cos_result.value, a.value, defaults::rnd); }
    static mpfr_class sec(const mpfr_class &a) {
        mpfr_class result;
        mpfr_sec(result.value, a.value, defaults::rnd);
        return result;
    }
    static mpfr_class csc(const mpfr_class &a) {
        mpfr_class result;
        mpfr_csc(result.value, a.value, defaults::rnd);
        return result;
    }
    static mpfr_class cot(const mpfr_class &a) {
        mpfr_class result;
        mpfr_cot(result.value, a.value, defaults::rnd);
        return result;
    }
    static mpfr_class acos(const mpfr_class &a) {
        mpfr_class result;
        mpfr_acos(result.value, a.value, defaults::rnd);
        return result;
    }
    static mpfr_class asin(const mpfr_class &a) {
        mpfr_class result;
        mpfr_asin(result.value, a.value, defaults::rnd);
        return result;
    }
    static mpfr_class atan(const mpfr_class &a) {
        mpfr_class result;
        mpfr_atan(result.value, a.value, defaults::rnd);
        return result;
    }
    static mpfr_class acosu(const mpfr_class &a, unsigned long int u) {
        mpfr_class result;
        mpfr_acosu(result.value, a.value, u, defaults::rnd);
        return result;
    }
    static mpfr_class asinu(const mpfr_class &a, unsigned long int u) {
        mpfr_class result;
        mpfr_asinu(result.value, a.value, u, defaults::rnd);
        return result;
    }
    static mpfr_class atanu(const mpfr_class &a, unsigned long int u) {
        mpfr_class result;
        mpfr_atanu(result.value, a.value, u, defaults::rnd);
        return result;
    }
    static mpfr_class acospi(const mpfr_class &a) {
        mpfr_class result;
        mpfr_acospi(result.value, a.value, defaults::rnd);
        return result;
    }
    static mpfr_class asinpi(const mpfr_class &a) {
        mpfr_class result;
        mpfr_asinpi(result.value, a.value, defaults::rnd);
        return result;
    }
    static mpfr_class atanpi(const mpfr_class &a) {
        mpfr_class result;
        mpfr_atanpi(result.value, a.value, defaults::rnd);
        return result;
    }
    static mpfr_class atan2(const mpfr_class &y, const mpfr_class &x) {
        mpfr_class result;
        mpfr_atan2(result.value, y.value, x.value, defaults::rnd);
        return result;
    }
    static mpfr_class atan2u(const mpfr_class &y, const mpfr_class &x, unsigned long int u) {
        mpfr_class result;
        mpfr_atan2u(result.value, y.value, x.value, u, defaults::rnd);
        return result;
    }
    static mpfr_class atan2pi(const mpfr_class &y, const mpfr_class &x) {
        mpfr_class result;
        mpfr_atan2pi(result.value, y.value, x.value, defaults::rnd);
        return result;
    }
    static mpfr_class cosh(const mpfr_class &a) {
        mpfr_class result;
        mpfr_cosh(result.value, a.value, defaults::rnd);
        return result;
    }
    static mpfr_class sinh(const mpfr_class &a) {
        mpfr_class result;
        mpfr_sinh(result.value, a.value, defaults::rnd);
        return result;
    }
    static mpfr_class tanh(const mpfr_class &a) {
        mpfr_class result;
        mpfr_tanh(result.value, a.value, defaults::rnd);
        return result;
    }
    static void sinh_cosh(mpfr_class &sinh_result, mpfr_class &cosh_result, const mpfr_class &a) { mpfr_sinh_cosh(sinh_result.value, cosh_result.value, a.value, defaults::rnd); }
    static mpfr_class sech(const mpfr_class &a) {
        mpfr_class result;
        mpfr_sech(result.value, a.value, defaults::rnd);
        return result;
    }
    static mpfr_class csch(const mpfr_class &a) {
        mpfr_class result;
        mpfr_csch(result.value, a.value, defaults::rnd);
        return result;
    }
    static mpfr_class coth(const mpfr_class &a) {
        mpfr_class result;
        mpfr_coth(result.value, a.value, defaults::rnd);
        return result;
    }
    static mpfr_class acosh(const mpfr_class &a) {
        mpfr_class result;
        mpfr_acosh(result.value, a.value, defaults::rnd);
        return result;
    }
    static mpfr_class asinh(const mpfr_class &a) {
        mpfr_class result;
        mpfr_asinh(result.value, a.value, defaults::rnd);
        return result;
    }
    static mpfr_class atanh(const mpfr_class &a) {
        mpfr_class result;
        mpfr_atanh(result.value, a.value, defaults::rnd);
        return result;
    }
    static mpfr_class eint(const mpfr_class &a) {
        mpfr_class result;
        mpfr_eint(result.value, a.value, defaults::rnd);
        return result;
    }
    static mpfr_class li2(const mpfr_class &a) {
        mpfr_class result;
        mpfr_li2(result.value, a.value, defaults::rnd);
        return result;
    }
    static mpfr_class gamma(const mpfr_class &a) {
        mpfr_class result;
        mpfr_gamma(result.value, a.value, defaults::rnd);
        return result;
    }
    static mpfr_class gamma_inc(const mpfr_class &a, const mpfr_class &x) {
        mpfr_class result;
        mpfr_gamma_inc(result.value, a.value, x.value, defaults::rnd);
        return result;
    }
    static mpfr_class lngamma(const mpfr_class &a) {
        mpfr_class result;
        mpfr_lngamma(result.value, a.value, defaults::rnd);
        return result;
    }
    static mpfr_class lgamma(const mpfr_class &a, int &signp) {
        mpfr_class result;
        mpfr_lgamma(result.value, &signp, a.value, defaults::rnd);
        return result;
    }
    static mpfr_class digamma(const mpfr_class &a) {
        mpfr_class result;
        mpfr_digamma(result.value, a.value, defaults::rnd);
        return result;
    }
    static mpfr_class beta(const mpfr_class &x, const mpfr_class &y) {
        mpfr_class result;
        mpfr_beta(result.value, x.value, y.value, defaults::rnd);
        return result;
    }
    static mpfr_class zeta(const mpfr_class &s) {
        mpfr_class result;
        mpfr_zeta(result.value, s.value, defaults::rnd);
        return result;
    }
    static mpfr_class zeta_ui(unsigned long int s) {
        mpfr_class result;
        mpfr_zeta_ui(result.value, s, defaults::rnd);
        return result;
    }
    static mpfr_class erf(const mpfr_class &x) {
        mpfr_class result;
        mpfr_erf(result.value, x.value, defaults::rnd);
        return result;
    }
    static mpfr_class erfc(const mpfr_class &x) {
        mpfr_class result;
        mpfr_erfc(result.value, x.value, defaults::rnd);
        return result;
    }
    static mpfr_class j0(const mpfr_class &x) {
        mpfr_class result;
        mpfr_j0(result.value, x.value, defaults::rnd);
        return result;
    }
    static mpfr_class j1(const mpfr_class &x) {
        mpfr_class result;
        mpfr_j1(result.value, x.value, defaults::rnd);
        return result;
    }
    static mpfr_class jn(long int n, const mpfr_class &x) {
        mpfr_class result;
        mpfr_jn(result.value, n, x.value, defaults::rnd);
        return result;
    }
    static mpfr_class y0(const mpfr_class &x) {
        mpfr_class result;
        mpfr_y0(result.value, x.value, defaults::rnd);
        return result;
    }
    static mpfr_class y1(const mpfr_class &x) {
        mpfr_class result;
        mpfr_y1(result.value, x.value, defaults::rnd);
        return result;
    }
    static mpfr_class yn(long int n, const mpfr_class &x) {
        mpfr_class result;
        mpfr_yn(result.value, n, x.value, defaults::rnd);
        return result;
    }
    static mpfr_class agm(const mpfr_class &a, const mpfr_class &b) {
        mpfr_class result;
        mpfr_agm(result.value, a.value, b.value, defaults::rnd);
        return result;
    }
    static mpfr_class ai(const mpfr_class &x) {
        mpfr_class result;
        mpfr_ai(result.value, x.value, defaults::rnd);
        return result;
    }
    static mpfr_class const_log2() {
        mpfr_class result;
        mpfr_const_log2(result.value, defaults::rnd);
        return result;
    }
    static mpfr_class const_pi() {
        mpfr_class result;
        mpfr_const_pi(result.value, defaults::rnd);
        return result;
    }
    static mpfr_class const_euler() {
        mpfr_class result;
        mpfr_const_euler(result.value, defaults::rnd);
        return result;
    }
    static mpfr_class const_catalan() {
        mpfr_class result;
        mpfr_const_catalan(result.value, defaults::rnd);
        return result;
    }

    ////////////////////////////////////////////////////////////////////////////////////////
    // 5.8 Input and Output Functions
    ////////////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////////////
    bool is_nan() const { return mpfr_nan_p(value) != 0; }
    bool is_inf() const { return mpfr_inf_p(value) != 0; }

    mpfr_t *get_mpfr_t() { return &value; }

  private:
    mpfr_t value;
};

} // namespace mpfrcxx

mpfr_prec_t mpfrcxx::defaults::prec;
mpfr_rnd_t mpfrcxx::defaults::rnd;
int mpfrcxx::defaults::base;

class Initializer {
  public:
    Initializer() {
        mpfrcxx::defaults::set_default_prec(512);
        mpfrcxx::defaults::set_default_rounding_mode(MPFR_RNDN);
        mpfrcxx::defaults::set_default_base(10);
    }
};

Initializer globalInitializer;
