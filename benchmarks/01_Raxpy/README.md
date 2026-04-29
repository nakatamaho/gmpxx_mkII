<!-- SPDX-License-Identifier: BSD-2-Clause -->

# 01_Raxpy

This directory benchmarks the GMP real AXPY operation

```text
y_i = y_i + alpha * x_i
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
build_bench_release/benchmarks/01_Raxpy/
```

## Run

Run the whole benchmark set through the top-level runner:

```bash
benchmarks/run_benchmarks.sh build_bench_release 512
```

For a quick Raxpy-sized smoke run, pass smaller dimensions:

```bash
benchmarks/run_benchmarks.sh build_bench_release 128 1000 1000 32 32 16 16 16 \
    benchmarks/results-smoke
```

The second vector-size argument is used for Raxpy.  Individual executables take:

```text
<vector size> <precision>
```

Example:

```bash
build_bench_release/benchmarks/01_Raxpy/Raxpy_gmp_kernel_01_mkII 1000000 512
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

The committed sample run uses the original `go.sh` dimensions:

```text
N = 100000000, precision = 512
```

Results are stored in `../results-go-sh-sample/`:

- Raw log: `../results-go-sh-sample/benchmark_20260430_081331.log`
- Plot: `../results-go-sh-sample/benchmark_20260430_081331_Linux_Ryzen_3970X_32-Core_Raxpy.png`
- PDF plot: `../results-go-sh-sample/benchmark_20260430_081331_Linux_Ryzen_3970X_32-Core_Raxpy.pdf`

All Raxpy variants in that run report `Result OK`.
