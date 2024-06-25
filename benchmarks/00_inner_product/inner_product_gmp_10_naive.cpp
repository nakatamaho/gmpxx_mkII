#include <iostream>
#include <chrono>
#include <gmp.h>

gmp_randstate_t state;

void Rdot(int64_t n, mpf_t *dx, int64_t incx, mpf_t *dy, int64_t incy, mpf_t *ans, mp_bitcnt_t prec) {
    int64_t ix = 0;
    int64_t iy = 0;
    int64_t i;
    mpf_t templ;
    mpf_t temp;

    if (incx < 0)
        ix = (-n + 1) * incx;
    if (incy < 0)
        iy = (-n + 1) * incy;

    if (incx != 1 || incy != 1) {
        printf("Not supported, exitting\n");
        exit(-1);
    }
    mpf_set_d(*ans, 0.0);
    mpf_init2(temp, prec);
    mpf_init2(templ, prec);
    mpf_set_d(temp, 0.0);
    mpf_set_d(templ, 0.0);
    for (i = 0; i < n; i++) {
        mpf_mul(templ, dx[i], dy[i]);
        mpf_add(temp, temp, templ);
    }
    mpf_clear(templ);
    mpf_swap(*ans, temp);
    mpf_clear(temp);

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
    mpf_t ans, dot_product;

    mpf_init2(dot_product, prec);
    mpf_init2(ans, prec);
    init_mpf_vec(vec1, N, prec);
    init_mpf_vec(vec2, N, prec);

    auto start = std::chrono::high_resolution_clock::now();
    Rdot(N, vec1, 1, vec2, 1, &ans, prec);
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> elapsed_seconds = end - start;
    std::cout << "Elapsed time: " << elapsed_seconds.count() << " s" << std::endl;

    std::cout << "Dot product: ";
    gmp_printf("%.128Ff", ans);
    std::cout << std::endl;

    clear_mpf_vec(vec1, N);
    clear_mpf_vec(vec2, N);
    mpf_clear(dot_product);
    delete[] vec1;
    delete[] vec2;

    return 0;
}
