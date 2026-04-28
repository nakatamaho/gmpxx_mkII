// SPDX-License-Identifier: BSD-2-Clause

#include "gmpxx_mkII.h"

#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <iomanip>
#include <ios>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <type_traits>

namespace {

std::size_t gmp_alloc_count = 0;
std::size_t gmp_free_count = 0;

void* test_alloc(std::size_t n) {
    ++gmp_alloc_count;
    return std::malloc(n);
}

void* test_realloc(void* p, std::size_t, std::size_t n) {
    if (p == nullptr) {
        ++gmp_alloc_count;
    }
    return std::realloc(p, n);
}

void test_free(void* p, std::size_t) {
    ++gmp_free_count;
    std::free(p);
}

void reset_memory_counts() {
    gmp_alloc_count = 0;
    gmp_free_count = 0;
}

void assert_mpf_equal(mpf_class const& lhs, mpf_class const& rhs) {
    assert(mpf_cmp(lhs.get_mpf_t(), rhs.get_mpf_t()) == 0);
}

static_assert(requires(std::ostream& os, mpz_class const& z) {
    { os << z } -> std::same_as<std::ostream&>;
});
static_assert(requires(std::istream& is, mpz_class& z) {
    { is >> z } -> std::same_as<std::istream&>;
});
static_assert(requires(std::ostream& os, mpq_class const& q) {
    { os << q } -> std::same_as<std::ostream&>;
});
static_assert(requires(std::istream& is, mpq_class& q) {
    { is >> q } -> std::same_as<std::istream&>;
});
static_assert(requires(std::ostream& os, mpf_class const& f) {
    { os << f } -> std::same_as<std::ostream&>;
});
static_assert(requires(std::istream& is, mpf_class& f) {
    { is >> f } -> std::same_as<std::istream&>;
});
static_assert(requires(std::ostream& os, mpz_class const& a, mpz_class const& b) {
    { os << (a + b) } -> std::same_as<std::ostream&>;
});

template<class A, class B>
concept expression_extractable = requires(std::istream& is, A a, B b) {
    is >> (a + b);
};

static_assert(!expression_extractable<mpz_class, mpz_class>);

void test_mpz_strings() {
    mpz_class z("123456789012345678901234567890");
    assert(z.get_str() == "123456789012345678901234567890");
    assert(z.to_string() == z.get_str());
    assert(z.get_str(16) == "18ee90ff6c373e0ee4e3f0ad2");
    assert(z.get_str(8) == "143564417755415637016711617605322");

    mpz_class n("-255");
    assert(n.get_str(10) == "-255");
    assert(n.get_str(16) == "-ff");

    mpz_class x("123");
    assert(x.set_str("456") == 0);
    assert(x.get_str() == "456");
    assert(x.set_str("not-an-integer") != 0);
    assert(x.get_str() == "456");
}

void test_mpq_strings() {
    mpq_class q("2/4");
    assert(q.get_str() == "1/2");
    assert(q.to_string() == q.get_str());

    assert(q.set_str("-6/8") == 0);
    assert(q.get_str() == "-3/4");

    mpq_class i("42");
    assert(i.get_str() == "42");

    mpq_class old("1/3");
    assert(old.set_str("bad/rational") != 0);
    assert(old.get_str() == "1/3");
}

void test_mpf_strings() {
    mpf_class f(1.25, static_cast<mp_bitcnt_t>(128));
    mp_bitcnt_t old_prec = f.get_prec();

    mp_exp_t exp = 0;
    std::string digits = f.get_str(exp, 10);
    assert(!digits.empty());
    assert(f.get_prec() == old_prec);
    assert(!f.to_string().empty());
    assert(f.get_prec() == old_prec);

    mpf_class g(static_cast<mp_bitcnt_t>(128));
    assert(g.set_str("1.25") == 0);
    assert_mpf_equal(g, f);
    assert(g.get_prec() == old_prec);
    assert(g.set_str("-0.5") == 0);
    assert_mpf_equal(g, mpf_class(-0.5, old_prec));

    mpf_class unchanged(2.5, old_prec);
    assert(unchanged.set_str("not-a-float") != 0);
    assert_mpf_equal(unchanged, mpf_class(2.5, old_prec));
    assert(unchanged.get_prec() == old_prec);
}

void test_stream_output() {
    mpz_class z(std::int64_t{255});

    std::ostringstream dec;
    dec << z;
    assert(dec.str() == "255");

    std::ostringstream hex;
    hex << std::hex << z;
    assert(hex.str() == "ff");

    std::ostringstream upper_hex;
    upper_hex << std::uppercase << std::hex << z;
    assert(upper_hex.str() == "FF");

    std::ostringstream oct;
    oct << std::oct << z;
    assert(oct.str() == "377");

    mpq_class q("2/4");
    std::ostringstream q_os;
    q_os << q;
    assert(q_os.str() == "1/2");

    mpf_class f(1.25, static_cast<mp_bitcnt_t>(128));
    mp_bitcnt_t old_prec = f.get_prec();

    std::ostringstream f_default;
    f_default << f;
    assert(!f_default.str().empty());

    std::ostringstream f_sci;
    f_sci << std::scientific << std::setprecision(8) << f;
    assert(!f_sci.str().empty());
    assert(f_sci.str().find('e') != std::string::npos);

    std::ostringstream f_fixed;
    f_fixed << std::fixed << std::setprecision(3) << f;
    assert(!f_fixed.str().empty());
    assert(f_fixed.str().find('.') != std::string::npos);

    std::ostringstream f_upper;
    f_upper << std::uppercase << std::scientific << std::setprecision(3) << f;
    assert(!f_upper.str().empty());
    assert(f_upper.str().find('E') != std::string::npos);

    assert(f.get_prec() == old_prec);
}

void test_stream_input() {
    mpz_class z;
    std::istringstream z_in("12345");
    z_in >> z;
    assert(!z_in.fail());
    assert(z == mpz_class(std::int64_t{12345}));

    mpz_class z_fail(std::int64_t{7});
    std::istringstream z_bad("bad");
    z_bad >> z_fail;
    assert(z_bad.fail());
    assert(z_fail == mpz_class(std::int64_t{7}));

    mpz_class z_hex;
    std::istringstream z_hex_in("ff");
    z_hex_in >> std::hex >> z_hex;
    assert(z_hex == mpz_class(std::int64_t{255}));

    mpq_class q;
    std::istringstream q_in("2/4");
    q_in >> q;
    assert(!q_in.fail());
    assert(q == mpq_class("1/2"));

    mpq_class q_fail("1/3");
    std::istringstream q_bad("bad/rational");
    q_bad >> q_fail;
    assert(q_bad.fail());
    assert(q_fail == mpq_class("1/3"));

    mpf_class f(static_cast<mp_bitcnt_t>(128));
    mp_bitcnt_t old_prec = f.get_prec();
    std::istringstream f_in("1.25");
    f_in >> f;
    assert(!f_in.fail());
    assert(f.get_prec() == old_prec);
    assert_mpf_equal(f, mpf_class(1.25, old_prec));

    mpf_class f_fail(2.5, old_prec);
    std::istringstream f_bad("bad-float");
    f_bad >> f_fail;
    assert(f_bad.fail());
    assert(f_fail.get_prec() == old_prec);
    assert_mpf_equal(f_fail, mpf_class(2.5, old_prec));
}

void test_expression_output() {
    mpz_class a(std::int64_t{2});
    mpz_class b(std::int64_t{3});
    std::ostringstream z_os;
    z_os << (a + b);
    assert(z_os.str() == "5");

    mpq_class q1("1/2");
    mpq_class q2("1/3");
    std::ostringstream q_os;
    q_os << (q1 + q2);
    assert(q_os.str() == "5/6");

    mpf_class f1(1.25, static_cast<mp_bitcnt_t>(128));
    mpf_class f2(1.25, static_cast<mp_bitcnt_t>(128));
    std::ostringstream f_os;
    f_os << (f1 + f2);
    assert(!f_os.str().empty());

    std::ostringstream hex_expr;
    hex_expr << std::hex << (mpz_class(std::int64_t{10}) +
                             mpz_class(std::int64_t{5}));
    assert(hex_expr.str() == "f");
}

void test_gmp_string_frees() {
    mpz_class z("12345678901234567890");
    mpq_class q("2/4");
    mpf_class f(1.25, static_cast<mp_bitcnt_t>(128));

    reset_memory_counts();
    {
        std::string s = z.get_str();
        assert(!s.empty());
    }
    assert(gmp_alloc_count == gmp_free_count);

    reset_memory_counts();
    {
        std::string s = q.get_str();
        assert(!s.empty());
    }
    assert(gmp_alloc_count == gmp_free_count);

    reset_memory_counts();
    {
        mp_exp_t exp = 0;
        std::string s = f.get_str(exp);
        assert(!s.empty());
    }
    assert(gmp_alloc_count == gmp_free_count);

    reset_memory_counts();
    {
        std::ostringstream os;
        os << f;
        assert(!os.str().empty());
    }
    assert(gmp_alloc_count == gmp_free_count);
}

}  // namespace

int main() {
    mp_set_memory_functions(test_alloc, test_realloc, test_free);

    test_mpz_strings();
    test_mpq_strings();
    test_mpf_strings();
    test_stream_output();
    test_stream_input();
    test_expression_output();
    test_gmp_string_frees();
    return 0;
}
