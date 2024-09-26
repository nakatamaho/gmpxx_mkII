Linux c77313517c6c 6.5.0-17-generic #17~22.04.1-Ubuntu SMP PREEMPT_DYNAMIC Tue Jan 16 14:32:32 UTC 2 x86_64 x86_64 x86_64 GNU/Linux
model name	: AMD Ryzen Threadripper 3970X 32-Core Processor

./gemm_gmp_10_naive_ijl 1000 1000 1000 512
    m     n     k     MFLOPS    Elapsed(s)
 1000  1000  1000     26.420     75.776

./gemm_gmp_11_naive_jli 1000 1000 1000 512
    m     n     k     MFLOPS      DIFF     Elapsed(s)
 1000  1000  1000     34.181   0.000e+00     58.571

./gemm_gmp_12_naive_jli_openmp 1000 1000 1000 512
    m     n     k     MFLOPS      DIFF     Elapsed(s)
 1000  1000  1000   1073.527   0.000e+00     1.865

./gemm_gmp_30_mpblaslike_naive_ijl_orig 1000 1000 1000 512
    m     n     k     MFLOPS       Elapsed(s)
 1000  1000  1000      6.129     326.628

./gemm_gmp_30_mpblaslike_naive_ijl_compat 1000 1000 1000 512
    m     n     k     MFLOPS       Elapsed(s)
 1000  1000  1000      6.880     290.975

./gemm_gmp_30_mpblaslike_naive_ijl_mkIISR 1000 1000 1000 512
    m     n     k     MFLOPS       Elapsed(s)
 1000  1000  1000      9.807     204.135

./gemm_gmp_30_mpblaslike_naive_ijl_mkII 1000 1000 1000 512
    m     n     k     MFLOPS       Elapsed(s)
 1000  1000  1000      7.042     284.292

