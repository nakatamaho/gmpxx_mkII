// SPDX-License-Identifier: BSD-2-Clause

#include "gmpxx_mkII.h"

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <utility>
#include <type_traits>
#include <typeinfo>

static_assert(std::is_same_v<scalar_normalize_t<long long>, std::int64_t>);
static_assert(std::is_same_v<scalar_normalize_t<long>, std::int64_t>);
static_assert(std::is_same_v<scalar_normalize_t<int>, std::int64_t>);
static_assert(std::is_same_v<scalar_normalize_t<short>, std::int64_t>);
static_assert(std::is_same_v<scalar_normalize_t<signed char>, std::int64_t>);
static_assert(std::is_same_v<scalar_normalize_t<unsigned long long>, std::uint64_t>);
static_assert(std::is_same_v<scalar_normalize_t<std::size_t>, std::uint64_t>);
static_assert(std::is_same_v<scalar_normalize_t<unsigned>, std::uint64_t>);
static_assert(std::is_same_v<scalar_normalize_t<bool>, std::uint64_t>);
static_assert(std::is_same_v<scalar_normalize_t<float>, double>);
static_assert(std::is_same_v<scalar_normalize_t<double>, double>);

static_assert(phase0_operand<mpf_class>);
static_assert(phase0_operand<binary_expr<add_op, mpf_class, mpf_class>>);
static_assert(phase0_operand<unary_expr<neg_op, mpf_class>>);
static_assert(!phase0_operand<int>);
static_assert(!phase0_operand<double>);
static_assert(!phase0_operand<long long>);

static_assert(scalar_operand<int>);
static_assert(scalar_operand<long>);
static_assert(scalar_operand<std::size_t>);
static_assert(scalar_operand<double>);
static_assert(!scalar_operand<long double>);
static_assert(!scalar_operand<mpf_class>);
static_assert(!scalar_operand<binary_expr<add_op, mpf_class, mpf_class>>);

static_assert(phase1_operand<int>);
static_assert(phase1_operand<long>);
static_assert(phase1_operand<std::size_t>);
static_assert(phase1_operand<double>);
static_assert(phase1_operand<mpf_class>);
static_assert(phase1_operand<binary_expr<add_op, mpf_class, mpf_class>>);
static_assert(!phase1_operand<long double>);

static_assert(phase2_operand<int>);
static_assert(phase2_operand<mpf_class>);
static_assert(phase2_operand<mpz_class>);
static_assert(phase2_operand<mpq_class>);
static_assert(mpz_operand<mpz_class>);
static_assert(!mpz_operand<int>);
static_assert(!mpz_operand<mpf_class>);
static_assert(mpq_operand<mpq_class>);
static_assert(!mpq_operand<int>);
static_assert(!mpq_operand<mpf_class>);

using gmpxx_mkII_detail::result_type_t;
static_assert(std::same_as<result_type_t<mpf_class, mpz_class>, mpf_class>);
static_assert(std::same_as<result_type_t<mpf_class, mpq_class>, mpf_class>);
static_assert(std::same_as<result_type_t<mpz_class, mpz_class>, mpz_class>);
static_assert(std::same_as<result_type_t<mpz_class, mpq_class>, mpq_class>);
static_assert(std::same_as<result_type_t<mpq_class, mpz_class>, mpq_class>);
static_assert(std::same_as<result_type_t<mpz_class, int>, mpz_class>);
static_assert(std::same_as<result_type_t<int, mpq_class>, mpq_class>);
static_assert(std::same_as<result_type_t<mpz_class, double>, mpf_class>);
static_assert(std::same_as<result_type_t<double, mpq_class>, mpf_class>);
static_assert(gmpxx_mkII_detail::kind_of_v<
              binary_expr<add_op, mpf_class, mpz_class>> ==
              gmpxx_mkII_detail::value_kind::mpf);

static_assert(gmpxx_expr<binary_expr<add_op, mpf_class, mpf_class>>);
static_assert(gmpxx_expr<unary_expr<neg_op, mpf_class>>);
static_assert(!gmpxx_expr<mpf_class>);
static_assert(!gmpxx_expr<int>);

template<class T>
concept has_mpf_plus = requires(mpf_class a, T b) { a + b; };

static_assert(requires(mpf_class a, mpf_class b) { a + b; });
static_assert(has_mpf_plus<int>);
static_assert(has_mpf_plus<double>);
static_assert(!has_mpf_plus<long double>);
static_assert(std::is_same_v<decltype(1 + 2), int>);
static_assert(!gmpxx_expr<decltype(1 + 2)>);

static_assert(std::is_same_v<decltype(std::declval<mpf_class const&>() + 5LL),
                             binary_expr<add_op, mpf_class, std::int64_t>>);
static_assert(std::is_same_v<decltype(5u * std::declval<mpf_class const&>()),
                             binary_expr<mul_op, std::uint64_t, mpf_class>>);
static_assert(std::is_same_v<decltype(std::declval<mpf_class const&>() / 0.5f),
                             binary_expr<div_op, mpf_class, double>>);

int main() {
    mpf_class a, b;
    std::cout << "TYPE_ID(a+b) = " << typeid(a + b).name() << "\n";
    std::cout << "TYPE_ID(-a)  = " << typeid(-a).name() << "\n";
    std::cout << "TYPE_ID((a+b)*a) = " << typeid((a + b) * a).name() << "\n";
    std::cout << "Note: typeid().name() is compiler-specific and not directly "
                 "comparable across toolchains.\n";
    return 0;
}
