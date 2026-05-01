/*
 * Copyright (c) 2026
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

/*
 * Example 11: Muller's recurrence.
 *
 * This recurrence is a compact finite-precision trap:
 *
 *     u_{n+1} = 111 - 1130 / u_n + 3000 / (u_n u_{n-1}),
 *     u_0 = 5.5,  u_1 = 61 / 11.
 *
 * In exact arithmetic, set u_n = y_{n+1} / y_n.  Then y_n satisfies
 *
 *     y_{n+2} = 111 y_{n+1} - 1130 y_n + 3000 y_{n-1},
 *
 * whose characteristic polynomial is
 *
 *     t^3 - 111 t^2 + 1130 t - 3000 = (t - 5)(t - 6)(t - 100).
 *
 * The prescribed initial values select the exact solution
 *
 *     y_n = (5^n + 6^n) / 2,
 *     u_n = (5^{n+1} + 6^{n+1}) / (5^n + 6^n),
 *
 * so u_n tends to 6.  Rounded arithmetic can introduce a tiny coefficient of
 * the 100^n mode.  That mode is absent in the exact sequence, but once present
 * it grows rapidly and can make the computed sequence drift toward 100.
 *
 * Jean-Michel Muller's original example is usually cited from:
 *
 * - Jean-Michel Muller, "Arithmetique des ordinateurs: operateurs et
 *   fonctions elementaires", Masson, 1989.  ISBN: 978-2225816895.
 *   This book predates routine DOI assignment for such examples.
 *
 * Related analysis of recurrences with a wrong finite-precision limiting
 * behavior:
 *
 * - Siegfried M. Rump, "On recurrences converging to the wrong limit in
 *   finite precision and some new examples", Electronic Transactions on
 *   Numerical Analysis 52, 358-369, 2020.
 *   DOI: 10.1553/etna_vol52s358
 *
 * General floating-point background:
 *
 * - David Goldberg, "What Every Computer Scientist Should Know About
 *   Floating-Point Arithmetic", ACM Computing Surveys 23(1), 5-48, 1991.
 *   DOI: 10.1145/103162.103163
 */

#include "gmpxx_mkII.h"

#include <array>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <vector>

namespace {

using gmpxx::mpf_class;

mpf_class pow_ui(mpf_class base, std::size_t exponent, mp_bitcnt_t precision) {
    mpf_class result(1, precision);
    while (exponent != 0) {
        if ((exponent & 1U) != 0U) {
            result *= base;
        }
        exponent >>= 1U;
        if (exponent != 0) {
            base *= base;
        }
    }
    return result;
}

mpf_class exact_reference(std::size_t n, mp_bitcnt_t precision) {
    mpf_class five(5, precision);
    mpf_class six(6, precision);
    mpf_class numerator =
        pow_ui(five, n + 1, precision) + pow_ui(six, n + 1, precision);
    mpf_class denominator =
        pow_ui(five, n, precision) + pow_ui(six, n, precision);
    return numerator / denominator;
}

double muller_step(double current, double previous) {
    return 111.0 - 1130.0 / current +
           3000.0 / (current * previous);
}

std::vector<double> double_sequence(std::size_t last_index) {
    std::vector<double> values(last_index + 1);
    values[0] = 5.5;
    values[1] = 61.0 / 11.0;
    for (std::size_t n = 1; n < last_index; ++n) {
        values[n + 1] = muller_step(values[n], values[n - 1]);
    }
    return values;
}

mpf_class muller_step(mpf_class const& current, mpf_class const& previous,
                      mp_bitcnt_t precision) {
    return mpf_class(111, precision) -
           mpf_class(1130, precision) / current +
           mpf_class(3000, precision) / (current * previous);
}

std::vector<mpf_class> mpf_sequence(std::size_t last_index,
                                    mp_bitcnt_t precision) {
    gmpxx::gmpxx_defaults::set_initial_default_prec(precision);

    std::vector<mpf_class> values;
    values.reserve(last_index + 1);
    values.emplace_back("5.5", precision);
    values.emplace_back(mpf_class(61, precision) / mpf_class(11, precision));
    for (std::size_t n = 1; n < last_index; ++n) {
        values.push_back(muller_step(values[n], values[n - 1], precision));
    }
    return values;
}

void print_double_row(std::size_t n, double value, mpf_class const& reference,
                      mp_bitcnt_t reference_precision) {
    mpf_class value_as_mpf(value, reference_precision);
    std::cout << std::setw(4) << n << "  "
              << std::setw(28) << value << "  "
              << std::setw(28) << reference << "  "
              << gmpxx::abs(value_as_mpf - reference) << '\n';
}

void print_mpf_row(std::size_t n, mpf_class const& value,
                   mpf_class const& reference) {
    std::cout << std::setw(4) << n << "  "
              << std::setw(28) << value << "  "
              << std::setw(28) << reference << "  "
              << gmpxx::abs(value - reference) << '\n';
}

void run_double_case(std::array<std::size_t, 9> const& sample_indices,
                     std::size_t last_index,
                     mp_bitcnt_t reference_precision) {
    std::vector<double> values = double_sequence(last_index);

    std::cout << "\ndouble recurrence\n";
    std::cout << "   n                    computed"
                 "                     exact                  |error|\n";
    for (std::size_t n : sample_indices) {
        print_double_row(n, values[n],
                         exact_reference(n, reference_precision),
                         reference_precision);
    }
}

void run_mpf_case(mp_bitcnt_t precision,
                  std::array<std::size_t, 9> const& sample_indices,
                  std::size_t last_index,
                  mp_bitcnt_t reference_precision) {
    std::vector<mpf_class> values = mpf_sequence(last_index, precision);

    std::cout << "\nmpf_class recurrence, precision = "
              << precision << " bits\n";
    std::cout << "   n                    computed"
                 "                     exact                  |error|\n";
    for (std::size_t n : sample_indices) {
        print_mpf_row(n, values[n],
                      exact_reference(n, reference_precision));
    }
}

}  // namespace

int main() {
    constexpr std::size_t last_index = 160;
    constexpr mp_bitcnt_t reference_precision = 1024;
    constexpr std::array<std::size_t, 9> sample_indices = {
        0, 1, 10, 20, 30, 50, 80, 120, 160};

    std::cout << std::scientific << std::setprecision(18);
    std::cout << "Muller's recurrence finite-precision example\n";
    std::cout << "u_{n+1} = 111 - 1130/u_n + 3000/(u_n u_{n-1})\n";
    std::cout << "u_0 = 5.5, u_1 = 61/11\n";
    std::cout << "Exact closed form tends to 6; finite precision can"
                 " inject the 100^n mode.\n";

    run_double_case(sample_indices, last_index, reference_precision);
    for (mp_bitcnt_t precision : {53, 128, 256, 512}) {
        run_mpf_case(precision, sample_indices, last_index,
                     reference_precision);
    }

    return 0;
}
