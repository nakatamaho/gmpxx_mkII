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

gmp_randstate_t state;

mpf_class _Rdot(int64_t n, mpf_class *dx, int64_t incx, mpf_class *dy, int64_t incy) {
    int64_t i;

    if (incx != 1 || incy != 1) {
        printf("Not supported, exitting\n");
        exit(-1);
    }

    mpf_class temp;
    temp = 0.0;
    for (i = 0; i < n; i++) {
        mpf_class templ = dx[i] * dy[i];
        temp += templ;
    }
    return temp;
}

void init_mpf_vec(mpf_t *vec, int n, int prec) {
    for (int i = 0; i < n; i++) {
        mpf_init2(vec[i], prec);
        mpf_urandomb(vec[i], state, prec);
    }
}

void clear_mpf_vec(mpf_t *vec, int n) {
    for (int i = 0; i < n; i++) {
        mpf_clear(vec[i]);
    }
}

int main(int argc, char **argv) {
    gmp_randinit_default(state);
    gmp_randseed_ui(state, 42);

    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <vector size> <precision>" << std::endl;
        return 1;
    }

    int N = std::atoi(argv[1]);
    int prec = std::atoi(argv[2]);
    mpf_set_default_prec(prec);

    mpf_t *vec1 = new mpf_t[N];
    mpf_t *vec2 = new mpf_t[N];
    mpf_t tmp, dot_product;

    mpf_init2(dot_product, prec);
    mpf_init2(tmp, prec);
    init_mpf_vec(vec1, N, prec);
    init_mpf_vec(vec2, N, prec);

    mpf_class *vec1_mpf_class = new mpf_class[N];
    mpf_class *vec2_mpf_class = new mpf_class[N];
    mpf_class ans;

    for (int i = 0; i < N; i++) {
        vec1_mpf_class[i] = mpf_class(vec1[i]);
        vec2_mpf_class[i] = mpf_class(vec2[i]);
    }

    auto start = std::chrono::high_resolution_clock::now();
    ans = _Rdot(N, vec1_mpf_class, 1, vec2_mpf_class, 1);
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> elapsed_seconds = end - start;
    std::cout << "Elapsed time: " << elapsed_seconds.count() << " s" << std::endl;

    std::cout << "Dot product: ";
    gmp_printf("%.128Ff", ans);
    std::cout << std::endl;

    clear_mpf_vec(vec1, N);
    clear_mpf_vec(vec2, N);
    mpf_clear(tmp);
    mpf_clear(dot_product);
    delete[] vec1;
    delete[] vec2;

    return 0;
}
