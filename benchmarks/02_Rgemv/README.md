<!-- SPDX-License-Identifier: BSD-2-Clause -->

# 02_Rgemv

This directory benchmarks the GMP real dense matrix-vector product

```text
y = alpha * A * x + beta * y
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
build_bench_release/benchmarks/02_Rgemv/
```

## Run

Run the whole benchmark set through the top-level runner:

```bash
benchmarks/run_benchmarks.sh build_bench_release 512
```

For a quick Rgemv-sized smoke run, pass smaller dimensions:

```bash
benchmarks/run_benchmarks.sh build_bench_release 128 1000 1000 32 32 16 16 16 \
    benchmarks/results-smoke
```

The `RGEMV_M RGEMV_N` runner arguments are used for Rgemv.  Individual
executables take:

```text
<rows m> <cols n> <precision>
```

Example:

```bash
build_bench_release/benchmarks/02_Rgemv/Rgemv_gmp_kernel_01_mkII 4000 4000 512
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
M = 4000, N = 4000, precision = 512
```

Results are stored in `../results-go-sh-sample/`:

- Raw log: `../results-go-sh-sample/benchmark_20260430_081331.log`
- Serial plot: `../results-go-sh-sample/benchmark_20260430_081331_Linux_Ryzen_3970X_32-Core_serial_Rgemv.png`
- Serial PDF: `../results-go-sh-sample/benchmark_20260430_081331_Linux_Ryzen_3970X_32-Core_serial_Rgemv.pdf`
- OpenMP plot: `../results-go-sh-sample/benchmark_20260430_081331_Linux_Ryzen_3970X_32-Core_openmp_Rgemv.png`
- OpenMP PDF: `../results-go-sh-sample/benchmark_20260430_081331_Linux_Ryzen_3970X_32-Core_openmp_Rgemv.pdf`

`kernel_openmp_02` reports `Result NG` for `orig`, `mkII`, and
`mkII_NOPRECCHANGE` in that run.  The same failure across all three variants
indicates a benchmark-variant issue in this ported OpenMP case, not a
`gmpxx_mkII`-specific difference.

Ignoring the `kernel_openmp_02` correctness failure for performance
interpretation, OpenMP improves the timed Rgemv body by about 14x for native
`mpf_t` and about 21-23x for the `kernel_01` wrapper variants.  The
`kernel_02` OpenMP variants show about 9-10x speedup but are not currently
valid correctness data because they report `Result NG`.
