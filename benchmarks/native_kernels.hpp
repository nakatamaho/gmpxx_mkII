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

#pragma once

#include <gmp.h>

#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <iostream>

namespace native_kernels {

constexpr double MFLOPS = 1.0e6;

inline void init_vector(mpf_t* x, std::int64_t n, mp_bitcnt_t prec,
                        gmp_randstate_t state) {
    for (std::int64_t i = 0; i < n; ++i) {
        mpf_init2(x[i], prec);
        mpf_urandomb(x[i], state, prec);
    }
}

inline void clear_vector(mpf_t* x, std::int64_t n) {
    for (std::int64_t i = 0; i < n; ++i) {
        mpf_clear(x[i]);
    }
}

inline void copy_vector(mpf_t* dst, mpf_t const* src, std::int64_t n,
                        mp_bitcnt_t prec) {
    for (std::int64_t i = 0; i < n; ++i) {
        mpf_init2(dst[i], prec);
        mpf_set(dst[i], src[i]);
    }
}

inline void init_matrix(mpf_t* a, std::int64_t rows, std::int64_t cols,
                        std::int64_t ld, mp_bitcnt_t prec,
                        gmp_randstate_t state) {
    for (std::int64_t j = 0; j < cols; ++j) {
        for (std::int64_t i = 0; i < rows; ++i) {
            mpf_init2(a[i + j * ld], prec);
            mpf_urandomb(a[i + j * ld], state, prec);
        }
    }
}

inline void clear_matrix(mpf_t* a, std::int64_t rows, std::int64_t cols,
                         std::int64_t ld) {
    for (std::int64_t j = 0; j < cols; ++j) {
        for (std::int64_t i = 0; i < rows; ++i) {
            mpf_clear(a[i + j * ld]);
        }
    }
}

inline void copy_matrix(mpf_t* dst, mpf_t const* src, std::int64_t rows,
                        std::int64_t cols, std::int64_t ld,
                        mp_bitcnt_t prec) {
    for (std::int64_t j = 0; j < cols; ++j) {
        for (std::int64_t i = 0; i < rows; ++i) {
            mpf_init2(dst[i + j * ld], prec);
            mpf_set(dst[i + j * ld], src[i + j * ld]);
        }
    }
}

inline void rdot(std::int64_t n, mpf_t const* x, mpf_t const* y,
                 mpf_t result) {
    mpf_t temp;
    mpf_init2(temp, mpf_get_prec(result));
    mpf_set_ui(result, 0);
    for (std::int64_t i = 0; i < n; ++i) {
        mpf_mul(temp, x[i], y[i]);
        mpf_add(result, result, temp);
    }
    mpf_clear(temp);
}

inline void raxpy(std::int64_t n, mpf_t const alpha, mpf_t const* x,
                  mpf_t* y) {
    mpf_t temp;
    mpf_init2(temp, mpf_get_prec(alpha));
    for (std::int64_t i = 0; i < n; ++i) {
        mpf_mul(temp, alpha, x[i]);
        mpf_add(y[i], y[i], temp);
    }
    mpf_clear(temp);
}

inline void rgemv(std::int64_t m, std::int64_t n, mpf_t const alpha,
                  mpf_t const* a, std::int64_t lda, mpf_t const* x,
                  mpf_t const beta, mpf_t* y) {
    mpf_t temp;
    mpf_init2(temp, mpf_get_prec(alpha));
    for (std::int64_t i = 0; i < m; ++i) {
        mpf_mul(y[i], beta, y[i]);
    }
    for (std::int64_t j = 0; j < n; ++j) {
        for (std::int64_t i = 0; i < m; ++i) {
            mpf_mul(temp, alpha, a[i + j * lda]);
            mpf_mul(temp, temp, x[j]);
            mpf_add(y[i], y[i], temp);
        }
    }
    mpf_clear(temp);
}

inline void rgemm(std::int64_t m, std::int64_t k, std::int64_t n,
                  mpf_t const alpha, mpf_t const* a, std::int64_t lda,
                  mpf_t const* b, std::int64_t ldb, mpf_t const beta,
                  mpf_t* c, std::int64_t ldc) {
    mpf_t sum;
    mpf_t temp;
    mpf_init2(sum, mpf_get_prec(alpha));
    mpf_init2(temp, mpf_get_prec(alpha));

    for (std::int64_t j = 0; j < n; ++j) {
        for (std::int64_t i = 0; i < m; ++i) {
            mpf_mul(c[i + j * ldc], c[i + j * ldc], beta);
        }
    }

    for (std::int64_t i = 0; i < m; ++i) {
        for (std::int64_t j = 0; j < n; ++j) {
            mpf_set_ui(sum, 0);
            for (std::int64_t l = 0; l < k; ++l) {
                mpf_mul(temp, a[i + l * lda], b[l + j * ldb]);
                mpf_add(sum, sum, temp);
            }
            mpf_mul(sum, sum, alpha);
            mpf_add(c[i + j * ldc], c[i + j * ldc], sum);
        }
    }

    mpf_clear(sum);
    mpf_clear(temp);
}

inline bool less_than_threshold(mpf_t value, char const* threshold,
                                mp_bitcnt_t prec) {
    mpf_t limit;
    mpf_init2(limit, prec);
    mpf_set_str(limit, threshold, 10);
    int cmp = mpf_cmp(value, limit);
    mpf_clear(limit);
    return cmp < 0;
}

}  // namespace native_kernels
