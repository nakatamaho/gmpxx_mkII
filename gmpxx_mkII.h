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
    mpz_class() { mpz_init(value); }
    ~mpz_class() { mpz_clear(value); }

    mpz_class(unsigned long int op) {
        mpz_init(value);
        mpz_set_ui(value, op);
    }
    mpz_class(signed long int op) {
        mpz_init(value);
        mpz_set_si(value, op);
    }
    mpz_class(double op) {
        mpz_init(value);
        mpz_set_d(value, op);
    }
    mpz_class(const mpq_t op) {
        mpz_init(value);
        mpz_set_q(value, op);
    }
    mpz_class(const mpf_t op) {
        mpz_init(value);
        mpz_set_f(value, op);
    }
    mpz_class(const mpz_class &op) {
        mpz_init(value);
        mpz_set(value, op.value);
    }
    explicit mpz_class(const mpz_t z) {
        mpz_init(value);
        mpz_set(value, z);
    }
    explicit mpz_class(const char *str, int base = 0) {
        mpz_init(value);
        if (mpz_set_str(value, str, base) != 0) {
            std::cerr << "Error initializing mpz_class from const char*: " << str << std::endl;
            throw std::runtime_error("Failed to initialize mpz_class with given string.");
        }
    }
    explicit mpz_class(const std::string &str, int base = 0) {
        mpz_init(value);
        if (mpz_set_str(value, str.c_str(), base) != 0) {
            std::cerr << "Error initializing mpz_class from std::string: " << str << std::endl;
            throw std::runtime_error("Failed to initialize mpz_class with given string.");
        }
    }
    mpz_class &operator=(mpz_class op) noexcept {
        mpz_swap(value, op.value);
        return *this;
    }
    mpz_class &operator=(double d) noexcept {
        mpz_set_d(value, d);
        return *this;
    }
    mpz_class &operator=(signed long int op) noexcept {
        mpz_set_si(value, op);
        return *this;
    }
    mpz_class &operator=(unsigned long int op) noexcept {
        mpz_set_ui(value, op);
        return *this;
    }
    mpz_class &operator=(const char *str) {
        if (mpz_set_str(value, str, 0) != 0) {
            std::cerr << "Error assigning mpz_class from char:" << std::endl;
            throw std::runtime_error("Failed to initialize mpz_class with given string.");
        }
        return *this;
    }
    mpz_class &operator=(const std::string &str) {
        if (mpz_set_str(value, str.c_str(), 0) != 0) {
            std::cerr << "Error assigning mpz_class from string: " << str << std::endl;
            throw std::runtime_error("Failed to initialize mpz_class with given string.");
        }
        return *this;
    }
    mpz_class &operator=(const mpf_class &) = delete;
    mpz_t *get_mpz_t() { return &value; }
    mpz_srcptr get_mpz_t() const { return value; }

  private:
    mpz_t value;
};

class mpq_class {
  public:
    // constructor
    mpq_class() { mpq_init(value); }
    ~mpq_class() { mpq_clear(value); }

    mpq_class(unsigned long int op1, unsigned long int op2) {
        mpq_init(value);
        mpq_set_ui(value, op1, op2);
    }
    mpq_class(signed long int op1, signed long int op2) {
        mpq_init(value);
        mpq_set_si(value, op1, op2);
    }

  private:
    mpq_t value;
};

class mpf_class {
  public:
    ////////////////////////////////////////////////////////////////////////////////////////
    // 12.4 C++ Interface Floats
    ////////////////////////////////////////////////////////////////////////////////////////
    // constructor
    mpf_class() { mpf_init(value); }
    ~mpf_class() { mpf_clear(value); }
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
    mpf_class(const mpf_class &op) {
        mpf_init2(value, mpf_get_prec(op.value));
        mpf_set(value, op.value);
    }
    mpf_class(mpf_class &&op) noexcept { mpf_swap(value, op.value); }
    // mpf_class::mpf_class (type op, mp_bitcnt_t prec)
    mpf_class(const mpf_class &op, mp_bitcnt_t prec) {
        mpf_init2(value, prec);
        mpf_set(value, op.value);
    }
    ___MPF_CLASS_EXPLICIT___ mpf_class(unsigned long int op, mp_bitcnt_t prec = defaults::get_default_prec()) noexcept {
        mpf_init2(value, prec);
        mpf_set_ui(value, op);
    }
    ___MPF_CLASS_EXPLICIT___ mpf_class(unsigned int op, mp_bitcnt_t prec = defaults::get_default_prec()) noexcept {
        mpf_init2(value, prec);
        mpf_set_ui(value, (unsigned long)op);
    }
    ___MPF_CLASS_EXPLICIT___ mpf_class(signed long int op, mp_bitcnt_t prec = defaults::get_default_prec()) noexcept {
        mpf_init2(value, prec);
        mpf_set_si(value, op);
    }
    ___MPF_CLASS_EXPLICIT___ mpf_class(signed int op, mp_bitcnt_t prec = defaults::get_default_prec()) noexcept {
        mpf_init2(value, prec);
        mpf_set_si(value, (signed long)op);
    }
    ___MPF_CLASS_EXPLICIT___ mpf_class(double op, mp_bitcnt_t prec = defaults::get_default_prec()) noexcept {
        mpf_init2(value, prec);
        mpf_set_d(value, op);
    }
    // explicit mpf_class::mpf_class (const char *s)
    // mpf_class::mpf_class (const char *s, mp_bitcnt_t prec, int base = 0)
    // explicit mpf_class::mpf_class (const string& s)
    // mpf_class::mpf_class (const string& s, mp_bitcnt_t prec, int base = 0)
    mpf_class(const char *str) {
        mpf_init(value);
        if (mpf_set_str(value, str, defaults::base) != 0) {
            std::cerr << "Error initializing mpf_class from const char*: " << str << std::endl;
            throw std::runtime_error("Failed to initialize mpf_class with given string.");
        }
    }
    mpf_class(const char *str, mp_bitcnt_t prec, int base = defaults::base) {
        mpf_init2(value, prec);
        if (mpf_set_str(value, str, base) != 0) {
            std::cerr << "Error initializing mpf_class from const char*: " << str << std::endl;
            throw std::runtime_error("Failed to initialize mpf_class with given string.");
        }
    }
    mpf_class(const std::string &str) {
        mpf_init(value);
        if (mpf_set_str(value, str.c_str(), defaults::base) != 0) {
            std::cerr << "Error initializing mpf_class from std::string: " << str << std::endl;
            throw std::runtime_error("Failed to initialize mpf_class with given string.");
        }
    }
    mpf_class(const std::string &str, mp_bitcnt_t prec, int base = defaults::base) {
        mpf_init2(value, prec);
        if (mpf_set_str(value, str.c_str(), base) != 0) {
            std::cerr << "Error initializing mpf_class from std::string: " << str << std::endl;
            throw std::runtime_error("Failed to initialize mpf_class with given string.");
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
    friend void swap(mpf_class &op1, mpf_class &op2) { mpf_swap(op1.value, op2.value); }
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
    mpf_class &operator=(mpf_class op) noexcept {
#if defined __GMPXX_MKII_NOPRECCHANGE__
        mpf_swap(value, op.value);
#else
        mpf_init2(value, mpf_get_prec(this->get_mpf_t()));
        mpf_set(value, op.value);
#endif
        return *this;
    }
    mpf_class &operator=(double d) noexcept {
        mpf_set_d(value, d);
        return *this;
    }
    mpf_class &operator=(const char *str) {
        if (mpf_set_str(value, str, defaults::base) != 0) {
            std::cerr << "Error assigning mpf_class from char:" << std::endl;
            throw std::runtime_error("Failed to initialize mpf_class with given string.");
        }
        return *this;
    }
    mpf_class &operator=(const std::string &str) {
        if (mpf_set_str(value, str.c_str(), defaults::base) != 0) {
            std::cerr << "Error assigning mpf_class from string: " << str << std::endl;
            throw std::runtime_error("Failed to initialize mpf_class with given string.");
        }
        return *this;
    }
    friend inline mpf_class &operator+=(mpf_class &lhs, const mpf_class &rhs);
    friend inline mpf_class &operator-=(mpf_class &lhs, const mpf_class &rhs);
    friend inline mpf_class &operator*=(mpf_class &lhs, const mpf_class &rhs);
    friend inline mpf_class &operator/=(mpf_class &lhs, const mpf_class &rhs);
    friend inline mpf_class operator+(const mpf_class &op);
    friend inline mpf_class operator-(const mpf_class &op);
    friend inline mpf_class operator+(const mpf_class &lhs, const mpf_class &rhs);
    friend inline mpf_class operator-(const mpf_class &lhs, const mpf_class &rhs);
    friend inline mpf_class operator*(const mpf_class &lhs, const mpf_class &rhs);
    friend inline mpf_class operator/(const mpf_class &lhs, const mpf_class &rhs);

    friend inline bool operator==(const mpf_class &op1, const mpf_class &op2) { return mpf_cmp(op1.value, op2.value) == 0; }
    friend inline bool operator!=(const mpf_class &op1, const mpf_class &op2) { return mpf_cmp(op1.value, op2.value) != 0; }
    friend inline bool operator<(const mpf_class &op1, const mpf_class &op2) { return mpf_cmp(op1.value, op2.value) < 0; }
    friend inline bool operator>(const mpf_class &op1, const mpf_class &op2) { return mpf_cmp(op1.value, op2.value) > 0; }
    friend inline bool operator<=(const mpf_class &op1, const mpf_class &op2) { return mpf_cmp(op1.value, op2.value) <= 0; }
    friend inline bool operator>=(const mpf_class &op1, const mpf_class &op2) { return mpf_cmp(op1.value, op2.value) >= 0; }

    friend inline mpf_class &operator+=(mpf_class &lhs, double rhs);
    friend inline mpf_class &operator-=(mpf_class &lhs, double rhs);
    friend inline mpf_class &operator*=(mpf_class &lhs, double rhs);
    friend inline mpf_class &operator/=(mpf_class &lhs, double rhs);
    friend inline mpf_class operator+(const mpf_class &lhs, const double rhs);
    friend inline mpf_class operator+(const double lhs, const mpf_class &rhs);
    friend inline mpf_class operator-(const mpf_class &lhs, const double rhs);
    friend inline mpf_class operator-(const double lhs, const mpf_class &rhs);
    friend inline mpf_class operator*(const mpf_class &lhs, const double rhs);
    friend inline mpf_class operator*(const double lhs, const mpf_class &rhs);
    friend inline mpf_class operator/(const mpf_class &lhs, double rhs);
    friend inline mpf_class operator/(const double lhs, const mpf_class &rhs);

    friend std::ostream &operator<<(std::ostream &os, const mpf_class &m);

    mpf_srcptr get_mpf_t() const { return value; }
    operator mpz_class() const;

  private:
    mpf_t value;
};

mpf_class::operator mpz_class() const {
    mpz_class rop;
    mpz_set_f(*const_cast<mpz_t *>(rop.get_mpz_t()), this->get_mpf_t());
    return rop;
}

inline mp_bitcnt_t largerprec(const mpf_class &lhs, const mpf_class &rhs) {
    mp_bitcnt_t prec1 = lhs.get_prec(), prec2 = rhs.get_prec();
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
inline mpf_class operator+(const mpf_class &lhs, const mpf_class &rhs) {
#if defined __GMPXX_MKII_NOPRECCHANGE__
    mpf_class result;
    mpf_add(result.value, lhs.value, rhs.value);
#else
    mpf_class result;
    mp_bitcnt_t prec = largerprec(lhs, rhs);
    mpf_init2(result.value, prec);
    mpf_add(result.value, lhs.value, rhs.value);
#endif
    return result;
}
inline mpf_class operator-(const mpf_class &lhs, const mpf_class &rhs) {
#if defined __GMPXX_MKII_NOPRECCHANGE__
    mpf_class result;
    mpf_sub(result.value, lhs.value, rhs.value);
#else
    mpf_class result;
    mp_bitcnt_t prec = largerprec(lhs, rhs);
    mpf_init2(result.value, prec);
    mpf_sub(result.value, lhs.value, rhs.value);
#endif
    return result;
}
inline mpf_class operator*(const mpf_class &lhs, const mpf_class &rhs) {
#if defined __GMPXX_MKII_NOPRECCHANGE__
    mpf_class result;
    mpf_mul(result.value, lhs.value, rhs.value);
#else
    mpf_class result;
    mp_bitcnt_t prec = largerprec(lhs, rhs);
    mpf_init2(result.value, prec);
    mpf_mul(result.value, lhs.value, rhs.value);
#endif
    return result;
}
inline mpf_class operator/(const mpf_class &lhs, const mpf_class &rhs) {
#if defined __GMPXX_MKII_NOPRECCHANGE__
    mpf_class result;
    mpf_div(result.value, lhs.value, rhs.value);
#else
    mpf_class result;
    mp_bitcnt_t prec = largerprec(lhs, rhs);
    mpf_init2(result.value, prec);
    mpf_div(result.value, lhs.value, rhs.value);
#endif
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
#if defined __GMPXX_MKII_NOPRECCHANGE__
    mpf_class rop;
    rop = gmp::sqrt(op1 * op1 + op2 * op2);
#else
    mpf_class rop;
    mp_bitcnt_t prec = largerprec(op1, op2);
    mpf_init2(rop.value, prec);
    rop = gmp::sqrt(op1 * op1 + op2 * op2);
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
inline mpf_class operator+(const mpf_class &lhs, const double rhs) {
    mpf_class result(lhs);
    result += rhs;
    return result;
}
inline mpf_class operator+(const double lhs, const mpf_class &rhs) {
    mpf_class result(lhs);
    result += rhs;
    return result;
}
inline mpf_class operator-(const mpf_class &lhs, const double rhs) {
    mpf_class result(lhs);
    result -= rhs;
    return result;
}
inline mpf_class operator-(const double lhs, const mpf_class &rhs) {
    mpf_class result(lhs);
    result -= rhs;
    return result;
}
inline mpf_class operator*(const mpf_class &lhs, const double rhs) {
    mpf_class result = (lhs);
    result *= rhs;
    return result;
}
inline mpf_class operator*(const double lhs, const mpf_class &rhs) {
    mpf_class result(lhs);
    result *= rhs;
    return result;
}
inline mpf_class operator/(const mpf_class &lhs, const double rhs) {
    mpf_class result = lhs;
    result /= rhs;
    return result;
}
inline mpf_class operator/(const double lhs, const mpf_class &rhs) {
    mpf_class result(lhs);
    result /= rhs;
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

} // namespace gmp

// mpf_class operator"" _mpf (const char *str)
gmp::mpf_class operator"" _mpf(const char *str, [[maybe_unused]] std::size_t length) { return gmp::mpf_class(str); }

int gmp::defaults::base;

class mpf_class_initializer {
  public:
    mpf_class_initializer() {
        gmp::defaults::set_default_prec(512);
        gmp::defaults::set_default_prec_raw(512);
        gmp::defaults::base = 10;
    }
};

mpf_class_initializer global_mpf_class_initializer;
