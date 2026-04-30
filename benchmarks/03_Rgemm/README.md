<!-- SPDX-License-Identifier: BSD-2-Clause -->

# 03_Rgemm

This directory benchmarks the GMP real dense matrix-matrix product

```text
C = alpha * A * B + beta * C
```

with random `mpf` data at a fixed precision.  It compares raw `mpf_t`,
upstream `gmpxx.h`, `gmpxx_mkII`, and `gmpxx_mkII` built with
`GMPXX_MKII_NOPRECCHANGE`.

## Build

From the repository root:

```bash
cmake -S . -B build_bench_release -DCMAKE_BUILD_TYPE=Release
cmake --build build_bench_release -j
```

The executables are created under:

```text
build_bench_release/benchmarks/03_Rgemm/
```

## Run

Run the whole benchmark set through the top-level runner:

```bash
benchmarks/run_benchmarks.sh build_bench_release 512
```

For a quick Rgemm-sized smoke run, pass smaller dimensions:

```bash
benchmarks/run_benchmarks.sh build_bench_release 128 1000 1000 32 32 16 16 16 \
    benchmarks/results-smoke
```

The `RGEMM_M RGEMM_K RGEMM_N` runner arguments are used for Rgemm.  Individual
executables take:

```text
<rows m> <cols k> <cols n> <precision>
```

Example:

```bash
build_bench_release/benchmarks/03_Rgemm/Rgemm_gmp_kernel_01_mkII 500 500 500 512
```

## Reading Results

Each executable prints `Elapsed time`, `MFLOPS`, `L1 Norm of difference`, and a
`Result OK` or `Result NG` check against the reference result.  Higher MFLOPS is
better when the correctness check is `Result OK`.

Variant names:

- `C_native`: raw `mpf_t` implementation.
- `C_native_openmp`: raw `mpf_t` implementation with OpenMP.
- `*_orig`: upstream `gmpxx.h`.
- `*_mkII`: this header with the default precision policy.
- `*_mkII_NOPRECCHANGE`: this header with `GMPXX_MKII_NOPRECCHANGE`.
- `*_openmp_*`: OpenMP variant where the eager benchmark provided one.

## Recorded go.sh Sample

![Rgemm serial benchmark](../results-go-sh-sample/benchmark_20260430_081331_Linux_Ryzen_3970X_32-Core_serial_Rgemm.png)

![Rgemm OpenMP benchmark](../results-go-sh-sample/benchmark_20260430_081331_Linux_Ryzen_3970X_32-Core_openmp_Rgemm.png)

The committed sample run uses the original `go.sh` dimensions:

```text
M = 500, K = 500, N = 500, precision = 512
```

Results are stored in `../results-go-sh-sample/`:

- Raw log: `../results-go-sh-sample/benchmark_20260430_081331.log`
- Serial plot: `../results-go-sh-sample/benchmark_20260430_081331_Linux_Ryzen_3970X_32-Core_serial_Rgemm.png`
- Serial PDF: `../results-go-sh-sample/benchmark_20260430_081331_Linux_Ryzen_3970X_32-Core_serial_Rgemm.pdf`
- OpenMP plot: `../results-go-sh-sample/benchmark_20260430_081331_Linux_Ryzen_3970X_32-Core_openmp_Rgemm.png`
- OpenMP PDF: `../results-go-sh-sample/benchmark_20260430_081331_Linux_Ryzen_3970X_32-Core_openmp_Rgemm.pdf`

All Rgemm variants in that run report `Result OK`.

OpenMP has the strongest effect here: the timed matrix-matrix body improves by
about 24-31x in the recorded run.  Rgemm has much higher arithmetic intensity
than the vector kernels, so the OpenMP speedup is closer to what one expects
from the available cores.  In serial mode, `kernel_03` is the fastest wrapper
family in this run, while OpenMP makes the differences between wrapper
variants smaller than the difference between serial and parallel execution.
