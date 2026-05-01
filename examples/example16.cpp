// SPDX-License-Identifier: BSD-2-Clause
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
 * Example 16: hexadecimal digits of log(2).
 *
 * This example computes log(2) with gmpxx_mkII's GMP-only transcendental
 * function and then extracts base-16 fractional digits.  The point is simple
 * but useful: hexadecimal digits map directly to binary precision, so asking
 * for n hex digits means asking for about 4n reliable bits after the binary
 * point.
 *
 * The extraction loop keeps the fractional part r of log(2), repeatedly forms
 *
 *     16 r = d + r_next,    d in {0, ..., 15},
 *
 * and prints d as a hexadecimal digit.  Guard bits are added before the
 * computation so that the final requested digit is not determined by a
 * barely-rounded value.
 *
 * Primary reference:
 *
 * - Richard P. Brent, "Fast multiple-precision evaluation of elementary
 *   functions", Journal of the ACM 23, 242-251, 1976.
 *   DOI: 10.1145/321941.321944
 */

#include "gmpxx_mkII.h"

#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>

namespace {

using gmpxx::mpf_class;
using gmpxx::mpz_class;

struct options {
    int hex_digits = 80;
    int group = 16;
    int guard_bits = 128;
};

void print_usage(char const* program) {
    std::cout
        << "Usage: " << program << " [options]\n"
        << "\n"
        << "Compute hexadecimal fractional digits of log(2).\n"
        << "\n"
        << "Options:\n"
        << "  --help          Show this help and exit.\n"
        << "  --digits N      Number of hexadecimal digits after the point "
           "(default: 80).\n"
        << "  --group N       Group output every N hex digits; 0 disables "
           "grouping (default: 16).\n"
        << "  --guard-bits N  Extra precision bits used internally "
           "(default: 128).\n"
        << "\n"
        << "Examples:\n"
        << "  " << program << "\n"
        << "  " << program << " --digits 256\n"
        << "  " << program << " --digits 1024 --group 32\n";
}

int parse_nonnegative_int(char const* option, char const* text) {
    try {
        std::string value(text);
        std::size_t parsed = 0;
        int result = std::stoi(value, &parsed);
        if (parsed != value.size() || result < 0) {
            throw std::invalid_argument("not a non-negative integer");
        }
        return result;
    } catch (std::exception const&) {
        throw std::invalid_argument(std::string(option) +
                                    " expects a non-negative integer");
    }
}

int parse_positive_int(char const* option, char const* text) {
    int result = parse_nonnegative_int(option, text);
    if (result == 0) {
        throw std::invalid_argument(std::string(option) +
                                    " expects a positive integer");
    }
    return result;
}

char const* require_value(int argc, char** argv, int& index) {
    if (index + 1 >= argc) {
        throw std::invalid_argument(std::string(argv[index]) +
                                    " requires a value");
    }
    ++index;
    return argv[index];
}

options parse_options(int argc, char** argv) {
    options result;

    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--help" || arg == "-h") {
            print_usage(argv[0]);
            std::exit(0);
        } else if (arg == "--digits") {
            result.hex_digits =
                parse_positive_int(argv[i], require_value(argc, argv, i));
        } else if (arg == "--group") {
            result.group =
                parse_nonnegative_int(argv[i], require_value(argc, argv, i));
        } else if (arg == "--guard-bits") {
            result.guard_bits =
                parse_nonnegative_int(argv[i], require_value(argc, argv, i));
        } else {
            throw std::invalid_argument("unknown option: " + arg);
        }
    }

    return result;
}

mp_bitcnt_t precision_for_hex_digits(int hex_digits, int guard_bits) {
    return static_cast<mp_bitcnt_t>(hex_digits) * 4U +
           static_cast<mp_bitcnt_t>(guard_bits);
}

char hex_digit(unsigned value) {
    static constexpr char digits[] = "0123456789abcdef";
    return digits[value & 0x0fU];
}

std::string hexadecimal_fraction(mpf_class const& value, int digits,
                                 int group, mp_bitcnt_t precision) {
    mpf_class fraction = value - gmpxx::floor(value);
    mpf_class sixteen(16, precision);
    std::string result;
    result.reserve(static_cast<std::size_t>(digits) +
                   static_cast<std::size_t>(digits / 16 + 4));

    for (int i = 0; i < digits; ++i) {
        fraction *= sixteen;
        mpz_class integer_digit(fraction);
        unsigned digit = integer_digit.get_ui();
        result.push_back(hex_digit(digit));
        fraction -= mpf_class(integer_digit, precision);

        if (group > 0 && i + 1 < digits && ((i + 1) % group) == 0) {
            result.push_back(' ');
        }
    }

    return result;
}

void run(options const& opts) {
    mp_bitcnt_t precision =
        precision_for_hex_digits(opts.hex_digits, opts.guard_bits);
    gmpxx::gmpxx_defaults::set_initial_default_prec(precision);

    mpf_class log_two = gmpxx::log_two(precision);
    std::string fraction =
        hexadecimal_fraction(log_two, opts.hex_digits, opts.group, precision);

    std::cout << "log(2) hexadecimal expansion\n";
    std::cout << "hex digits requested = " << opts.hex_digits << '\n';
    std::cout << "working precision    = " << precision << " bits\n";
    std::cout << "guard bits           = " << opts.guard_bits << "\n\n";
    std::cout << "log(2) = 0x0." << fraction << "p+0\n";
}

}  // namespace

int main(int argc, char** argv) {
    options opts;
    try {
        opts = parse_options(argc, argv);
    } catch (std::exception const& error) {
        std::cerr << "error: " << error.what() << "\n\n";
        print_usage(argv[0]);
        return 1;
    }

    run(opts);
    return 0;
}
