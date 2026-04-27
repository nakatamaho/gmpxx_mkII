# gmpxx_mkII

`gmpxx_mkII` is a C++20, header-only wrapper around GNU GMP numeric types.
The current repository state is the v2.0.0 Phase 0 expression-template
rewrite: it implements the `mpf_class` floating-point arithmetic core only.

Phase 0 is intentionally small. It is meant to validate the new lazy
expression machinery, precision policy, thread-local default precision, and
allocation behavior before restoring the broader v1.0.0 surface.

## Current Scope

Implemented now:

- `mpf_class` RAII ownership of `mpf_t`.
- Lazy expression templates for `+`, `-`, `*`, and `/`.
- Unary `+` and unary `-`.
- Direct expression construction and assignment:
  `mpf_class r = a + b * c;`, `r = (a + b) / c;`.
- `.eval()` for explicitly materializing an expression as `mpf_class`.
- Operand-max precision propagation by default.
- `GMPXX_MKII_NOPRECCHANGE` compatibility mode.
- Thread-local wrapper default precision initialized from
  `GMPXX_MKII_DEFAULT_PREC`.
- `gmpxx_defaults::set_initial_default_prec(uint64_t)`.
- CMake + CTest build with five Phase 0 regression tests.

Deferred to later phases:

- `mpz_class` and `mpq_class`.
- Scalar arithmetic such as `mpf_class + int` or `double * mpf_class`.
- Compound assignment (`+=`, `-=`, `*=`, `/=`).
- Comparisons.
- I/O and formatting.
- User-defined literals.
- Fortran bridge compatibility.
- Full `find_package(gmpxx_mkII)` package config.

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

`GMPXX_MKII_NOPRECCHANGE` build:

```bash
cmake -S . -B build_np -DCMAKE_BUILD_TYPE=Debug \
      -DGMPXX_MKII_NOPRECCHANGE=ON
cmake --build build_np -j
ctest --test-dir build_np --output-on-failure
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
    dst = a + b + c;

    auto materialized = (a + b).eval();
    (void)materialized;
}
```

## Expression Lifetime Rule

Phase 0 expression nodes store operands by `const&`. This is deliberate for
the v2.0.0 Phase 0 performance and ABI experiment, but it means expression
trees must not be saved in `auto` variables.

Use immediate evaluation:

```cpp
mpf_class y = a + b + c;
y = a + b + c;
auto value = (a + b + c).eval();
```

Do not save the expression tree:

```cpp
auto expr = a + b + c;  // Do not do this in Phase 0.
mpf_class y = expr;     // Internal references may dangle.
```

## Precision Policy

Default build:

- The expression result precision is the maximum precision of the `mpf_class`
  leaves in the expression tree.
- Top-level evaluation computes this final precision once and passes it down
  through the whole expression.

`GMPXX_MKII_NOPRECCHANGE` build:

- Expression precision is the current thread's wrapper default precision.
- Operand-specific precision does not affect expression result precision.

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

## Reference Material

Local GMP reference material is kept under:

```text
docs/reference/upstream/gmp-6.3.0/
```

The v1.0.0 eager header is retained for later compatibility work under:

```text
eager/gmpxx_mkII.h.in
```

The Phase 0 header does not include or depend on the eager header.

## License

`gmpxx_mkII` source code is distributed under the BSD-2-Clause license. See
[LICENSE](LICENSE).

The upstream GMP reference documentation under `docs/reference/upstream/` is
reference material and may carry different upstream documentation licenses.
