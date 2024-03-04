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

#include <gmp.h>
#include <iostream>
#include <utility>

#define ___MPF_CLASS_EXPLICIT___ explicit

namespace gmp {

class defaults {
  public:
    static mp_bitcnt_t prec;
    static int base;
    static inline void set_default_prec(const mp_bitcnt_t prec) { mpf_set_default_prec(prec); }
    static inline mp_bitcnt_t get_default_prec() { return mpf_get_default_prec(); }
};

class mpf_class {
  public:
    ////////////////////////////////////////////////////////////////////////////////////////
    // 7.1 Initialization Functions
    ////////////////////////////////////////////////////////////////////////////////////////
    mpf_class() { mpf_init(value); }
    ~mpf_class() { mpf_clear(value); }
    void set_prec(mp_bitcnt_t prec) { mpf_set_prec(value, prec); }
    mp_bitcnt_t get_prec() const { return mpf_get_prec(value); }

    mpf_class(const mpf_class &other) {
        mpf_init2(value, mpf_get_prec(other.value));
        mpf_set(value, other.value);
    }

    mpf_class(mpf_class &&other) noexcept { mpf_swap(value, other.value); }
    ___MPF_CLASS_EXPLICIT___ mpf_class(unsigned long int ui) noexcept {
        mpf_init(value);
        mpf_set_ui(value, ui);
    }
    ___MPF_CLASS_EXPLICIT___ mpf_class(signed long int si) noexcept {
        mpf_init(value);
        mpf_set_si(value, si);
    }
    mpf_class(const char *str, int base = defaults::base) {
        mpf_init(value);
        if (mpf_set_str(value, str, base) != 0) {
            std::cerr << "Error initializing mpf_t from const char*: " << str << std::endl;
            throw std::runtime_error("Failed to initialize mpf_t with given string.");
        }
    }
    mpf_class(const std::string &str, int base = defaults::base) {
        mpf_init(value);
        if (mpf_set_str(value, str.c_str(), base) != 0) {
            std::cerr << "Error initializing mpf_t from std::string: " << str << std::endl;
            throw std::runtime_error("Failed to initialize mpf_t with given string.");
        }
    }
    // Initialization using assignment operator
    // Copy-and-Swap Idiom; it does both the copy assignment operator and the move assignment operator.
    mpf_class &operator=(mpf_class other) noexcept {
        mpf_swap(value, other.value);
        return *this;
    }
    mpf_class &operator=(double d) noexcept {
        mpf_set_d(value, d);
        return *this;
    }
    mpf_class &operator=(const char *str) {
        if (mpf_set_str(value, str, defaults::base) != 0) {
            std::cerr << "Error assigning mpf_t from char:" << std::endl;
            throw std::runtime_error("Failed to initialize mpf_t with given string.");
        }
        return *this;
    }
    mpf_class &operator=(const std::string &str) {
        if (mpf_set_str(value, str.c_str(), defaults::base) != 0) {
            std::cerr << "Error assigning mpf_t from string: " << str << std::endl;
            throw std::runtime_error("Failed to initialize mpf_t with given string.");
        }
        return *this;
    }

    mpf_class operator+(const mpf_class &rhs) const {
        mpf_class result;
        mpf_add(result.value, value, rhs.value);
        return result;
    }
    mpf_class &operator+=(const mpf_class &rhs) {
        mpf_add(value, value, rhs.value);
        return *this;
    }
    mpf_class operator*(const mpf_class &rhs) const {
        mpf_class result;
        mpf_mul(result.value, this->value, rhs.value);
        return result;
    }
    mpf_class &operator*=(const mpf_class &rhs) {
        mpf_mul(value, value, rhs.value);
        return *this;
    }
    mpf_class operator-(const mpf_class &rhs) const {
        mpf_class result;
        mpf_sub(result.value, this->value, rhs.value);
        return result;
    }
    mpf_class &operator-=(const mpf_class &rhs) {
        mpf_sub(value, value, rhs.value);
        return *this;
    }
    mpf_class operator/(const mpf_class &rhs) const {
        mpf_class result;
        mpf_div(result.value, this->value, rhs.value);
        return result;
    }
    mpf_class &operator/=(const mpf_class &rhs) {
        mpf_div(value, value, rhs.value);
        return *this;
    }
    static mpf_class sqrt(const mpf_class &a) {
        mpf_class result;
        mpf_sqrt(result.value, a.value);
        return result;
    }
    friend inline bool operator==(const mpf_class &lhs, const mpf_class &rhs) { return mpf_cmp(lhs.value, rhs.value) == 0; }
    friend inline bool operator!=(const mpf_class &lhs, const mpf_class &rhs) { return mpf_cmp(lhs.value, rhs.value) != 0; }
    mpf_t *get_mpf_t() { return &value; }

  private:
    mpf_t value;
};

} // namespace gmp

mp_bitcnt_t gmp::defaults::prec;
int gmp::defaults::base;

class mpf_class_initializer {
  public:
    mpf_class_initializer() {
        gmp::defaults::set_default_prec(512);
        gmp::defaults::base = 10;
    }
};

mpf_class_initializer global_mpf_class_initializer;
