# gmpxx_mkII

`gmpxx_mkII` is a C++20, single-header wrapper around GNU GMP numeric
types.  It is intended for code that likes the public names and general usage
style of GMP's existing `gmpxx.h`, but wants a wrapper whose ownership,
precision policy, expression evaluation, and build integration are explicit in
this repository.

The project is GMP-only.  It does not use MPFR or MPC for implementation,
normal tests, parsing, formatting, or reference calculations.

## Overview

Compared with the upstream GMP C++ wrapper, `gmpxx_mkII` differs in these
main ways:

- The public API is one project-owned header,
  [gmpxx_mkII.h](gmpxx_mkII.h), with CMake package support; consumers do not
  link to `libgmpxx`.
- The public type names and ordinary expression style are close to
  `gmpxx.h`: `mpf_class`, `mpz_class`, `mpq_class`, and `gmp_randclass` are
  provided.  The main visible namespace difference is that the enhanced API
  lives under `gmpxx`; compatibility details are tracked in
  [STATUS.md](STATUS.md).
- Scalar dispatch covers fixed-width integer paths such as `int64_t` and
  `uint64_t`, avoiding the `long`/`long long` ambiguity that matters across
  LP64 and LLP64 platforms.
- The wrapper default precision is thread-local after first use and is
  initialized from `GMPXX_MKII_DEFAULT_PREC` or from
  `gmpxx_defaults::set_initial_default_prec()`.  This avoids changing GMP's
  process-global `mpf_set_default_prec()` state as a library side effect.
- GMP-only special functions are provided for `mpf_class`, including `log`,
  `exp`, `cos`, `sin`, `atan`, `atan2`, `pow`, `pi`, `log_two`, `log1p`, and
  `expm1`.
- Mixed exact/floating expressions and scalar promotion rules are tested and
  documented here, including the project policy that `mpz`/`mpq` mixed with
  floating scalars produce floating results where a floating result is defined.
- Floating expression precision is defined by this project: default builds use
  the maximum precision of `mpf_class` leaves, while assignment into an
  existing `mpf_class` preserves the destination precision.

The target is source-level convenience close to `gmpxx.h`, not a drop-in ABI
replacement.  Existing programs must be recompiled and should check
[STATUS.md](STATUS.md) for known compatibility differences.

## Features

- BSD-2-Clause licensed, header-only public API.
- Public type names compatible with GMP's C++ wrapper:
  `mpf_class`, `mpz_class`, `mpq_class`, and `gmp_randclass`.
- RAII ownership of `mpf_t`, `mpz_t`, and `mpq_t`.
- Expression templates for `+`, `-`, `*`, `/`, unary operators, and compound
  assignment.
- Mixed arithmetic across GMP floating, integer, rational, and supported
  scalar operands.
- Assignment into an existing `mpf_class` preserves the destination precision.
- Default precision is wrapper-controlled and thread-local after first use.
- Exact `mpz_class` and `mpq_class` arithmetic stays native where the public
  API defines exact behavior.
- `mpq_class` values are canonicalized at public boundaries.
- String conversion, base-aware parsing, stream I/O, and user-defined
  literals.
- GMP-only `mpf_class` math surface: `sqrt`, `abs`, `pi`, `log_two`, `log`,
  `log1p`, `exp`, `expm1`, `sin`, `cos`, `atan`, `atan2`, and `pow`.
- Ported eager benchmark programs for Rdot, Raxpy, Rgemv, and Rgemm.

See [STATUS.md](STATUS.md) for the detailed implementation matrix and known
compatibility differences.

## Requirements

- C++20 compiler:
  - GCC 12 or newer
  - Clang 16 or newer
  - MSVC 19.30 or newer
- GMP 6.2 or newer
- CMake 3.20 or newer

The header checks the language mode with:

```cpp
static_assert(__cplusplus >= 202002L, "gmpxx_mkII v2.0.0 requires C++20");
```

## Build And Test

Default precision policy build:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j
ctest --test-dir build --output-on-failure
```

The default build also compiles the small programs under `examples/` and the
eager benchmark source layout under `benchmarks/`.  The benchmark build emits
native `mpf_t`, original `gmpxx.h`, `mkII`, `mkII_NOPRECCHANGE`, and OpenMP target
variants where the eager benchmark set provides them. Disable them with:

```bash
cmake -S . -B build -DGMPXX_MKII_BUILD_EXAMPLES=OFF
cmake -S . -B build -DGMPXX_MKII_BUILD_BENCHMARKS=OFF
```

## Benchmarks

Run the ported benchmark set and generate plots with:

```bash
benchmarks/run_benchmarks.sh build 512
```

The benchmark runner defaults to the eager benchmark dimensions:
`Rdot/Raxpy n=100000000`, `Rgemv 4000x4000`, and `Rgemm 500x500x500`.
Pass smaller dimensions explicitly for smoke runs.

The full argument order is:

```bash
benchmarks/run_benchmarks.sh BUILD_DIR PRECISION \
    RDOT_N RAXPY_N RGEMV_M RGEMV_N RGEMM_M RGEMM_K RGEMM_N OUTPUT_DIR
```

For example, a quick correctness and plotting smoke run is:

```bash
cmake -S . -B build_bench_release -DCMAKE_BUILD_TYPE=Release
cmake --build build_bench_release -j
benchmarks/run_benchmarks.sh build_bench_release 128 1000 1000 32 32 16 16 16 \
    benchmarks/results-smoke
```

Use release builds for timing.  Debug builds are useful only for checking that
the benchmark programs compile and complete.

Benchmark directories:

- `benchmarks/00_Rdot`: dot product, `sum_i x_i * y_i`.
- `benchmarks/01_Raxpy`: AXPY, `y_i = y_i + alpha * x_i`.
- `benchmarks/02_Rgemv`: dense matrix-vector multiply.
- `benchmarks/03_Rgemm`: dense matrix-matrix multiply.

Each directory keeps the eager benchmark layout.  `*_gmp_C_native_*` programs
use raw `mpf_t`; `*_kernel_*_orig` uses upstream `gmpxx.h`; `*_kernel_*_mkII`
uses this header; `*_kernel_*_mkII_NOPRECCHANGE` builds this header with
`GMPXX_MKII_NOPRECCHANGE`; `*_openmp_*` variants use OpenMP where the eager
benchmark provided one.

The runner writes a timestamped log and calls `benchmarks/plot.py` through
matplotlib.  The log records one `COMMAND` block per executable, followed by
`Elapsed time`, `MFLOPS`, and the benchmark's result check.  The generated
plots separate serial and OpenMP variants: `*_serial_summary.{png,pdf}` and
`*_openmp_summary.{png,pdf}` compare all kernels, while
`*_serial_{Rdot,Raxpy,Rgemv,Rgemm}.{png,pdf}` and
`*_openmp_{Rdot,Raxpy,Rgemv,Rgemm}.{png,pdf}` give per-kernel comparisons.
Higher MFLOPS is better; compare variants within the same kernel, precision,
matrix size, compiler flags, and machine.

A committed run using the eager `go.sh` sample dimensions is stored under
`benchmarks/results_raw/Linux_Ryzen_3970X_32-Core/`.  It was generated with:

```bash
benchmarks/run_benchmarks.sh build_bench_release 512 \
    100000000 100000000 4000 4000 500 500 500 \
    benchmarks/results_raw/Linux_Ryzen_3970X_32-Core
```

The generated files include the raw log `benchmark_20260430_081331.log`,
serial plots, and OpenMP plots.  In this run, `Rdot`, `Raxpy`, and `Rgemm`
report `Result OK` for all variants.  `Rgemv kernel_openmp_02` reports
`Result NG` for `orig`, `mkII`, and `mkII_NOPRECCHANGE`; the same failure
across all three variants points to that ported OpenMP benchmark variant
rather than a `gmpxx_mkII`-only difference.

For this 32-core Threadripper run, OpenMP improves the timed kernel-body
MFLOPS substantially: roughly 17-22x for Rdot, 11-14x for Raxpy, 9-23x for
Rgemv, and 24-31x for Rgemm, depending on the native/orig/mkII variant.  These
ratios use `Elapsed time`/`MFLOPS`, not `WALL_SECONDS`; vector initialization
and result checking dominate wall time for Rdot and Raxpy, so end-to-end
speedup is much smaller than the plotted kernel-body speedup.  The recorded
PNG plots and per-kernel discussion are in the benchmark directories linked
above.

## Installation

Install the generated header, exported CMake target, and package config files:

```bash
cmake --install build --prefix /path/to/prefix
```

This installs:

```text
include/gmpxx_mkII.h
lib/cmake/gmpxx_mkII/gmpxx_mkIIConfig.cmake
lib/cmake/gmpxx_mkII/gmpxx_mkIIConfigVersion.cmake
lib/cmake/gmpxx_mkII/gmpxx_mkIITargets.cmake
```

An installed consumer can use:

```cmake
find_package(gmpxx_mkII CONFIG REQUIRED)

add_executable(example example.cpp)
target_link_libraries(example PRIVATE gmpxx_mkII::gmpxx_mkII)
```

## Additional Build Modes

`GMPXX_MKII_NOPRECCHANGE` build:

```bash
cmake -S . -B build_np -DCMAKE_BUILD_TYPE=Debug \
      -DGMPXX_MKII_NOPRECCHANGE=ON
cmake --build build_np -j
ctest --test-dir build_np --output-on-failure
```

LP64-to-LLP64 dispatch simulation for unsigned-long-width scalar paths:

```bash
cmake -S . -B build_llp64sim -DCMAKE_BUILD_TYPE=Debug \
      -DCMAKE_CXX_FLAGS="-DGMPXX_MKII_TEST_LLP64_PATH"
cmake --build build_llp64sim -j
ctest --test-dir build_llp64sim --output-on-failure -R long_width_dispatch
```

Optional ThreadSanitizer check for the thread-local default precision tests:

```bash
cmake -S . -B build_tsan \
      -DCMAKE_CXX_FLAGS="-fsanitize=thread -g -O1" \
      -DCMAKE_EXE_LINKER_FLAGS="-fsanitize=thread"
cmake --build build_tsan -j
ctest --test-dir build_tsan --output-on-failure -R thread_safety
```

## Basic Use

When using the CMake target from this tree:

```cmake
add_executable(example example.cpp)
target_link_libraries(example PRIVATE gmpxx_mkII::gmpxx_mkII)
```

Example:

```cpp
#include "gmpxx_mkII.h"

#include <cassert>

int main() {
    using namespace gmpxx;

    mpf_class a("1.5", 256);
    mpf_class b("2.5", 256);
    mpf_class c("4.0", 512);

    mpf_class r = (a + b) * c;
    assert(r.get_prec() >= c.get_prec());

    mpf_class dst;
    dst = 2.0 * a + b + 5LL;
    dst += c * 3u;

    auto materialized = (a + b).eval();
    (void)materialized;
}
```

## Expression Lifetime

Expression-template nodes follow the storage policy documented in
[AGENTS.md](AGENTS.md): leaf RAII objects are stored by `const&`, while
intermediate expression nodes are stored by value.  This supports natural
immediate-use expressions:

```cpp
mpf_class y = a + b + c;
y = a + b + c;
auto value = (a + b + c).eval();
```

Do not treat expression node types as a stable public API.  Materialize with
`.eval()` or assign into a wrapper object when a value must outlive the full
expression.

## Precision Policy

Default build:

- Expression construction and `.eval()` use the maximum precision of the
  `mpf_class` leaves in the expression tree.
- Assignment to an existing `mpf_class` preserves the destination precision;
  the right-hand side expression is evaluated at that precision.
- Scalar, `mpz_class`, and `mpq_class` leaves do not contribute floating
  precision in the default build.
- Top-level evaluation computes this final precision once and passes it down
  through the whole expression.

`GMPXX_MKII_NOPRECCHANGE` build:

- Expression precision is the current thread's wrapper default precision.
- Operand-specific precision does not affect expression construction or
  `.eval()` result precision.
- Assignment to an existing `mpf_class` still preserves the destination
  precision.
- Exact `mpz_class` and `mpq_class` expressions are not precision-bearing in
  either build mode.

GMP rounds `mpf_t` precision to implementation-dependent limb boundaries.
`mpf_class::get_prec()` returns the effective GMP precision.

## Default Precision

The wrapper does not call GMP's global `mpf_set_default_prec()` or
`mpf_get_default_prec()`.

Default construction uses a wrapper-controlled requested precision:

1. A process-wide atomic initial value is initialized from
   `GMPXX_MKII_DEFAULT_PREC`, or 512 if the environment variable is missing or
   invalid.
2. Each thread snapshots that value lazily on first use.
3. A thread's snapshot is immutable after the first access.

Set the process-wide initial value before creating `mpf_class` values in a
thread:

```cpp
gmpxx_defaults::set_initial_default_prec(1024);
mpf_class x;
```

`set_initial_default_prec(0)` is a no-op.

Query helpers are available:

```cpp
auto requested = gmpxx_defaults::get_initial_default_prec();
auto effective = gmpxx_defaults::get_default_prec();
```

`get_initial_default_prec()` returns the current process-wide requested
initial precision. `get_default_prec()` returns the effective precision used by
default-constructed `mpf_class` objects in the current thread.

## Default Base

String constructors and string APIs without an explicit base use a
thread-local wrapper default base. The initial base is 10.

```cpp
gmpxx_defaults::set_default_base(16);

mpz_class z("ff");
mpq_class q("10/20");

assert(z == mpz_class(255));
assert(q == mpq_class("1/2", 10));
assert(z.get_str() == "ff");
```

Valid bases are 2 through 62. Invalid bases throw `std::invalid_argument`.
Stream formatting does not use this default base; `operator<<` follows the
stream's `std::dec`, `std::hex`, and `std::oct` flags.

## User-Defined Literals

Literals are opt-in under `gmpxx::literals`:

```cpp
using namespace gmpxx::literals;

mpz_class z = "123456789012345678901234567890"_mpz;
mpq_class q = "2/4"_mpq;
mpf_class f = 1.25_mpf;
```

Raw numeric literal forms are parsed in base 10. String literal forms use the
current wrapper default base.

## Compatibility

The v2 wrapper preserves many source-compatible names and behaviors expected
from GMP's `gmpxx.h`, but it is not a binary-compatible replacement for
`libgmpxx`.  Existing programs must be recompiled.

Key compatibility choices:

- Assignment into an existing `mpf_class` preserves destination precision.
- No-base string construction follows GMP base-detection policy, including
  `mpz_class("0x...")`.
- Mixed exact GMP types with floating scalars produce floating results where
  this project defines a floating result.
- `mpq_class` values are canonicalized before public comparison, formatting,
  and serialization.
- `bool` is intentionally not accepted as a scalar arithmetic type.

`GMPXX_MKII_NOPRECCHANGE` is a compatibility build mode selected at CMake
configure time:

```bash
cmake -S . -B build_np -DGMPXX_MKII_NOPRECCHANGE=ON
```

This mode is useful for comparing against the eager no-precision-change
benchmark style.  In benchmark target names it appears as
`mkII_NOPRECCHANGE`.

Known differences and unsupported items are tracked in [STATUS.md](STATUS.md).

## Quality Assurance

The test suite combines project-specific regression tests with translated
coverage from GMP's legacy C++ wrapper tests.  The normal CTest build covers:

- construction, assignment, move safety, and aliasing;
- arithmetic and mixed scalar operations;
- precision/default-policy behavior;
- comparisons, strings, stream I/O, and literals;
- random-number wrappers;
- GMP integer/rational behavior and canonicalization;
- package configuration.

CTest target names are intentionally small and feature-oriented to make
regression triage cheap.

## Examples

The [examples](examples/) directory contains small standalone programs,
including a DKA/Aberth root finder example implemented without
`std::complex`.

## Reference Material

Local GMP reference material is kept under:

```text
docs/reference/upstream/gmp-6.3.0/
```

The v1.0.0 eager header is retained for later compatibility work under:

```text
eager/gmpxx_mkII.h.in
```

The v2.0.0 header does not include or depend on the eager header.

## License

`gmpxx_mkII` source code is distributed under the BSD-2-Clause license. See
[LICENSE](LICENSE).

The upstream GMP reference documentation under `docs/reference/upstream/` is
reference material and may carry different upstream documentation licenses.
