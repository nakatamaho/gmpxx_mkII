// SPDX-License-Identifier: BSD-2-Clause

#include "gmpxx_mkII.h"

#include <cstddef>
#include <cstdint>
#include <iostream>
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

static_assert(gmpxx_expr<binary_expr<add_op, mpf_class, mpf_class>>);
static_assert(gmpxx_expr<unary_expr<neg_op, mpf_class>>);
static_assert(!gmpxx_expr<mpf_class>);
static_assert(!gmpxx_expr<int>);

template<class T>
concept has_mpf_plus = requires(mpf_class a, T b) { a + b; };

static_assert(requires(mpf_class a, mpf_class b) { a + b; });
static_assert(!has_mpf_plus<int>);
static_assert(!has_mpf_plus<double>);

int main() {
    mpf_class a, b;
    std::cout << "TYPE_ID(a+b) = " << typeid(a + b).name() << "\n";
    std::cout << "TYPE_ID(-a)  = " << typeid(-a).name() << "\n";
    std::cout << "TYPE_ID((a+b)*a) = " << typeid((a + b) * a).name() << "\n";
    std::cout << "Note: typeid().name() is compiler-specific and not directly "
                 "comparable across toolchains.\n";
    return 0;
}
