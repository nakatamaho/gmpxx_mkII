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

#if defined(USE_ORIGINAL_GMPXX)
#include <gmpxx.h>
#else
#include "gmpxx_mkII.h"
#endif

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <stdexcept>

namespace benchmark_kernels {

#if defined(USE_ORIGINAL_GMPXX)
using ::gmp_randclass;
using ::mpf_class;
#else
using namespace gmpxx;
using gmpxx::gmp_randclass;
using gmpxx::mpf_class;
#endif

inline void set_default_precision(mp_bitcnt_t precision) {
#if defined(USE_ORIGINAL_GMPXX)
    mpf_set_default_prec(precision);
#else
    gmpxx::gmpxx_defaults::set_initial_default_prec(precision);
#endif
}

inline mp_bitcnt_t default_precision() {
#if defined(USE_ORIGINAL_GMPXX)
    return mpf_get_default_prec();
#else
    return gmpxx::gmpxx_defaults::get_default_prec();
#endif
}

inline mpf_class abs_value(mpf_class const& value) {
#if defined(USE_ORIGINAL_GMPXX)
    return abs(value);
#else
    return gmpxx::abs(value);
#endif
}

inline bool mlsame(char a, char b) {
    return std::toupper(static_cast<unsigned char>(a)) ==
           std::toupper(static_cast<unsigned char>(b));
}

inline void require(bool condition, char const* message) {
    if (!condition) {
        throw std::invalid_argument(message);
    }
}

inline mpf_class rdot(std::int64_t n, mpf_class const* dx,
                      std::int64_t incx, mpf_class const* dy,
                      std::int64_t incy) {
    mpf_class dtemp(0, default_precision());
    if (n <= 0) {
        return dtemp;
    }

    if (incx == 1 && incy == 1) {
        std::int64_t m = n % 5;
        for (std::int64_t i = 0; i < m; ++i) {
            dtemp += dx[i] * dy[i];
        }
        for (std::int64_t i = m; i < n; i += 5) {
            dtemp += dx[i] * dy[i] + dx[i + 1] * dy[i + 1] +
                     dx[i + 2] * dy[i + 2] + dx[i + 3] * dy[i + 3] +
                     dx[i + 4] * dy[i + 4];
        }
        return dtemp;
    }

    std::int64_t ix = incx < 0 ? (-n + 1) * incx : 0;
    std::int64_t iy = incy < 0 ? (-n + 1) * incy : 0;
    for (std::int64_t i = 0; i < n; ++i) {
        dtemp += dx[ix] * dy[iy];
        ix += incx;
        iy += incy;
    }
    return dtemp;
}

inline mpf_class rdot_reference(std::int64_t n, mpf_class const* dx,
                                mpf_class const* dy) {
    mpf_class result(0, default_precision());
    for (std::int64_t i = 0; i < n; ++i) {
        result += dx[i] * dy[i];
    }
    return result;
}

inline void raxpy(std::int64_t n, mpf_class const& da, mpf_class const* dx,
                  std::int64_t incx, mpf_class* dy, std::int64_t incy) {
    if (n <= 0 || da == mpf_class(0, da.get_prec())) {
        return;
    }

    if (incx == 1 && incy == 1) {
        std::int64_t m = n % 4;
        for (std::int64_t i = 0; i < m; ++i) {
            dy[i] += da * dx[i];
        }
        for (std::int64_t i = m; i < n; i += 4) {
            dy[i] += da * dx[i];
            dy[i + 1] += da * dx[i + 1];
            dy[i + 2] += da * dx[i + 2];
            dy[i + 3] += da * dx[i + 3];
        }
        return;
    }

    std::int64_t ix = incx < 0 ? (-n + 1) * incx : 0;
    std::int64_t iy = incy < 0 ? (-n + 1) * incy : 0;
    for (std::int64_t i = 0; i < n; ++i) {
        dy[iy] += da * dx[ix];
        ix += incx;
        iy += incy;
    }
}

inline void raxpy_reference(std::int64_t n, mpf_class const& alpha,
                            mpf_class const* x, mpf_class* y) {
    for (std::int64_t i = 0; i < n; ++i) {
        y[i] += alpha * x[i];
    }
}

inline void rgemv(char trans, std::int64_t m, std::int64_t n,
                  mpf_class const& alpha, mpf_class const* a,
                  std::int64_t lda, mpf_class const* x, std::int64_t incx,
                  mpf_class const& beta, mpf_class* y, std::int64_t incy) {
    require(mlsame(trans, 'N') || mlsame(trans, 'T') || mlsame(trans, 'C'),
            "Rgemv: unsupported transpose flag");
    require(m >= 0, "Rgemv: m must be non-negative");
    require(n >= 0, "Rgemv: n must be non-negative");
    require(lda >= std::max<std::int64_t>(1, m), "Rgemv: invalid lda");
    require(incx != 0, "Rgemv: incx must be nonzero");
    require(incy != 0, "Rgemv: incy must be nonzero");

    mpf_class zero(0, default_precision());
    mpf_class one(1, default_precision());
    if (m == 0 || n == 0 || (alpha == zero && beta == one)) {
        return;
    }

    std::int64_t lenx = mlsame(trans, 'N') ? n : m;
    std::int64_t leny = mlsame(trans, 'N') ? m : n;
    std::int64_t kx = incx > 0 ? 0 : -(lenx - 1) * incx;
    std::int64_t ky = incy > 0 ? 0 : -(leny - 1) * incy;

    if (beta != one) {
        std::int64_t iy = ky;
        for (std::int64_t i = 0; i < leny; ++i) {
            if (beta == zero) {
                y[iy] = zero;
            } else {
                y[iy] = beta * y[iy];
            }
            iy += incy;
        }
    }
    if (alpha == zero) {
        return;
    }

    if (mlsame(trans, 'N')) {
        std::int64_t jx = kx;
        for (std::int64_t j = 0; j < n; ++j) {
            mpf_class temp = alpha * x[jx];
            std::int64_t iy = ky;
            for (std::int64_t i = 0; i < m; ++i) {
                y[iy] += temp * a[i + j * lda];
                iy += incy;
            }
            jx += incx;
        }
        return;
    }

    std::int64_t jy = ky;
    for (std::int64_t j = 0; j < n; ++j) {
        mpf_class temp(0, default_precision());
        std::int64_t ix = kx;
        for (std::int64_t i = 0; i < m; ++i) {
            temp += a[i + j * lda] * x[ix];
            ix += incx;
        }
        y[jy] += alpha * temp;
        jy += incy;
    }
}

inline void rgemv_reference(std::int64_t m, std::int64_t n,
                            mpf_class const& alpha, mpf_class const* a,
                            std::int64_t lda, mpf_class const* x,
                            mpf_class const& beta, mpf_class* y) {
    for (std::int64_t i = 0; i < m; ++i) {
        mpf_class temp(0, default_precision());
        for (std::int64_t j = 0; j < n; ++j) {
            temp += a[i + j * lda] * x[j];
        }
        y[i] = alpha * temp + beta * y[i];
    }
}

inline void rgemm(char transa, char transb, std::int64_t m, std::int64_t n,
                  std::int64_t k, mpf_class const& alpha, mpf_class const* a,
                  std::int64_t lda, mpf_class const* b, std::int64_t ldb,
                  mpf_class const& beta, mpf_class* c, std::int64_t ldc) {
    bool nota = mlsame(transa, 'N');
    bool notb = mlsame(transb, 'N');
    std::int64_t nrowa = nota ? m : k;
    std::int64_t nrowb = notb ? k : n;

    require(nota || mlsame(transa, 'T') || mlsame(transa, 'C'),
            "Rgemm: unsupported transa flag");
    require(notb || mlsame(transb, 'T') || mlsame(transb, 'C'),
            "Rgemm: unsupported transb flag");
    require(m >= 0, "Rgemm: m must be non-negative");
    require(n >= 0, "Rgemm: n must be non-negative");
    require(k >= 0, "Rgemm: k must be non-negative");
    require(lda >= std::max<std::int64_t>(1, nrowa), "Rgemm: invalid lda");
    require(ldb >= std::max<std::int64_t>(1, nrowb), "Rgemm: invalid ldb");
    require(ldc >= std::max<std::int64_t>(1, m), "Rgemm: invalid ldc");

    mpf_class zero(0, default_precision());
    mpf_class one(1, default_precision());
    if (m == 0 || n == 0 || ((alpha == zero || k == 0) && beta == one)) {
        return;
    }

    if (alpha == zero) {
        for (std::int64_t j = 0; j < n; ++j) {
            for (std::int64_t i = 0; i < m; ++i) {
                if (beta == zero) {
                    c[i + j * ldc] = zero;
                } else {
                    c[i + j * ldc] = beta * c[i + j * ldc];
                }
            }
        }
        return;
    }

    for (std::int64_t j = 0; j < n; ++j) {
        for (std::int64_t i = 0; i < m; ++i) {
            mpf_class temp(0, default_precision());
            for (std::int64_t l = 0; l < k; ++l) {
                mpf_class aval = nota ? a[i + l * lda] : a[l + i * lda];
                mpf_class bval = notb ? b[l + j * ldb] : b[j + l * ldb];
                temp += aval * bval;
            }
            if (beta == zero) {
                c[i + j * ldc] = alpha * temp;
            } else {
                c[i + j * ldc] = alpha * temp + beta * c[i + j * ldc];
            }
        }
    }
}

inline void rgemm_reference(std::int64_t m, std::int64_t k, std::int64_t n,
                            mpf_class const& alpha, mpf_class const* a,
                            std::int64_t lda, mpf_class const* b,
                            std::int64_t ldb, mpf_class const& beta,
                            mpf_class* c, std::int64_t ldc) {
    for (std::int64_t j = 0; j < n; ++j) {
        for (std::int64_t i = 0; i < m; ++i) {
            c[i + j * ldc] = beta * c[i + j * ldc];
        }
    }

    for (std::int64_t i = 0; i < m; ++i) {
        for (std::int64_t j = 0; j < n; ++j) {
            mpf_class temp(0, default_precision());
            for (std::int64_t l = 0; l < k; ++l) {
                temp += a[i + l * lda] * b[l + j * ldb];
            }
            c[i + j * ldc] += alpha * temp;
        }
    }
}

}  // namespace benchmark_kernels
