# gmpxx_mkII

`gmpxx_mkII` is a C++20, header-only wrapper around GNU GMP numeric types.
The current repository state is the v2.0.0 Phase 5 expression-template
rewrite: it implements `mpf_class`, `mpz_class`, and `mpq_class` arithmetic
cores plus mixed scalar arithmetic.

The v2.0.0 rewrite is being restored in phases. Phase 5 validates the new
lazy expression machinery, precision policy, thread-local default precision,
scalar leaf normalization, compound assignment, long-width dispatch,
power-of-two integer scaling fusion, unary double-negation simplification,
integer/rational GMP wrappers, mixed mpf/mpz/mpq expressions, native mpz
multiply-add fusion, comparison operators, string conversion, stream I/O,
user-defined literals, runtime defaults/base policy, package config support,
and allocation behavior before restoring any remaining v1.0.0 surface.

## Current Scope

Implemented now:

- `mpf_class` RAII ownership of `mpf_t`.
- `mpz_class` RAII ownership of `mpz_t`.
- `mpq_class` RAII ownership of `mpq_t`.
- Lazy expression templates for `+`, `-`, `*`, and `/`.
- `mpz_class` integer division uses GMP truncating integer quotient semantics.
- `mpq_class` arithmetic canonicalizes GMP rational results.
- Mixed scalar arithmetic with signed integers, unsigned integers, `float`,
  and `double`; scalar leaves are normalized internally to `int64_t`,
  `uint64_t`, or `double`.
- Unary `+` and unary `-`.
- Direct expression construction and assignment:
  `mpf_class r = a + b * c;`, `r = (a + b) / c;`.
- Compound assignment (`+=`, `-=`, `*=`, `/=`) for `mpf_class`,
  `mpz_class`, and `mpq_class` with wrapper, expression, and scalar
  right-hand sides where supported.
- `.eval()` for explicitly materializing an expression as its result type
  (`mpf_class`, `mpz_class`, or `mpq_class`).
- Operand-max precision for expression construction and `.eval()` by default.
- `gmpxx.h`-compatible expression assignment that preserves destination
  precision.
- `GMPXX_MKII_NOPRECCHANGE` compatibility mode.
- Unary `-(-x)` simplification through a positive identity expression node.
- Integer power-of-two multiplication/division fusion through
  `mpf_mul_2exp` and `mpf_div_2exp` where applicable.
- Native `mpz_class` compound-assignment fusion for direct `a += b * c` and
  `a -= b * c` forms through `mpz_addmul`, `mpz_submul`, `mpz_addmul_ui`,
  and `mpz_submul_ui` where valid.
- `cmp()`, `==`, `!=`, `<`, `<=`, `>`, and `>=` for wrapper values,
  expression operands, and supported scalar operands.
- `get_str()`, `set_str()`, and `to_string()` for concrete wrapper values.
- `mpf_class` assignment from `double`, `char const*`, and `std::string`
  with destination precision preserved.
- `mpf_class` free functions `sqrt`, `abs`, and legacy `neg`, implemented
  directly through GMP `mpf_t` APIs.
- GMP-only `mpf_class` transcendental functions: `pi`, `log_two`, `log`,
  `log1p`, `exp`, `expm1`, `sin`, `cos`, `atan`, `atan2`, and `pow`.
- `operator<<` and `operator>>` for concrete wrapper values, plus
  immediate-evaluation `operator<<` for expression operands.
- User-defined literals in `gmpxx::literals`: `_mpz`, `_mpq`, and
  `_mpf`.
- `gmp_randclass` random state ownership, seeding, random `mpz_class`
  generation, and random `mpf_class` generation.
- Thread-local wrapper default precision initialized from
  `GMPXX_MKII_DEFAULT_PREC`.
- `gmpxx_defaults` default precision queries and default base policy.
- Full CMake package config for install-tree `find_package(gmpxx_mkII)`.
- CMake + CTest build with Phase 0 through Phase 6 regression tests.

Deferred to later phases:

- Remaining expression-aware math overloads and special functions beyond the
  current concrete GMP-only math surface.

Fortran bridge support is intentionally not planned for v2.0.0.

See [STATUS.md](STATUS.md) for the detailed implementation matrix.

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
`*_summary.{png,pdf}` compares all variants together, and
`*_{Rdot,Raxpy,Rgemv,Rgemm}.{png,pdf}` gives per-kernel comparisons.  Higher
MFLOPS is better; compare variants within the same kernel, precision, matrix
size, compiler flags, and machine.

A committed run using the eager `go.sh` sample dimensions is stored under
`benchmarks/results-go-sh-sample/`.  It was generated with:

```bash
benchmarks/run_benchmarks.sh build_bench_release 512 \
    100000000 100000000 4000 4000 500 500 500 \
    benchmarks/results-go-sh-sample
```

The generated files include the raw log
`benchmark_20260430_081331.log`, the summary plot, and one plot per benchmark
kernel.  In this run, `Rdot`, `Raxpy`, and `Rgemm` report `Result OK` for all
variants.  `Rgemv kernel_openmp_02` reports `Result NG` for `orig`, `mkII`, and
`mkII_NOPRECCHANGE`; the same failure across all three variants points to that
ported OpenMP benchmark variant rather than a `gmpxx_mkII`-only difference.

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

## Expression Lifetime Rule

Phase 5 expression nodes store `mpf_class`, `mpz_class`, `mpq_class`, and
expression subtrees by `const&`, while scalar leaves are normalized and stored
by value. This is deliberate for the v2.0.0 performance and ABI experiment,
but expression trees must not be saved in `auto` variables.

Use immediate evaluation:

```cpp
mpf_class y = a + b + c;
y = a + b + c;
auto value = (a + b + c).eval();
```

Do not save the expression tree:

```cpp
auto expr = a + b + c;  // Do not do this in the current L1 storage model.
mpf_class y = expr;     // Internal references may dangle.
```

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
