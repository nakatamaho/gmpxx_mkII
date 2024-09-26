#include <iostream>
#include <chrono>
#include <gmp.h>
#include <gmpxx.h>
#include <cstring>
#include <cstdlib>

gmp_randstate_t state;

void Raxpy(int64_t n, const mpf_t alpha, const mpf_t *x, int64_t incx, mpf_t *y, int64_t incy);
mpf_class L1Norm(int64_t n, const mpf_class *y_r, const mpf_t *y_o, int64_t incy_o);

void init_mpf_class_vec(mpf_class *vec, int64_t n);
void init_mpf_t_vec(mpf_t *vec, int64_t n, mp_bitcnt_t prec);
void clear_mpf_t_vec(mpf_t *vec, int64_t n);

int main(int argc, char **argv) {
    gmp_randinit_default(state);
    gmp_randseed_ui(state, 42);

    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <vector size> <precision>" << std::endl;
        return 1;
    }

    int64_t N = std::atoll(argv[1]);
    mp_bitcnt_t prec = std::atoi(argv[2]);
    mpf_set_default_prec(prec);

    mpf_class *x = new mpf_class[N];
    mpf_class *y_r = new mpf_class[N];
    mpf_t *y_o = new mpf_t[N];

    init_mpf_class_vec(x, N);
    init_mpf_class_vec(y_r, N);
    init_mpf_t_vec(y_o, N, prec);

    for (int64_t i = 0; i < N; i++) {
        mpf_set(y_o[i], y_r[i].get_mpf_t());
    }

    mpf_class alpha_class;
    mpf_init(alpha_class.get_mpf_t());
    mpf_urandomb(alpha_class.get_mpf_t(), state, prec);

    mpf_t alpha_t;
    mpf_init(alpha_t);
    mpf_set(alpha_t, alpha_class.get_mpf_t());

    auto start_r = std::chrono::high_resolution_clock::now();
    Raxpy(N, alpha_class, x, 1, y_r, 1);
    auto end_r = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_r = end_r - start_r;

    auto start_o = std::chrono::high_resolution_clock::now();
    axpy(N, alpha_t, reinterpret_cast<const mpf_t *>(x), 1, y_o, 1);
    auto end_o = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_o = end_o - start_o;

    mpf_class l1_norm = L1Norm(N, y_r, y_o, 1);

    std::cout << "Alpha: ";
    gmp_printf("%.128Ff\n", alpha_class.get_mpf_t());

    std::cout << "Raxpy elapsed time: " << elapsed_r.count() << " s" << std::endl;
    std::cout << "axpy elapsed time: " << elapsed_o.count() << " s" << std::endl;

    std::cout << "L1 norm of difference: ";
    gmp_printf("%.128Ff\n", l1_norm.get_mpf_t());

    delete[] x;
    delete[] y_r;
    clear_mpf_t_vec(y_o, N);
    delete[] y_o;

    mpf_clear(alpha_t);
    mpf_clear(alpha_class.get_mpf_t());

    gmp_randclear(state);

    return 0;
}

void axpy(int64_t n, const mpf_t alpha, const mpf_t *x, int64_t incx, mpf_t *y, int64_t incy) {
    int64_t ix = 0;
    int64_t iy = 0;
    mpf_t temp;
    mpf_init(temp);

    if (incx < 0)
        ix = (-n + 1) * incx;
    if (incy < 0)
        iy = (-n + 1) * incy;

    for (int64_t i = 0; i < n; i++) {
        mpf_mul(temp, alpha, x[ix]);
        mpf_add(y[iy], temp, y[iy]);
        ix += incx;
        iy += incy;
    }
    mpf_clear(temp);
}

mpf_class L1Norm(int64_t n, const mpf_class *y_r, const mpf_t *y_o, int64_t incy_o) {
    mpf_class norm = 0.0;
    mpf_class y_o_class;

    for (int64_t i = 0; i < n; i++) {
        y_o_class = mpf_class(y_o[i]);
        mpf_class diff = y_r[i] - y_o_class;
        norm += abs(diff);
    }
    return norm;
}

void init_mpf_class_vec(mpf_class *vec, int64_t n) {
    for (int64_t i = 0; i < n; i++) {
        vec[i] = 0;
        mpf_urandomb(vec[i].get_mpf_t(), state, mpf_get_prec(vec[i].get_mpf_t()));
    }
}
void init_mpf_t_vec(mpf_t *vec, int64_t n, mp_bitcnt_t prec) {
    for (int64_t i = 0; i < n; i++) {
        mpf_init2(vec[i], prec);
        mpf_urandomb(vec[i], state, prec);
    }
}
void clear_mpf_t_vec(mpf_t *vec, int64_t n) {
    for (int64_t i = 0; i < n; i++) {
        mpf_clear(vec[i]);
    }
}
