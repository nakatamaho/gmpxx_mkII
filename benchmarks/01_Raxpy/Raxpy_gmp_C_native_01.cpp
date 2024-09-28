#include <iostream>
#include <chrono>
#include <gmp.h>

#if defined USE_ORIGINAL_GMPXX
#include <gmpxx.h>
#else
#include "gmpxx_mkII.h"
#if !defined ___GMPXX_STRICT_COMPATIBILITY___
using namespace gmp;
#endif
#endif

#include "Raxpy.hpp"

#define MFLOPS 1e+6

gmp_randstate_t state;

void _Raxpy(int64_t n, const mpf_t alpha, mpf_t *x, int64_t incx, mpf_t *y, int64_t incy) {
    if (incx != 1 || incy != 1) {
        std::cerr << "Increments other than 1 are not supported." << std::endl;
        exit(EXIT_FAILURE);
    }

    mpf_t temp;
    mpf_init(temp);

    for (int64_t i = 0; i < n; ++i) {
        mpf_mul(temp, alpha, x[i]); // temp = alpha * x[i]
        mpf_add(y[i], y[i], temp);  // y[i] = y[i] + temp
    }

    mpf_clear(temp);
}

void init_mpf_vec(mpf_t *vec, int64_t n, int prec) {
    for (int64_t i = 0; i < n; ++i) {
        mpf_init2(vec[i], prec);
        mpf_urandomb(vec[i], state, prec); // 0 <= vec[i] < 1
    }
}

void clear_mpf_vec(mpf_t *vec, int64_t n) {
    for (int64_t i = 0; i < n; ++i) {
        mpf_clear(vec[i]);
    }
}

int main(int argc, char **argv) {
    gmp_randinit_default(state);
    gmp_randseed_ui(state, 42);

    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <vector size> <precision>" << std::endl;
        return EXIT_FAILURE;
    }

    int64_t N = std::atoll(argv[1]);
    int prec = std::atoi(argv[2]);
    mpf_set_default_prec(prec);

    mpf_t *x = new mpf_t[N];
    mpf_t *y = new mpf_t[N];
    mpf_t alpha;
    mpf_init2(alpha, prec);
    mpf_urandomb(alpha, state, prec);

    init_mpf_vec(x, N, prec);
    init_mpf_vec(y, N, prec);

    mpf_class *x_mpf_class = new mpf_class[N];
    mpf_class *y_mpf_class = new mpf_class[N];
    mpf_class alpha_class = mpf_class(alpha);

    for (int64_t i = 0; i < N; ++i) {
        x_mpf_class[i] = mpf_class(x[i]);
        y_mpf_class[i] = mpf_class(y[i]);
    }

    auto start = std::chrono::high_resolution_clock::now();
    _Raxpy(N, alpha, x, 1, y, 1);
    auto end = std::chrono::high_resolution_clock::now();

    Raxpy(N, alpha_class, x_mpf_class, 1, y_mpf_class, 1);

    std::chrono::duration<double> elapsed_seconds = end - start;
    double mflops = (2.0 * double(N)) / (elapsed_seconds.count() * MFLOPS);

    std::cout << "Elapsed time: " << elapsed_seconds.count() << " s" << std::endl;
    std::cout << "MFLOPS: " << mflops << std::endl;

    mpf_class l1_norm = 0;
    for (int64_t i = 0; i < N; ++i) {
        mpf_class diff = abs(mpf_class(y[i]) - y_mpf_class[i]);
        l1_norm += diff;
    }

    std::cout << "L1 Norm of difference: " << l1_norm << std::endl;

    mpf_class threshold = 1e-5;
    if (l1_norm < threshold) {
        std::cout << "Result OK" << std::endl;
    } else {
        std::cout << "Result NG" << std::endl;
    }

    clear_mpf_vec(x, N);
    clear_mpf_vec(y, N);
    mpf_clear(alpha);
    delete[] x;
    delete[] y;
    delete[] x_mpf_class;
    delete[] y_mpf_class;

    return EXIT_SUCCESS;
}
