# gmpxx_mkII Status

This document summarizes the current implementation state of
`gmpxx_mkII.h` for the v2.0.0 expression-template rewrite.

## Overview

The project is currently at Phase 1 of the v2.0.0 rewrite. The implemented
surface is a clean, header-only C++20 `mpf_class` expression-template core for
lazy floating-point arithmetic over GMP `mpf_t`, including mixed scalar
arithmetic and compound assignment.

The v1.0.0 eager implementation is retained only as reference material in
`eager/gmpxx_mkII.h.in`. It is not included by, copied into, or built as part
of the v2.0.0 header.

| Area | Status | Short Description |
|---|---:|---|
| Header-only layout | Done | `include/gmpxx_mkII.h.in` is configured to `gmpxx_mkII.h`; no `.cpp` source is used. |
| CMake scaffold | Done | Provides `gmpxx_mkII::gmpxx_mkII`, inline GMP detection, C++20 requirements, install of the generated header, and test targets. |
| C++20 concepts | Done | `gmpxx_expr`, `phase0_operand`, `scalar_operand`, and `phase1_operand` constrain expression and scalar overloads. |
| `mpf_class` RAII | Done | Owns `mpf_t`; supports default/precision/double/string construction, copy/move, assignment, compound assignment, precision access, raw GMP access, and swap. |
| Binary expression templates | Done | `binary_expr<Op, L, R>` implements lazy `+`, `-`, `*`, and `/` for `mpf_class`, expression, and scalar operands. |
| Unary expression templates | Done | `unary_expr<Op, X>` implements lazy unary `+` and unary `-`. |
| Scalar expression leaves | Done for Phase 1 | Signed integers, unsigned integers, `float`, and `double` participate in `mpf_class` expressions after ABI-normalizing to `int64_t`, `uint64_t`, or `double`. |
| Compound assignment | Done for Phase 1 | `+=`, `-=`, `*=`, and `/=` accept `mpf_class`, expression nodes, and scalar operands while preserving left-hand side precision. |
| Long-width dispatch | Done for Phase 1 | `uint64_t` paths dispatch through `unsigned long` fast paths where valid and through temporary conversion when simulating or running on LLP64. |
| Expression evaluation | Done | Top-level evaluation computes one final precision, propagates it downward, and uses `contains_address()` for alias-safe assignment. |
| Allocation minimization | Done | Direct chains such as `dst = a + b + c + d` and integer scalar fast paths evaluate with zero temporary `mpf_t` allocations when `dst` is already sized; two independent subtrees use one temporary. |
| Precision propagation | Done | Default build uses max operand precision. `GMPXX_MKII_NOPRECCHANGE` uses the thread-local default precision. |
| Default precision policy | Done | `GMPXX_MKII_DEFAULT_PREC` initializes a process-wide requested precision; each thread snapshots it lazily on first use. |
| `gmpxx_defaults` | Minimal | Only `set_initial_default_prec(uint64_t)` is implemented. No getter, base policy, or legacy initializer is present. |
| Scalar normalization traits | Done | `scalar_normalize_t<T>` maps scalar categories to `int64_t`, `uint64_t`, and `double`; unsupported types are SFINAE-friendly exclusions. |
| `mpz_class` / `mpq_class` | Missing by design | Not declared and not accepted as operands in Phase 1. Planned for later phases. |
| I/O and formatting | Missing by design | No stream operators, `get_str()`, or formatting policy APIs in Phase 1. |
| Comparisons | Missing by design | No comparison operators or `cmp()` in Phase 1. |
| User-defined literals | Missing by design | `_mpf`, `_mpz`, and `_mpq` are deferred. |
| Random support | Missing by design | `gmp_randclass` and random helpers are deferred. |
| Test coverage | Present through Phase 1 | Ten CTest targets cover ABI traits, numeric equivalence, allocation counts, alias safety, thread-local default precision, scalar arithmetic, scalar allocation counts, compound assignment, and long-width dispatch. |

## Implementation Summary

| Component | Implemented Items | Important Notes |
|---|---|---|
| `mpf_class` | Default constructor, explicit precision constructor, double constructors, `const char*`/`std::string` constructors, copy/move construction, copy/move assignment, expression construction, expression assignment, compound assignment, destructor, `get_mpf_t()`, `get_prec()`, `contains_address()`, and `swap()` | Default construction uses the wrapper's thread-local requested precision, not GMP's global `mpf_set_default_prec` state. String constructors use `mpf_set_str` directly and throw on parse failure. Existing-object expression assignment may resize to the expression final precision. Compound assignment preserves left-hand side precision. |
| `gmpxx_defaults` | `set_initial_default_prec(uint64_t)` | `bits == 0` is a no-op. The stored value is requested precision. Threads that have already snapshotted the default are not affected by later stores. |
| Precision helpers | `effective_mpf_prec()`, `normalize_mpf_prec()`, `checked_mp_bitcnt()`, `parse_default_prec_env()`, `process_initial_prec()`, `thread_default_prec()` | `effective_mpf_prec()` models GMP limb-boundary precision rounding for expected-value checks. Header code narrows precision through `checked_mp_bitcnt()`. |
| Default precision initialization | `GMPXX_MKII_DEFAULT_PREC` environment parsing | Empty, negative, zero, trailing-garbage, and exception cases fall back to 512 bits. GMP's global default precision APIs are not used by the wrapper. |
| `scalar_normalize_t<T>` | Integral signed types to `int64_t`, unsigned integral types including `bool` to `uint64_t`, `float`/`double` to `double` | Used by Phase 1 scalar leaves and scalar operator overloads. `long double` and compiler `__int128` types are intentionally not scalar operands. |
| `expr_base<Derived>` | `suggested_prec()` and `.eval()` | `.eval()` returns a value-type `mpf_class`. `suggested_prec()` switches between operand-max and `GMPXX_MKII_NOPRECCHANGE` policies. |
| `unary_expr<Op, X>` | Stores operand by `const&`, implements `suggested_prec_impl()`, `contains_address()`, and `eval_to_prec()` | Uses the L1 lifetime policy. Do not save expression trees in `auto`; evaluate immediately with `.eval()` or initialize/assign an `mpf_class`. |
| `binary_expr<Op, L, R>` | Stores `mpf_class`/expression operands by `const&`, stores scalar leaves by normalized value, implements `suggested_prec_impl()`, `contains_address()`, and allocation-aware `eval_to_prec()` | Leaf/leaf and chain cases evaluate directly into the destination. Independent expression/expression inputs allocate one temporary. Scalar leaves do not contribute to operand-max precision. |
| Operation tags | `add_op`, `sub_op`, `mul_op`, `div_op`, `neg_op`, `pos_op` | Direct wrappers over GMP `mpf_*` arithmetic. Integer scalar paths use `_ui` fast paths when possible; double scalar paths use one temporary `mpf_class`. |

## Operator Summary

| Operator Family | Status | Covered Operand Shapes |
|---|---:|---|
| Binary `+` | Done through Phase 1 | `mpf_class`, expression, and scalar combinations except scalar/scalar |
| Binary `-` | Done through Phase 1 | `mpf_class`, expression, and scalar combinations except scalar/scalar |
| Binary `*` | Done through Phase 1 | `mpf_class`, expression, and scalar combinations except scalar/scalar |
| Binary `/` | Done through Phase 1 | `mpf_class`, expression, and scalar combinations except scalar/scalar |
| Unary `-` | Done | `-mpf_class` and `-expression` |
| Unary `+` | Done | `+mpf_class` and `+expression` |
| Scalar arithmetic | Done for Phase 1 | Signed integer, unsigned integer, `float`, and `double` operands normalize to `int64_t`, `uint64_t`, and `double`. |
| `mpz_class` / `mpq_class` operands | Missing by design | No `mpz_class` or `mpq_class` declarations or operand acceptance in Phase 1. |
| Compound assignment | Done for Phase 1 | `mpf_class`, expression, and scalar RHS forms preserve left-hand side precision. |
| Comparisons | Missing by design | `==`, `!=`, `<`, `<=`, `>`, `>=`, `<=>`, and `cmp()` are deferred. |
| Math functions | Missing by design | `abs`, `sqrt`, `hypot`, `floor`, `ceil`, `trunc`, transcendental functions, and related overloads are deferred. |

## Test Summary

| Test Target | Status | Coverage |
|---|---:|---|
| `test_abi_fingerprint` | Present | `scalar_normalize_t` ABI categories, `gmpxx_expr`, `phase0_operand`, operator constraints rejecting scalar operands, and diagnostic-only expression type names. |
| `test_numeric_equivalence` | Present | Bit-exact comparison against raw GMP `mpf_t` reference calculations for unary and binary operations, nested expressions, mixed precisions, positive/negative/zero values, string construction, and double construction. |
| `test_alloc_count` | Present | Registers GMP memory hooks before object construction and verifies allocation counts for `dst = a + b`, `dst = a + b + c`, `dst = a + b + c + d`, and `dst = (a+b) * (c+d)` as `0, 0, 0, 1`. |
| `test_alias_safety` | Present | Self-alias and mixed-alias expression assignment cases compare against independent raw GMP references. |
| `test_thread_safety` | Present | Thread-local default precision lazy snapshots, isolation from GMP global default precision, `set_initial_default_prec()` before thread spawn, and snapshot immutability after first thread-local touch. |
| `test_scalar_arithmetic` | Present | Scalar arithmetic for signed integers, unsigned integers, `float`, and `double` in both operand orders, including `INT64_MIN`, `UINT64_MAX`, precision 8, and expression/scalar composition. |
| `test_scalar_alloc_count` | Present | Allocation counts for scalar fast paths and double slow paths: `a + 5LL`, `a + 5.0`, `a + b + 5LL`, and `dst += 5LL`. |
| `test_compound_assign` | Present | `+=`, `-=`, `*=`, and `/=` with `mpf_class`, expression, scalar, self-alias, mixed-alias, and precision-preservation cases. |
| `test_long_width_dispatch` | Present | Normal LP64 and simulated LLP64 `uint64_t` dispatch, including large integers and `INT64_MIN`. |
| `test_long_width_dispatch_llp64` | Present | Same source compiled with `GMPXX_MKII_TEST_LLP64_PATH` to force the slow branch in the normal test suite. |
| `GMPXX_MKII_NOPRECCHANGE` build | Present | The same ten tests pass when expression precision is the thread-local default instead of max operand precision. |
| Environment override check | Present manually | `GMPXX_MKII_DEFAULT_PREC=1024 ctest --test-dir build --output-on-failure` passes. |
| Clang coverage | Present | Clang 21.1.8 passes both default and `GMPXX_MKII_NOPRECCHANGE=ON` Phase 1 builds. |
| TSan coverage | Present for T4 | ThreadSanitizer build passes `test_thread_safety`. |
| ASan/UBSan coverage | Present | GCC 15.2.0 AddressSanitizer/UndefinedBehaviorSanitizer build passes all ten tests. |

## Verified Build Matrix

| Compiler / Build | Result | Notes |
|---|---:|---|
| GCC 15.2.0, default | Pass | All ten tests pass. |
| GCC 15.2.0, `GMPXX_MKII_NOPRECCHANGE=ON` | Pass | All ten tests pass. |
| GCC 15.2.0, `GMPXX_MKII_TEST_LLP64_PATH` | Pass | Long-width dispatch tests pass with the slow path forced. |
| Clang 21.1.8, default | Pass | All ten tests pass. |
| Clang 21.1.8, `GMPXX_MKII_NOPRECCHANGE=ON` | Pass | All ten tests pass. |
| GCC 15.2.0, TSan | Pass | `test_thread_safety` passes. |
| GCC 15.2.0, ASan/UBSan | Pass | All ten tests pass. |
| C++17 direct include | Fails as intended | Header `static_assert(__cplusplus >= 202002L, ...)` fires. |

## Missing Feature Summary

| Category | Missing Items | Planned Phase / Notes |
|---|---|---|
| Scalar arithmetic | Additional scalar types beyond the Phase 1 normalized set | Phase 1 supports `int64_t`, `uint64_t`, and `double` leaves only. |
| Scalar fusion | Unary `-` double-negation cleanup and power-of-two multiplication/division fast paths such as `mpf_mul_2exp` | Phase 1.5. |
| GMP integer/rational mixing | `mpz_class` and `mpq_class` RAII types, leaves, and mixed `mpf` expression participation | Phase 2 and later. Phase 1 intentionally has no declarations for these names. |
| Native integer ET | `mpz_class` expression templates, integer arithmetic, and `mpz_addmul`/`mpz_submul` style fusion | Phase 3. |
| Native rational ET | `mpq_class` expression templates and canonicalized rational arithmetic | Phase 4. |
| Math functions | `abs`, `sqrt`, `hypot`, `floor`, `ceil`, and later transcendental functions | Phase 4 and later. Must remain GMP-only for this project; no MPFR/MPC fallback. |
| I/O and formatting | Stream input/output, `get_str()`, base policy, and raw GMP stream helpers | Phase 5. |
| User-defined literals | `_mpf`, `_mpz`, and `_mpq` | Phase 5. |
| Fortran bridge | v1.0.0 compatibility bridge APIs | Phase 5. |
| Runtime defaults | `gmpxx_defaults::base` and any broader default-policy surface | Phase 5. Phase 1 exposes only `set_initial_default_prec()`. |
| Package config | Full `find_package(gmpxx_mkII)` config files | Deferred to Phase 5. Phase 1 installs the generated header and exports the target only. |
| Benchmarks | Allocation/performance microbenchmarks beyond `test_alloc_count` | Phase 6 or separate benchmark work. |

## Important Current Semantics

### Expression Lifetime

Phase 1 uses the L1 expression-node storage policy for `mpf_class` leaves and
expression subtrees: those operands are stored by `const&`. Scalar leaves are
stored by normalized value (`int64_t`, `uint64_t`, or `double`) to avoid
temporary lifetime hazards and ABI variation from platform `long` width.
Expression trees still must not be saved in `auto` variables.

Use one of these immediate-evaluation forms:

```cpp
mpf_class y = a + b + c;
y = a + b + c;
auto value = (a + b + c).eval();
```

Do not use:

```cpp
auto expr = a + b + c;
mpf_class y = expr;
```

The outer expression can contain references to inner temporary expression
nodes whose lifetime ended at the end of the original full-expression.

### Precision Policy

In the default build, expression precision is the maximum effective precision
reported by the `mpf_class` leaves in the expression tree. The top-level
evaluation computes this once and passes the requested final precision down
the whole expression tree.

When `GMPXX_MKII_NOPRECCHANGE` is defined, expression precision is the current
thread's wrapper default precision. Operand-specific precision is ignored for
expression construction in that compatibility mode.

### Default Precision

The wrapper's default precision is independent of GMP's global default
precision. `mpf_class()` uses:

1. A process-wide atomic requested precision initialized lazily from
   `GMPXX_MKII_DEFAULT_PREC`, falling back to 512.
2. A per-thread `thread_local` snapshot taken on first access.

`gmpxx_defaults::set_initial_default_prec(bits)` updates only the process-wide
initial value. It does not alter snapshots already taken by existing threads.

### Alias Safety

Expression assignment checks whether the destination `mpf_class` appears as a
leaf in the expression tree. Aliased assignments evaluate into a temporary and
then swap. Non-aliased assignments evaluate directly into the destination,
which is what enables the zero-allocation chain cases covered by
`test_alloc_count`.

## Notes Compared With v1.0.0 Eager Header

The current v2.0.0 header is not a drop-in replacement for the v1.0.0 eager public
surface. It intentionally removes or defers:

- `mpz_class` and `mpq_class`.
- Comparisons.
- Stream I/O and string output.
- User-defined literals.
- Fortran bridge APIs.
- Legacy global initializers such as `mpf_class_initializer`.
- Strict GMP C++ compatibility switches.

The retained v1.0.0 eager header under `eager/gmpxx_mkII.h.in` is reference
material for later integration phases only.
