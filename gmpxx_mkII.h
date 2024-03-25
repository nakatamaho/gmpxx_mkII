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
    static mp_bitcnt_t prec_raw;
    static int base;
    static inline void set_default_prec(const mp_bitcnt_t _prec) { mpf_set_default_prec(_prec); }
    static inline void set_default_prec_raw(const mp_bitcnt_t prec_raw) { mpf_set_default_prec(prec_raw); }
    static inline mp_bitcnt_t get_default_prec() { return mpf_get_default_prec(); }
    static inline void set_default_base(const int _base) { base = _base; }
};

class mpf_class {
  public:
    ////////////////////////////////////////////////////////////////////////////////////////
    // 12.4 C++ Interface Floats
    ////////////////////////////////////////////////////////////////////////////////////////
    // bool mpf_class::fits_sint_p (void)
    // bool mpf_class::fits_slong_p (void)
    // bool mpf_class::fits_sshort_p (void)
    // bool mpf_class::fits_uint_p (void)
    // bool mpf_class::fits_ulong_p (void)
    // bool mpf_class::fits_ushort_p (void)
    // string mpf_class::get_str (mp_exp_t& exp, int base = 10, size_t digits = 0)
    // mpf_class trunc (mpf_class op)

    // constructor
    mpf_class() { mpf_init(value); }
    ~mpf_class() { mpf_clear(value); }
    // explicit mpf_class::mpf_class (const mpf_t f)
    explicit mpf_class(const mpf_t f) {
        mpf_init(value);
        mpf_set(value, f);
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
    ___MPF_CLASS_EXPLICIT___ mpf_class(unsigned long int op, mp_bitcnt_t prec = defaults::prec) noexcept {
        mpf_init2(value, prec);
        mpf_set_ui(value, op);
    }
    ___MPF_CLASS_EXPLICIT___ mpf_class(unsigned int op, mp_bitcnt_t prec = defaults::prec) noexcept {
        mpf_init2(value, prec);
        mpf_set_ui(value, (unsigned long)op);
    }
    ___MPF_CLASS_EXPLICIT___ mpf_class(signed long int op, mp_bitcnt_t prec = defaults::prec) noexcept {
        mpf_init2(value, prec);
        mpf_set_si(value, op);
    }
    ___MPF_CLASS_EXPLICIT___ mpf_class(signed int op, mp_bitcnt_t prec = defaults::prec) noexcept {
        mpf_init2(value, prec);
        mpf_set_si(value, (signed long)op);
    }
    ___MPF_CLASS_EXPLICIT___ mpf_class(double op, mp_bitcnt_t prec = defaults::prec) noexcept {
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
            std::cerr << "Error initializing mpf_t from const char*: " << str << std::endl;
            throw std::runtime_error("Failed to initialize mpf_t with given string.");
        }
    }
    mpf_class(const char *str, mp_bitcnt_t prec, int base = defaults::base) {
        mpf_init2(value, prec);
        if (mpf_set_str(value, str, base) != 0) {
            std::cerr << "Error initializing mpf_t from const char*: " << str << std::endl;
            throw std::runtime_error("Failed to initialize mpf_t with given string.");
        }
    }
    mpf_class(const std::string &str) {
        mpf_init(value);
        if (mpf_set_str(value, str.c_str(), defaults::base) != 0) {
            std::cerr << "Error initializing mpf_t from std::string: " << str << std::endl;
            throw std::runtime_error("Failed to initialize mpf_t with given string.");
        }
    }
    mpf_class(const std::string &str, mp_bitcnt_t prec, int base = defaults::base) {
        mpf_init2(value, prec);
        if (mpf_set_str(value, str.c_str(), base) != 0) {
            std::cerr << "Error initializing mpf_t from std::string: " << str << std::endl;
            throw std::runtime_error("Failed to initialize mpf_t with given string.");
        }
    }
    // int mpf_class::set_str (const char *str, int base)
    // int mpf_class::set_str (const string& str, int base)
    int set_str(const char *str, int base) { return mpf_set_str(value, str, base); }
    int set_str(const std::string &str, int base) { return mpf_set_str(value, str.c_str(), base); }

    // mpf_class sqrt (mpf_class op)
    // mpf_class abs (mpf_class op)
    // mpf_class ceil (mpf_class op)
    // mpf_class floor (mpf_class op)
    // mpf_class hypot (mpf_class op1, mpf_class op2)
    // int sgn (mpf_class op)
    friend mpf_class sqrt(const mpf_class &op);
    friend mpf_class abs(const mpf_class &op);
    friend mpf_class ceil(const mpf_class &op);
    friend mpf_class floor(const mpf_class &op);
    friend mpf_class hypot(const mpf_class &op1, const mpf_class &op2);
    friend mpf_class neg(const mpf_class &op);
    friend int sgn(const mpf_class &op);

    // mpf_class trunc (mpf_class op)
    friend mpf_class trunc(const mpf_class &op);

    // void swap (mpf_class& op1, mpf_class& op2)
    void swap(mpf_class &op) { mpf_swap(this->value, op.value); }

    // Initialization using assignment operator
    // Copy-and-Swap Idiom; it does both the copy assignment operator and the move assignment operator.
    // mpf_class& mpf_class::operator= (type op)
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

    friend inline bool operator==(const mpf_class &op1, const mpf_class &op2) { return mpf_cmp(op1.value, op2.value) == 0; }
    friend inline bool operator!=(const mpf_class &op1, const mpf_class &op2) { return mpf_cmp(op1.value, op2.value) != 0; }
    friend inline bool operator<(const mpf_class &op1, const mpf_class &op2) { return mpf_cmp(op1.value, op2.value) < 0; }
    friend inline bool operator>(const mpf_class &op1, const mpf_class &op2) { return mpf_cmp(op1.value, op2.value) > 0; }
    friend inline bool operator<=(const mpf_class &op1, const mpf_class &op2) { return mpf_cmp(op1.value, op2.value) <= 0; }
    friend inline bool operator>=(const mpf_class &op1, const mpf_class &op2) { return mpf_cmp(op1.value, op2.value) >= 0; }

    mpf_class &operator+=(double rhs) {
        mpf_t temp;
        mpf_init_set_d(temp, rhs);
        mpf_add(value, value, temp);
        mpf_clear(temp);
        return *this;
    }
    mpf_class &operator-=(double rhs) {
        mpf_t temp;
        mpf_init_set_d(temp, rhs);
        mpf_sub(value, value, temp);
        mpf_clear(temp);
        return *this;
    }
    mpf_class &operator*=(double rhs) {
        mpf_t temp;
        mpf_init_set_d(temp, rhs);
        mpf_mul(value, value, temp);
        mpf_clear(temp);
        return *this;
    }
    mpf_class &operator/=(double rhs) {
        mpf_t temp;
        mpf_init_set_d(temp, rhs);
        mpf_div(value, value, temp);
        mpf_clear(temp);
        return *this;
    }
    friend mpf_class operator+(const mpf_class &lhs, const double rhs);
    friend mpf_class operator+(const double lhs, const mpf_class &rhs);
    friend mpf_class operator-(const mpf_class &lhs, const double rhs);
    friend mpf_class operator-(const double lhs, const mpf_class &rhs);
    friend mpf_class operator*(const mpf_class &lhs, const double rhs);
    friend mpf_class operator*(const double lhs, const mpf_class &rhs);
    friend mpf_class operator/(const mpf_class &lhs, double rhs);
    friend mpf_class operator/(const double lhs, const mpf_class &rhs);

    friend std::ostream &operator<<(std::ostream &os, const mpf_class &m);

    mpf_srcptr get_mpf_t() const { return value; }
    // double mpf_class::get_d (void)
    // unsigned long mpf_class::get_ui (void)
    // long mpf_class::get_si (void)
    // mp_bitcnt_t mpf_class::get_prec ()
    // void mpf_class::set_prec (mp_bitcnt_t prec)
    // void mpf_class::set_prec_raw (mp_bitcnt_t prec)
    double get_d() const { return mpf_get_d(value); }
    unsigned long get_ui() const { return mpf_get_ui(value); }
    long get_si() const { return mpf_get_si(value); }
    mp_bitcnt_t get_prec() const { return mpf_get_prec(value); }
    void set_prec(mp_bitcnt_t prec) { mpf_set_prec(value, prec); }
    void set_prec_raw(mp_bitcnt_t prec) { mpf_set_prec_raw(value, prec); }

  private:
    mpf_t value;
};

inline mpf_class trunc(const mpf_class &op) {
    mpf_class rop;
    mpf_trunc(rop.value, op.get_mpf_t());
    return rop;
}

inline mpf_class sqrt(const mpf_class &op) {
    mpf_class rop;
    mpf_sqrt(rop.value, op.get_mpf_t());
    return rop;
}
inline mpf_class neg(const mpf_class &op) {
    mpf_class rop;
    mpf_neg(rop.value, op.get_mpf_t());
    return rop;
}
inline mpf_class abs(const mpf_class &op) {
    mpf_class rop;
    mpf_abs(rop.value, op.get_mpf_t());
    return rop;
}
inline mpf_class ceil(const mpf_class &op) {
    mpf_class rop;
    mpf_ceil(rop.value, op.get_mpf_t());
    return rop;
}
inline mpf_class floor(const mpf_class &op) {
    mpf_class rop;
    mpf_floor(rop.value, op.get_mpf_t());
    return rop;
}
inline mpf_class hypot(const mpf_class &op1, const mpf_class &op2) {
    mpf_class rop;
    rop = gmp::sqrt(op1 * op1 + op2 * op2);
    return rop;
}
inline int sgn(const mpf_class &op) {
    int flag = mpf_sgn(op.get_mpf_t());
    return flag;
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

} // namespace gmp

// mpf_class operator"" _mpf (const char *str)
gmp::mpf_class operator"" _mpf(const char *str, [[maybe_unused]] std::size_t length) { return gmp::mpf_class(str); }

mp_bitcnt_t gmp::defaults::prec;
mp_bitcnt_t gmp::defaults::prec_raw;
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
