<!-- SPDX-License-Identifier: BSD-2-Clause -->

# 00_Rdot

This directory benchmarks the GMP real dot product

```text
sum_i x_i * y_i
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
build_bench_release/benchmarks/00_Rdot/
```

## Run

Run the whole benchmark set through the top-level runner:

```bash
benchmarks/run_benchmarks.sh build_bench_release 512
```

For a quick Rdot-sized smoke run, pass smaller dimensions:

```bash
benchmarks/run_benchmarks.sh build_bench_release 128 1000 1000 32 32 16 16 16 \
    benchmarks/results-smoke
```

The first vector-size argument is used for Rdot.  Individual executables take:

```text
<vector size> <precision>
```

Example:

```bash
build_bench_release/benchmarks/00_Rdot/Rdot_gmp_kernel_01_mkII 1000000 512
```

## Reading Results

Each executable prints `Elapsed time` and `MFLOPS`.  Higher MFLOPS is better
when comparing runs with the same vector size, precision, compiler flags, and
machine.

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
- Plot: `../results-go-sh-sample/benchmark_20260430_081331_Linux_Ryzen_3970X_32-Core_Rdot.png`
- PDF plot: `../results-go-sh-sample/benchmark_20260430_081331_Linux_Ryzen_3970X_32-Core_Rdot.pdf`

All Rdot variants in that run report `Result OK`.
