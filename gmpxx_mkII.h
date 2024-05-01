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
#include <cassert>
#include <cstring>

#define ___MPF_CLASS_EXPLICIT___ explicit

#if !defined ___GMPXX_STRICT_COMPATIBILITY___
namespace gmp {
#endif

class gmpxx_defaults {
  public:
    static int base;
    static inline void set_default_prec(const mp_bitcnt_t _prec) { mpf_set_default_prec(_prec); }
    static inline void set_default_prec_raw(const mp_bitcnt_t prec_raw) { mpf_set_default_prec(prec_raw); }
    static inline mp_bitcnt_t get_default_prec() { return mpf_get_default_prec(); }
    static inline void set_default_base(const int _base) { base = _base; }
};
class mpz_class;
class mpq_class;
class mpf_class;

class mpz_class {
  public:
    ////////////////////////////////////////////////////////////////////////////////////////
    // 12.2 C++ Interface Integers
    ////////////////////////////////////////////////////////////////////////////////////////
    // constructor
    mpz_class() { mpz_init(value); }   // default constructor
    ~mpz_class() { mpz_clear(value); } // The rule 3 of 5 default deconstructor
    explicit mpz_class(const mpz_t z) {
        mpz_init(value);
        mpz_set(value, z);
    }
    explicit operator unsigned long int() const { return mpz_get_ui(this->value); }
    explicit operator signed long int() const { return mpz_get_si(this->value); }
    mpz_class(unsigned long int op) { mpz_init_set_ui(value, op); }
    mpz_class(signed long int op) { mpz_init_set_si(value, op); }
    mpz_class(double op) { mpz_init_set_d(value, op); }
    mpz_class(unsigned int op) { mpz_init_set_ui(value, (unsigned long int)op); }
    mpz_class(signed int op) { mpz_init_set_si(value, (signed long int)op); }
    mpz_class(const mpq_t op) {
        mpz_init(value);
        mpz_set_q(value, op);
    }
    mpz_class(const mpf_t op) {
        mpz_init(value);
        mpz_set_f(value, op);
    }
    mpz_class(const mpz_class &op) { // The rule 1 of 5 copy constructor
                                     // std::cout << "The rule 1 of 5 copy constructor\n" ;
        mpz_init(value);
        mpz_set(value, op.value);
    }
    mpz_class(mpz_class &&op) noexcept { // The rule 4 of 5 move constructor
                                         // std::cout << "The rule 4 of 5 move constructor\n" ;
        mpz_init(value);
        mpz_swap(value, op.value);
    }
    mpz_class(const char *str, int base = 0) {
        mpz_init(value);
        if (mpz_set_str(value, str, base) != 0) {
            throw std::invalid_argument("");
        }
    }
    mpz_class(const std::string &str, int base = 0) {
        mpz_init(value);
        if (mpz_set_str(value, str.c_str(), base) != 0) {
            throw std::invalid_argument("");
        }
    }
    // mpz_class& mpz_class::operator= (type op)
    mpz_class &operator=(const mpz_class &op) noexcept { // The rule 2 of 5 copy assignment operator
        // std::cout << "The rule 2 of 5 copy assignment operator\n" ;
        if (this != &op) {
            mpz_set(value, op.value);
        }
        return *this;
    }
    mpz_class &operator=(mpz_class &&op) noexcept { // The rule 5 of 5 move assignment operator
        // std::cout << "The rule 5 of 5 move assignment operator\n" ;
        if (this != &op) {
            mpz_swap(value, op.value);
        }
        return *this;
    }
    mpz_class &operator=(double d) noexcept {
        mpz_set_d(value, d);
        return *this;
    }
    mpz_class &operator=(const char *str) {
        if (mpz_set_str(value, str, 0) != 0) {
            throw std::invalid_argument("");
        }
        return *this;
    }
    mpz_class &operator=(const std::string &str) {
        if (mpz_set_str(value, str.c_str(), 0) != 0) {
            throw std::invalid_argument("");
        }
        return *this;
    }
    // mpz_class operator/ (mpz_class a, mpz_class d)
    // mpz_class operator% (mpz_class a, mpz_class d)
    inline friend mpz_class &operator+=(mpz_class &lhs, const mpz_class &rhs);
    inline friend mpz_class &operator-=(mpz_class &lhs, const mpz_class &rhs);
    inline friend mpz_class &operator*=(mpz_class &lhs, const mpz_class &rhs);
    inline friend mpz_class &operator/=(mpz_class &lhs, const mpz_class &rhs);
    inline friend mpz_class &operator%=(mpz_class &lhs, const mpz_class &rhs);
    inline friend mpz_class operator+(const mpz_class &op);
    inline friend mpz_class operator-(const mpz_class &op);
    inline friend mpz_class operator+(const mpz_class &op1, const mpz_class &op2);
    inline friend mpz_class operator-(const mpz_class &op1, const mpz_class &op2);
    inline friend mpz_class operator*(const mpz_class &op1, const mpz_class &op2);
    inline friend mpz_class operator/(const mpz_class &op1, const mpz_class &op2);
    inline friend mpz_class operator%(const mpz_class &op1, const mpz_class &op2);
    inline friend mpz_class operator&(const mpz_class &op1, const mpz_class &op2);
    inline friend mpz_class operator|(const mpz_class &op1, const mpz_class &op2);
    inline friend mpz_class operator^(const mpz_class &op1, const mpz_class &op2);

    // int cmp (mpz_class op1, type op2)
    // int cmp (type op1, mpz_class op2)
    inline friend bool operator==(const mpz_class &op1, const mpz_class &op2) { return mpz_cmp(op1.value, op2.value) == 0; }
    inline friend bool operator!=(const mpz_class &op1, const mpz_class &op2) { return mpz_cmp(op1.value, op2.value) != 0; }
    inline friend bool operator<(const mpz_class &op1, const mpz_class &op2) { return mpz_cmp(op1.value, op2.value) < 0; }
    inline friend bool operator>(const mpz_class &op1, const mpz_class &op2) { return mpz_cmp(op1.value, op2.value) > 0; }
    inline friend bool operator<=(const mpz_class &op1, const mpz_class &op2) { return mpz_cmp(op1.value, op2.value) <= 0; }
    inline friend bool operator>=(const mpz_class &op1, const mpz_class &op2) { return mpz_cmp(op1.value, op2.value) >= 0; }

    inline mpz_class &operator=(const signed long int op);
    inline mpz_class &operator=(const unsigned long int op);
    inline mpz_class &operator=(const signed int op);
    inline mpz_class &operator=(const unsigned int op);
    inline mpz_class &operator=(const signed char op);
    inline mpz_class &operator=(const unsigned char op);
    inline mpz_class &operator=(const char op);

    inline friend mpz_class operator<<(const mpz_class &op1, const unsigned int op2) {
        mpz_class result;
        mpz_mul_2exp(result.value, op1.get_mpz_t(), op2);
        return result;
    }
    friend mpz_class operator>>(const mpz_class &op1, const unsigned int op2) {
        mpz_class result;
        mpz_fdiv_q_2exp(result.value, op1.get_mpz_t(), op2);
        return result;
    }
    friend mpz_class operator>>(const mpz_class &op1, const signed int op2) {
        mpz_class result;
        mpz_fdiv_q_2exp(result.value, op1.get_mpz_t(), op2);
        return result;
    }

    // mpz_class abs (mpz_class op)
    inline friend mpz_class abs(const mpz_class &op);

    // bool mpz_class::fits_sint_p (void)
    // bool mpz_class::fits_slong_p (void)
    // bool mpz_class::fits_sshort_p (void)
    // bool mpz_class::fits_uint_p (void)
    // bool mpz_class::fits_ulong_p (void)
    // bool mpz_class::fits_ushort_p (void)
    inline bool fits_sint_p() const { return mpz_fits_sint_p(value) != 0; }
    inline bool fits_slong_p() const { return mpz_fits_slong_p(value) != 0; }
    inline bool fits_sshort_p() const { return mpz_fits_sshort_p(value) != 0; }
    inline bool fits_uint_p() const { return mpz_fits_uint_p(value) != 0; }
    inline bool fits_ulong_p() const { return mpz_fits_ulong_p(value) != 0; }
    inline bool fits_ushort_p() const { return mpz_fits_ushort_p(value) != 0; }

    // double mpz_class::get_d (void)
    // long mpz_class::get_si (void)
    // unsigned long mpz_class::get_ui (void)
    inline double get_d() const { return mpz_get_d(value); }
    inline signed long int get_si() const { return mpz_get_si(value); }
    inline unsigned long int get_ui() const { return mpz_get_ui(value); }
    // string mpz_class::get_str (int base = 10)
    inline std::string get_str(int base = 10) const {
        char *temp = mpz_get_str(nullptr, base, value);
        std::string result(temp);
        void (*freefunc)(void *, size_t);
        mp_get_memory_functions(nullptr, nullptr, &freefunc);
        freefunc(temp, std::strlen(temp) + 1);
        return result;
    }
    // int mpz_class::set_str (const char *str, int base)
    // int mpz_class::set_str (const string& str, int base)
    int set_str(const char *str, int base) { return mpz_set_str(value, str, base); }
    int set_str(const std::string &str, int base) { return mpz_set_str(value, str.c_str(), base); }

    // int sgn (mpz_class op)
    // mpz_class sqrt (mpz_class op)
    // mpz_class gcd (mpz_class op1, mpz_class op2)
    // mpz_class lcm (mpz_class op1, mpz_class op2)
    friend int sgn(const mpz_class &op);
    friend mpz_class sqrt(const mpz_class &op);
    friend mpz_class gcd(const mpz_class &op1, const mpz_class &op2);
    friend mpz_class lcm(const mpz_class &op1, const mpz_class &op2);

    // mpz_class mpz_class::factorial (type op)
    // mpz_class factorial (mpz_class op)
    // mpz_class mpz_class::fibonacci (type op)
    // mpz_class fibonacci (mpz_class op)
    // mpz_class mpz_class::primorial (type op)
    // mpz_class primorial (mpz_class op)
    static mpz_class factorial(unsigned long int n) {
        mpz_class result;
        mpz_fac_ui(result.value, n);
        return result;
    }
    friend mpz_class factorial(const mpz_class &op);
    static mpz_class primorial(unsigned long int n) {
        mpz_class result;
        mpz_primorial_ui(result.value, n);
        return result;
    }
    friend mpz_class primorial(const mpz_class &op);
    static mpz_class fibonacci(unsigned long int n) {
        mpz_class result;
        mpz_fib_ui(result.value, n);
        return result;
    }
    friend mpz_class fibonacci(const mpz_class &op);

    // void mpz_class::swap (mpz_class& op)
    // void swap (mpz_class& op1, mpz_class& op2)
    void swap(mpz_class &op) { mpz_swap(this->value, op.value); }
#if !defined ___GMPXX_STRICT_COMPATIBILITY___
    friend void swap(mpz_class &op1, mpz_class &op2) { mpz_swap(op1.value, op2.value); }
#endif
    inline friend mpz_class &operator+=(mpz_class &lhs, const unsigned long int rhs);
    inline friend mpz_class &operator-=(mpz_class &lhs, const unsigned long int rhs);
    inline friend mpz_class &operator*=(mpz_class &lhs, const unsigned long int rhs);
    inline friend mpz_class &operator/=(mpz_class &lhs, const unsigned long int rhs);
    inline friend mpz_class &operator%=(mpz_class &lhs, const unsigned long int rhs);
    inline friend mpz_class operator+(const mpz_class &op1, const unsigned long int op2);
    inline friend mpz_class operator+(const unsigned long int op1, const mpz_class &op2);
    inline friend mpz_class operator-(const mpz_class &op1, const unsigned long int op2);
    inline friend mpz_class operator-(const unsigned long int op1, const mpz_class &op2);
    inline friend mpz_class operator*(const mpz_class &op1, const unsigned long int op2);
    inline friend mpz_class operator*(const unsigned long int op1, const mpz_class &op2);
    inline friend mpz_class operator/(const mpz_class &op1, unsigned long int op2);
    inline friend mpz_class operator/(const unsigned long int op1, const mpz_class &op2);
    inline friend mpz_class operator%(const mpz_class &op1, unsigned long int op2);
    inline friend mpz_class operator%(const unsigned long int op1, const mpz_class &op2);

    inline friend mpz_class &operator+=(mpz_class &lhs, const signed long int rhs);
    inline friend mpz_class &operator-=(mpz_class &lhs, const signed long int rhs);
    inline friend mpz_class &operator*=(mpz_class &lhs, const signed long int rhs);
    inline friend mpz_class &operator/=(mpz_class &lhs, const signed long int rhs);
    inline friend mpz_class &operator%=(mpz_class &lhs, const signed long int rhs);
    inline friend mpz_class operator+(const mpz_class &op1, const signed long int op2);
    inline friend mpz_class operator+(const signed long int op1, const mpz_class &op2);
    inline friend mpz_class operator-(const mpz_class &op1, const signed long int op2);
    inline friend mpz_class operator-(const signed long int op1, const mpz_class &op2);
    inline friend mpz_class operator*(const mpz_class &op1, const signed long int op2);
    inline friend mpz_class operator*(const signed long int op1, const mpz_class &op2);
    inline friend mpz_class operator/(const mpz_class &op1, signed long int op2);
    inline friend mpz_class operator/(const signed long int op1, const mpz_class &op2);
    inline friend mpz_class operator%(const mpz_class &op1, signed long int op2);
    inline friend mpz_class operator%(const signed long int op1, const mpz_class &op2);

    inline friend mpz_class &operator+=(mpz_class &lhs, const unsigned int rhs);
    inline friend mpz_class &operator-=(mpz_class &lhs, const unsigned int rhs);
    inline friend mpz_class &operator*=(mpz_class &lhs, const unsigned int rhs);
    inline friend mpz_class &operator/=(mpz_class &lhs, const unsigned int rhs);
    inline friend mpz_class &operator%=(mpz_class &lhs, const unsigned int rhs);
    inline friend mpz_class operator+(const mpz_class &op1, const unsigned int op2);
    inline friend mpz_class operator+(const unsigned int op1, const mpz_class &op2);
    inline friend mpz_class operator-(const mpz_class &op1, const unsigned int op2);
    inline friend mpz_class operator-(const unsigned int op1, const mpz_class &op2);
    inline friend mpz_class operator*(const mpz_class &op1, const unsigned int op2);
    inline friend mpz_class operator*(const unsigned int op1, const mpz_class &op2);
    inline friend mpz_class operator/(const mpz_class &op1, unsigned int op2);
    inline friend mpz_class operator/(const unsigned int op1, const mpz_class &op2);
    inline friend mpz_class operator%(const mpz_class &op1, unsigned int op2);
    inline friend mpz_class operator%(const unsigned int op1, const mpz_class &op2);

    inline friend mpz_class &operator+=(mpz_class &lhs, const int rhs);
    inline friend mpz_class &operator-=(mpz_class &lhs, const int rhs);
    inline friend mpz_class &operator*=(mpz_class &lhs, const int rhs);
    inline friend mpz_class &operator/=(mpz_class &lhs, const int rhs);
    inline friend mpz_class &operator%=(mpz_class &lhs, const int rhs);
    inline friend mpz_class operator+(const mpz_class &op1, const int op2);
    inline friend mpz_class operator+(const int op1, const mpz_class &op2);
    inline friend mpz_class operator-(const mpz_class &op1, const int op2);
    inline friend mpz_class operator-(const int op1, const mpz_class &op2);
    inline friend mpz_class operator*(const mpz_class &op1, const int op2);
    inline friend mpz_class operator*(const int op1, const mpz_class &op2);
    inline friend mpz_class operator/(const mpz_class &op1, int op2);
    inline friend mpz_class operator/(const int op1, const mpz_class &op2);
    inline friend mpz_class operator%(const mpz_class &op1, int op2);
    inline friend mpz_class operator%(const int op1, const mpz_class &op2);

    inline friend mpz_class &operator+=(mpz_class &lhs, const double rhs);
    inline friend mpz_class &operator-=(mpz_class &lhs, const double rhs);
    inline friend mpz_class &operator*=(mpz_class &lhs, const double rhs);
    inline friend mpz_class &operator/=(mpz_class &lhs, const double rhs);
    inline friend mpz_class &operator%=(mpz_class &lhs, const double rhs);
    inline friend mpz_class operator+(const mpz_class &op1, const double op2);
    inline friend mpz_class operator+(const double op1, const mpz_class &op2);
    inline friend mpz_class operator-(const mpz_class &op1, const double op2);
    inline friend mpz_class operator-(const double op1, const mpz_class &op2);
    inline friend mpz_class operator*(const mpz_class &op1, const double op2);
    inline friend mpz_class operator*(const double op1, const mpz_class &op2);
    inline friend mpz_class operator/(const mpz_class &op1, double op2);
    inline friend mpz_class operator/(const double op1, const mpz_class &op2);

    friend std::ostream &operator<<(std::ostream &os, const mpz_class &m);

    operator mpf_class() const;
    operator mpq_class() const;
    mpz_srcptr get_mpz_t() const { return value; }

  private:
    mpz_t value;
};
inline mpz_class &operator+=(mpz_class &lhs, const unsigned long int rhs) {
    mpz_add_ui(lhs.value, lhs.value, rhs);
    return lhs;
}
inline mpz_class &operator-=(mpz_class &lhs, const unsigned long int rhs) {
    mpz_sub_ui(lhs.value, lhs.value, rhs);
    return lhs;
}
inline mpz_class &operator*=(mpz_class &lhs, const unsigned long int rhs) {
    mpz_mul_ui(lhs.value, lhs.value, rhs);
    return lhs;
}
inline mpz_class &operator/=(mpz_class &lhs, const unsigned long int rhs) {
    mpz_div_ui(lhs.value, lhs.value, rhs);
    return lhs;
}
inline mpz_class &operator%=(mpz_class &lhs, const unsigned long int rhs) {
    mpz_mod_ui(lhs.value, lhs.value, rhs);
    return lhs;
}
inline mpz_class operator+(const mpz_class &op1, const unsigned long int op2) {
    mpz_class result;
    mpz_add_ui(result.value, op1.value, op2);
    return result;
}
inline mpz_class operator+(const unsigned long int op1, const mpz_class &op2) { return op2 + op1; }
inline mpz_class operator-(const mpz_class &op1, const unsigned long int op2) {
    mpz_class result;
    mpz_sub_ui(result.value, op1.value, op2);
    return result;
}
inline mpz_class operator-(const unsigned long int op1, const mpz_class &op2) {
    mpz_class result(op1);
    mpz_ui_sub(result.value, op1, op2.value);
    return result;
}
inline mpz_class operator*(const mpz_class &op1, const unsigned long int op2) {
    mpz_class result;
    mpz_mul_ui(result.value, op1.value, op2);
    return result;
}
inline mpz_class operator*(const unsigned long int op1, const mpz_class &op2) { return op2 * op1; }
inline mpz_class operator/(const mpz_class &op1, const unsigned long int op2) {
    mpz_class result;
    mpz_div_ui(result.value, op1.value, op2);
    return result;
}
inline mpz_class operator/(const unsigned long int op1, const mpz_class &op2) {
    mpz_class result(op1);
    result /= op2;
    return result;
}
inline mpz_class operator%(const mpz_class &op1, const unsigned long int op2) {
    mpz_class result;
    mpz_mod_ui(result.value, op1.value, op2);
    return result;
}
inline mpz_class operator%(const unsigned long int op1, const mpz_class &op2) {
    mpz_class result(op1);
    result %= op2;
    return result;
}
inline mpz_class &operator+=(mpz_class &lhs, const unsigned int rhs) {
    mpz_add_ui(lhs.value, lhs.value, rhs);
    return lhs;
}
inline mpz_class &operator-=(mpz_class &lhs, const unsigned int rhs) {
    mpz_sub_ui(lhs.value, lhs.value, rhs);
    return lhs;
}
inline mpz_class &operator*=(mpz_class &lhs, const unsigned int rhs) {
    mpz_mul_ui(lhs.value, lhs.value, rhs);
    return lhs;
}
inline mpz_class &operator/=(mpz_class &lhs, const unsigned int rhs) {
    mpz_div_ui(lhs.value, lhs.value, rhs);
    return lhs;
}
inline mpz_class &operator%=(mpz_class &lhs, const unsigned int rhs) {
    mpz_mod_ui(lhs.value, lhs.value, rhs);
    return lhs;
}
inline mpz_class operator+(const mpz_class &op1, const unsigned int op2) {
    mpz_class result;
    mpz_add_ui(result.value, op1.value, op2);
    return result;
}
inline mpz_class operator+(const unsigned int op1, const mpz_class &op2) { return op2 + op1; }
inline mpz_class operator-(const mpz_class &op1, const unsigned int op2) {
    mpz_class result;
    mpz_sub_ui(result.value, op1.value, op2);
    return result;
}
inline mpz_class operator-(const unsigned int op1, const mpz_class &op2) {
    mpz_class result(op1);
    mpz_ui_sub(result.value, op1, op2.value);
    return result;
}
inline mpz_class operator*(const mpz_class &op1, const unsigned int op2) {
    mpz_class result;
    mpz_mul_ui(result.value, op1.value, op2);
    return result;
}
inline mpz_class operator*(const unsigned int op1, const mpz_class &op2) { return op2 * op1; }
inline mpz_class operator/(const mpz_class &op1, const unsigned int op2) {
    mpz_class result;
    mpz_div_ui(result.value, op1.value, op2);
    return result;
}
inline mpz_class operator/(const unsigned int op1, const mpz_class &op2) {
    mpz_class result(op1);
    result /= op2;
    return result;
}
inline mpz_class operator%(const mpz_class &op1, const unsigned int op2) {
    mpz_class result;
    mpz_mod_ui(result.value, op1.value, op2);
    return result;
}
inline mpz_class operator%(const unsigned int op1, const mpz_class &op2) {
    mpz_class result(op1);
    result %= op2;
    return result;
}
inline mpz_class &operator+=(mpz_class &lhs, const signed long int rhs) {
    if (rhs >= 0)
        mpz_add_ui(lhs.value, lhs.value, (unsigned long int)rhs);
    else {
        unsigned int _rhs = -rhs;
        mpz_sub_ui(lhs.value, lhs.value, _rhs);
    }
    return lhs;
}
inline mpz_class &operator-=(mpz_class &lhs, const signed long int rhs) {
    if (rhs >= 0)
        mpz_sub_ui(lhs.value, lhs.value, (unsigned long int)rhs);
    else {
        unsigned int _rhs = -rhs;
        mpz_add_ui(lhs.value, lhs.value, _rhs);
    }
    return lhs;
}
inline mpz_class &operator*=(mpz_class &lhs, const signed long int rhs) {
    if (rhs >= 0)
        mpz_mul_ui(lhs.value, lhs.value, (unsigned long int)rhs);
    else {
        unsigned long int _rhs = -rhs;
        mpz_mul_ui(lhs.value, lhs.value, _rhs);
        mpz_neg(lhs.value, lhs.value);
    }
    return lhs;
}
inline mpz_class &operator/=(mpz_class &lhs, const signed long int rhs) {
    if (rhs == 0) {
        throw std::invalid_argument("Division by zero is undefined");
    }
    if (rhs > 0)
        mpz_div_ui(lhs.value, lhs.value, (unsigned long int)rhs);
    else {
        unsigned long int _rhs = -rhs;
        mpz_div_ui(lhs.value, lhs.value, _rhs);
        mpz_neg(lhs.value, lhs.value);
    }
    return lhs;
}
inline mpz_class &operator%=(mpz_class &lhs, const signed long int rhs) {
    if (rhs == 0) {
        throw std::invalid_argument("Modulo by zero is undefined");
    }
    unsigned long int abs_rhs = (rhs > 0) ? static_cast<unsigned long int>(rhs) : static_cast<unsigned long int>(-rhs);
    mpz_mod_ui(lhs.value, lhs.value, abs_rhs);
    return lhs;
}
inline mpz_class operator+(const mpz_class &op1, const signed long int op2) {
    mpz_class result(op1);
    result += op2;
    return result;
}
inline mpz_class operator+(const signed long int op1, const mpz_class &op2) { return op2 + op1; }
inline mpz_class operator-(const mpz_class &op1, const signed long int op2) {
    mpz_class result(op1);
    if (op2 >= 0)
        mpz_sub_ui(result.value, op1.value, (unsigned long int)op2);
    else {
        unsigned long int _op2 = (unsigned long int)(-op2);
        mpz_add_ui(result.value, op1.value, _op2);
    }
    return result;
}
inline mpz_class operator-(const signed long int op1, const mpz_class &op2) {
    mpz_class result;
    if (op1 >= 0) {
        mpz_ui_sub(result.value, (unsigned long int)op1, op2.value);
    } else {
        unsigned long int _op1 = (unsigned long int)(-op1);
        mpz_add_ui(result.value, op2.value, _op1);
        mpz_neg(result.value, result.value);
    }
    return result;
}
inline mpz_class operator*(const mpz_class &op1, const signed long int op2) {
    mpz_class result(op1);
    result *= op2;
    return result;
}
inline mpz_class operator*(const signed long int op1, const mpz_class &op2) { return op2 * op1; }
inline mpz_class operator/(const mpz_class &op1, const signed long int op2) {
    mpz_class result = op1;
    result /= op2;
    return result;
}
inline mpz_class operator/(const signed long int op1, const mpz_class &op2) {
    mpz_class lhs(op1);
    mpz_class result;

    if (mpz_sgn(op2.value) == 0) {
        throw std::domain_error("Division by zero");
    }
    mpz_tdiv_q(result.value, lhs.value, op2.value);

    return result;
}
inline mpz_class operator%(const mpz_class &op1, const signed long int op2) {
    if (op2 == 0) {
        throw std::invalid_argument("Modulo by zero is undefined");
    }
    mpz_class result(op1);
    result %= op2;
    return result;
}
inline mpz_class operator%(const signed long int op1, const mpz_class &op2) {
    if (mpz_sgn(op2.value) == 0) {
        throw std::invalid_argument("Modulo by zero is undefined");
    }
    mpz_class lhs(op1);
    lhs %= op2;
    return lhs;
}
inline mpz_class &operator+=(mpz_class &lhs, const signed int rhs) {
    if (rhs >= 0)
        mpz_add_ui(lhs.value, lhs.value, (unsigned int)rhs);
    else {
        unsigned int _rhs = -rhs;
        mpz_sub_ui(lhs.value, lhs.value, _rhs);
    }
    return lhs;
}
inline mpz_class &operator-=(mpz_class &lhs, const signed int rhs) {
    if (rhs >= 0)
        mpz_sub_ui(lhs.value, lhs.value, (unsigned int)rhs);
    else {
        unsigned int _rhs = -rhs;
        mpz_add_ui(lhs.value, lhs.value, _rhs);
    }
    return lhs;
}
inline mpz_class &operator*=(mpz_class &lhs, const signed int rhs) {
    if (rhs >= 0)
        mpz_mul_ui(lhs.value, lhs.value, (unsigned int)rhs);
    else {
        unsigned int _rhs = -rhs;
        mpz_mul_ui(lhs.value, lhs.value, _rhs);
        mpz_neg(lhs.value, lhs.value);
    }
    return lhs;
}
inline mpz_class &operator/=(mpz_class &lhs, const signed int rhs) {
    if (rhs == 0) {
        throw std::invalid_argument("Division by zero is undefined");
    }
    if (rhs > 0)
        mpz_div_ui(lhs.value, lhs.value, (unsigned long int)rhs);
    else {
        unsigned long int _rhs = -rhs;
        mpz_div_ui(lhs.value, lhs.value, _rhs);
        mpz_neg(lhs.value, lhs.value);
    }
    return lhs;
}
inline mpz_class &operator%=(mpz_class &lhs, const signed int rhs) {
    if (rhs == 0) {
        throw std::invalid_argument("Modulo by zero is undefined");
    }
    unsigned long int abs_rhs = (rhs > 0) ? static_cast<unsigned long int>(rhs) : static_cast<unsigned long int>(-rhs);
    mpz_mod_ui(lhs.value, lhs.value, abs_rhs);
    return lhs;
}
inline mpz_class operator+(const mpz_class &op1, const signed int op2) {
    mpz_class result(op1);
    result += op2;
    return result;
}
inline mpz_class operator+(const signed int op1, const mpz_class &op2) { return op2 + op1; }
inline mpz_class operator-(const mpz_class &op1, const signed int op2) {
    mpz_class result(op1);
    if (op2 >= 0)
        mpz_sub_ui(result.value, op1.value, (unsigned int)op2);
    else {
        unsigned int _op2 = (unsigned int)(-op2);
        mpz_add_ui(result.value, op1.value, _op2);
    }
    return result;
}
inline mpz_class operator-(const signed int op1, const mpz_class &op2) {
    mpz_class result(op2);
    if (op1 >= 0)
        mpz_ui_sub(result.value, (unsigned int)op1, op2.value);
    else {
        unsigned int _op1 = (unsigned int)(-op1);
        mpz_add_ui(result.value, op2.value, _op1);
        mpz_neg(result.value, result.value);
    }
    return result;
}
inline mpz_class operator*(const mpz_class &op1, const signed int op2) {
    mpz_class result(op1);
    result *= op2;
    return result;
}
inline mpz_class operator*(const signed int op1, const mpz_class &op2) { return op2 * op1; }
inline mpz_class operator/(const mpz_class &op1, const signed int op2) {
    mpz_class result = op1;
    result /= op2;
    return result;
}
inline mpz_class operator/(const signed int op1, const mpz_class &op2) {
    mpz_class lhs(op1);
    mpz_class result;

    if (mpz_sgn(op2.value) == 0) {
        throw std::domain_error("Division by zero");
    }
    mpz_tdiv_q(result.value, lhs.value, op2.value);

    return result;
}
inline mpz_class operator%(const mpz_class &op1, const signed int op2) {
    if (op2 == 0) {
        throw std::invalid_argument("Modulo by zero is undefined");
    }
    mpz_class result(op1);
    result %= op2;
    return result;
}
inline mpz_class operator%(const signed int op1, const mpz_class &op2) {
    if (mpz_sgn(op2.value) == 0) {
        throw std::invalid_argument("Modulo by zero is undefined");
    }
    mpz_class lhs(op1);
    lhs %= op2;
    return lhs;
}
inline mpz_class &operator+=(mpz_class &lhs, const double rhs) {
    mpz_class temp(rhs);
    mpz_add(lhs.value, lhs.value, temp.value);
    return lhs;
}
inline mpz_class &operator-=(mpz_class &lhs, const double rhs) {
    mpz_class temp(rhs);
    mpz_sub(lhs.value, lhs.value, temp.value);
    return lhs;
}
inline mpz_class &operator*=(mpz_class &lhs, const double rhs) {
    mpz_class temp(rhs);
    mpz_mul(lhs.value, lhs.value, temp.value);
    return lhs;
}
inline mpz_class &operator/=(mpz_class &lhs, const double rhs) {
    if (rhs == 0.0) {
        throw std::invalid_argument("Division by zero is undefined");
    }
    mpz_class temp(rhs);
    mpz_tdiv_q(lhs.value, lhs.value, temp.value);
    return lhs;
}
inline mpz_class operator+(const mpz_class &op1, const double op2) {
    mpz_class result(op1);
    result += op2;
    return result;
}
inline mpz_class operator+(const double op1, const mpz_class &op2) { return op2 + op1; }
inline mpz_class operator-(const mpz_class &op1, const double op2) {
    mpz_class result(op1);
    result -= op2;
    return result;
}
inline mpz_class operator-(const double op1, const mpz_class &op2) {
    mpz_class result(op1);
    mpz_sub(result.value, result.value, op2.value);
    return result;
}
inline mpz_class operator*(const mpz_class &op1, const double op2) {
    mpz_class result(op1);
    result *= op2;
    return result;
}
inline mpz_class operator*(const double op1, const mpz_class &op2) { return op2 * op1; }
inline mpz_class operator/(const mpz_class &op1, const double op2) {
    if (op2 == 0.0) {
        throw std::domain_error("Division by zero");
    }
    mpz_class result(op1);
    result /= op2;
    return result;
}
inline mpz_class operator/(const double op1, const mpz_class &op2) {
    if (mpz_sgn(op2.value) == 0) {
        throw std::domain_error("Division by zero");
    }
    mpz_class lhs(op1);
    mpz_class result;
    mpz_tdiv_q(result.value, lhs.value, op2.value);
    return result;
}
inline mpz_class &operator+=(mpz_class &op1, const mpz_class &op2) {
    mpz_add(op1.value, op1.value, op2.value);
    return op1;
}
inline mpz_class &operator-=(mpz_class &op1, const mpz_class &op2) {
    mpz_sub(op1.value, op1.value, op2.value);
    return op1;
}
inline mpz_class &operator/=(mpz_class &op1, const mpz_class &op2) {
    mpz_tdiv_q(op1.value, op1.value, op2.value);
    return op1;
}
inline mpz_class &operator%=(mpz_class &op1, const mpz_class &op2) {
    mpz_mod(op1.value, op1.value, op2.value);
    return op1;
}
inline mpz_class &operator*=(mpz_class &op1, const mpz_class &op2) {
    mpz_mul(op1.value, op1.value, op2.value);
    return op1;
}
inline mpz_class operator+(const mpz_class &op) { return op; }
inline mpz_class operator-(const mpz_class &op) {
    mpz_class result;
    mpz_neg(result.value, op.value);
    return result;
}
inline mpz_class operator+(const mpz_class &op1, const mpz_class &op2) {
    mpz_class result;
    mpz_add(result.value, op1.value, op2.value);
    return result;
}
inline mpz_class operator-(const mpz_class &op1, const mpz_class &op2) {
    mpz_class result;
    mpz_sub(result.value, op1.value, op2.value);
    return result;
}
inline mpz_class operator*(const mpz_class &op1, const mpz_class &op2) {
    mpz_class result;
    mpz_mul(result.value, op1.value, op2.value);
    return result;
}
inline mpz_class operator/(const mpz_class &op1, const mpz_class &op2) {
    mpz_class result;
    mpz_tdiv_q(result.value, op1.value, op2.value);
    return result;
}
inline mpz_class operator%(const mpz_class &op1, const mpz_class &op2) {
    mpz_class result;
    mpz_mod(result.value, op1.value, op2.value);
    return result;
}
inline mpz_class operator&(const mpz_class &op1, const mpz_class &op2) {
    mpz_class result;
    mpz_and(result.value, op1.value, op2.value);
    return result;
}
inline mpz_class operator|(const mpz_class &op1, const mpz_class &op2) {
    mpz_class result;
    mpz_ior(result.value, op1.value, op2.value);
    return result;
}
inline mpz_class operator^(const mpz_class &op1, const mpz_class &op2) {
    mpz_class result;
    mpz_xor(result.value, op1.value, op2.value);
    return result;
}
inline bool operator==(const mpz_class &lhs, const signed long int rhs) { return lhs.get_si() == rhs; }
inline bool operator==(const signed long int lhs, const mpz_class &rhs) { return rhs == lhs; }
inline bool operator==(const mpz_class &lhs, const unsigned long int rhs) { return lhs.get_ui() == rhs; }
inline bool operator==(const unsigned long int lhs, const mpz_class &rhs) { return rhs == lhs; }
inline bool operator==(const mpz_class &lhs, const signed int rhs) { return lhs.get_si() == rhs; }
inline bool operator==(const signed int lhs, const mpz_class &rhs) { return rhs == lhs; }
inline bool operator==(const mpz_class &lhs, const unsigned int rhs) { return lhs.get_ui() == rhs; }
inline bool operator==(const unsigned int lhs, const mpz_class &rhs) { return rhs == lhs; }
inline mpz_class &mpz_class::operator=(const signed long int op) {
    mpz_set_si(this->value, op);
    return *this;
}
inline mpz_class &mpz_class::operator=(const unsigned long int op) {
    mpz_set_ui(this->value, op);
    return *this;
}
inline mpz_class &mpz_class::operator=(const signed int op) {
    mpz_set_si(this->value, (signed long int)op);
    return *this;
}
inline mpz_class &mpz_class::operator=(const unsigned int op) {
    mpz_set_ui(this->value, (unsigned long int)op);
    return *this;
}
inline mpz_class &mpz_class::operator=(const signed char op) {
    mpz_set_si(this->value, (signed long int)op);
    return *this;
}
inline mpz_class &mpz_class::operator=(const unsigned char op) {
    mpz_set_ui(this->value, (unsigned long int)op);
    return *this;
}
inline mpz_class &mpz_class::operator=(const char op) {
    if (std::is_signed<char>::value)
        return *this = static_cast<signed char>(op);
    else
        return *this = static_cast<unsigned char>(op);
}
inline mpz_class abs(const mpz_class &op) {
    mpz_class result;
    mpz_abs(result.value, op.value);
    return result;
}
inline int sgn(const mpz_class &op) { return mpz_sgn(op.value); }
inline mpz_class sqrt(const mpz_class &op) {
    mpz_class result;
    mpz_sqrt(result.value, op.value);
    return result;
}
inline mpz_class gcd(const mpz_class &op1, const mpz_class &op2) {
    mpz_class result;
    mpz_gcd(result.value, op1.value, op2.value);
    return result;
}
inline mpz_class lcm(const mpz_class &op1, const mpz_class &op2) {
    mpz_class result;
    mpz_lcm(result.value, op1.value, op2.value);
    return result;
}
inline mpz_class factorial(const mpz_class &op) {
    mpz_class result;
    mpz_fac_ui(result.value, op.get_ui());
    return result;
}
inline mpz_class primorial(const mpz_class &op) {
    mpz_class result;
    mpz_primorial_ui(result.value, op.get_ui());
    return result;
}
inline mpz_class fibonacci(const mpz_class &op) {
    mpz_class result;
    mpz_fib_ui(result.value, op.get_ui());
    return result;
}
std::ostream &operator<<(std::ostream &os, const mpz_class &m) {
    std::ios_base::fmtflags flags = os.flags();

    char *str = nullptr;
    if (flags & std::ios::oct) { // Output in octal
        gmp_asprintf(&str, "%Zo", m.get_mpz_t());
    } else if (flags & std::ios::hex) { // Output in hexadecimal
        if (flags & std::ios::uppercase) {
            gmp_asprintf(&str, "%ZX", m.get_mpz_t());
        } else {
            gmp_asprintf(&str, "%Zx", m.get_mpz_t());
        }
    } else { // Default output (decimal)
        gmp_asprintf(&str, "%Zd", m.get_mpz_t());
    }
    os << str;
    free(str);
    return os;
}

class mpq_class {
  public:
    // constructor
    mpq_class() { mpq_init(value); }   // default constructor
    ~mpq_class() { mpq_clear(value); } // The rule 3 of 5 default deconstructor
    explicit mpq_class(const mpq_t q) {
        mpq_init(value);
        mpq_set(value, q);
    }
    mpq_class(const mpq_class &op) { // The rule 1 of 5 copy constructor
                                     // std::cout << "The rule 1 of 5 copy constructor\n" ;
        mpq_init(value);
        mpq_set(value, op.value);
    }
    mpq_class(mpq_class &&op) noexcept { // The rule 4 of 5 move constructor
                                         // std::cout << "The rule 4 of 5 move constructor\n" ;
        mpq_init(value);
        mpq_swap(value, op.value);
    }
    // mpq_class& mpq_class::operator= (type op)
    mpq_class &operator=(const mpq_class &op) noexcept { // The rule 2 of 5 copy assignment operator
        // std::cout << "The rule 2 of 5 copy assignment operator\n" ;
        if (this != &op) {
            mpq_set(value, op.value);
        }
        return *this;
    }
    mpq_class &operator=(mpq_class &&op) noexcept { // The rule 5 of 5 move assignment operator
        // std::cout << "The rule 5 of 5 move assignment operator\n" ;
        if (this != &op) {
            mpq_swap(value, op.value);
        }
        return *this;
    }
    mpq_class(const char *str, int base = 0) {
        mpq_init(value);
        if (mpq_set_str(value, str, base) != 0) {
            throw std::invalid_argument("");
        }
    }
    mpq_class(const std::string &str, int base = 0) {
        mpq_init(value);
        if (mpq_set_str(value, str.c_str(), base) != 0) {
            throw std::invalid_argument("");
        }
    }
    mpq_class(const mpz_t op) {
        mpq_init(value);
        mpq_set_z(value, op);
    }
    mpq_class(const mpf_t op) {
        mpq_init(value);
        mpq_set_f(value, op);
    }
    mpq_class(const mpz_class &op) {
        mpq_init(value);
        mpq_set_z(value, op.get_mpz_t());
    }
    mpq_class(const mpz_class &op1, const mpz_class &op2) {
        mpq_init(value);
        mpq_set_num(value, op1.get_mpz_t());
        mpq_set_den(value, op2.get_mpz_t());
        if (op2 == 0) {
            mpq_clear(value);
            throw std::invalid_argument("Denominator cannot be zero in a rational number.");
        }
        mpq_canonicalize(value);
    }
    mpq_class(unsigned long int op1, unsigned long int op2) {
        mpq_init(value);
        mpq_set_ui(value, op1, op2);
    }
    mpq_class(signed long int op1, signed long int op2) {
        mpq_init(value);
        mpq_set_si(value, op1, op2);
    }
    mpq_class(int op1, int op2) {
        mpq_init(value);
        mpq_set_si(value, (signed long int)op1, (signed long int)op2);
    }
    mpq_class(unsigned int op1, unsigned int op2) {
        mpq_init(value);
        mpq_set_ui(value, (unsigned long int)op1, (unsigned long int)op2);
    }
    mpq_class(double op) {
        mpq_init(value);
        mpq_set_d(value, op);
    }
    inline friend mpq_class &operator+=(mpq_class &lhs, const mpq_class &rhs);
    inline friend mpq_class &operator-=(mpq_class &lhs, const mpq_class &rhs);
    inline friend mpq_class &operator*=(mpq_class &lhs, const mpq_class &rhs);
    inline friend mpq_class &operator/=(mpq_class &lhs, const mpq_class &rhs);
    inline friend mpq_class operator+(const mpq_class &op);
    inline friend mpq_class operator-(const mpq_class &op);
    inline friend mpq_class operator+(const mpq_class &op1, const mpq_class &op2);
    inline friend mpq_class operator-(const mpq_class &op1, const mpq_class &op2);
    inline friend mpq_class operator*(const mpq_class &op1, const mpq_class &op2);
    inline friend mpq_class operator/(const mpq_class &op1, const mpq_class &op2);

    inline friend bool operator==(const mpq_class &op1, const mpq_class &op2) { return mpq_cmp(op1.value, op2.value) == 0; }
    inline friend bool operator!=(const mpq_class &op1, const mpq_class &op2) { return mpq_cmp(op1.value, op2.value) != 0; }
    inline friend bool operator<(const mpq_class &op1, const mpq_class &op2) { return mpq_cmp(op1.value, op2.value) < 0; }
    inline friend bool operator>(const mpq_class &op1, const mpq_class &op2) { return mpq_cmp(op1.value, op2.value) > 0; }
    inline friend bool operator<=(const mpq_class &op1, const mpq_class &op2) { return mpq_cmp(op1.value, op2.value) <= 0; }
    inline friend bool operator>=(const mpq_class &op1, const mpq_class &op2) { return mpq_cmp(op1.value, op2.value) >= 0; }

    inline mpq_class &operator=(const mpz_class &op);
    inline mpq_class &operator=(const signed long int op);
    inline mpq_class &operator=(const unsigned long int op);
    inline mpq_class &operator=(const signed int op);
    inline mpq_class &operator=(const unsigned int op);
    inline mpq_class &operator=(const signed char op);
    inline mpq_class &operator=(const unsigned char op);
    inline mpq_class &operator=(const char op);
    inline mpq_class &operator=(const float op);
    inline mpq_class &operator=(const double op);
    inline mpq_class &operator=(const char *op);
    inline mpq_class &operator=(const std::string &op);

    inline mpq_class operator<<(const mp_bitcnt_t shift) const {
        mpq_class result;
        mpq_mul_2exp(result.value, value, shift);
        return result;
    }
    // void mpq_class::canonicalize ()
    // mpq_class abs (mpq_class op)
    // double mpq_class::get_d (void)
    void canonicalize() { mpq_canonicalize(value); }
    friend mpq_class abs(const mpq_class &op);
    double get_d() const { return mpq_get_d(value); }

    // string mpq_class::get_str (int base = 10)
    // int mpq_class::set_str (const char *str, int base)
    // int mpq_class::set_str (const string& str, int base)
    std::string get_str(int base = 10) const {
        char *str = mpq_get_str(NULL, base, value);
        std::string result(str);
        void (*freefunc)(void *, size_t);
        mp_get_memory_functions(NULL, NULL, &freefunc);
        freefunc(str, std::strlen(str) + 1);
        return result;
    }
    int set_str(const char *str, int base = 10) {
        int ret = mpq_set_str(value, str, base);
        if (ret == 0) {
            mpq_canonicalize(value);
        }
        return ret;
    }

    int set_str(const std::string &str, int base = 10) { return set_str(str.c_str(), base); }
    // int sgn (mpq_class op)
    // void mpq_class::swap (mpq_class& op)
    // void swap (mpq_class& op1, mpq_class& op2)
    void swap(mpq_class &op) { mpq_swap(this->value, op.value); }
    friend int sgn(const mpq_class &op) { return mpq_sgn(op.value); }
#if !defined ___GMPXX_STRICT_COMPATIBILITY___
    friend void swap(mpq_class &op1, mpq_class &op2) { mpq_swap(op1.value, op2.value); }
#endif
    inline friend mpq_class &operator+=(mpq_class &lhs, const unsigned long int rhs);
    inline friend mpq_class &operator-=(mpq_class &lhs, const unsigned long int rhs);
    inline friend mpq_class &operator*=(mpq_class &lhs, const unsigned long int rhs);
    inline friend mpq_class &operator/=(mpq_class &lhs, const unsigned long int rhs);
    inline friend mpq_class operator+(const mpq_class &op1, const unsigned long int op2);
    inline friend mpq_class operator+(const unsigned long int op1, const mpq_class &op2);
    inline friend mpq_class operator-(const mpq_class &op1, const unsigned long int op2);
    inline friend mpq_class operator-(const unsigned long int op1, const mpq_class &op2);
    inline friend mpq_class operator*(const mpq_class &op1, const unsigned long int op2);
    inline friend mpq_class operator*(const unsigned long int op1, const mpq_class &op2);
    inline friend mpq_class operator/(const mpq_class &op1, const unsigned long int op2);
    inline friend mpq_class operator/(const unsigned long int op1, const mpq_class &op2);

    inline friend mpq_class &operator+=(mpq_class &lhs, const unsigned long int rhs);
    inline friend mpq_class &operator-=(mpq_class &lhs, const unsigned long int rhs);
    inline friend mpq_class &operator*=(mpq_class &lhs, const unsigned long int rhs);
    inline friend mpq_class &operator/=(mpq_class &lhs, const unsigned long int rhs);
    inline friend mpq_class operator+(const mpq_class &op1, const unsigned long int op2);
    inline friend mpq_class operator+(const unsigned long int op1, const mpq_class &op2);
    inline friend mpq_class operator-(const mpq_class &op1, const unsigned long int op2);
    inline friend mpq_class operator-(const unsigned long int op1, const mpq_class &op2);
    inline friend mpq_class operator*(const mpq_class &op1, const unsigned long int op2);
    inline friend mpq_class operator*(const unsigned long int op1, const mpq_class &op2);
    inline friend mpq_class operator/(const mpq_class &op1, const unsigned long int op2);
    inline friend mpq_class operator/(const unsigned long int op1, const mpq_class &op2);

    inline friend mpq_class &operator+=(mpq_class &lhs, const signed long int rhs);
    inline friend mpq_class &operator-=(mpq_class &lhs, const signed long int rhs);
    inline friend mpq_class &operator*=(mpq_class &lhs, const signed long int rhs);
    inline friend mpq_class &operator/=(mpq_class &lhs, const signed long int rhs);
    inline friend mpq_class operator+(const mpq_class &op1, const signed long int op2);
    inline friend mpq_class operator+(const signed long int op1, const mpq_class &op2);
    inline friend mpq_class operator-(const mpq_class &op1, const signed long int op2);
    inline friend mpq_class operator-(const signed long int op1, const mpq_class &op2);
    inline friend mpq_class operator*(const mpq_class &op1, const signed long int op2);
    inline friend mpq_class operator*(const signed long int op1, const mpq_class &op2);
    inline friend mpq_class operator/(const mpq_class &op1, const signed long int op2);
    inline friend mpq_class operator/(const signed long int op1, const mpq_class &op2);

    inline friend mpq_class &operator+=(mpq_class &lhs, const unsigned int rhs);
    inline friend mpq_class &operator-=(mpq_class &lhs, const unsigned int rhs);
    inline friend mpq_class &operator*=(mpq_class &lhs, const unsigned int rhs);
    inline friend mpq_class &operator/=(mpq_class &lhs, const unsigned int rhs);
    inline friend mpq_class operator+(const mpq_class &op1, const unsigned int op2);
    inline friend mpq_class operator+(const unsigned int op1, const mpq_class &op2);
    inline friend mpq_class operator-(const mpq_class &op1, const unsigned int op2);
    inline friend mpq_class operator-(const unsigned int op1, const mpq_class &op2);
    inline friend mpq_class operator*(const mpq_class &op1, const unsigned int op2);
    inline friend mpq_class operator*(const unsigned int op1, const mpq_class &op2);
    inline friend mpq_class operator/(const mpq_class &op1, const unsigned int op2);
    inline friend mpq_class operator/(const unsigned int op1, const mpq_class &op2);

    inline friend mpq_class &operator+=(mpq_class &lhs, const signed int rhs);
    inline friend mpq_class &operator-=(mpq_class &lhs, const signed int rhs);
    inline friend mpq_class &operator*=(mpq_class &lhs, const signed int rhs);
    inline friend mpq_class &operator/=(mpq_class &lhs, const signed int rhs);
    inline friend mpq_class operator+(const mpq_class &op1, const signed int op2);
    inline friend mpq_class operator+(const signed int op1, const mpq_class &op2);
    inline friend mpq_class operator-(const mpq_class &op1, const signed int op2);
    inline friend mpq_class operator-(const signed int op1, const mpq_class &op2);
    inline friend mpq_class operator*(const mpq_class &op1, const signed int op2);
    inline friend mpq_class operator*(const signed int op1, const mpq_class &op2);
    inline friend mpq_class operator/(const mpq_class &op1, const signed int op2);
    inline friend mpq_class operator/(const signed int op1, const mpq_class &op2);

    inline friend mpq_class &operator+=(mpq_class &lhs, const mpz_class &rhs);
    inline friend mpq_class &operator-=(mpq_class &lhs, const mpz_class &rhs);
    inline friend mpq_class &operator*=(mpq_class &lhs, const mpz_class &rhs);
    inline friend mpq_class &operator/=(mpq_class &lhs, const mpz_class &rhs);
    inline friend mpq_class operator+(const mpq_class &op1, const mpz_class &op2);
    inline friend mpq_class operator+(const mpz_class &op1, const mpq_class &op2);
    inline friend mpq_class operator-(const mpq_class &op1, const mpz_class &op2);
    inline friend mpq_class operator-(const mpz_class &op1, const mpq_class &op2);
    inline friend mpq_class operator*(const mpq_class &op1, const mpz_class &op2);
    inline friend mpq_class operator*(const mpz_class &op1, const mpq_class &op2);
    inline friend mpq_class operator/(const mpq_class &op1, const mpz_class &op2);
    inline friend mpq_class operator/(const mpz_class &op1, const mpq_class &op2);

    inline friend mpq_class &operator+=(mpq_class &lhs, const double rhs);
    inline friend mpq_class &operator-=(mpq_class &lhs, const double rhs);
    inline friend mpq_class &operator*=(mpq_class &lhs, const double rhs);
    inline friend mpq_class &operator/=(mpq_class &lhs, const double rhs);
    inline friend mpq_class operator+(const mpq_class &op1, const double op2);
    inline friend mpq_class operator+(const double op1, const mpq_class &op2);
    inline friend mpq_class operator-(const mpq_class &op1, const double op2);
    inline friend mpq_class operator-(const double op1, const mpq_class &op2);
    inline friend mpq_class operator*(const mpq_class &op1, const double op2);
    inline friend mpq_class operator*(const double op1, const mpq_class &op2);
    inline friend mpq_class operator/(const mpq_class &op1, const double op2);
    inline friend mpq_class operator/(const double op1, const mpq_class &op2);

    // mpz_class& mpq_class::get_num ()
    // mpz_class& mpq_class::get_den ()
    // mpz_t mpq_class::get_num_mpz_t ()
    // mpz_t mpq_class::get_den_mpz_t ()
    mpz_class get_num() const {
        mpz_class num(mpq_numref(value));
        return num;
    }
    mpz_class get_den() const {
        mpz_class den(mpq_denref(value));
        return den;
    }
    mpz_ptr get_num_mpz_t() { return mpq_numref(value); }
    mpz_ptr get_den_mpz_t() { return mpq_denref(value); }

    // istream& operator>> (istream& stream, mpq_class& rop)
    friend std::ostream &operator<<(std::ostream &os, const mpq_class &m);
    friend std::istream &operator>>(std::istream &stream, mpq_class &rop);

    operator mpf_class() const;
    operator mpz_class() const;
    mpq_srcptr get_mpq_t() const { return value; }

  private:
    mpq_t value;
};
std::ostream &operator<<(std::ostream &os, const mpq_class &m) {
    std::ios_base::fmtflags flags = os.flags();

    char *str = nullptr;
    if (flags & std::ios::oct) { // Output in octal
        gmp_asprintf(&str, "%Qo", m.get_mpq_t());
    } else if (flags & std::ios::hex) { // Output in hexadecimal
        if (flags & std::ios::uppercase) {
            gmp_asprintf(&str, "%QX", m.get_mpq_t());
        } else {
            gmp_asprintf(&str, "%Qx", m.get_mpq_t());
        }
    } else { // Default output (decimal)
        gmp_asprintf(&str, "%Qd", m.get_mpq_t());
    }
    os << str;
    free(str);
    return os;
}
std::istream &operator>>(std::istream &stream, mpq_class &rop) {
    std::string input;
    std::getline(stream, input);
    if (input.empty() || rop.set_str(input, 10) != 0) {
        stream.setstate(std::ios::failbit);
    }
    return stream;
}
inline mpq_class &operator+=(mpq_class &op1, const mpq_class &op2) {
    mpq_add(op1.value, op1.value, op2.value);
    return op1;
}
inline mpq_class &operator-=(mpq_class &op1, const mpq_class &op2) {
    mpq_sub(op1.value, op1.value, op2.value);
    return op1;
}
inline mpq_class &operator/=(mpq_class &op1, const mpq_class &op2) {
    mpq_div(op1.value, op1.value, op2.value);
    return op1;
}
inline mpq_class &operator*=(mpq_class &op1, const mpq_class &op2) {
    mpq_mul(op1.value, op1.value, op2.value);
    return op1;
}
inline mpq_class operator+(const mpq_class &op) { return op; }
inline mpq_class operator-(const mpq_class &op) {
    mpq_class result;
    mpq_neg(result.value, op.value);
    return result;
}
inline mpq_class operator+(const mpq_class &op1, const mpq_class &op2) {
    mpq_class result;
    mpq_add(result.value, op1.value, op2.value);
    return result;
}
inline mpq_class operator-(const mpq_class &op1, const mpq_class &op2) {
    mpq_class result;
    mpq_sub(result.value, op1.value, op2.value);
    return result;
}
inline mpq_class operator*(const mpq_class &op1, const mpq_class &op2) {
    mpq_class result;
    mpq_mul(result.value, op1.value, op2.value);
    return result;
}
inline mpq_class operator/(const mpq_class &op1, const mpq_class &op2) {
    mpq_class result;
    mpq_div(result.value, op1.value, op2.value);
    return result;
}

inline mpq_class abs(const mpq_class &op) {
    mpq_class rop(op);
    mpq_abs(rop.value, op.get_mpq_t());
    return rop;
}
inline bool operator==(const mpq_class &lhs, double rhs) { return lhs.get_d() == rhs; }
inline bool operator==(double lhs, const mpq_class &rhs) { return lhs == rhs.get_d(); }
inline bool operator==(const mpq_class &lhs, signed long int rhs) { return mpq_cmp_si(lhs.get_mpq_t(), rhs, (signed long int)1) == 0; }
inline bool operator==(signed long int lhs, const mpq_class &rhs) { return rhs == lhs; }
inline bool operator==(const mpq_class &lhs, unsigned long int rhs) { return mpq_cmp_ui(lhs.get_mpq_t(), rhs, (unsigned long int)1) == 0; }
inline bool operator==(unsigned long int lhs, const mpq_class &rhs) { return rhs == lhs; }
inline bool operator==(const mpq_class &lhs, signed int rhs) { return mpq_cmp_si(lhs.get_mpq_t(), (signed long int)rhs, (signed long int)1) == 0; }
inline bool operator==(signed int lhs, const mpq_class &rhs) { return rhs == lhs; }
inline bool operator==(const mpq_class &lhs, unsigned int rhs) { return mpq_cmp_ui(lhs.get_mpq_t(), (unsigned long int)rhs, (unsigned long int)1) == 0; }
inline bool operator==(unsigned int lhs, const mpq_class &rhs) { return rhs == lhs; }
inline bool operator==(const mpq_class &lhs, signed char rhs) { return mpq_cmp_si(lhs.get_mpq_t(), (signed long int)rhs, (signed long int)1) == 0; }
inline bool operator==(signed char lhs, const mpq_class &rhs) { return rhs == lhs; }
inline bool operator==(const mpq_class &lhs, unsigned char rhs) { return mpq_cmp_ui(lhs.get_mpq_t(), (unsigned long int)rhs, (unsigned long int)1) == 0; }
inline bool operator==(unsigned char lhs, const mpq_class &rhs) { return rhs == lhs; }

inline mpq_class &mpq_class::operator=(const mpz_class &op) {
    mpq_set_z(this->value, op.get_mpz_t());
    return *this;
}
inline mpq_class &mpq_class::operator=(signed long int op) {
    mpq_set_si(this->value, op, (signed long int)1);
    return *this;
}
inline mpq_class &mpq_class::operator=(unsigned long int op) {
    mpq_set_ui(this->value, op, (unsigned long int)1);
    return *this;
}
inline mpq_class &mpq_class::operator=(signed int op) {
    mpq_set_si(this->value, (signed long int)op, (signed long int)1);
    return *this;
}
inline mpq_class &mpq_class::operator=(unsigned int op) {
    mpq_set_ui(this->value, (unsigned long int)op, (unsigned long int)1);
    return *this;
}
inline mpq_class &mpq_class::operator=(signed char op) {
    mpq_set_si(this->value, (signed long int)op, (signed long int)1);
    return *this;
}
inline mpq_class &mpq_class::operator=(unsigned char op) {
    mpq_set_ui(this->value, (unsigned long int)op, (unsigned long int)1);
    return *this;
}
inline mpq_class &mpq_class::operator=(char op) {
    if (std::is_signed<char>::value)
        return *this = static_cast<signed char>(op);
    else
        return *this = static_cast<unsigned char>(op);
}
inline mpq_class &mpq_class::operator=(float op) {
    mpq_set_d(this->value, (double)op);
    return *this;
}
inline mpq_class &mpq_class::operator=(double op) {
    mpq_set_d(this->value, op);
    return *this;
}
inline mpq_class &mpq_class::operator=(const char *op) {
    if (mpq_set_str(value, op, 10) != 0) {
        throw std::invalid_argument("Invalid string format for mpq_class");
    }
    return *this;
}
inline mpq_class &mpq_class::operator=(const std::string &op) {
    if (mpq_set_str(value, op.c_str(), 10) != 0) {
        throw std::invalid_argument("Invalid string format for mpq_class");
    }
    return *this;
}
inline mpq_class &operator+=(mpq_class &lhs, const mpz_class &rhs) {
    mpq_class result(rhs);
    mpq_add(lhs.value, lhs.value, result.value);
    return lhs;
}
inline mpq_class &operator-=(mpq_class &lhs, const mpz_class &rhs) {
    mpq_class result(rhs);
    mpq_sub(lhs.value, lhs.value, result.value);
    return lhs;
}
inline mpq_class &operator*=(mpq_class &lhs, const mpz_class &rhs) {
    mpq_class result(rhs);
    mpq_mul(lhs.value, lhs.value, result.value);
    return lhs;
}
inline mpq_class &operator/=(mpq_class &lhs, const mpz_class &rhs) {
    if (rhs == 0)
        throw std::runtime_error("Division by zero");
    mpq_class op(rhs);
    mpq_div(lhs.value, lhs.value, op.value);
    return lhs;
}
inline mpq_class operator+(const mpq_class &op1, const mpz_class &op2) {
    mpq_class result(op1);
    result += op2;
    return result;
}
inline mpq_class operator+(const mpz_class &op1, const mpq_class &op2) { return op2 + op1; }
inline mpq_class operator-(const mpq_class &op1, const mpz_class &op2) {
    mpq_class result(op1);
    result -= op2;
    return result;
}
inline mpq_class operator-(const mpz_class &op1, const mpq_class &op2) { return -(op2 - op1); }
inline mpq_class operator*(const mpq_class &op1, const mpz_class &op2) {
    mpq_class result(op1);
    result *= op2;
    return result;
}
inline mpq_class operator*(const mpz_class &op1, const mpq_class &op2) { return op2 * op1; }
inline mpq_class operator/(const mpq_class &op1, const mpz_class &op2) {
    mpq_class result(op1);
    result /= op2;
    return result;
}
inline mpq_class operator/(const mpz_class &op1, const mpq_class &op2) {
    mpq_class result(op1, (unsigned long int)1);
    result /= op2;
    return result;
}
inline mpq_class &operator+=(mpq_class &lhs, const double rhs) {
    mpq_class result(rhs);
    mpq_add(lhs.value, lhs.value, result.value);
    return lhs;
}
inline mpq_class &operator-=(mpq_class &lhs, const double rhs) {
    mpq_class result(rhs);
    mpq_sub(lhs.value, lhs.value, result.value);
    return lhs;
}
inline mpq_class &operator*=(mpq_class &lhs, const double rhs) {
    mpq_class result(rhs);
    mpq_mul(lhs.value, lhs.value, result.value);
    return lhs;
}
inline mpq_class &operator/=(mpq_class &lhs, const double rhs) {
    if (rhs == 0)
        throw std::runtime_error("Division by zero");
    mpq_class op(rhs);
    mpq_div(lhs.value, lhs.value, op.value);
    return lhs;
}
inline mpq_class operator+(const mpq_class &op1, const double op2) {
    mpq_class result(op1);
    result += op2;
    return result;
}
inline mpq_class operator+(const double op1, const mpq_class &op2) { return op2 + op1; }
inline mpq_class operator-(const mpq_class &op1, const double op2) {
    mpq_class result(op1);
    result -= op2;
    return result;
}
inline mpq_class operator-(const double op1, const mpq_class &op2) { return -(op2 - op1); }
inline mpq_class operator*(const mpq_class &op1, const double op2) {
    mpq_class result(op1);
    result *= op2;
    return result;
}
inline mpq_class operator*(const double op1, const mpq_class &op2) { return op2 * op1; }
inline mpq_class operator/(const mpq_class &op1, const double op2) {
    mpq_class result(op1);
    result /= op2;
    return result;
}
inline mpq_class operator/(const double op1, const mpq_class &op2) {
    mpq_class result(op1, (unsigned long int)1);
    result /= op2;
    return result;
}
inline mpq_class &operator+=(mpq_class &lhs, const unsigned long int rhs) {
    mpq_class result(rhs, (unsigned long int)1);
    mpq_add(lhs.value, lhs.value, result.value);
    return lhs;
}
inline mpq_class &operator-=(mpq_class &lhs, const unsigned long int rhs) {
    mpq_class result(rhs, (unsigned long int)1);
    mpq_sub(lhs.value, lhs.value, result.value);
    return lhs;
}
inline mpq_class &operator*=(mpq_class &lhs, const unsigned long int rhs) {
    mpq_class result(rhs, (unsigned long int)1);
    mpq_mul(result.value, lhs.value, result.value);
    return lhs;
}
inline mpq_class &operator/=(mpq_class &lhs, const unsigned long int rhs) {
    if (rhs == 0)
        throw std::runtime_error("Division by zero");
    mpq_class result(rhs, (unsigned long int)1);
    mpq_mul(lhs.value, lhs.value, result.value);
    return lhs;
}
inline mpq_class operator+(const mpq_class &op1, const unsigned long int op2) {
    mpq_class result(op1);
    result += op2;
    return result;
}
inline mpq_class operator+(const unsigned long int op1, const mpq_class &op2) { return op2 + op1; }
inline mpq_class operator-(const mpq_class &op1, const unsigned long int op2) {
    mpq_class result(op1);
    result -= op2;
    return result;
}
inline mpq_class operator-(const unsigned long int op1, const mpq_class &op2) { return -(op2 - op1); }
inline mpq_class operator*(const mpq_class &op1, const unsigned long int op2) {
    mpq_class result(op1);
    result *= op2;
    return result;
}
inline mpq_class operator*(const unsigned long int op1, const mpq_class &op2) { return op2 * op1; }
inline mpq_class operator/(const mpq_class &op1, const unsigned long int op2) {
    mpq_class result(op1);
    result /= op2;
    return result;
}
inline mpq_class operator/(const unsigned long int op1, const mpq_class &op2) {
    mpq_class result(op1, (unsigned long int)1);
    result /= op2;
    return result;
}
inline mpq_class &operator+=(mpq_class &lhs, const signed long int rhs) {
    mpq_class result(rhs, (signed long int)1);
    mpq_add(lhs.value, lhs.value, result.value);
    return lhs;
}
inline mpq_class &operator-=(mpq_class &lhs, const signed long int rhs) {
    mpq_class result(rhs, (signed long int)1);
    mpq_sub(lhs.value, lhs.value, result.value);
    return lhs;
}
inline mpq_class &operator*=(mpq_class &lhs, const signed long int rhs) {
    mpq_class result(rhs, (signed long int)1);
    mpq_mul(lhs.value, lhs.value, result.value);
    return lhs;
}
inline mpq_class &operator/=(mpq_class &lhs, const signed long int rhs) {
    if (rhs == 0)
        throw std::runtime_error("Division by zero");
    mpq_class result(rhs, (signed long int)1);
    mpq_mul(lhs.value, lhs.value, result.value);
    return lhs;
}
inline mpq_class operator+(const mpq_class &op1, const signed long int op2) {
    mpq_class result(op1);
    result += op2;
    return result;
}
inline mpq_class operator+(const signed long int op1, const mpq_class &op2) { return op2 + op1; }
inline mpq_class operator-(const mpq_class &op1, const signed long int op2) {
    mpq_class result(op1);
    result -= op2;
    return result;
}
inline mpq_class operator-(const signed long int op1, const mpq_class &op2) { return -(op2 - op1); }
inline mpq_class operator*(const mpq_class &op1, const signed long int op2) {
    mpq_class result(op1);
    result *= op2;
    return result;
}
inline mpq_class operator*(const signed long int op1, const mpq_class &op2) { return op2 * op1; }
inline mpq_class operator/(const mpq_class &op1, const signed long int op2) {
    mpq_class result(op1);
    result /= op2;
    return result;
}
inline mpq_class operator/(const signed long int op1, const mpq_class &op2) {
    mpq_class result(op1, (signed long int)1);
    result /= op2;
    return result;
}
inline mpq_class &operator+=(mpq_class &lhs, const unsigned int rhs) {
    mpq_class result(rhs, (unsigned int)1);
    mpq_add(lhs.value, lhs.value, result.value);
    return lhs;
}
inline mpq_class &operator-=(mpq_class &lhs, const unsigned int rhs) {
    mpq_class result(rhs, (unsigned int)1);
    mpq_sub(lhs.value, lhs.value, result.value);
    return lhs;
}
inline mpq_class &operator*=(mpq_class &lhs, const unsigned int rhs) {
    mpq_class result(rhs, (unsigned int)1);
    mpq_mul(lhs.value, lhs.value, result.value);
    return lhs;
}
inline mpq_class &operator/=(mpq_class &lhs, const unsigned int rhs) {
    if (rhs == 0)
        throw std::runtime_error("Division by zero");
    mpq_class result(rhs, (unsigned int)1);
    mpq_mul(lhs.value, lhs.value, result.value);
    return lhs;
}
inline mpq_class operator+(const mpq_class &op1, const unsigned int op2) {
    mpq_class result(op1);
    result += op2;
    return result;
}
inline mpq_class operator+(const unsigned int op1, const mpq_class &op2) { return op2 + op1; }
inline mpq_class operator-(const mpq_class &op1, const unsigned int op2) {
    mpq_class result(op1);
    result -= op2;
    return result;
}
inline mpq_class operator-(const unsigned int op1, const mpq_class &op2) { return -(op2 - op1); }
inline mpq_class operator*(const mpq_class &op1, const unsigned int op2) {
    mpq_class result(op1);
    result *= op2;
    return result;
}
inline mpq_class operator*(const unsigned int op1, const mpq_class &op2) { return op2 * op1; }
inline mpq_class operator/(const mpq_class &op1, const unsigned int op2) {
    mpq_class result(op1);
    result /= op2;
    return result;
}
inline mpq_class operator/(const unsigned int op1, const mpq_class &op2) {
    mpq_class result(op1, (unsigned int)1);
    result /= op2;
    return result;
}
inline mpq_class &operator+=(mpq_class &lhs, const signed int rhs) {
    mpq_class result(rhs, (signed int)1);
    mpq_add(lhs.value, lhs.value, result.value);
    return lhs;
}
inline mpq_class &operator-=(mpq_class &lhs, const signed int rhs) {
    mpq_class result(rhs, (signed int)1);
    mpq_sub(lhs.value, lhs.value, result.value);
    return lhs;
}
inline mpq_class &operator*=(mpq_class &lhs, const signed int rhs) {
    mpq_class result(rhs, (signed int)1);
    mpq_mul(lhs.value, lhs.value, result.value);
    return lhs;
}
inline mpq_class &operator/=(mpq_class &lhs, const signed int rhs) {
    if (rhs == 0)
        throw std::runtime_error("Division by zero");
    mpq_class result(rhs, (signed int)1);
    mpq_mul(lhs.value, lhs.value, result.value);
    return lhs;
}
inline mpq_class operator+(const mpq_class &op1, const signed int op2) {
    mpq_class result(op1);
    result += (signed int)op2;
    return result;
}
inline mpq_class operator+(const signed int op1, const mpq_class &op2) { return op2 + op1; }
inline mpq_class operator-(const mpq_class &op1, const signed int op2) {
    mpq_class result(op1);
    result -= (signed int)op2;
    return result;
}
inline mpq_class operator-(const signed int op1, const mpq_class &op2) { return -(op2 - op1); }
inline mpq_class operator*(const mpq_class &op1, const signed int op2) {
    mpq_class result(op1);
    result *= (signed int)op2;
    return result;
}
inline mpq_class operator*(const signed int op1, const mpq_class &op2) { return op2 * op1; }
inline mpq_class operator/(const mpq_class &op1, const signed int op2) {
    mpq_class result(op1);
    result /= (signed int)op2;
    return result;
}
inline mpq_class operator/(const signed int op1, const mpq_class &op2) {
    mpq_class result(op1, (signed int)1);
    result /= op2;
    return result;
}

class mpf_class {
  public:
    ////////////////////////////////////////////////////////////////////////////////////////
    // 12.4 C++ Interface Floats
    ////////////////////////////////////////////////////////////////////////////////////////
    // constructor
    mpf_class() { mpf_init(value); }   // default constructor
    ~mpf_class() { mpf_clear(value); } // The rule 3 of 5 default deconstructor
    // explicit mpf_class::mpf_class (const mpf_t f)
    explicit mpf_class(const mpf_t op) {
        mpf_init2(value, mpf_get_prec(op));
        mpf_set(value, op);
    }
    //  mpf_class::mpf_class (const mpf_t f, mp_bitcnt_t prec)
    mpf_class(const mpf_t f, mp_bitcnt_t prec) {
        mpf_init2(value, prec);
        mpf_set(value, f);
    }
    // mpf_class::mpf_class (type op)
    mpf_class(const mpf_class &op) { // The rule 1 of 5 copy constructor
                                     // std::cout << "The rule 1 of 5 copy constructor\n" ;
        mpf_init2(value, mpf_get_prec(op.value));
        mpf_set(value, op.value);
    }
    mpf_class(mpf_class &&op) noexcept { // The rule 4 of 5 move constructor
                                         // std::cout << "The rule 4 of 5 move constructor\n" ;
        mpf_init(value);
        mpf_swap(value, op.value);
    }
    mpf_class(const mpz_t op) {
        mpf_init(value);
        mpf_set_z(value, op);
    }
    mpf_class(const mpq_t op) {
        mpf_init(value);
        mpf_set_q(value, op);
    }
    // mpf_class::mpf_class (type op, mp_bitcnt_t prec)
    mpf_class(const mpf_class &op, mp_bitcnt_t prec) {
        mpf_init2(value, prec);
        mpf_set(value, op.value);
    }
    ___MPF_CLASS_EXPLICIT___ mpf_class(unsigned long int op, mp_bitcnt_t prec = gmpxx_defaults::get_default_prec()) noexcept {
        mpf_init2(value, prec);
        mpf_set_ui(value, op);
    }
    ___MPF_CLASS_EXPLICIT___ mpf_class(unsigned int op, mp_bitcnt_t prec = gmpxx_defaults::get_default_prec()) noexcept {
        mpf_init2(value, prec);
        mpf_set_ui(value, (unsigned long)op);
    }
    ___MPF_CLASS_EXPLICIT___ mpf_class(signed long int op, mp_bitcnt_t prec = gmpxx_defaults::get_default_prec()) noexcept {
        mpf_init2(value, prec);
        mpf_set_si(value, op);
    }
    ___MPF_CLASS_EXPLICIT___ mpf_class(signed int op, mp_bitcnt_t prec = gmpxx_defaults::get_default_prec()) noexcept {
        mpf_init2(value, prec);
        mpf_set_si(value, (signed long)op);
    }
    ___MPF_CLASS_EXPLICIT___ mpf_class(double op, mp_bitcnt_t prec = gmpxx_defaults::get_default_prec()) noexcept {
        mpf_init2(value, prec);
        mpf_set_d(value, op);
    }
    // explicit mpf_class::mpf_class (const char *s)
    // mpf_class::mpf_class (const char *s, mp_bitcnt_t prec, int base = 0)
    // explicit mpf_class::mpf_class (const string& s)
    // mpf_class::mpf_class (const string& s, mp_bitcnt_t prec, int base = 0)
    mpf_class(const char *str) {
        mpf_init(value);
        if (mpf_set_str(value, str, gmpxx_defaults::base) != 0) {
            throw std::invalid_argument("");
        }
    }
    mpf_class(const char *str, mp_bitcnt_t prec, int base = gmpxx_defaults::base) {
        mpf_init2(value, prec);
        if (mpf_set_str(value, str, base) != 0) {
            throw std::invalid_argument("");
        }
    }
    mpf_class(const std::string &str) {
        mpf_init(value);
        if (mpf_set_str(value, str.c_str(), gmpxx_defaults::base) != 0) {
            throw std::invalid_argument("");
        }
    }
    mpf_class(const std::string &str, mp_bitcnt_t prec, int base = gmpxx_defaults::base) {
        mpf_init2(value, prec);
        if (mpf_set_str(value, str.c_str(), base) != 0) {
            throw std::invalid_argument("");
        }
    }
    // mpf_class abs (mpf_class op)
    // mpf_class ceil (mpf_class op)
    friend mpf_class abs(const mpf_class &op);
    friend mpf_class ceil(const mpf_class &op);

    // bool mpf_class::fits_sint_p (void)
    // bool mpf_class::fits_slong_p (void)
    // bool mpf_class::fits_sshort_p (void)
    // bool mpf_class::fits_uint_p (void)
    // bool mpf_class::fits_ulong_p (void)
    // bool mpf_class::fits_ushort_p (void)
    bool fits_sint_p() const { return mpf_fits_sint_p(value); }
    bool fits_slong_p() const { return mpf_fits_slong_p(value); }
    bool fits_sshort_p() const { return mpf_fits_sshort_p(value); }
    bool fits_uint_p() const { return mpf_fits_uint_p(value); }
    bool fits_ulong_p() const { return mpf_fits_ulong_p(value); }
    bool fits_ushort_p() const { return mpf_fits_ushort_p(value); }

    // mpf_class floor (mpf_class op)
    // mpf_class hypot (mpf_class op1, mpf_class op2)
    friend mpf_class floor(const mpf_class &op);
    friend mpf_class hypot(const mpf_class &op1, const mpf_class &op2);

    // double mpf_class::get_d (void)
    // long mpf_class::get_si (void)
    // unsigned long mpf_class::get_ui (void)
    // string mpf_class::get_str (mp_exp_t& exp, int base = 10, size_t digits = 0)
    double get_d() const { return mpf_get_d(value); }
    unsigned long get_ui() const { return mpf_get_ui(value); }
    long get_si() const { return mpf_get_si(value); }
    std::string get_str(mp_exp_t &exp, int base = 10, size_t digits = 0) const {
        char *temp = mpf_get_str(nullptr, &exp, base, digits, value);
        std::string result = temp;
        void (*freefunc)(void *, size_t);
        mp_get_memory_functions(nullptr, nullptr, &freefunc);
        freefunc(temp, std::strlen(temp) + 1);
        return result;
    }
    void div_2exp(mp_bitcnt_t exp) {
        mpf_ptr non_const_ptr = const_cast<mpf_ptr>(this->get_mpf_t());
        mpf_div_2exp(non_const_ptr, this->get_mpf_t(), exp);
    }
    void mul_2exp(mp_bitcnt_t exp) {
        mpf_ptr non_const_ptr = const_cast<mpf_ptr>(this->get_mpf_t());
        mpf_mul_2exp(non_const_ptr, this->get_mpf_t(), exp);
    }
    // int mpf_class::set_str (const char *str, int base)
    // int mpf_class::set_str (const string& str, int base)
    int set_str(const char *str, int base) { return mpf_set_str(value, str, base); }
    int set_str(const std::string &str, int base) { return mpf_set_str(value, str.c_str(), base); }

    // int sgn (mpf_class op)
    // mpf_class sqrt (mpf_class op)
    // void mpf_class::swap (mpf_class& op)
    // void swap (mpf_class& op1, mpf_class& op2)
    // mpf_class trunc (mpf_class op)
    friend int sgn(const mpf_class &op);
    friend mpf_class sqrt(const mpf_class &op);
    friend mpf_class neg(const mpf_class &op);
    void swap(mpf_class &op) { mpf_swap(this->value, op.value); }
#if !defined ___GMPXX_STRICT_COMPATIBILITY___
    friend void swap(mpf_class &op1, mpf_class &op2) { mpf_swap(op1.value, op2.value); }
#endif
    friend mpf_class trunc(const mpf_class &op);

    // mp_bitcnt_t mpf_class::get_prec()
    // void mpf_class::set_prec (mp_bitcnt_t prec)
    // void mpf_class::set_prec_raw (mp_bitcnt_t prec)
    mp_bitcnt_t get_prec() const { return mpf_get_prec(value); }
    void set_prec(mp_bitcnt_t prec) { mpf_set_prec(value, prec); }
    void set_prec_raw(mp_bitcnt_t prec) { mpf_set_prec_raw(value, prec); }

    // Initialization using assignment operator
    // Copy-and-Swap Idiom; it does both the copy assignment operator and the move assignment operator.
    // mpf_class& mpf_class::operator= (type op)
    mpf_class &operator=(const mpf_class &op) noexcept { // The rule 2 of 5 copy assignment operator
        // std::cout << "The rule 2 of 5 copy assignment operator\n" ;
        if (this != &op) {
#if !defined ___GMPXX_MKII_NOPRECCHANGE___
            mpf_init2(value, mpf_get_prec(this->get_mpf_t()));
#endif
            mpf_set(value, op.value);
        }
        return *this;
    }
    mpf_class &operator=(mpf_class &&op) noexcept { // The rule 5 of 5 move assignment operator
        // std::cout << "The rule 5 of 5 move assignment operator\n" ;
        if (this != &op) {
#if !defined ___GMPXX_MKII_NOPRECCHANGE___
            mpf_init2(value, mpf_get_prec(this->get_mpf_t()));
            mpf_set(value, op.value);
#else
            mpf_swap(value, op.value);
#endif
        }
        return *this;
    }
    mpf_class &operator=(double d) noexcept {
        mpf_set_d(value, d);
        return *this;
    }
    mpf_class &operator=(const char *str) {
        if (mpf_set_str(value, str, gmpxx_defaults::base) != 0) {
            throw std::invalid_argument("");
        }
        return *this;
    }
    mpf_class &operator=(const std::string &str) {
        if (mpf_set_str(value, str.c_str(), gmpxx_defaults::base) != 0) {
            throw std::invalid_argument("");
        }
        return *this;
    }
    inline friend mpf_class &operator+=(mpf_class &lhs, const mpf_class &rhs);
    inline friend mpf_class &operator-=(mpf_class &lhs, const mpf_class &rhs);
    inline friend mpf_class &operator*=(mpf_class &lhs, const mpf_class &rhs);
    inline friend mpf_class &operator/=(mpf_class &lhs, const mpf_class &rhs);
    inline friend mpf_class operator+(const mpf_class &op);
    inline friend mpf_class operator-(const mpf_class &op);
    inline friend mpf_class operator+(const mpf_class &op1, const mpf_class &op2);
    inline friend mpf_class operator-(const mpf_class &op1, const mpf_class &op2);
    inline friend mpf_class operator*(const mpf_class &op1, const mpf_class &op2);
    inline friend mpf_class operator/(const mpf_class &op1, const mpf_class &op2);

    inline friend mpf_class operator>>(const mpf_class &op1, const unsigned int op2);

    inline friend bool operator==(const mpf_class &op1, const mpf_class &op2) { return mpf_cmp(op1.value, op2.value) == 0; }
    inline friend bool operator!=(const mpf_class &op1, const mpf_class &op2) { return mpf_cmp(op1.value, op2.value) != 0; }
    inline friend bool operator<(const mpf_class &op1, const mpf_class &op2) { return mpf_cmp(op1.value, op2.value) < 0; }
    inline friend bool operator>(const mpf_class &op1, const mpf_class &op2) { return mpf_cmp(op1.value, op2.value) > 0; }
    inline friend bool operator<=(const mpf_class &op1, const mpf_class &op2) { return mpf_cmp(op1.value, op2.value) <= 0; }
    inline friend bool operator>=(const mpf_class &op1, const mpf_class &op2) { return mpf_cmp(op1.value, op2.value) >= 0; }

    inline friend mpf_class &operator+=(mpf_class &lhs, const mpz_class &rhs);
    inline friend mpf_class &operator-=(mpf_class &lhs, const mpz_class &rhs);
    inline friend mpf_class &operator*=(mpf_class &lhs, const mpz_class &rhs);
    inline friend mpf_class &operator/=(mpf_class &lhs, const mpz_class &rhs);
    inline friend mpf_class operator+(const mpf_class &op1, const mpz_class &op2);
    inline friend mpf_class operator+(const mpz_class &op1, const mpf_class &op2);
    inline friend mpf_class operator-(const mpf_class &op1, const mpz_class &op2);
    inline friend mpf_class operator-(const mpz_class &op1, const mpf_class &op2);
    inline friend mpf_class operator*(const mpf_class &op1, const mpz_class &op2);
    inline friend mpf_class operator*(const mpz_class &op1, const mpf_class &op2);
    inline friend mpf_class operator/(const mpf_class &op1, const mpz_class &op2);
    inline friend mpf_class operator/(const mpz_class &op1, const mpf_class &op2);

    inline friend mpf_class &operator+=(mpf_class &lhs, const mpq_class &rhs);
    inline friend mpf_class &operator-=(mpf_class &lhs, const mpq_class &rhs);
    inline friend mpf_class &operator*=(mpf_class &lhs, const mpq_class &rhs);
    inline friend mpf_class &operator/=(mpf_class &lhs, const mpq_class &rhs);
    inline friend mpf_class operator+(const mpf_class &op1, const mpq_class &op2);
    inline friend mpf_class operator+(const mpq_class &op1, const mpf_class &op2);
    inline friend mpf_class operator-(const mpf_class &op1, const mpq_class &op2);
    inline friend mpf_class operator-(const mpq_class &op1, const mpf_class &op2);
    inline friend mpf_class operator*(const mpf_class &op1, const mpq_class &op2);
    inline friend mpf_class operator*(const mpq_class &op1, const mpf_class &op2);
    inline friend mpf_class operator/(const mpf_class &op1, const mpq_class &op2);
    inline friend mpf_class operator/(const mpq_class &op1, const mpf_class &op2);

    inline friend mpf_class &operator+=(mpf_class &lhs, const double rhs);
    inline friend mpf_class &operator-=(mpf_class &lhs, const double rhs);
    inline friend mpf_class &operator*=(mpf_class &lhs, const double rhs);
    inline friend mpf_class &operator/=(mpf_class &lhs, const double rhs);
    inline friend mpf_class operator+(const mpf_class &op1, const double op2);
    inline friend mpf_class operator+(const double op1, const mpf_class &op2);
    inline friend mpf_class operator-(const mpf_class &op1, const double op2);
    inline friend mpf_class operator-(const double op1, const mpf_class &op2);
    inline friend mpf_class operator*(const mpf_class &op1, const double op2);
    inline friend mpf_class operator*(const double op1, const mpf_class &op2);
    inline friend mpf_class operator/(const mpf_class &op1, const double op2);
    inline friend mpf_class operator/(const double op1, const mpf_class &op2);

    inline friend mpf_class &operator+=(mpf_class &lhs, const signed long int rhs);
    inline friend mpf_class &operator-=(mpf_class &lhs, const signed long int rhs);
    inline friend mpf_class &operator*=(mpf_class &lhs, const signed long int rhs);
    inline friend mpf_class &operator/=(mpf_class &lhs, const signed long int rhs);
    inline friend mpf_class operator+(const mpf_class &op1, const signed long int op2);
    inline friend mpf_class operator+(const signed long int op1, const mpf_class &op2);
    inline friend mpf_class operator-(const mpf_class &op1, const signed long int op2);
    inline friend mpf_class operator-(const signed long int op1, const mpf_class &op2);
    inline friend mpf_class operator*(const mpf_class &op1, const signed long int op2);
    inline friend mpf_class operator*(const signed long int op1, const mpf_class &op2);
    inline friend mpf_class operator/(const mpf_class &op1, const signed long int op2);
    inline friend mpf_class operator/(const signed long int op1, const mpf_class &op2);

    inline friend mpf_class &operator+=(mpf_class &lhs, const signed int rhs);
    inline friend mpf_class &operator-=(mpf_class &lhs, const signed int rhs);
    inline friend mpf_class &operator*=(mpf_class &lhs, const signed int rhs);
    inline friend mpf_class &operator/=(mpf_class &lhs, const signed int rhs);
    inline friend mpf_class operator+(const mpf_class &op1, const signed int op2);
    inline friend mpf_class operator+(const signed int op1, const mpf_class &op2);
    inline friend mpf_class operator-(const mpf_class &op1, const signed int op2);
    inline friend mpf_class operator-(const signed int op1, const mpf_class &op2);
    inline friend mpf_class operator*(const mpf_class &op1, const signed int op2);
    inline friend mpf_class operator*(const signed int op1, const mpf_class &op2);
    inline friend mpf_class operator/(const mpf_class &op1, const signed int op2);
    inline friend mpf_class operator/(const signed int op1, const mpf_class &op2);

    inline friend mpf_class &operator+=(mpf_class &lhs, const unsigned long int rhs);
    inline friend mpf_class &operator-=(mpf_class &lhs, const unsigned long int rhs);
    inline friend mpf_class &operator*=(mpf_class &lhs, const unsigned long int rhs);
    inline friend mpf_class &operator/=(mpf_class &lhs, const unsigned long int rhs);
    inline friend mpf_class operator+(const mpf_class &op1, const unsigned long int op2);
    inline friend mpf_class operator+(const unsigned long int op1, const mpf_class &op2);
    inline friend mpf_class operator-(const mpf_class &op1, const unsigned long int op2);
    inline friend mpf_class operator-(const unsigned long int op1, const mpf_class &op2);
    inline friend mpf_class operator*(const mpf_class &op1, const unsigned long int op2);
    inline friend mpf_class operator*(const unsigned long int op1, const mpf_class &op2);
    inline friend mpf_class operator/(const mpf_class &op1, const unsigned long int op2);
    inline friend mpf_class operator/(const unsigned long int op1, const mpf_class &op2);

    inline friend mpf_class &operator+=(mpf_class &lhs, const unsigned int rhs);
    inline friend mpf_class &operator-=(mpf_class &lhs, const unsigned int rhs);
    inline friend mpf_class &operator*=(mpf_class &lhs, const unsigned int rhs);
    inline friend mpf_class &operator/=(mpf_class &lhs, const unsigned int rhs);
    inline friend mpf_class operator+(const mpf_class &op1, const unsigned int op2);
    inline friend mpf_class operator+(const unsigned int op1, const mpf_class &op2);
    inline friend mpf_class operator-(const mpf_class &op1, const unsigned int op2);
    inline friend mpf_class operator-(const unsigned int op1, const mpf_class &op2);
    inline friend mpf_class operator*(const mpf_class &op1, const unsigned int op2);
    inline friend mpf_class operator*(const unsigned int op1, const mpf_class &op2);
    inline friend mpf_class operator/(const mpf_class &op1, const unsigned int op2);
    inline friend mpf_class operator/(const unsigned int op1, const mpf_class &op2);

    friend std::ostream &operator<<(std::ostream &os, const mpf_class &m);

    static mpf_class const_pi();
    static mpf_class const_e();
    static mpf_class const_log10();
    static mpf_class const_log2();
    static void reset_pi_cache();
    static void reset_e_cache();
    static void reset_log10_cache();
    static void reset_log2_cache();

    operator mpq_class() const;
    operator mpz_class() const;
    mpf_class &operator=(const mpz_class &other) {
        mpf_set_z(this->value, other.get_mpz_t());
        return *this;
    }
    mpf_class &operator=(const mpq_class &other) {
        mpf_set_q(this->value, other.get_mpq_t());
        return *this;
    }

    mpf_srcptr get_mpf_t() const { return value; }

  private:
    mpf_t value;

    static mpf_class pi_cached;
    static mpf_class e_cached;
    static mpf_class log10_cached;
    static mpf_class log2_cached;
};
// casts
mpf_class::operator mpq_class() const { return mpq_class(this->get_mpf_t()); }
mpf_class::operator mpz_class() const { return mpz_class(this->get_mpf_t()); }
mpz_class::operator mpf_class() const { return mpf_class(this->get_mpz_t()); }
mpq_class::operator mpf_class() const { return mpf_class(this->get_mpq_t()); }
mpz_class::operator mpq_class() const { return mpq_class(this->get_mpz_t()); }
mpq_class::operator mpz_class() const { return mpz_class(this->get_mpq_t()); }

inline mp_bitcnt_t largerprec(const mpf_class &op1, const mpf_class &op2) {
    mp_bitcnt_t prec1 = op1.get_prec(), prec2 = op2.get_prec();
    return (prec1 > prec2) ? prec1 : prec2;
}
inline mpf_class &operator+=(mpf_class &lhs, const mpf_class &rhs) {
    mpf_add(lhs.value, lhs.value, rhs.value);
    return lhs;
}
inline mpf_class &operator-=(mpf_class &lhs, const mpf_class &rhs) {
    mpf_sub(lhs.value, lhs.value, rhs.value);
    return lhs;
}
inline mpf_class &operator*=(mpf_class &lhs, const mpf_class &rhs) {
    mpf_mul(lhs.value, lhs.value, rhs.value);
    return lhs;
}
inline mpf_class &operator/=(mpf_class &lhs, const mpf_class &rhs) {
    mpf_div(lhs.value, lhs.value, rhs.value);
    return lhs;
}
inline mpf_class operator+(const mpf_class &op1, const mpf_class &op2) {
#if defined ___GMPXX_MKII_NOPRECCHANGE___
    mpf_class result;
    mpf_add(result.value, op1.value, op2.value);
#else
    mpf_class result;
    mp_bitcnt_t prec = largerprec(op1, op2);
    mpf_init2(result.value, prec);
    mpf_add(result.value, op1.value, op2.value);
#endif
    return result;
}
inline mpf_class operator-(const mpf_class &op1, const mpf_class &op2) {
#if defined ___GMPXX_MKII_NOPRECCHANGE___
    mpf_class result;
    mpf_sub(result.value, op1.value, op2.value);
#else
    mpf_class result;
    mp_bitcnt_t prec = largerprec(op1, op2);
    mpf_init2(result.value, prec);
    mpf_sub(result.value, op1.value, op2.value);
#endif
    return result;
}
inline mpf_class operator*(const mpf_class &op1, const mpf_class &op2) {
#if defined ___GMPXX_MKII_NOPRECCHANGE___
    mpf_class result;
    mpf_mul(result.value, op1.value, op2.value);
#else
    mpf_class result;
    mp_bitcnt_t prec = largerprec(op1, op2);
    mpf_init2(result.value, prec);
    mpf_mul(result.value, op1.value, op2.value);
#endif
    return result;
}
inline mpf_class operator/(const mpf_class &op1, const mpf_class &op2) {
#if defined ___GMPXX_MKII_NOPRECCHANGE___
    mpf_class result;
    mpf_div(result.value, op1.value, op2.value);
#else
    mpf_class result;
    mp_bitcnt_t prec = largerprec(op1, op2);
    mpf_init2(result.value, prec);
    mpf_div(result.value, op1.value, op2.value);
#endif
    return result;
}
inline mpf_class operator>>(const mpf_class &op1, const unsigned int op2) {
    mpf_class result;
    mpf_div_2exp(result.value, op1.value, op2);
    return result;
}
inline mpf_class operator+(const mpf_class &op) {
    mpf_class result(op.value);
    return result;
}
inline mpf_class operator-(const mpf_class &op) {
    mpf_class result(op.value);
    mpf_neg(result.value, op.value);
    return result;
}

inline bool operator==(const mpf_class &lhs, signed long int rhs) { return lhs.get_si() == rhs; }
inline bool operator==(signed long int lhs, const mpf_class &rhs) { return rhs == lhs; }
inline bool operator==(const mpf_class &lhs, unsigned long int rhs) { return lhs.get_ui() == rhs; }
inline bool operator==(unsigned long int lhs, const mpf_class &rhs) { return rhs == lhs; }
inline bool operator==(const mpf_class &lhs, signed int rhs) { return lhs.get_si() == (signed long int)rhs; }
inline bool operator==(signed int lhs, const mpf_class &rhs) { return rhs == lhs; }
inline bool operator==(const mpf_class &lhs, unsigned int rhs) { return lhs.get_ui() == (unsigned long int)rhs; }
inline bool operator==(unsigned int lhs, const mpf_class &rhs) { return rhs == lhs; }
inline bool operator==(const mpf_class &lhs, double rhs) { return lhs.get_d() == rhs; }
inline bool operator==(double lhs, const mpf_class &rhs) { return rhs == lhs; }

// int cmp (mpf_class op1, type op2)
// int cmp (type op1, mpf_class op2)
template <typename T> int cmp(const mpf_class &op1, const T &op2) {
    mpf_class temp(op2);
    return mpf_cmp(op1.get_mpf_t(), temp.get_mpf_t());
}
template <typename T> int cmp(const T &op1, const mpf_class &op2) {
    mpf_class temp(op1);
    return mpf_cmp(temp.get_mpf_t(), op2.get_mpf_t());
}
template <typename T> int cmp(const mpz_class &op1, const T &op2) {
    mpz_class temp(op2);
    return mpz_cmp(op1.get_mpz_t(), temp.get_mpz_t());
}
template <typename T> int cmp(const T &op1, const mpz_class &op2) {
    mpz_class temp(op1);
    return mpz_cmp(temp.get_mpz_t(), op2.get_mpz_t());
}
// int cmp (mpq_class op1, type op2)
// int cmp (type op1, mpq_class op2)
template <typename T> int cmp(const mpq_class &op1, const T &op2) {
    mpq_class temp(op2);
    return mpq_cmp(op1.get_mpq_t(), temp.get_mpq_t());
}
template <typename T> int cmp(const T &op1, const mpq_class &op2) {
    mpq_class temp(op1);
    return mpq_cmp(temp.get_mpq_t(), op2.get_mpq_t());
}
inline int cmp(const mpq_class &op1, const mpq_class &op2) { return mpq_cmp(op1.get_mpq_t(), op2.get_mpq_t()); }
template <> int cmp<double>(const mpf_class &op1, const double &op2) { return mpf_cmp_d(op1.get_mpf_t(), op2); }
template <> int cmp<double>(const double &op1, const mpf_class &op2) { return -mpf_cmp_d(op2.get_mpf_t(), op1); }
template <> int cmp<unsigned long int>(const mpf_class &op1, const unsigned long int &op2) { return mpf_cmp_ui(op1.get_mpf_t(), op2); }
template <> int cmp<unsigned long int>(const unsigned long int &op1, const mpf_class &op2) { return -mpf_cmp_ui(op2.get_mpf_t(), op1); }
template <> int cmp<signed long int>(const mpf_class &op1, const signed long int &op2) { return mpf_cmp_si(op1.get_mpf_t(), op2); }
template <> int cmp<signed long int>(const signed long int &op1, const mpf_class &op2) { return -mpf_cmp_si(op2.get_mpf_t(), op1); }

inline mpf_class trunc(const mpf_class &op) {
    mpf_class rop(op);
    mpf_trunc(rop.value, op.get_mpf_t());
    return rop;
}
inline mpf_class sqrt(const mpf_class &op) {
    mpf_class rop(op);
    mpf_sqrt(rop.value, op.get_mpf_t());
    return rop;
}
inline mpf_class neg(const mpf_class &op) {
    mpf_class rop(op);
    mpf_neg(rop.value, op.get_mpf_t());
    return rop;
}
inline mpf_class abs(const mpf_class &op) {
    mpf_class rop(op);
    mpf_abs(rop.value, op.get_mpf_t());
    return rop;
}
inline mpf_class ceil(const mpf_class &op) {
    mpf_class rop(op);
    mpf_ceil(rop.value, op.get_mpf_t());
    return rop;
}
inline mpf_class floor(const mpf_class &op) {
    mpf_class rop(op);
    mpf_floor(rop.value, op.get_mpf_t());
    return rop;
}
inline mpf_class hypot(const mpf_class &op1, const mpf_class &op2) {
#if defined ___GMPXX_MKII_NOPRECCHANGE___
    mpf_class rop;
    rop = sqrt(op1 * op1 + op2 * op2);
#else
    mpf_class rop;
    mp_bitcnt_t prec = largerprec(op1, op2);
    mpf_init2(rop.value, prec);
    rop = sqrt(op1 * op1 + op2 * op2);
#endif
    return rop;
}
inline int sgn(const mpf_class &op) {
    int flag = mpf_sgn(op.get_mpf_t());
    return flag;
}
inline mpf_class &operator+=(mpf_class &lhs, double rhs) {
    mpf_t temp;
    mpf_init_set_d(temp, rhs);
    mpf_add(lhs.value, lhs.value, temp);
    mpf_clear(temp);
    return lhs;
}
inline mpf_class &operator-=(mpf_class &lhs, double rhs) {
    mpf_t temp;
    mpf_init_set_d(temp, rhs);
    mpf_sub(lhs.value, lhs.value, temp);
    mpf_clear(temp);
    return lhs;
}
inline mpf_class &operator*=(mpf_class &lhs, double rhs) {
    mpf_t temp;
    mpf_init_set_d(temp, rhs);
    mpf_mul(lhs.value, lhs.value, temp);
    mpf_clear(temp);
    return lhs;
}
inline mpf_class &operator/=(mpf_class &lhs, double rhs) {
    mpf_t temp;
    mpf_init_set_d(temp, rhs);
    mpf_div(lhs.value, lhs.value, temp);
    mpf_clear(temp);
    return lhs;
}
inline mpf_class operator+(const mpf_class &op1, const double op2) {
    mpf_class result(op1);
    result += op2;
    return result;
}
inline mpf_class operator+(const double op1, const mpf_class &op2) {
    mpf_class result(op1);
    result += op2;
    return result;
}
inline mpf_class operator-(const mpf_class &op1, const double op2) {
    mpf_class result(op1);
    result -= op2;
    return result;
}
inline mpf_class operator-(const double op1, const mpf_class &op2) {
    mpf_class result(op1);
    result -= op2;
    return result;
}
inline mpf_class operator*(const mpf_class &op1, const double op2) {
    mpf_class result = (op1);
    result *= op2;
    return result;
}
inline mpf_class operator*(const double op1, const mpf_class &op2) {
    mpf_class result(op1);
    result *= op2;
    return result;
}
inline mpf_class operator/(const mpf_class &op1, const double op2) {
    mpf_class result = op1;
    result /= op2;
    return result;
}
inline mpf_class operator/(const double op1, const mpf_class &op2) {
    mpf_class result(op1);
    result /= op2;
    return result;
}
inline mpf_class &operator+=(mpf_class &lhs, signed long int rhs) {
    mpf_t temp;
    mpf_init_set_si(temp, rhs);
    mpf_add(lhs.value, lhs.value, temp);
    mpf_clear(temp);
    return lhs;
}
inline mpf_class &operator-=(mpf_class &lhs, signed long int rhs) {
    mpf_t temp;
    mpf_init_set_si(temp, rhs);
    mpf_sub(lhs.value, lhs.value, temp);
    mpf_clear(temp);
    return lhs;
}
inline mpf_class &operator*=(mpf_class &lhs, signed long int rhs) {
    mpf_t temp;
    mpf_init_set_si(temp, rhs);
    mpf_mul(lhs.value, lhs.value, temp);
    mpf_clear(temp);
    return lhs;
}
inline mpf_class &operator/=(mpf_class &lhs, signed long int rhs) {
    mpf_t temp;
    mpf_init_set_si(temp, rhs);
    mpf_div(lhs.value, lhs.value, temp);
    mpf_clear(temp);
    return lhs;
}
inline mpf_class operator+(const mpf_class &op1, const signed long int op2) {
    mpf_class result(op1);
    result += op2;
    return result;
}
inline mpf_class operator+(const signed long int op1, const mpf_class &op2) {
    mpf_class result(op1);
    result += op2;
    return result;
}
inline mpf_class operator-(const mpf_class &op1, const signed long int op2) {
    mpf_class result(op1);
    result -= op2;
    return result;
}
inline mpf_class operator-(const signed long int op1, const mpf_class &op2) {
    mpf_class result(op1);
    result -= op2;
    return result;
}
inline mpf_class operator*(const mpf_class &op1, const signed long int op2) {
    mpf_class result = (op1);
    result *= op2;
    return result;
}
inline mpf_class operator*(const signed long int op1, const mpf_class &op2) {
    mpf_class result(op1);
    result *= op2;
    return result;
}
inline mpf_class operator/(const mpf_class &op1, const signed long int op2) {
    mpf_class result = op1;
    result /= op2;
    return result;
}
inline mpf_class operator/(const signed long int op1, const mpf_class &op2) {
    mpf_class result(op1);
    result /= op2;
    return result;
}
inline mpf_class &operator+=(mpf_class &lhs, const mpz_class &rhs) {
    mpf_class temp = mpf_class(rhs);
    lhs += temp;
    return lhs;
}
inline mpf_class &operator-=(mpf_class &lhs, const mpz_class &rhs) {
    mpf_class temp = mpf_class(rhs);
    lhs -= temp;
    return lhs;
}
inline mpf_class &operator*=(mpf_class &lhs, const mpz_class &rhs) {
    mpf_class temp = mpf_class(rhs);
    lhs *= temp;
    return lhs;
}
inline mpf_class &operator/=(mpf_class &lhs, const mpz_class &rhs) {
    mpf_class temp = mpf_class(rhs);
    lhs /= temp;
    return lhs;
}
inline mpf_class operator+(const mpf_class &op1, const mpz_class &op2) {
    mpf_class _op1(op1);
    mpf_class _op2(op2);
    _op1 += _op2;
    return _op1;
}
inline mpf_class operator+(const mpz_class &op1, const mpf_class &op2) {
    mpf_class _op1(op1);
    mpf_class _op2(op2);
    _op2 += _op1;
    return _op2;
}
inline mpf_class operator-(const mpf_class &op1, const mpz_class &op2) {
    mpf_class _op1(op1);
    mpf_class _op2(op2);
    _op1 -= _op2;
    return _op1;
}
inline mpf_class operator-(const mpz_class &op1, const mpf_class &op2) {
    mpf_class _op1(op2); // 'op2' is used for correct precision initialization, furthur optimization may be possible for ___GMPXX_MKII_NOPRECCHANGE___
    mpf_class _op2(op2);
    _op1 = op1;
    _op1 -= _op2;
    return _op1;
}
inline mpf_class operator*(const mpf_class &op1, const mpz_class &op2) {
    mpf_class _op1(op1);
    mpf_class _op2(op2);
    _op1 *= _op2;
    return _op1;
}
inline mpf_class operator*(const mpz_class &op1, const mpf_class &op2) {
    mpf_class _op1(op2); // 'op2' is used for correct precision initialization, furthur optimization may be possible for ___GMPXX_MKII_NOPRECCHANGE___
    mpf_class _op2(op2);
    _op1 = op1;
    _op1 *= _op2;
    return _op1;
}
inline mpf_class operator/(const mpf_class &op1, const mpz_class &op2) {
    mpf_class _op1(op1);
    mpf_class _op2(op2);
    _op1 /= _op2;
    return _op1;
}
inline mpf_class operator/(const mpz_class &op1, const mpf_class &op2) {
    mpf_class _op1(op2); // 'op2' is used for correct precision initialization, furthur optimization may be possible for ___GMPXX_MKII_NOPRECCHANGE___
    mpf_class _op2(op2);
    _op1 = op1;
    _op1 /= _op2;
    return _op1;
}
inline mpf_class &operator+=(mpf_class &lhs, const mpq_class &rhs) {
    mpf_class temp(rhs);
    lhs += temp;
    return lhs;
}
inline mpf_class &operator-=(mpf_class &lhs, const mpq_class &rhs) {
    mpf_class temp(rhs);
    lhs -= temp;
    return lhs;
}

inline mpf_class &operator*=(mpf_class &lhs, const mpq_class &rhs) {
    mpf_class temp(rhs);
    lhs *= temp;
    return lhs;
}

inline mpf_class &operator/=(mpf_class &lhs, const mpq_class &rhs) {
    mpf_class temp(rhs);
    lhs /= temp;
    return lhs;
}
inline mpf_class operator+(const mpf_class &op1, const mpq_class &op2) {
    mpf_class result(op1);
    result += op2;
    return result;
}

inline mpf_class operator+(const mpq_class &op1, const mpf_class &op2) {
    mpf_class result(op2);
    result += op1;
    return result;
}

inline mpf_class operator-(const mpf_class &op1, const mpq_class &op2) {
    mpf_class result(op1);
    result -= op2;
    return result;
}
inline mpf_class operator-(const mpq_class &op1, const mpf_class &op2) {
    mpf_class _op1(op2); // 'op2' is used for correct precision initialization, furthur optimization may be possible for ___GMPXX_MKII_NOPRECCHANGE___
    mpf_class _op2(op2);
    _op1 = op1;
    _op1 -= _op2;
    return _op1;
}
inline mpf_class operator*(const mpf_class &op1, const mpq_class &op2) {
    mpf_class _op1(op1);
    mpf_class _op2(op2);
    _op1 *= _op2;
    return _op1;
}
inline mpf_class operator*(const mpq_class &op1, const mpf_class &op2) {
    mpf_class _op1(op2); // 'op2' is used for correct precision initialization, furthur optimization may be possible for ___GMPXX_MKII_NOPRECCHANGE___
    mpf_class _op2(op2);
    _op1 = op1;
    _op1 *= _op2;
    return _op1;
}
inline mpf_class operator/(const mpf_class &op1, const mpq_class &op2) {
    mpf_class _op1(op1);
    mpf_class _op2(op2);
    _op1 /= _op2;
    return _op1;
}
inline mpf_class operator/(const mpq_class &op1, const mpf_class &op2) {
    mpf_class _op1(op2); // 'op2' is used for correct precision initialization, furthur optimization may be possible for ___GMPXX_MKII_NOPRECCHANGE___
    mpf_class _op2(op2);
    _op1 = op1;
    _op1 /= _op2;
    return _op1;
}
inline mpf_class &operator+=(mpf_class &lhs, signed int rhs) {
    mpf_t temp;
    mpf_init_set_si(temp, (signed long int)rhs);
    mpf_add(lhs.value, lhs.value, temp);
    mpf_clear(temp);
    return lhs;
}
inline mpf_class &operator-=(mpf_class &lhs, signed int rhs) {
    mpf_t temp;
    mpf_init_set_si(temp, (signed long int)rhs);
    mpf_sub(lhs.value, lhs.value, temp);
    mpf_clear(temp);
    return lhs;
}
inline mpf_class &operator*=(mpf_class &lhs, signed int rhs) {
    mpf_t temp;
    mpf_init_set_si(temp, (signed long int)rhs);
    mpf_mul(lhs.value, lhs.value, temp);
    mpf_clear(temp);
    return lhs;
}
inline mpf_class &operator/=(mpf_class &lhs, signed int rhs) {
    mpf_t temp;
    mpf_init_set_si(temp, (signed long int)rhs);
    mpf_div(lhs.value, lhs.value, temp);
    mpf_clear(temp);
    return lhs;
}
inline mpf_class operator+(const mpf_class &op1, const signed int op2) {
    mpf_class result(op1);
    result += op2;
    return result;
}
inline mpf_class operator+(const signed int op1, const mpf_class &op2) {
    mpf_class result(op1);
    result += op2;
    return result;
}
inline mpf_class operator-(const mpf_class &op1, const signed int op2) {
    mpf_class result(op1);
    result -= op2;
    return result;
}
inline mpf_class operator-(const signed int op1, const mpf_class &op2) {
    mpf_class result(op1);
    result -= op2;
    return result;
}
inline mpf_class operator*(const mpf_class &op1, const signed int op2) {
    mpf_class result = (op1);
    result *= op2;
    return result;
}
inline mpf_class operator*(const signed int op1, const mpf_class &op2) {
    mpf_class result(op1);
    result *= op2;
    return result;
}
inline mpf_class operator/(const mpf_class &op1, const signed int op2) {
    mpf_class result = op1;
    result /= op2;
    return result;
}
inline mpf_class operator/(const signed int op1, const mpf_class &op2) {
    mpf_class result(op1);
    result /= op2;
    return result;
}
inline mpf_class &operator+=(mpf_class &lhs, unsigned long int rhs) {
    mpf_t temp;
    mpf_init_set_ui(temp, rhs);
    mpf_add(lhs.value, lhs.value, temp);
    mpf_clear(temp);
    return lhs;
}
inline mpf_class &operator-=(mpf_class &lhs, unsigned long int rhs) {
    mpf_t temp;
    mpf_init_set_ui(temp, rhs);
    mpf_sub(lhs.value, lhs.value, temp);
    mpf_clear(temp);
    return lhs;
}
inline mpf_class &operator*=(mpf_class &lhs, unsigned long int rhs) {
    mpf_t temp;
    mpf_init_set_ui(temp, rhs);
    mpf_mul(lhs.value, lhs.value, temp);
    mpf_clear(temp);
    return lhs;
}
inline mpf_class &operator/=(mpf_class &lhs, unsigned long int rhs) {
    mpf_t temp;
    mpf_init_set_ui(temp, rhs);
    mpf_div(lhs.value, lhs.value, temp);
    mpf_clear(temp);
    return lhs;
}
inline mpf_class operator+(const mpf_class &op1, const unsigned long int op2) {
    mpf_class result(op1);
    result += op2;
    return result;
}
inline mpf_class operator+(const unsigned long int op1, const mpf_class &op2) {
    mpf_class result(op1);
    result += op2;
    return result;
}
inline mpf_class operator-(const mpf_class &op1, const unsigned long int op2) {
    mpf_class result(op1);
    result -= op2;
    return result;
}
inline mpf_class operator-(const unsigned long int op1, const mpf_class &op2) {
    mpf_class result(op1);
    result -= op2;
    return result;
}
inline mpf_class operator*(const mpf_class &op1, const unsigned long int op2) {
    mpf_class result = (op1);
    result *= op2;
    return result;
}
inline mpf_class operator*(const unsigned long int op1, const mpf_class &op2) {
    mpf_class result(op1);
    result *= op2;
    return result;
}
inline mpf_class operator/(const mpf_class &op1, const unsigned long int op2) {
    mpf_class result = op1;
    result /= op2;
    return result;
}
inline mpf_class operator/(const unsigned long int op1, const mpf_class &op2) {
    mpf_class result(op1);
    result /= op2;
    return result;
}
inline mpf_class &operator+=(mpf_class &lhs, unsigned int rhs) {
    mpf_t temp;
    mpf_init_set_ui(temp, (unsigned long int)rhs);
    mpf_add(lhs.value, lhs.value, temp);
    mpf_clear(temp);
    return lhs;
}
inline mpf_class &operator-=(mpf_class &lhs, unsigned int rhs) {
    mpf_t temp;
    mpf_init_set_ui(temp, (unsigned long int)rhs);
    mpf_sub(lhs.value, lhs.value, temp);
    mpf_clear(temp);
    return lhs;
}
inline mpf_class &operator*=(mpf_class &lhs, unsigned int rhs) {
    mpf_t temp;
    mpf_init_set_ui(temp, (unsigned long int)rhs);
    mpf_mul(lhs.value, lhs.value, temp);
    mpf_clear(temp);
    return lhs;
}
inline mpf_class &operator/=(mpf_class &lhs, unsigned int rhs) {
    mpf_t temp;
    mpf_init_set_ui(temp, (unsigned long int)rhs);
    mpf_div(lhs.value, lhs.value, temp);
    mpf_clear(temp);
    return lhs;
}
inline mpf_class operator+(const mpf_class &op1, const unsigned int op2) {
    mpf_class result(op1);
    result += op2;
    return result;
}
inline mpf_class operator+(const unsigned int op1, const mpf_class &op2) {
    mpf_class result(op1);
    result += op2;
    return result;
}
inline mpf_class operator-(const mpf_class &op1, const unsigned int op2) {
    mpf_class result(op1);
    result -= op2;
    return result;
}
inline mpf_class operator-(const unsigned int op1, const mpf_class &op2) {
    mpf_class result(op1);
    result -= op2;
    return result;
}
inline mpf_class operator*(const mpf_class &op1, const unsigned int op2) {
    mpf_class result = (op1);
    result *= op2;
    return result;
}
inline mpf_class operator*(const unsigned int op1, const mpf_class &op2) {
    mpf_class result(op1);
    result *= op2;
    return result;
}
inline mpf_class operator/(const mpf_class &op1, const unsigned int op2) {
    mpf_class result = op1;
    result /= op2;
    return result;
}
inline mpf_class operator/(const unsigned int op1, const mpf_class &op2) {
    mpf_class result(op1);
    result /= op2;
    return result;
}
std::ostream &operator<<(std::ostream &os, const mpf_class &m) {
    std::streamsize prec = os.precision();
    std::ios_base::fmtflags flags = os.flags();

    char *str = nullptr;
    if (flags & std::ios::scientific) {
        gmp_asprintf(&str, "%.*Fe", static_cast<int>(prec), m.value);
    } else if (flags & std::ios::fixed) {
        gmp_asprintf(&str, "%.*Ff", static_cast<int>(prec), m.value);
    } else {
        gmp_asprintf(&str, "%.*Fg", static_cast<int>(prec), m.value);
    }
    os << str;
    free(str);
    return os;
}

mpf_class pi_cached;
mpf_class const_pi() {
    static bool calculated = false;
    static mp_bitcnt_t calculated_pi_precision = 0;
    mp_bitcnt_t _default_prec = mpf_get_default_prec();

    if (!calculated || (calculated && calculated_pi_precision != _default_prec)) {
        pi_cached = mpf_class();
        calculated_pi_precision = mpf_get_default_prec();
        // calculating approximate pi using arithmetic-geometric mean
        mpf_class one(1.0);
        mpf_class two(2.0);
        mpf_class four(4.0);
        mpf_class a(one), b(one / sqrt(two)), t(0.25), p(one);
        mpf_class a_next, b_next, t_next, tmp_pi, pi_previous;

        bool converged = false;
        int iteration = 0;

        mpf_class epsilon = one;
        epsilon.div_2exp(_default_prec);

        while (!converged) {
            iteration++;
            a_next = (a + b) / two;
            b_next = sqrt(a * b);
            t_next = t - p * (a - a_next) * (a - a_next);
            p = two * p;

            // Update values for the next iteration
            a = a_next;
            b = b_next;
            t = t_next;

            // Calculate pi
            pi_previous = tmp_pi;
            tmp_pi = (a + b) * (a + b) / (four * t);

            // Check for convergence
            if (abs(tmp_pi - pi_previous) < epsilon) {
                converged = true;
            }
        }
        calculated = true;
        pi_cached = tmp_pi;
    } else {
        //      std::cout << "pi cached\n";
    }
    return pi_cached;
}
mpf_class const_pi(mp_bitcnt_t req_precision) {
#if defined ___GMPXX_MKII_NOPRECCHANGE___
    assert(req_precision == mpf_get_default_prec());
#endif
    // calculating approximate pi using arithmetic-geometric mean
    mpf_class zero(0.0, req_precision);
    mpf_class quarter(0.25, req_precision);
    mpf_class one(1.0, req_precision);
    mpf_class two(2.0, req_precision);
    mpf_class four(4.0, req_precision);

    mpf_class calculated_pi(zero);
    mpf_class a(one), b(one / sqrt(two)), t(quarter), p(one);
    mpf_class a_next(zero), b_next(zero), t_next(zero), tmp_pi(zero), pi_previous(zero);
    mpf_class epsilon(zero), tmp(zero);

    assert(calculated_pi.get_prec() == req_precision);
    assert(a.get_prec() == req_precision);
    assert(b.get_prec() == req_precision);
    assert(t.get_prec() == req_precision);
    assert(p.get_prec() == req_precision);
    assert(a_next.get_prec() == req_precision);
    assert(b_next.get_prec() == req_precision);
    assert(t_next.get_prec() == req_precision);
    assert(tmp_pi.get_prec() == req_precision);
    assert(pi_previous.get_prec() == req_precision);
    assert(epsilon.get_prec() == req_precision);
    assert(tmp.get_prec() == req_precision);

    bool converged = false;
    int iteration = 0;

    epsilon = one;
    epsilon.div_2exp(req_precision);
    while (!converged) {
        iteration++;
        a_next = (a + b) / two;
        b_next = sqrt(a * b);
        t_next = t - p * (a - a_next) * (a - a_next);
        p = two * p;

        // Update values for the next iteration
        a = a_next;
        b = b_next;
        t = t_next;

        // Calculate pi
        pi_previous = tmp_pi;
        tmp_pi = (a + b) * (a + b) / (four * t);

        // Check for convergence
        tmp = abs(tmp_pi - pi_previous);
        if (tmp < epsilon) {
            converged = true;
        }
    }
    calculated_pi = tmp_pi;

    assert(calculated_pi.get_prec() == req_precision);
    assert(a.get_prec() == req_precision);
    assert(b.get_prec() == req_precision);
    assert(t.get_prec() == req_precision);
    assert(p.get_prec() == req_precision);
    assert(a_next.get_prec() == req_precision);
    assert(b_next.get_prec() == req_precision);
    assert(t_next.get_prec() == req_precision);
    assert(tmp_pi.get_prec() == req_precision);
    assert(pi_previous.get_prec() == req_precision);
    assert(epsilon.get_prec() == req_precision);
    assert(tmp.get_prec() == req_precision);

    return calculated_pi;
}

mpf_class log2_cached;
mpf_class const_log2() {
    static mpf_class log2_cached;
    static bool calculated = false;
    static mp_bitcnt_t calculated_log2_precision = 0;
    mp_bitcnt_t _default_prec = mpf_get_default_prec();

    if (!calculated || (calculated && calculated_log2_precision != _default_prec)) {
        log2_cached = mpf_class();
        calculated_log2_precision = mpf_get_default_prec();
        // calculating approximate log2 using arithmetic-geometric mean
        mpf_class one(1.0);
        mpf_class two(2.0);
        mpf_class a(one);
        mpf_class epsilon = one;
        epsilon.div_2exp((_default_prec / 2) - 2);

        mpf_class b = epsilon;
        mpf_class sum = one;
        mpf_class a_next, b_next;
        mpf_class tmp;

        bool converged = false;

        while (!converged) {
            a_next = (a + b) / two;
            b_next = sqrt(a * b);

            // Check for convergence
            if (abs(a - b) < epsilon) {
                converged = true;
            }
            a = a_next;
            b = b_next;
        }
        log2_cached = const_pi() / (mpf_class(_default_prec) * a);
        calculated = true;
    }
    return log2_cached;
}
mpf_class const_log2(mp_bitcnt_t req_precision) {
#if defined ___GMPXX_MKII_NOPRECCHANGE___
    assert(req_precision == mpf_get_default_prec());
#endif
    mpf_class zero(0.0, req_precision);
    mpf_class one(1.0, req_precision);
    mpf_class two(2.0, req_precision);

    mpf_class log2(zero);
    mpf_class epsilon(one), tmp(zero);
    mpf_class a(one), b(one);
    mpf_class a_next(zero), b_next(zero);
    mpf_class sum(one);

    bool converged = false;

    // calculating approximate log2 using arithmetic-geometric mean
    b.div_2exp((req_precision / 2) - 2);
    epsilon.div_2exp(req_precision);

    assert(log2.get_prec() == req_precision);
    assert(epsilon.get_prec() == req_precision);
    assert(tmp.get_prec() == req_precision);
    assert(sum.get_prec() == req_precision);
    assert(a.get_prec() == req_precision);
    assert(b.get_prec() == req_precision);
    assert(a_next.get_prec() == req_precision);
    assert(b_next.get_prec() == req_precision);
    assert(one.get_prec() == req_precision);
    assert(two.get_prec() == req_precision);

    while (!converged) {
        a_next = (a + b) / two;
        b_next = sqrt(a * b);
        assert(b_next.get_prec() == req_precision);

        // Check for convergence
        if (abs(a - b) < epsilon) {
            converged = true;
        }
        a = a_next;
        b = b_next;
    }
    log2 = const_pi(req_precision) / (mpf_class(req_precision, req_precision) * a);

    assert(const_pi(req_precision).get_prec() == req_precision);
    assert(mpf_class(req_precision, req_precision).get_prec() == req_precision);
    assert(log2.get_prec() == req_precision);
    assert(epsilon.get_prec() == req_precision);
    assert(tmp.get_prec() == req_precision);
    assert(sum.get_prec() == req_precision);
    assert(a.get_prec() == req_precision);
    assert(b.get_prec() == req_precision);
    assert(a_next.get_prec() == req_precision);
    assert(b_next.get_prec() == req_precision);
    assert(one.get_prec() == req_precision);
    assert(two.get_prec() == req_precision);

    return log2;
}
mpf_class log(const mpf_class &x) {
    mp_bitcnt_t req_precision = x.get_prec();
#if defined ___GMPXX_MKII_NOPRECCHANGE___
    assert(req_precision == mpf_get_default_prec());
#endif
    mpf_class zero(0.0, req_precision);
    mpf_class one(1.0, req_precision);
    mpf_class two(2.0, req_precision);
    mpf_class four(4.0, req_precision);

    mpf_class _log(zero);
    mpf_class epsilon(one), tmp(zero);
    mpf_class a(one), b(one);
    mpf_class a_next(zero), b_next(zero);
    mpf_class s(one);
    mpf_class _pi(const_pi(req_precision));
    mpf_class _log2(const_log2(req_precision));
    mp_exp_t m;
    bool converged = false;

    assert(_log.get_prec() == req_precision);
    assert(epsilon.get_prec() == req_precision);
    assert(tmp.get_prec() == req_precision);
    assert(a.get_prec() == req_precision);
    assert(b.get_prec() == req_precision);
    assert(a_next.get_prec() == req_precision);
    assert(b_next.get_prec() == req_precision);
    assert(s.get_prec() == req_precision);
    assert(_pi.get_prec() == req_precision);
    assert(_log2.get_prec() == req_precision);

    // calculating approximate log2 using arithmetic-geometric mean
    b = one;
    b.mul_2exp(req_precision / 2);
    s = b / x;
    mpf_get_d_2exp(&m, s.get_mpf_t());

    b = one;
    b.mul_2exp(m);
    s = x * b;

    b = four / s;
    epsilon.div_2exp(req_precision);
    int counter = 0;
    while (!converged) {
        counter++;
        a_next = (a + b) / two;
        b_next = sqrt(a * b);

        // Check for convergence
        if (abs(a - b) < epsilon) {
            converged = true;
        }
        a = a_next;
        b = b_next;
    }
    _log = _pi / (two * b) - m * _log2;

    assert(_log.get_prec() == req_precision);
    assert(epsilon.get_prec() == req_precision);
    assert(tmp.get_prec() == req_precision);
    assert(a.get_prec() == req_precision);
    assert(b.get_prec() == req_precision);
    assert(a_next.get_prec() == req_precision);
    assert(b_next.get_prec() == req_precision);
    assert(s.get_prec() == req_precision);
    assert(_pi.get_prec() == req_precision);
    assert(_log2.get_prec() == req_precision);

    return _log;
}
mpf_class exp(const mpf_class &x) {
    // https://www.mpfr.org/algorithms.pdf section 4.4
    mp_bitcnt_t req_precision = x.get_prec();
#if defined ___GMPXX_MKII_NOPRECCHANGE___
    assert(req_precision == mpf_get_default_prec());
#endif
    mpf_class zero(0.0, req_precision);
    mpf_class one(1.0, req_precision);
    mpf_class _exp(one);
    mpf_class _x(x);
    mpf_class r(zero);
    mpf_class _pi(const_pi(req_precision));
    mpf_class _log2(const_log2(req_precision));
    mp_exp_t k = 0, l = 0, n = 0;

    if (x < zero)
        _x = -_x; // avoid cancellation of significant digits
    // calculating approximate exp
    // taking modulo of log2
    mpf_get_d_2exp(&k, _x.get_mpf_t());
    if (k > 0) {
        _x.div_2exp(k);    // 0.5 <= |x| < 1
        _log2.div_2exp(k); // log2/2 = 0.346574
        n = floor(_x / _log2).get_si();
        r = _x - n * _log2;
        l = req_precision / k;
    } else {
        k = 0;
        l = req_precision;
        r = _x;
        n = 0;
    }
    for (int i = l; i > 0; i--) {
        _exp = one + ((r * _exp) / mpf_class(i, req_precision));
    }
    for (int i = 0; i < k; i++) {
        _exp = _exp * _exp;
    }
    if (n > 0)
        _exp.mul_2exp(n);
    if (n < 0)
        _exp.div_2exp(-n);
    if (x < zero)
        _exp = one / _exp; // avoid cancellation of significant digits
    return _exp;
}
#if !defined ___GMPXX_STRICT_COMPATIBILITY___
} // namespace gmp
#endif

// mpf_class operator"" _mpf (const char *str)
// mpz_class operator"" _mpz (const char *str)
// mpq_class operator"" _mpq (const char *str)
#if defined ___GMPXX_STRICT_COMPATIBILITY___
mpz_class operator"" _mpz(unsigned long long int val) { return mpz_class(static_cast<unsigned long int>(val)); }
mpq_class operator"" _mpq(unsigned long long int val) { return mpq_class(static_cast<unsigned long int>(val), static_cast<unsigned long int>(1)); }
mpf_class operator"" _mpf(long double val) { return mpf_class(static_cast<double>(val)); }
#else
gmp::mpz_class operator"" _mpz(unsigned long long int val) { return gmp::mpz_class(static_cast<unsigned long int>(val)); }
gmp::mpq_class operator"" _mpq(unsigned long long int val) { return gmp::mpq_class(static_cast<unsigned long int>(val), static_cast<unsigned long int>(1)); }
gmp::mpf_class operator"" _mpf(long double val) { return gmp::mpf_class(static_cast<double>(val)); }
#endif
// in the manual, the following functions are avilable, but actually not.
// cf. https://gmplib.org/manual/C_002b_002b-Interface-Rationals
// "With C++11 compilers, integral rationals can be constructed with the syntax 123_mpq which is equivalent to mpq_class(123_mpz). Other rationals can be built as -1_mpq/2 or 0xb_mpq/123456_mpz."
#if !defined ___GMPXX_POSSIBLE_BUGS___
gmp::mpz_class operator"" _mpz(const char *str, [[maybe_unused]] std::size_t length) { return gmp::mpz_class(str); }
gmp::mpq_class operator"" _mpq(const char *str, [[maybe_unused]] std::size_t length) { return gmp::mpq_class(str); }
gmp::mpf_class operator"" _mpf(const char *str, [[maybe_unused]] std::size_t length) { return gmp::mpf_class(str); }
#endif

#if defined ___GMPXX_STRICT_COMPATIBILITY___
void swap(mpz_class &op1, mpz_class &op2) noexcept { op1.swap(op2); }
void swap(mpq_class &op1, mpq_class &op2) noexcept { op1.swap(op2); }
void swap(mpf_class &op1, mpf_class &op2) noexcept { op1.swap(op2); }
#endif

#if defined ___GMPXX_STRICT_COMPATIBILITY___
int gmpxx_defaults::base;

class mpf_class_initializer {
  public:
    mpf_class_initializer() {
        gmpxx_defaults::set_default_prec(512);
        gmpxx_defaults::set_default_prec_raw(512);
        gmpxx_defaults::base = 10;
    }
};
#else
int gmp::gmpxx_defaults::base;

class mpf_class_initializer {
  public:
    mpf_class_initializer() {
        gmp::gmpxx_defaults::set_default_prec(512);
        gmp::gmpxx_defaults::set_default_prec_raw(512);
        gmp::gmpxx_defaults::base = 10;
    }
};
#endif

mpf_class_initializer global_mpf_class_initializer;
