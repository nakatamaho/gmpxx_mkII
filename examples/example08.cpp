// SPDX-License-Identifier: BSD-2-Clause

#include "gmpxx_mkII.h"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace {

using gmpxx::mpf_class;
using gmpxx::mpfc_class;

mp_bitcnt_t bits_for_decimal_digits(int digits, int guard_bits) {
    double raw_bits = std::ceil(static_cast<double>(digits) * std::log2(10.0));
    return static_cast<mp_bitcnt_t>(raw_bits) +
           static_cast<mp_bitcnt_t>(guard_bits);
}

mpfc_class make_complex(mpf_class const& real_value,
                        mpf_class const& imag_value) {
    return mpfc_class(real_value, imag_value);
}

mpfc_class make_real(mpf_class const& real_value, mp_bitcnt_t precision) {
    return make_complex(real_value, mpf_class(0, precision));
}

std::vector<mpf_class> wilkinson_coefficients(std::size_t degree,
                                              mp_bitcnt_t precision) {
    std::vector<mpf_class> coefficients(1, mpf_class(1, precision));

    for (std::size_t root = 1; root <= degree; ++root) {
        std::vector<mpf_class> next(coefficients.size() + 1,
                                    mpf_class(0, precision));
        mpf_class root_value(static_cast<unsigned long>(root), precision);
        for (std::size_t i = 0; i < coefficients.size(); ++i) {
            next[i] -= coefficients[i] * root_value;
            next[i + 1] += coefficients[i];
        }
        coefficients = next;
    }

    return coefficients;
}

mpfc_class evaluate_polynomial(std::vector<mpf_class> const& coefficients,
                               mpfc_class const& z, mp_bitcnt_t precision) {
    if (coefficients.empty()) {
        return make_real(mpf_class(0, precision), precision);
    }

    mpfc_class value = make_real(coefficients.back(), precision);
    for (std::size_t k = coefficients.size() - 1; k-- > 0;) {
        value *= z;
        value += coefficients[k];
    }
    return value;
}

mpfc_class evaluate_derivative(std::vector<mpf_class> const& coefficients,
                               mpfc_class const& z, mp_bitcnt_t precision) {
    if (coefficients.size() < 2) {
        return make_real(mpf_class(0, precision), precision);
    }

    std::size_t degree = coefficients.size() - 1;
    mpfc_class value = make_real(
        coefficients[degree] *
            mpf_class(static_cast<unsigned long>(degree), precision),
        precision);

    for (std::size_t k = degree - 1; k > 0; --k) {
        value *= z;
        mpf_class derivative_coefficient =
            coefficients[k] *
            mpf_class(static_cast<unsigned long>(k), precision);
        value += derivative_coefficient;
    }
    return value;
}

std::vector<mpfc_class> initial_wilkinson_guesses(std::size_t degree,
                                                  mp_bitcnt_t precision) {
    std::vector<mpfc_class> guesses;
    guesses.reserve(degree);

    mpf_class imag_scale("0.01", precision);
    for (std::size_t i = 1; i <= degree; ++i) {
        mpf_class real(static_cast<unsigned long>(i), precision);
        mpf_class imag =
            imag_scale / mpf_class(static_cast<unsigned long>(i), precision);
        guesses.push_back(make_complex(real, imag));
    }
    return guesses;
}

std::vector<mpfc_class> solve_with_aberth(
    std::vector<mpf_class> const& coefficients,
    std::vector<mpfc_class> roots, mpf_class const& tolerance,
    int max_iterations, mp_bitcnt_t precision) {
    if (coefficients.size() < 2) {
        throw std::invalid_argument("polynomial degree must be at least one");
    }

    std::size_t degree = coefficients.size() - 1;
    if (roots.size() != degree) {
        throw std::invalid_argument("initial root count must match degree");
    }

    for (int iteration = 1; iteration <= max_iterations; ++iteration) {
        mpf_class max_update(0, precision);
        std::vector<mpfc_class> next_roots = roots;

        for (std::size_t i = 0; i < degree; ++i) {
            mpfc_class f =
                evaluate_polynomial(coefficients, roots[i], precision);
            mpfc_class df =
                evaluate_derivative(coefficients, roots[i], precision);
            mpfc_class ratio = f / df;

            mpfc_class one = make_real(mpf_class(1, precision), precision);
            mpfc_class repulsion =
                make_real(mpf_class(0, precision), precision);
            for (std::size_t j = 0; j < degree; ++j) {
                if (i != j) {
                    mpfc_class separation = roots[i] - roots[j];
                    repulsion += one / separation;
                }
            }

            mpfc_class correction = ratio / (one - ratio * repulsion);
            next_roots[i] = roots[i] - correction;

            mpf_class update_size = gmpxx::abs(correction);
            if (update_size > max_update) {
                max_update = update_size;
            }
        }

        roots = next_roots;
        if (iteration <= 6 || iteration % 5 == 0) {
            std::cout << "iteration " << std::setw(2) << iteration
                      << ", max correction = " << max_update << '\n';
        }
        if (max_update < tolerance) {
            break;
        }
    }

    return roots;
}

void sort_by_real_part(std::vector<mpfc_class>& roots) {
    std::sort(roots.begin(), roots.end(),
              [](mpfc_class const& a, mpfc_class const& b) {
                  return a.real() < b.real();
              });
}

void print_roots(char const* title, std::vector<mpfc_class> roots,
                 std::vector<mpf_class> const& coefficients,
                 mp_bitcnt_t precision) {
    sort_by_real_part(roots);
    std::cout << '\n' << title << '\n';
    std::cout << " index                 root"
                 "                         |f(root)|\n";

    for (std::size_t i = 0; i < roots.size(); ++i) {
        mpfc_class residual =
            evaluate_polynomial(coefficients, roots[i], precision);
        std::cout << std::setw(5) << (i + 1) << "  " << roots[i]
                  << "  " << gmpxx::abs(residual) << '\n';
    }
}

void print_root_shift(std::vector<mpfc_class> exact_roots,
                      std::vector<mpfc_class> perturbed_roots) {
    sort_by_real_part(exact_roots);
    sort_by_real_part(perturbed_roots);

    std::cout << "\nRoot movement after perturbing the x^19 coefficient\n";
    std::cout << " index            |delta root|\n";
    for (std::size_t i = 0; i < exact_roots.size(); ++i) {
        mpf_class shift = gmpxx::abs(perturbed_roots[i] - exact_roots[i]);
        std::cout << std::setw(5) << (i + 1) << "  " << shift << '\n';
    }
}

}  // namespace

int main() {
    constexpr std::size_t degree = 20;
    constexpr int decimal_digits = 60;
    const mp_bitcnt_t precision = bits_for_decimal_digits(decimal_digits, 192);

    gmpxx::gmpxx_defaults::set_initial_default_prec(precision);

    std::vector<mpf_class> exact = wilkinson_coefficients(degree, precision);
    std::vector<mpf_class> perturbed = exact;
    perturbed[degree - 1] += mpf_class("1e-10", precision);

    mpf_class tolerance("1e-45", precision);

    std::cout << std::scientific << std::setprecision(decimal_digits);
    std::cout << "Wilkinson polynomial example using gmpxx::mpfc_class\n";
    std::cout << "W_20(x) = product_{k=1}^{20} (x-k)\n";
    std::cout << "The second solve perturbs only the x^19 coefficient by"
                 " 1e-10.\n\n";

    std::cout << "Solving exact W_20\n";
    std::vector<mpfc_class> exact_roots = solve_with_aberth(
        exact, initial_wilkinson_guesses(degree, precision), tolerance, 40,
        precision);

    std::cout << "\nSolving perturbed W_20\n";
    std::vector<mpfc_class> perturbed_roots = solve_with_aberth(
        perturbed, initial_wilkinson_guesses(degree, precision), tolerance, 40,
        precision);

    print_roots("Exact polynomial roots", exact_roots, exact, precision);
    print_roots("Perturbed polynomial roots", perturbed_roots, perturbed,
                precision);
    print_root_shift(exact_roots, perturbed_roots);

    return 0;
}
