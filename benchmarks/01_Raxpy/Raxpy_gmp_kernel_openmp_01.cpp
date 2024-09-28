#include <iostream>
#include <chrono>

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

#include <omp.h>

void _Raxpy(int64_t n, const mpf_class &alpha, mpf_class *x, int64_t incx, mpf_class *y, int64_t incy) {
    if (incx != 1 || incy != 1) {
        std::cerr << "Increments other than 1 are not supported." << std::endl;
        exit(EXIT_FAILURE);
    }

#pragma omp parallel for
    for (int64_t i = 0; i < n; ++i) {
        y[i] += alpha * x[i]; // y[i] = y[i] + alpha * x[i]
    }
}

int main(int argc, char **argv) {
    gmp_randclass r(gmp_randinit_default);
    r.seed(42);

    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <vector size> <precision>" << std::endl;
        return EXIT_FAILURE;
    }

    int64_t N = std::atoll(argv[1]);
    int prec = std::atoi(argv[2]);
    mpf_set_default_prec(prec);

    mpf_class *x = new mpf_class[N];
    mpf_class *y = new mpf_class[N];
    mpf_class *yy = new mpf_class[N];
    mpf_class alpha;
    alpha = r.get_f(prec);

    for (int64_t i = 0; i < N; ++i) {
        x[i] = r.get_f(prec);
        y[i] = r.get_f(prec);
        yy[i] = y[i];
    }

    auto start = std::chrono::high_resolution_clock::now();
    _Raxpy(N, alpha, x, 1, y, 1);
    auto end = std::chrono::high_resolution_clock::now();

    Raxpy(N, alpha, x, 1, yy, 1);

    std::chrono::duration<double> elapsed_seconds = end - start;
    double mflops = (2.0 * double(N)) / (elapsed_seconds.count() * MFLOPS);

    std::cout << "Elapsed time: " << elapsed_seconds.count() << " s" << std::endl;
    std::cout << "MFLOPS: " << mflops << std::endl;

    mpf_class l1_norm = 0;
    for (int64_t i = 0; i < N; ++i) {
        mpf_class diff = abs(y[i] - yy[i]);
        l1_norm += diff;
    }

    std::cout << "L1 Norm of difference: " << l1_norm << std::endl;

    mpf_class threshold = 1e-5;
    if (l1_norm < threshold) {
        std::cout << "Result OK" << std::endl;
    } else {
        std::cout << "Result NG" << std::endl;
    }

    delete[] x;
    delete[] y;

    return EXIT_SUCCESS;
}
