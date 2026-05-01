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
 * Example 16: hexadecimal digits of log(2) and pi.
 *
 * This example computes log(2) or pi with gmpxx_mkII's GMP-only
 * transcendental functions and then extracts base-16 digits.  The point is
 * simple but useful: hexadecimal digits map directly to binary precision, so
 * asking for n hex digits means asking for about 4n reliable bits after the
 * binary point.
 *
 * The extraction loop keeps the fractional part r of the selected constant,
 * repeatedly forms
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
    std::string constant = "log2";
    std::string method = "auto";
    int start = 0;
    int hex_digits = 80;
    int group = 16;
    int guard_bits = 128;
};

void print_usage(char const* program) {
    std::cout
        << "Usage: " << program << " [options]\n"
        << "\n"
        << "Compute hexadecimal digits of log(2) or pi.\n"
        << "\n"
        << "Options:\n"
        << "  --help          Show this help and exit.\n"
        << "  --constant NAME Constant to compute: log2 or pi "
           "(default: log2).\n"
        << "  --start N       Start at hexadecimal fractional digit N; "
           "this is binary bit 4*N (default: 0).\n"
        << "  --method NAME   Digit method: auto, full, or bbp "
           "(default: auto).\n"
        << "  --digits N      Number of hexadecimal digits after the point "
           "(default: 80).\n"
        << "  --group N       Group output every N hex digits; 0 disables "
           "grouping (default: 16).\n"
        << "  --guard-bits N  Extra precision bits used internally "
           "(default: 128).\n"
        << "\n"
        << "Examples:\n"
        << "  " << program << "\n"
        << "  " << program << " --constant pi --digits 64\n"
        << "  " << program << " --constant pi --start 1000 --digits 64\n"
        << "  " << program
        << " --constant log2 --start 10000 --digits 128 --method bbp\n";
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
        } else if (arg == "--constant") {
            result.constant = require_value(argc, argv, i);
            if (result.constant != "log2" && result.constant != "pi") {
                throw std::invalid_argument(
                    "--constant expects log2 or pi");
            }
        } else if (arg == "--start") {
            result.start =
                parse_nonnegative_int(argv[i], require_value(argc, argv, i));
        } else if (arg == "--method") {
            result.method = require_value(argc, argv, i);
            if (result.method != "auto" && result.method != "full" &&
                result.method != "bbp") {
                throw std::invalid_argument(
                    "--method expects auto, full, or bbp");
            }
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

std::string resolved_method(options const& opts) {
    if (opts.method != "auto") {
        return opts.method;
    }
    return opts.start == 0 ? "full" : "bbp";
}

mpf_class selected_constant(options const& opts, mp_bitcnt_t precision) {
    if (opts.constant == "pi") {
        return gmpxx::const_pi(precision);
    }
    return gmpxx::log_two(precision);
}

std::string constant_label(options const& opts) {
    if (opts.constant == "pi") {
        return "pi";
    }
    return "log(2)";
}

std::string hexadecimal_prefix(mpf_class const& value) {
    mpz_class integer_part(value);
    return integer_part.get_str(16);
}

mp_bitcnt_t precision_for_hex_digits(int hex_digits, int guard_bits) {
    return static_cast<mp_bitcnt_t>(hex_digits) * 4U +
           static_cast<mp_bitcnt_t>(guard_bits);
}

mp_bitcnt_t precision_for_full_digits(int start, int hex_digits,
                                      int guard_bits) {
    return static_cast<mp_bitcnt_t>(start + hex_digits) * 4U +
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

std::string hexadecimal_digits_from_fraction(mpf_class fraction, int digits,
                                             int group,
                                             mp_bitcnt_t precision) {
    fraction -= gmpxx::floor(fraction);
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

void skip_hex_digits(mpf_class& fraction, int start, mp_bitcnt_t precision) {
    mpf_class sixteen(16, precision);
    for (int i = 0; i < start; ++i) {
        fraction *= sixteen;
        fraction -= gmpxx::floor(fraction);
    }
}

mpf_class fractional_part(mpf_class value) {
    return value - gmpxx::floor(value);
}

mpf_class pi_bbp_fraction(int start, mp_bitcnt_t precision) {
    auto series = [&](unsigned coefficient, unsigned offset) {
        mpf_class sum(0, precision);
        for (int k = 0; k <= start; ++k) {
            unsigned long modulus = static_cast<unsigned long>(8 * k + offset);
            mpz_class residue;
            mpz_class base(16);
            mpz_class mod(modulus);
            mpz_powm_ui(residue.get_mpz_t(), base.get_mpz_t(),
                        static_cast<unsigned long>(start - k),
                        mod.get_mpz_t());
            sum += mpf_class(coefficient, precision) *
                   mpf_class(residue, precision) /
                   mpf_class(modulus, precision);
            sum = fractional_part(sum);
        }

        mpf_class scale("0.0625", precision);
        mpf_class tolerance =
            gmpxx::pow(mpf_class(16, precision),
                       -mpf_class(precision / 4 + 4, precision));
        for (int k = start + 1; k < start + 100000; ++k) {
            unsigned long denominator =
                static_cast<unsigned long>(8 * k + offset);
            mpf_class term = mpf_class(coefficient, precision) * scale /
                             mpf_class(denominator, precision);
            sum += term;
            if (term < tolerance) {
                break;
            }
            scale /= mpf_class(16, precision);
        }

        return sum;
    };

    mpf_class result = series(4, 1) - series(2, 4) - series(1, 5) -
                       series(1, 6);
    return fractional_part(result);
}

mpf_class log2_bbp_fraction(int start, mp_bitcnt_t precision) {
    int bit_start = start * 4;
    mpf_class sum(0, precision);

    for (int k = 1; k <= bit_start; ++k) {
        mpz_class residue;
        mpz_class base(2);
        mpz_class mod(k);
        mpz_powm_ui(residue.get_mpz_t(), base.get_mpz_t(),
                    static_cast<unsigned long>(bit_start - k),
                    mod.get_mpz_t());
        sum += mpf_class(residue, precision) / mpf_class(k, precision);
        sum = fractional_part(sum);
    }

    mpf_class scale("0.5", precision);
    mpf_class tolerance =
        gmpxx::pow(mpf_class(16, precision),
                   -mpf_class(precision / 4 + 4, precision));
    for (int k = bit_start + 1; k < bit_start + 100000; ++k) {
        mpf_class term = scale / mpf_class(k, precision);
        sum += term;
        if (term < tolerance) {
            break;
        }
        scale /= mpf_class(2, precision);
    }

    return fractional_part(sum);
}

std::string full_method_digits(options const& opts, mp_bitcnt_t precision,
                               std::string& integer_hex) {
    mpf_class value = selected_constant(opts, precision);
    integer_hex = hexadecimal_prefix(value);
    mpf_class fraction = value - gmpxx::floor(value);
    skip_hex_digits(fraction, opts.start, precision);
    return hexadecimal_digits_from_fraction(fraction, opts.hex_digits,
                                            opts.group, precision);
}

std::string bbp_method_digits(options const& opts, mp_bitcnt_t precision) {
    mpf_class fraction =
        opts.constant == "pi" ? pi_bbp_fraction(opts.start, precision)
                              : log2_bbp_fraction(opts.start, precision);
    return hexadecimal_digits_from_fraction(fraction, opts.hex_digits,
                                            opts.group, precision);
}

void run(options const& opts) {
    std::string method = resolved_method(opts);
    mp_bitcnt_t precision = method == "full"
        ? precision_for_full_digits(opts.start, opts.hex_digits,
                                    opts.guard_bits)
        : precision_for_hex_digits(opts.hex_digits, opts.guard_bits);
    gmpxx::gmpxx_defaults::set_initial_default_prec(precision);

    std::string label = constant_label(opts);
    std::string integer_hex;
    std::string fraction = method == "full"
        ? full_method_digits(opts, precision, integer_hex)
        : bbp_method_digits(opts, precision);

    std::cout << label << " hexadecimal expansion\n";
    std::cout << "method               = " << method << '\n';
    std::cout << "hex start digit      = " << opts.start << '\n';
    std::cout << "binary start bit     = " << opts.start * 4 << '\n';
    std::cout << "hex digits requested = " << opts.hex_digits << '\n';
    std::cout << "working precision    = " << precision << " bits\n";
    std::cout << "guard bits           = " << opts.guard_bits << "\n\n";

    if (opts.start == 0) {
        if (integer_hex.empty()) {
            integer_hex = opts.constant == "pi" ? "3" : "0";
        }
        std::cout << label << " = 0x" << integer_hex << "." << fraction
                  << "p+0\n";
    } else {
        std::cout << label << " hex digits [" << opts.start << ", "
                  << opts.start + opts.hex_digits - 1 << "] = "
                  << fraction << '\n';
    }
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
