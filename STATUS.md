# gmpxx_mkII Status

This document summarizes the current implementation state of
`gmpxx_mkII.h` for the v2.0.0 expression-template rewrite.

## Overview

The project is currently at Phase 5 of the v2.0.0 rewrite. The implemented
surface is a clean, header-only C++20 expression-template core for lazy
arithmetic over GMP `mpf_t`, `mpz_t`, and `mpq_t`, including mixed scalar,
integer, rational, and floating-point expressions plus native mpz
multiply-add fusion, immediate comparisons, string conversion, stream I/O,
user-defined literals, runtime defaults/base policy, and install-tree package
config support.

The v1.0.0 eager implementation is retained only as reference material in
`eager/gmpxx_mkII.h.in`. It is not included by, copied into, or built as part
of the v2.0.0 header.

| Area | Status | Short Description |
|---|---:|---|
| Header-only layout | Done for Phase 5 | `include/gmpxx_mkII.h.in` is configured to `gmpxx_mkII.h`; no `.cpp` source is used. |
| CMake scaffold | Done for Phase 5 | Provides `gmpxx_mkII::gmpxx_mkII`, inline GMP detection, C++20 requirements, generated-header install, exported target file, package config files, and test targets. |
| C++20 concepts | Done for Phase 5 | `gmpxx_expr`, `phase0_operand`, `scalar_operand`, `phase1_operand`, `mpz_operand`, `mpq_operand`, `phase2_operand`, and comparison constraints gate overloads. |
| `mpf_class` RAII | Done through Phase 5 | Owns `mpf_t`; supports default/precision/integral+precision/double/string construction, copy/move, assignment including double and string assignment, compound assignment, precision access, string conversion, stream I/O, raw GMP access, and swap. |
| `mpz_class` RAII | Done through Phase 5 | Owns `mpz_t`; supports integer/string construction, copy/move, expression assignment, compound assignment, string conversion, stream I/O, raw GMP access, swap, and sign query. |
| `mpq_class` RAII | Done through Phase 5 | Owns `mpq_t`; supports integer, `mpz_class`, double, and string construction, copy/move, expression assignment, compound assignment, string conversion, stream I/O, raw GMP access, numerator/denominator extraction, swap, sign query, and canonicalization. |
| Binary expression templates | Done through Phase 5 | `binary_expr<Op, L, R>` implements lazy `+`, `-`, `*`, and `/` for `mpf_class`, `mpz_class`, `mpq_class`, expression, and scalar operands. |
| Unary expression templates | Done through Phase 5 | `unary_expr<Op, X>` implements lazy unary `+` and unary `-` for mpf/mpz/mpq expressions. |
| Scalar expression leaves | Done through Phase 5 | Signed integers, unsigned integers, `float`, and `double` participate in mpf/mpz/mpq expressions after ABI-normalizing to `int64_t`, `uint64_t`, or `double`. |
| Compound assignment | Done through Phase 5 | `+=`, `-=`, `*=`, and `/=` accept wrapper values, expression nodes, and scalar operands for `mpf_class`, `mpz_class`, and `mpq_class` where supported. |
| Long-width dispatch | Done through Phase 5 | `uint64_t` paths dispatch through `unsigned long` fast paths where valid and through temporary conversion when simulating or running on LLP64. |
| Unary double-negation simplification | Done through Phase 5 | `-(-x)` returns a positive identity expression node instead of nesting two runtime negations. |
| Power-of-two integer scaling fusion | Done through Phase 5 | `mpf * 2^k`, `2^k * mpf`, and `mpf / 2^k` dispatch through `mpf_mul_2exp` or `mpf_div_2exp` for integer scalar leaves. |
| Expression evaluation | Done through Phase 5 | Expression construction and `.eval()` use one computed expression precision for floating results. Existing-object expression assignment preserves destination precision for `mpf_class` and uses `contains_address()` for alias-safe temporary evaluation across mpf/mpz/mpq leaves. |
| Allocation minimization | Done through Phase 5 | Direct mpf chains such as `dst = a + b + c + d` and integer scalar fast paths evaluate with zero temporary `mpf_t` allocations when `dst` is already sized; wrapper temporary counts are tracked separately for mixed mpz/mpq and fused mpz paths. |
| Precision propagation | Done through Phase 5 | Default build uses max mpf operand precision for floating expression construction and `.eval()`. `GMPXX_MKII_NOPRECCHANGE` uses the thread-local default precision for those paths. Existing-object `mpf_class` assignment preserves destination precision. |
| Default precision policy | Done through Phase 5 | `GMPXX_MKII_DEFAULT_PREC` initializes a process-wide requested precision; each thread snapshots it lazily on first use. Phase 5 exposes query helpers without using GMP's global default precision. |
| `gmpxx_defaults` | Done for Phase 5 | Provides initial default precision set/get, current thread effective default precision query, and thread-local default base set/get. Legacy global initializer objects are not restored. |
| Scalar normalization traits | Done through Phase 5 | `scalar_normalize_t<T>` maps scalar categories to `int64_t`, `uint64_t`, and `double`; unsupported types are SFINAE-friendly exclusions. |
| Operand kind dispatch | Done through Phase 5 | `value_kind`, `kind_of_v`, and `result_type_t` classify mpf/mpz/mpq/scalar operands and expression result types. |
| `mpz_class` / `mpq_class` | Done through Phase 5 | Declared, owned, and accepted as expression operands. |
| Native integer addmul fusion | Done for Phase 3 | `mpz_class` compound assignment fuses direct `a += b*c` and `a -= b*c` forms through `mpz_addmul`, `mpz_submul`, `mpz_addmul_ui`, and `mpz_submul_ui` where valid. |
| Comparisons | Done for Phase 4A | `cmp()`, `==`, `!=`, `<`, `<=`, `>`, and `>=` are implemented for `mpf_class`, `mpz_class`, `mpq_class`, supported scalar operands, and expression operands. |
| Basic GMP math functions | Done after Phase 5 | `sqrt`, `abs`, `neg`, `ceil`, `floor`, `trunc`, `hypot`, sign queries, and exact integer helpers such as `gcd`, `lcm`, `factorial`, `primorial`, and `fibonacci` are implemented through GMP APIs where supported. |
| GMP-only transcendental functions | Done for Phase 6 | `pi`, `const_pi`, `log_two`, `const_log2`, `log`, `log2`, `log10`, `log1p`, `exp`, `expm1`, `sin`, `cos`, `tan`, `atan`, `atan2`, and `pow` are integrated for concrete `mpf_class` inputs without MPFR/MPC or `double` fallback. |
| String conversion | Done for Phase 4B | `get_str()`, `set_str()`, and `to_string()` are implemented for `mpf_class`, `mpz_class`, and `mpq_class` with GMP-compatible parsing and output semantics. Phase 5 no-base overloads use the wrapper default base. |
| Stream I/O | Done for Phase 4B | `operator<<` and `operator>>` are implemented for concrete wrapper types, and expression nodes support immediate-evaluation stream output. |
| User-defined literals | Done for Phase 5 | `_mpz`, `_mpq`, and `_mpf` are available in `gmpxx::literals`. |
| Runtime defaults | Done for Phase 5 | Default precision query helpers and a thread-local default base policy are implemented. |
| Package config | Done for Phase 5 | Installed packages provide `gmpxx_mkIIConfig.cmake`, a version config, and an exported `gmpxx_mkII::gmpxx_mkII` target usable through `find_package`. |
| Random support | Done after Phase 5 | `gmp_randclass` owns `gmp_randstate_t`, supports default/MT/LC initialization, seeding, random `mpz_class` generation, and random `mpf_class` generation. Bare `get_f()` returns a random floating expression/proxy so assignment into an existing `mpf_class` preserves destination precision. |
| Fortran bridge | Not planned for v2.0.0 | Fortran bridge APIs are intentionally dropped from the v2.0.0 roadmap. |
| Test coverage | Present through Phase 6 | Thirty CTest targets cover ABI traits, construction/copy/swap semantics, type conversions, basic mpf math functions, mpf transcendental functions, numeric equivalence, allocation counts, alias safety, thread-local default precision, scalar arithmetic, scalar allocation counts, compound assignment, long-width dispatch, precision policy, unary simplification, power-of-two fusion, mpz arithmetic, mpq arithmetic, mixed-type arithmetic, wrapper temporary counts, mpz addmul fusion, comparisons, I/O/string conversion, UDLs, defaults/base policy, package config, and random support. |

## Implementation Summary

| Component | Implemented Items | Important Notes |
|---|---|---|
| `mpf_class` | Default constructor, explicit precision constructor, integral+precision constructor, double constructors, `const char*`/`std::string` constructors, copy/move construction, copy/move assignment, double assignment, string assignment, expression construction, expression assignment, compound assignment, destructor, `get_str()`, `set_str()`, `to_string()`, scalar conversion/fits queries, `mul_2exp()`, `div_2exp()`, stream I/O, `get_mpf_t()`, `get_prec()`, `contains_address()`, and `swap()` | Default construction uses the wrapper's thread-local requested precision, not GMP's global `mpf_set_default_prec` state. `mpf_class(0, precision)` and other integral+precision construction forms construct numeric values, not null strings. String constructors and string assignment throw on parse failure; `set_str()` and stream extraction preserve destination precision and leave the object unchanged on parse failure. No-base string APIs use the wrapper default base. Existing-object expression assignment and compound assignment preserve left-hand side precision. |
| `mpz_class` | Integer/string construction, copy/move, expression construction/assignment, compound assignment, `%=` support, scalar conversion/fits queries, `get_str()`, `set_str()`, `to_string()`, stream I/O, `get_mpz_t()`, `contains_address()`, `swap()`, and `sgn()` | `mpz / mpz` uses `mpz_tdiv_q`; `%=` uses `mpz_tdiv_r`. `set_str()` and stream extraction parse into a temporary and leave the object unchanged on failure. No-base string APIs use the wrapper default base. |
| `mpq_class` | Integer/mpz/double/string construction, copy/move, expression construction/assignment, compound assignment, `get_str()`, `set_str()`, `to_string()`, `get_d()`, stream I/O, `get_mpq_t()`, `get_num()`, `get_den()`, `get_num_mpz_t()`, `get_den_mpz_t()`, `contains_address()`, `swap()`, `sgn()`, and `canonicalize()` | String and numerator/denominator construction canonicalize the rational value. `set_str()` and stream extraction canonicalize on success and leave the object unchanged on failure. No-base string APIs use the wrapper default base. |
| `gmpxx_defaults` | `set_initial_default_prec(uint64_t)`, `get_initial_default_prec()`, `get_default_prec()`, `set_default_base(int)`, and `get_default_base()` | `set_initial_default_prec(0)` is a no-op. The stored precision is requested precision. Threads that have already snapshotted the default precision are not affected by later stores. The default base is thread-local, defaults to 10, and accepts bases 2 through 62. |
| Precision helpers | `effective_mpf_prec()`, `normalize_mpf_prec()`, `checked_mp_bitcnt()`, `parse_default_prec_env()`, `process_initial_prec()`, `thread_default_prec()` | `effective_mpf_prec()` models GMP limb-boundary precision rounding for expected-value checks. Header code narrows precision through `checked_mp_bitcnt()`. |
| Default precision initialization | `GMPXX_MKII_DEFAULT_PREC` environment parsing | Empty, negative, zero, trailing-garbage, and exception cases fall back to 512 bits. GMP's global default precision APIs are not used by the wrapper. |
| `scalar_normalize_t<T>` | Integral signed types to `int64_t`, unsigned integral types including `bool` to `uint64_t`, `float`/`double` to `double` | Used by scalar leaves and scalar operator overloads through Phase 5. `long double` and compiler `__int128` types are intentionally not scalar operands. |
| `expr_base<Derived>` | `suggested_prec()`, `.eval()`, `eval_to(mpz_class&)`, and `eval_to(mpq_class&)` | `.eval()` returns the expression `result_type`. `suggested_prec()` switches between operand-max and `GMPXX_MKII_NOPRECCHANGE` policies for floating results. |
| `unary_expr<Op, X>` | Stores operand by `const&`, implements `result_type`, `operand()`, `suggested_prec_impl()`, `contains_address()`, `eval_to_prec()`, `eval_to_mpz()`, and `eval_to_mpq()` | Uses the L1 lifetime policy. `-(-x)` is represented as a `pos_op` expression node. |
| `binary_expr<Op, L, R>` | Stores mpf/mpz/mpq/expression operands by `const&`, stores scalar leaves by normalized value, implements `result_type`, `suggested_prec_impl()`, `contains_address()`, `eval_to_prec()`, `eval_to_mpz()`, and `eval_to_mpq()` | Scalar, mpz, and mpq leaves do not contribute to operand-max mpf precision. Mixed mpf/mpz/mpq floating results convert exact operands through required wrapper temporaries. |
| Operation tags | `add_op`, `sub_op`, `mul_op`, `div_op`, `neg_op`, `pos_op` | Direct wrappers over GMP arithmetic. Existing mpf scalar fast paths remain; mpf×mpz/mpq paths use `mpf_set_z`/`mpf_set_q` temporaries because GMP has no direct `mpf_*_z` or `mpf_*_q` APIs. |
| mpz addmul fusion | `is_mpz_addmul_fusable_v`, `addmul_fused_apply()`, `submul_fused_apply()` | Direct `binary_expr<mul_op, ...>` shapes with mpz/mpz or mpz/integral-scalar operands bypass the generic temporary compound-assignment path. Unary-minus, multiplication-chain, and inner-add/subtract forms remain generic. |
| Comparisons | `cmp()`, comparison operators, comparison materialization helpers | Comparisons are immediate operations. Expression operands are evaluated once, scalar/scalar overloads are rejected, and values are compared through exact GMP rational comparison without string or universal `double` fallback. |
| String and stream I/O | `get_str()`, `set_str()`, `to_string()`, `operator<<`, `operator>>`, and expression stream output | GMP-allocated strings are released through the active GMP free function. Integer and rational stream output respects `std::dec`, `std::hex`, `std::oct`, and `std::uppercase`; mpf stream output uses GMP formatted output without conversion through `double`. |
| User-defined literals | `_mpz`, `_mpq`, `_mpf` in `gmpxx::literals` | Raw numeric literal overloads parse in base 10. String literal overloads use the current wrapper default base. `_mpf` parses literal text directly into `mpf_class` at the wrapper default precision. |
| GMP-only transcendental functions | `pi`, `const_pi`, `log_two`, `const_log2`, `log`, `log2`, `log10`, `log1p`, `exp`, `expm1`, `sin`, `cos`, `tan`, `atan`, `atan2`, and `pow` | Ported and compatibility-completed in the single header. Implementations use `mpf_t` arithmetic, cached AGM constants, and guard precision; no MPFR/MPC dependency or universal `double` fallback is introduced. Concrete `mpf_class` overloads preserve input/result precision policy. |
| Random support | `gmp_randclass`, `random_mpf_expr`, `get_z_bits()`, `get_z_range()`, `get_f()` | `gmp_randclass` is a non-copyable owner for `gmp_randstate_t`. `get_f(mp_bitcnt_t)` and `get_f(mpf_class const&)` return immediate `mpf_class` values. Bare `get_f()` returns `random_mpf_expr`, which evaluates through the normal floating expression assignment path and therefore uses the left-hand side precision on existing-object assignment. |
| Package config | `gmpxx_mkIIConfig.cmake`, `gmpxx_mkIIConfigVersion.cmake`, `gmpxx_mkIITargets.cmake` | Installed consumers can use `find_package(gmpxx_mkII CONFIG REQUIRED)` and link `gmpxx_mkII::gmpxx_mkII`. The config locates GMP without embedding build-tree paths. |

## Operator Summary

| Operator Family | Status | Covered Operand Shapes |
|---|---:|---|
| Binary `+` | Done through Phase 5 | `mpf_class`, `mpz_class`, `mpq_class`, expression, and scalar combinations except scalar/scalar |
| Binary `-` | Done through Phase 5 | `mpf_class`, `mpz_class`, `mpq_class`, expression, and scalar combinations except scalar/scalar |
| Binary `*` | Done through Phase 5 | `mpf_class`, `mpz_class`, `mpq_class`, expression, and scalar combinations except scalar/scalar; integer power-of-two mpf scaling uses `mpf_mul_2exp` |
| Binary `/` | Done through Phase 5 | `mpf_class`, `mpz_class`, `mpq_class`, expression, and scalar combinations except scalar/scalar; `mpz/mpz` remains integer division |
| Unary `-` | Done through Phase 5 | `-mpf_class`, `-mpz_class`, `-mpq_class`, `-expression`, and `-(-x)` simplification |
| Unary `+` | Done through Phase 5 | `+mpf_class`, `+mpz_class`, `+mpq_class`, and `+expression` |
| Scalar arithmetic | Done through Phase 5 | Signed integer, unsigned integer, `float`, and `double` operands normalize to `int64_t`, `uint64_t`, and `double`. |
| `mpz_class` / `mpq_class` operands | Done through Phase 5 | mpz/mpq leaves participate in ET arithmetic and mixed mpf/mpz/mpq expressions. |
| Compound assignment | Done through Phase 5 | `mpf_class`, `mpz_class`, `mpq_class`, expression, and scalar RHS forms preserve left-hand side object state. |
| Native mpz addmul fusion | Done for Phase 3 | Direct `mpz_class += mpz*mpz`, `mpz_class -= mpz*mpz`, and integral-scalar variants use `mpz_addmul`, `mpz_submul`, `mpz_addmul_ui`, or `mpz_submul_ui` where valid. |
| Comparisons | Done for Phase 4A | `cmp()`, `==`, `!=`, `<`, `<=`, `>`, and `>=`; `operator<=>` remains out of scope. |
| Stream output | Done for Phase 4B | Concrete wrappers and expression nodes support `operator<<`; expression output evaluates once and then streams the concrete result. |
| Stream input | Done for Phase 4B | Concrete wrappers support `operator>>`; expression input remains intentionally unsupported. |
| User-defined literals | Done for Phase 5 | `_mpz`, `_mpq`, and `_mpf` are opt-in under `gmpxx::literals`. |
| Defaults/base policy | Done for Phase 5 | `gmpxx_defaults` exposes precision queries and a thread-local default base for no-base string APIs. |
| Package config | Done for Phase 5 | Install-tree package config supports `find_package(gmpxx_mkII CONFIG REQUIRED)`. |
| Random support | Done after Phase 5 | `gmp_randclass` is a non-copyable owner for GMP random state and exposes the GMP C++ random generation surface for mpz/mpf values. Bare `get_f()` is expression/proxy based for destination-precision-preserving assignment. |
| Basic mpf math functions | Done after Phase 5 | `sqrt(mpf_class)`, `abs(mpf_class)`, and legacy `neg(mpf_class)` are available. Remaining math functions are deferred. |
| GMP-only transcendental functions | Done for Phase 6 | `pi`, `const_pi`, `log_two`, `const_log2`, `log`, `log2`, `log10`, `log1p`, `exp`, `expm1`, `sin`, `cos`, `tan`, `atan`, `atan2`, and `pow` are available for concrete `mpf_class` values. |

## GMP C++ Binding Checklist

Source: GMP 6.3.0 manual
`docs/reference/upstream/gmp-6.3.0/generated/gmp.txt`, section 12
(`C++ Class Interface`), especially sections 12.2 through 12.5.

| Upstream Binding Area | GMP C++ Surface | Current Status | Notes for `gmpxx_mkII` |
|---|---|---:|---|
| Namespace and public class names | `mpz_class`, `mpq_class`, `mpf_class`, `gmp_randclass` | Implemented | Public C++ API is under namespace `gmpxx`. The CMake package target remains `gmpxx_mkII::gmpxx_mkII`. |
| Common arithmetic operators | `+`, `-`, `*`, `/`, unary `+`, unary `-`, compound assignment | Implemented | Expression-template implementation covers concrete wrappers, expressions, and supported scalar operands except scalar/scalar hijacking. |
| Comparisons | `cmp()`, `==`, `!=`, `<`, `<=`, `>`, `>=` | Implemented | Immediate comparisons evaluate expression operands once and compare through GMP values, not strings or a universal `double` fallback. |
| String and stream I/O | `get_str()`, `set_str()`, `operator<<`, `operator>>` | Implemented | Phase 4B implements concrete wrapper string/stream APIs and expression stream output. No-base string APIs use the wrapper default base. |
| User-defined literals | `_mpz`, `_mpq`, `_mpf` | Implemented | Available under `gmpxx::literals`; not injected into the global namespace. |
| `mpz_class` constructors | Standard integer/floating/GMP wrapper constructors, string constructors, `const mpz_t` constructor | Partial | Integer, `double`, string, expression, copy, and move construction are implemented. Raw `mpz_t` construction and explicit construction from `mpq_class`/`mpf_class` remain missing. Unlike upstream `gmpxx.h`, normalized `long long` operands are intentionally supported through the ABI-stable scalar policy. |
| `mpz_class` arithmetic semantics | Integer arithmetic, truncating `/` and `%` | Implemented | `mpz/mpz` division uses truncating integer division; modulo uses GMP integer remainder semantics. |
| `mpz_class` conversion queries | `get_d()`, `get_si()`, `get_ui()`, `fits_*_p()` | Implemented | Implemented for compatibility with the GMP C++ binding conversion surface. |
| `mpz_class` helpers | `abs`, `sgn`, `sqrt`, `gcd`, `lcm`, `factorial`, `primorial`, `fibonacci`, `swap` | Partial | Free-function helpers and swap are implemented. Upstream-style member/static forms such as `mpz_class::factorial(type)` are not restored. |
| `mpq_class` constructors | Integer numerator/denominator, `mpz_class`, `double`, string, `const mpq_t` constructor | Partial | Integer, `mpz_class`, `double`, string, expression, copy, and move construction are implemented. Raw `mpq_t` construction is missing. |
| `mpq_class` canonicalization | `canonicalize()`, canonical string construction/input | Implemented | Public construction, `set_str()`, stream input, and arithmetic maintain canonical rational values where required. |
| `mpq_class` numerator/denominator access | `get_num()`, `get_den()`, `get_num_mpz_t()`, `get_den_mpz_t()` | Partial | Copy-returning numerator/denominator access and const raw access are implemented. Upstream mutable reference-style numerator/denominator mutation is not implemented. |
| `mpq_class` helpers | `abs`, `cmp`, `get_d()`, `get_str()`, `set_str()`, `sgn`, `swap` | Implemented | Covered by Phase 4A/4B and later compatibility additions. |
| `mpf_class` constructors | Default, value constructors, value+precision constructors, string constructors, `const mpf_t` constructors | Partial | Default, precision-tag, integer+precision, `double`, string+precision, `mpz_class`+precision, expression, copy, and move construction are implemented. Raw `mpf_t` construction, `mpq_class`+precision construction, and expression+explicit-precision construction remain missing. |
| `mpf_class` assignment | Assignment from values preserves destination precision | Implemented | Existing-object assignment from expressions, strings, and scalar values preserves the left-hand side precision. |
| `mpf_class` precision control | `get_prec()`, `set_prec()`, `set_prec_raw()` | Partial | `get_prec()` is implemented. Public `set_prec()` and `set_prec_raw()` are not currently exposed. |
| `mpf_class` conversion queries | `get_d()`, `get_si()`, `get_ui()`, `fits_*_p()` | Implemented | Implemented for compatibility with the GMP C++ binding conversion surface. |
| `mpf_class` helpers | `abs`, `ceil`, `floor`, `trunc`, `hypot`, `sgn`, `sqrt`, `swap` | Implemented | Basic concrete GMP helpers are implemented without MPFR/MPC or `double` fallback. |
| Random state class | `gmp_randclass`, `seed()`, `get_z_bits()`, `get_z_range()`, `get_f()` | Partial | Wrapper-owned random state, seeding, integer random generation, and floating random generation are implemented. Bare `get_f()` is intentionally expression/proxy based so assignment can use destination precision. The upstream varargs `gmp_randclass(gmp_randalg_t, ...)` constructor surface is not fully mirrored. |
| C++ binding limitations | Manual limitations around template expressions and unsupported C++11 types | Deliberate differences | The wrapper uses C++20 concepts and ABI-normalized scalar leaves. `long double`, compiler `__int128`, and scalar/scalar ET overloads remain rejected. |

## Test Summary

| Test Target | Status | Coverage |
|---|---:|---|
| `test_abi_fingerprint` | Present | `scalar_normalize_t` ABI categories, `gmpxx_expr`, `phase0_operand`, operator constraints rejecting scalar/scalar ET operands, and diagnostic-only expression type names. |
| `test_construction_copy` | Present | `mpf_class`, `mpz_class`, and `mpq_class` default construction, copy/move construction, copy/move assignment, member/free `swap`, and mpf precision-preserving double/string/integral construction and assignment cases. |
| `test_mpf_math_functions` | Present | `sqrt`, `abs`, legacy `neg`, `ceil`, `floor`, `trunc`, `hypot`, `mul_2exp`, `div_2exp`, sign queries, and exact mpz/mpq helper functions match GMP behavior and preserve precision where applicable. |
| `test_mpf_transcendent_functions` | Present | `pi`, `const_pi`, `log_two`, `const_log2`, `log`, `log2`, `log10`, `log1p`, `exp`, `expm1`, `sin`, `cos`, `tan`, `atan`, `atan2`, and `pow` compile-time surface, upstream-derived reference literals, precision-doubling checks, near-zero/near-one paths, trigonometric reduction and axis cases, special values, identities, precision policy, and domain errors. |
| `test_type_conversions` | Present | `mpz_class` integer/double/string/base construction and assignment, `mpf/mpz/mpq` scalar conversion queries, fit predicates, `mpq_class` integer/mpz/double/string/base construction and canonicalization, `mpf_class(mpz_class, precision)`, and mpq numerator/denominator accessors. |
| `test_numeric_equivalence` | Present | Bit-exact comparison against raw GMP `mpf_t` reference calculations for unary and binary operations, nested expressions, mixed precisions, positive/negative/zero values, string construction, and double construction. |
| `test_alloc_count` | Present | Registers GMP memory hooks before object construction and verifies allocation counts for `dst = a + b`, `dst = a + b + c`, `dst = a + b + c + d`, and `dst = (a+b) * (c+d)` as `0, 0, 0, 1`. |
| `test_alias_safety` | Present | Self-alias and mixed-alias expression assignment cases compare against independent raw GMP references. |
| `test_thread_safety` | Present | Thread-local default precision lazy snapshots, isolation from GMP global default precision, `set_initial_default_prec()` before thread spawn, and snapshot immutability after first thread-local touch. |
| `test_scalar_arithmetic` | Present | Scalar arithmetic for signed integers, unsigned integers, `float`, and `double` in both operand orders, including `INT64_MIN`, `UINT64_MAX`, precision 8, and expression/scalar composition. |
| `test_scalar_alloc_count` | Present | Allocation counts for scalar fast paths and double slow paths: `a + 5LL`, `a + 5.0`, `a + b + 5LL`, and `dst += 5LL`. |
| `test_compound_assign` | Present | `+=`, `-=`, `*=`, and `/=` with `mpf_class`, expression, scalar, self-alias, mixed-alias, and precision-preservation cases. |
| `test_long_width_dispatch` | Present | Normal LP64 and simulated LLP64 `uint64_t` dispatch, including large integers and `INT64_MIN`. |
| `test_long_width_dispatch_llp64` | Present | Same source compiled with `GMPXX_MKII_TEST_LLP64_PATH` to force the slow branch in the normal test suite. |
| `test_precision_policy` | Present | Construction and `.eval()` use expression precision; existing-object assignment and compound assignment preserve destination precision. |
| `test_unary_minus_simplification` | Present | `-(-mpf)` and `-(-(expression))` produce identity values while preserving assignment precision. |
| `test_power_of_two_fusion` | Present | Integer power-of-two multiplication/division, negative signed scalars including `INT64_MIN`, generic non-power cases, scalar-left division, and compound `*=`, `/=` precision preservation. |
| `test_mpz_arithmetic` | Present | mpz arithmetic, scalar mixing, truncating integer division, `%=` modulo, ET composition, self-alias, compound assignment, and unary operators. |
| `test_mpq_arithmetic` | Present | mpq arithmetic, scalar mixing, canonicalization, ET composition, compound assignment, and unary operators. |
| `test_mixed_type_arithmetic` | Present | mpf×mpz, mpf×mpq, mpz×mpq, mpz/mpq plus double promotion to mpf, result type checks, and mixed precision policy. |
| `test_mpz_mpq_alloc_count` | Present | Test-only wrapper constructor counters for mpz/mpq/mpf temporaries in mixed-expression paths. |
| `test_mpz_addmul_fusion` | Present | Compile-time fusable-shape checks, fused-path counters, runtime GMP-equivalence checks, scalar sign and `INT64_MIN` cases, alias cases, and non-fused expression checks. |
| `test_mpz_addmul_alloc_count` | Present | Wrapper temporary and fused-counter checks for direct mpz addmul/submul and integral-scalar fast paths. |
| `test_mpz_addmul_alloc_count_llp64` | Present | Same allocation-count source compiled with `GMPXX_MKII_TEST_LLP64_PATH` to verify width-fallback scalar temporaries. |
| `test_comparisons` | Present | Compile-time comparison constraints, exact mpz/mpq comparisons, mpf and mixed comparisons, expression comparisons, scalar edge cases, operator consistency with `cmp()`, and integer-division comparison semantics. |
| `test_io_and_strings` | Present | `get_str()`, `set_str()`, `to_string()`, stream input/output, expression stream output, failure safety, precision preservation, base flags, and GMP-allocated string freeing. |
| `test_user_defined_literals` | Present | `_mpz`, `_mpq`, and `_mpf` literal construction, large string literals, rational canonicalization, direct mpf text parsing, default-base interaction, and raw numeric literal base independence. |
| `test_defaults_policy` | Present | Default precision getters, thread-local precision snapshot behavior, independence from GMP global default precision, default-base get/set, no-base string API integration, stream/base independence, invalid-base errors, and thread-local base behavior. |
| `test_random` | Present | `gmp_randclass` construction modes, deleted copy/move semantics, deterministic seeding, `get_z_bits`, `get_z_range`, immediate `get_f(prec)`/`get_f(mpf)` generation, bare `get_f()` expression/proxy assignment preserving destination precision, and LC initialization failure handling. |
| `test_package_config` | Present | Installs the project into a temporary prefix, configures an external consumer with `find_package(gmpxx_mkII CONFIG REQUIRED)`, builds it, and runs it. |
| `GMPXX_MKII_NOPRECCHANGE` build | Present | The same thirty tests pass when expression construction precision is the thread-local default instead of max operand precision. |
| Environment override check | Present manually | `GMPXX_MKII_DEFAULT_PREC=1024 ctest --test-dir build --output-on-failure` passes. |
| Clang coverage | Present | Clang passes both default and `GMPXX_MKII_NOPRECCHANGE=ON` Phase 5 builds. |
| TSan coverage | Present for T4 | ThreadSanitizer build passes `test_thread_safety`. |
| ASan/UBSan coverage | Present | GCC 15.2.0 AddressSanitizer/UndefinedBehaviorSanitizer build passes all thirty Phase 6 tests. |

## Verified Build Matrix

| Compiler / Build | Result | Notes |
|---|---:|---|
| GCC 15.2.0, default | Pass | All thirty tests pass. |
| GCC 15.2.0, `GMPXX_MKII_NOPRECCHANGE=ON` | Pass | All thirty tests pass. |
| GCC 15.2.0, `GMPXX_MKII_TEST_LLP64_PATH` | Pass | All thirty tests pass with the slow path forced. |
| Clang, default | Pass | All thirty tests pass. |
| Clang, `GMPXX_MKII_NOPRECCHANGE=ON` | Pass | All thirty tests pass. |
| GCC 15.2.0, TSan | Pass | `test_thread_safety` passes. |
| GCC 15.2.0, ASan/UBSan | Pass | All thirty tests pass. |
| C++17 direct include | Fails as intended | Header `static_assert(__cplusplus >= 202002L, ...)` fires. |

## Missing Feature Summary

| Category | Missing Items | Planned Phase / Notes |
|---|---|---|
| Scalar arithmetic | Additional scalar types beyond the Phase 1 normalized set | Phase 1 supports `int64_t`, `uint64_t`, and `double` leaves only. |
| Extended native integer fusion | Unary-minus addmul/submul normalization, multiplication-chain fusion, and distributive expansion such as `(b+c)*d` | Deferred beyond Phase 3. Direct `mpz_addmul`/`mpz_submul` compound-assignment fusion is implemented. |
| Remaining math functions | Expression-aware math overloads and additional special functions beyond the current concrete GMP-only surface | Future GMP-only work; no MPFR/MPC fallback. Basic concrete GMP math helpers, concrete `mpf_class` transcendental functions, and exact integer helpers are implemented. |
| Remaining GMP C++ binding compatibility | Raw `mpz_t`/`mpq_t`/`mpf_t` constructors, explicit `mpq_class`/`mpf_class` to `mpz_class` constructors, mutable upstream-style `mpq_class` numerator/denominator references, public `mpf_class::set_prec()`/`set_prec_raw()`, member/static integer helper forms, and full upstream `gmp_randclass(gmp_randalg_t, ...)` constructor compatibility | Tracked in the GMP C++ binding checklist above. These are compatibility surface gaps, not expression-template core gaps. |
| Extended literal parsing | Additional literal forms beyond `_mpz`, `_mpq`, and `_mpf` in `gmpxx::literals` | The Phase 5 UDL surface is intentionally limited to supported integer, rational, and floating text/machine literal forms. |
| Fortran bridge | v1.0.0 compatibility bridge APIs | Not planned for v2.0.0. Fortran bridge support was intentionally dropped from the roadmap. |
| Benchmarks | Allocation/performance microbenchmarks beyond `test_alloc_count` | Separate benchmark work. |

## Important Current Semantics

### Expression Lifetime

Phase 5 uses the L1 expression-node storage policy for `mpf_class`,
`mpz_class`, `mpq_class`, and expression subtrees: those operands are stored
by `const&`. Scalar leaves are stored by normalized value (`int64_t`,
`uint64_t`, or `double`) to avoid
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

In the default build, expression construction and `.eval()` use the maximum
effective precision reported by the `mpf_class` leaves in the expression tree.
Assignment to an existing `mpf_class` preserves the destination precision and
evaluates the right-hand side expression at that precision, matching GMP's
legacy `gmpxx.h` assignment policy.

When `GMPXX_MKII_NOPRECCHANGE` is defined, expression precision is the current
thread's wrapper default precision for construction and `.eval()`.
Operand-specific precision is ignored for those paths in that compatibility
mode; existing-object assignment still preserves destination precision.

### Default Precision

The wrapper's default precision is independent of GMP's global default
precision. `mpf_class()` uses:

1. A process-wide atomic requested precision initialized lazily from
   `GMPXX_MKII_DEFAULT_PREC`, falling back to 512.
2. A per-thread `thread_local` snapshot taken on first access.

`gmpxx_defaults::set_initial_default_prec(bits)` updates only the process-wide
initial value. It does not alter snapshots already taken by existing threads.

`gmpxx_defaults::get_initial_default_prec()` returns the current process-wide
requested initial precision. `gmpxx_defaults::get_default_prec()` returns the
current thread's effective wrapper default precision.

`gmpxx_defaults::set_default_base(base)` and `get_default_base()` manage a
thread-local base used by no-base string constructors and string APIs. Stream
formatting remains controlled by iostream flags, not by the wrapper default
base.

### Alias Safety

Expression assignment checks whether the destination wrapper object appears as
a leaf in the expression tree. This covers `mpf_class`, `mpz_class`, and
`mpq_class` leaves. Aliased floating assignments evaluate into a temporary at
the destination precision and then copy the value back, preserving that
precision. Aliased exact integer/rational assignments evaluate into an
independent exact temporary before updating the destination. Non-aliased
assignments evaluate directly into the destination where the evaluation path
supports it, which is what enables the zero-allocation mpf chain cases covered
by `test_alloc_count`.

## Notes Compared With v1.0.0 Eager Header

The current v2.0.0 header is not a drop-in replacement for the v1.0.0 eager public
surface. It intentionally removes or defers:

- Fortran bridge APIs, which are not planned for v2.0.0.
- Legacy global initializers such as `mpf_class_initializer`.
- Strict GMP C++ compatibility switches.

The retained v1.0.0 eager header under `eager/gmpxx_mkII.h.in` is reference
material for later integration phases only.
