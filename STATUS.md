# gmpxx_mkII Status

This document summarizes the current implementation state of
`gmpxx_mkII.h` for the v2.0.0 expression-template rewrite.

## Overview

The project is currently at Phase 5 of the v2.0.0 rewrite. The implemented
surface is a clean, header-only C++20 expression-template core for lazy
arithmetic over GMP `mpf_t`, `mpz_t`, and `mpq_t`, including mixed scalar,
integer, rational, floating-point, and `mpf_class`-backed complex expressions
plus native mpz
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
| `mpf_class` RAII | Done through Phase 5 | Owns `mpf_t`; supports default/precision/integral+precision/bool/double/string/raw-`mpf_t` construction, wrapper conversion construction from `mpz_class`/`mpq_class`, copy+precision construction, copy/move, assignment including integral, double, string, `mpz_class`, and `mpq_class` assignment, compound assignment, explicit bool conversion, precision access, string conversion, stream I/O, raw GMP access, and swap. |
| `mpz_class` RAII | Done through Phase 5 | Owns `mpz_t`; supports integer/bool/string/wrapper conversion construction from `mpf_class`/`mpq_class`, copy/move, expression assignment, wrapper assignment, compound assignment, explicit bool conversion, string conversion, stream I/O, raw GMP access, swap, and sign query. |
| `mpq_class` RAII | Done through Phase 5 | Owns `mpq_t`; supports integer, bool, `mpz_class`, `mpf_class`, double, and string construction, copy/move, scalar/string/expression/wrapper assignment, compound assignment, explicit bool conversion, string conversion, stream I/O, raw GMP access, numerator/denominator extraction, swap, sign query, and canonicalization. |
| Binary expression templates | Done through Phase 5 | `binary_expr<Op, L, R>` implements lazy `+`, `-`, `*`, and `/` for `mpf_class`, `mpz_class`, `mpq_class`, expression, and scalar operands; legacy-compatible immediate shift and mpz bitwise operators cover `t-binary` forms. |
| Unary expression templates | Done through Phase 5 | `unary_expr<Op, X>` implements lazy unary `+` and unary `-` for mpf/mpz/mpq expressions. |
| `gmpxx::mpfc_class` | Initial implementation | Provides a GMP-only complex floating type backed by two `mpf_class` values, with expression-template `+`, `-`, `*`, `/`, unary `-`, real-operand promotion, destination-precision-preserving assignment, equality comparison, `real`, `imag`, `conj`, `norm`, `abs`, `arg`, and `polar`. It is not a GNU MPC wrapper and does not depend on MPC. |
| Scalar expression leaves | Done through Phase 5 | Signed integers, unsigned integers, `float`, and `double` participate in mpf/mpz/mpq expressions after ABI-normalizing to `int64_t`, `uint64_t`, or `double`. |
| Compound assignment | Done through Phase 5 | `+=`, `-=`, `*=`, `/=`, and supported shift/bitwise compound forms accept wrapper values, expression nodes, and scalar operands for `mpf_class`, `mpz_class`, and `mpq_class` where applicable. Cross-wrapper expression RHS forms follow the same conversion policy as wrapper construction. |
| Long-width dispatch | Done through Phase 5 | `uint64_t` paths dispatch through `unsigned long` fast paths where valid and through temporary conversion when simulating or running on LLP64. |
| Unary double-negation simplification | Done through Phase 5 | `-(-x)` returns a positive identity expression node instead of nesting two runtime negations. |
| Power-of-two integer scaling fusion | Done through Phase 5 | `mpf * 2^k`, `2^k * mpf`, and `mpf / 2^k` dispatch through `mpf_mul_2exp` or `mpf_div_2exp` for integer scalar leaves. |
| Expression evaluation | Done through Phase 5 | Expression construction and `.eval()` use one computed expression precision for floating results. Existing-object expression assignment preserves destination precision for `mpf_class` and uses `contains_address()` for alias-safe temporary evaluation across mpf/mpz/mpq leaves. |
| Allocation minimization | Done through Phase 5 | Direct mpf chains such as `dst = a + b + c + d` and integer scalar fast paths evaluate with zero temporary `mpf_t` allocations when `dst` is already sized; wrapper temporary counts are tracked separately for mixed mpz/mpq and fused mpz paths. |
| Precision propagation | Done through Phase 5 | Default build uses max mpf operand precision for floating expression construction and `.eval()`. `GMPXX_MKII_NOPRECCHANGE` uses the thread-local default precision for those paths. Existing-object `mpf_class` assignment preserves destination precision. |
| Default precision policy | Done through Phase 5 | `GMPXX_MKII_DEFAULT_PREC` initializes a process-wide requested precision; each thread snapshots it lazily on first use. Phase 5 exposes query helpers without using GMP's global default precision. |
| `gmpxx_defaults` | Done for Phase 5 | Provides initial default precision set/get, current thread effective default precision query, and thread-local default base set/get. Legacy global initializer objects are not restored. |
| Scalar normalization traits | Done through Phase 5 | `scalar_normalize_t<T>` maps scalar categories to `int64_t`, `uint64_t`, and `double`; unsupported types are SFINAE-friendly exclusions. |
| Operand kind dispatch | Done through Phase 5 | `value_kind`, `kind_of_v`, `result_type_t`, and `std::common_type` specializations classify mpf/mpz/mpq/scalar operands and expression result types. |
| `mpz_class` / `mpq_class` | Done through Phase 5 | Declared, owned, and accepted as expression operands. |
| Native integer addmul fusion | Done for Phase 3 | `mpz_class` compound assignment fuses direct `a += b*c` and `a -= b*c` forms through `mpz_addmul`, `mpz_submul`, `mpz_addmul_ui`, and `mpz_submul_ui` where valid. |
| Comparisons | Done for Phase 4A | `cmp()`, `==`, `!=`, `<`, `<=`, `>`, and `>=` are implemented for `mpf_class`, `mpz_class`, `mpq_class`, supported scalar operands, and expression operands. |
| Basic GMP math functions | Done after Phase 5 | `sqrt`, `abs`, `neg`, `ceil`, `floor`, `trunc`, `hypot`, sign queries, and exact integer helpers such as `gcd`, `lcm`, `factorial`, `primorial`, and `fibonacci` are implemented through GMP APIs where supported. |
| GMP-only transcendental functions | Done for Phase 6 | `pi`, `const_pi`, `e`, `const_e`, `log_two`, `const_log2`, `inv_log_two`, `log_ten`, `const_log10`, `pi_over_two`, `pi_over_four`, `two_pi`, `log`, `log2`, `log10`, `log1p`, `exp`, `exp2`, `exp10`, `expm1`, `sin`, `cos`, `tan`, `asin`, `acos`, `atan`, `atan2`, `sinh`, `cosh`, `tanh`, `asinh`, `acosh`, `atanh`, and `pow` are integrated for concrete `mpf_class` inputs without MPFR/MPC or `double` fallback. `gmpxx::mpfc_class` also provides complex `sqrt`, `exp`, `log`, trigonometric, inverse trigonometric, hyperbolic, and inverse hyperbolic overloads built from the real GMP-only functions. |
| String conversion | Done for Phase 4B | `get_str()`, `set_str()`, and `to_string()` are implemented for `mpf_class`, `mpz_class`, and `mpq_class` with GMP-compatible parsing and output semantics. No-base string parsing follows GMP's base-0 autodetection policy. |
| Stream I/O | Done for Phase 4B | `print_mpz`, `print_mpq`, `print_mpf`, `operator<<`, and `operator>>` are implemented for raw GMP pointers and concrete wrapper types where applicable; expression nodes support immediate-evaluation stream output. `gmpxx::mpfc_class` uses `std::complex`-style `(real,imag)` stream formatting, but extraction intentionally requires the full pair form rather than accepting `std::complex` real-only forms. Decimal point input/output for mpf streams respects the stream locale. |
| User-defined literals | Done for Phase 5 | `_mpz`, `_mpq`, and `_mpf` are available in `gmpxx::literals` and exported at global scope for GMP `gmpxx.h` compatibility. |
| Runtime defaults | Done for Phase 5 | Default precision query helpers and a thread-local default base policy are implemented. |
| Package config | Done for Phase 5 | Installed packages provide `gmpxx_mkIIConfig.cmake`, a version config, and an exported `gmpxx_mkII::gmpxx_mkII` target usable through `find_package`. |
| Random support | Done after Phase 5 | `gmp_randclass` owns `gmp_randstate_t`, supports default/MT/LC initialization, seeding, random `mpz_class` generation, and random `mpf_class` generation. Bare `get_f()` returns a random floating expression/proxy so assignment into an existing `mpf_class` preserves destination precision. |
| Examples | Present | Eight CMake-built examples demonstrate basic mpf arithmetic, `sqrt`, Newton iteration for `sqrt(2)`, Gauss-Legendre iteration for `pi`, an Aberth root finder for a degree-10 integer-coefficient polynomial implemented with real-valued complex pairs, the same Aberth example implemented with `gmpxx::mpfc_class`, a dependency-free Mandelbrot boundary-zoom PPM renderer using `mpfc_class` complex iteration, and a Wilkinson polynomial sensitivity solve for an ill-conditioned degree-20 polynomial. |
| Benchmarks | Present | CMake builds the eager benchmark source layout for `00_Rdot`, `01_Raxpy`, `02_Rgemv`, and `03_Rgemm`, including native `mpf_t`, original `gmpxx.h`, `mkII`, `mkII_NOPRECCHANGE`, and OpenMP target variants where present. `benchmarks/run_benchmarks.sh` records logs and `benchmarks/plot.py` generates separate serial/OpenMP summary and per-kernel plots. |
| Fortran bridge | Not planned for v2.0.0 | Fortran bridge APIs are intentionally dropped from the v2.0.0 roadmap. |
| Test coverage | Present through Phase 6 | Thirty-seven maintained CTest targets cover ABI traits, exception support, standalone header inclusion, construction/copy/swap semantics, legacy compatibility coverage, type conversions, basic mpf math functions, mpf transcendental functions, extended constants/transcendentals, numeric equivalence, allocation counts, alias safety, thread-local default precision, scalar arithmetic, increment/decrement, scalar allocation counts, compound assignment, long-width dispatch, precision policy, unary simplification, power-of-two fusion, mpz arithmetic, mpq arithmetic, mixed-type arithmetic, mpfc arithmetic, I/O, and transcendental functions, wrapper temporary counts, mpz addmul fusion, comparisons, I/O/string conversion, UDLs, defaults/base policy, package config, and random support. |

## Implementation Summary

| Component | Implemented Items | Important Notes |
|---|---|---|
| `mpf_class` | Default constructor, explicit precision constructor, integral+precision constructor, bool constructors, double constructors, `const char*`/`std::string` constructors, wrapper conversion constructors from `mpz_class`/`mpq_class` with default or explicit precision, copy construction with default or explicit precision, move construction, copy/move assignment, double assignment, string assignment, wrapper assignment, expression construction, expression assignment, compound assignment, destructor, `get_str()`, `set_str()`, `to_string()`, explicit bool conversion, scalar conversion/fits queries, `mul_2exp()`, `div_2exp()`, stream I/O, `get_mpf_t()`, `get_prec()`, `contains_address()`, and `swap()` | Default construction uses the wrapper's thread-local requested precision, not GMP's global `mpf_set_default_prec` state. `mpf_class(0, precision)` and other integral+precision construction forms construct numeric values, not null strings. Bool construction and explicit bool conversion follow legacy `gmpxx.h`. String constructors and string assignment throw on parse failure; no-base string parsing uses GMP base-0 autodetection. `set_str()` and stream extraction preserve destination precision and leave the object unchanged on parse failure. Existing-object expression, wrapper, move assignment, and compound assignment preserve left-hand side precision; mpf move assignment uses `mpf_swap` only when source and destination precisions already match. |
| `mpz_class` | Integer/bool/string/wrapper construction, compiler 128-bit integer construction where available, copy/move, expression construction/assignment, string/wrapper assignment, compound assignment, `%=` support, explicit bool conversion, scalar conversion/fits queries, `get_str()`, `set_str()`, `to_string()`, stream I/O, `get_mpz_t()`, `contains_address()`, `swap()`, and `sgn()` | `mpz / mpz` uses `mpz_tdiv_q`; `%=` uses `mpz_tdiv_r`. `mpz_class(mpf_class)` and `mpz_class(mpq_class)` use GMP's truncating conversion semantics. Bool construction and explicit bool conversion follow legacy `gmpxx.h`. `__int128`/`unsigned __int128` are accepted by dedicated mpz-only overloads when the compiler provides them, but are not expression scalar leaves. String assignment throws on parse failure and leaves the object unchanged. `set_str()` and stream extraction parse into a temporary and leave the object unchanged on failure. No-base string parsing uses GMP base-0 autodetection. |
| `mpq_class` | Integer/bool/mpz/mpf/double/string construction, copy/move, scalar/string/expression construction/assignment, wrapper assignment, compound assignment, `get_str()`, `set_str()`, `to_string()`, explicit bool conversion, `get_d()`, stream I/O, `get_mpq_t()`, `get_num()`, `get_den()`, mutable/const `get_num_mpz_t()`, mutable/const `get_den_mpz_t()`, `contains_address()`, `swap()`, `sgn()`, and `canonicalize()` | String, numerator/denominator, and mpf conversion construction canonicalize the rational value. Bool construction and explicit bool conversion follow legacy `gmpxx.h`. `set_str()`, string assignment, double assignment, and stream extraction canonicalize on success and leave the object unchanged on failure where applicable. Mutable numerator/denominator raw access is low-level and requires explicit `canonicalize()` after mutation. No-base string parsing uses GMP base-0 autodetection. |
| `gmpxx::mpfc_class` | Default, real, and real/imag construction; real/imag accessors and mutators; expression construction and assignment; compound assignment; `+`, `-`, `*`, `/`, unary `-`; `==`, `!=`, `real`, `imag`, `conj`, `norm`, `abs`, `arg`, `polar`, `sqrt`, `exp`, `log`, trigonometric, inverse trigonometric, hyperbolic, inverse hyperbolic functions, and stream I/O | Implemented as two `mpf_class` values in namespace `gmpxx`. Numeric constructor arguments are values, matching `mpf_class`; precision-bearing construction is done by passing precision-bearing `mpf_class` real/imag values. Complex expression leaves preserve destination real/imag precision on existing-object assignment. Real operands promote to zero-imaginary complex values. Stream I/O uses `std::complex`-style `(real,imag)` formatting but intentionally requires full pair extraction; the class avoids GNU MPC and `std::complex` API dependencies. Complex transcendental functions use principal-branch formulas built from this project's real GMP-only `mpf_class` functions. |
| `gmpxx_defaults` | `set_initial_default_prec(uint64_t)`, `get_initial_default_prec()`, `get_default_prec()`, `set_default_base(int)`, and `get_default_base()` | `set_initial_default_prec(0)` is a no-op. The stored precision is requested precision. Threads that have already snapshotted the default precision are not affected by later stores. The default base is thread-local, defaults to 10, and accepts bases 2 through 62. |
| Precision helpers | `effective_mpf_prec()`, `normalize_mpf_prec()`, `checked_mp_bitcnt()`, `parse_default_prec_env()`, `process_initial_prec()`, `thread_default_prec()` | `effective_mpf_prec()` models GMP limb-boundary precision rounding for expected-value checks. Header code narrows precision through `checked_mp_bitcnt()`. |
| Default precision initialization | `GMPXX_MKII_DEFAULT_PREC` environment parsing | Empty, negative, zero, trailing-garbage, and exception cases fall back to 512 bits. GMP's global default precision APIs are not used by the wrapper. |
| `scalar_normalize_t<T>` | Integral signed types to `int64_t`, unsigned integral types including `bool` to `uint64_t`, `float`/`double` to `double` | Used by scalar leaves and scalar operator overloads through Phase 5. `long double` and compiler `__int128` types are intentionally not scalar operands. |
| `expr_base<Derived>` | `suggested_prec()`, `.eval()`, `eval_to(mpz_class&)`, and `eval_to(mpq_class&)` | `.eval()` returns the expression `result_type`. `suggested_prec()` switches between operand-max and `GMPXX_MKII_NOPRECCHANGE` policies for floating results. |
| `unary_expr<Op, X>` | Stores operand by `const&`, implements `result_type`, `operand()`, `suggested_prec_impl()`, `contains_address()`, `eval_to_prec()`, `eval_to_mpz()`, and `eval_to_mpq()` | Uses the L1 lifetime policy. `-(-x)` is represented as a `pos_op` expression node. |
| `binary_expr<Op, L, R>` | Stores mpf/mpz/mpq/expression operands by `const&`, stores scalar leaves by normalized value, implements `result_type`, `suggested_prec_impl()`, floating-result `get_prec()`, `contains_address()`, `eval_to_prec()`, `eval_to_mpz()`, and `eval_to_mpq()` | Scalar, mpz, and mpq leaves do not contribute to operand-max mpf precision. Mixed mpf/mpz/mpq floating results convert exact operands through required wrapper temporaries. `get_prec()` is a legacy-compatible alias for `suggested_prec()` on floating-result expression nodes. |
| Operation tags | `add_op`, `sub_op`, `mul_op`, `div_op`, `neg_op`, `pos_op` | Direct wrappers over GMP arithmetic. Existing mpf scalar fast paths remain; mpf×mpz/mpq paths use `mpf_set_z`/`mpf_set_q` temporaries because GMP has no direct `mpf_*_z` or `mpf_*_q` APIs. |
| mpz addmul fusion | `is_mpz_addmul_fusable_v`, `addmul_fused_apply()`, `submul_fused_apply()` | Direct `binary_expr<mul_op, ...>` shapes with mpz/mpz or mpz/integral-scalar operands bypass the generic temporary compound-assignment path. Unary-minus, multiplication-chain, and inner-add/subtract forms remain generic. |
| Comparisons | `cmp()`, comparison operators, comparison materialization helpers | Comparisons are immediate operations. Expression operands are evaluated once, scalar/scalar overloads are rejected, and values are compared through exact GMP rational comparison without string or universal `double` fallback. Compiler 128-bit integer operands are accepted for compatibility comparisons without becoming expression scalar leaves. |
| String and stream I/O | `get_str()`, `set_str()`, `to_string()`, `print_mpz`, `print_mpq`, `print_mpf`, `operator<<`, `operator>>`, and expression stream output | GMP-allocated strings are released through the active GMP free function. Integer and rational stream output respects `std::dec`, `std::hex`, `std::oct`, `std::showbase`, `std::uppercase`, width, fill, and adjustment flags; mpf stream output uses GMP formatted output or GMP `mpf_get_str` base formatting without conversion through `double`. |
| User-defined literals | `_mpz`, `_mpq`, `_mpf` in `gmpxx::literals` and global compatibility using-declarations | Raw numeric and string literal overloads use the same base-0 autodetection as no-base string construction. `_mpf` parses literal text directly into `mpf_class` at the wrapper default precision. |
| GMP-only transcendental functions | `pi`, `const_pi`, `e`, `const_e`, `log_two`, `const_log2`, `inv_log_two`, `log_ten`, `const_log10`, `pi_over_two`, `pi_over_four`, `two_pi`, `log`, `log2`, `log10`, `log1p`, `exp`, `exp2`, `exp10`, `expm1`, `sin`, `cos`, `tan`, `asin`, `acos`, `atan`, `atan2`, `sinh`, `cosh`, `tanh`, `asinh`, `acosh`, `atanh`, and `pow`; `gmpxx::mpfc_class` complex `sqrt`, `exp`, `log`, trigonometric, inverse trigonometric, hyperbolic, and inverse hyperbolic overloads | Ported and compatibility-completed in the single header. Implementations use `mpf_t` arithmetic, cached AGM constants, and guard precision; no MPFR/MPC dependency or universal `double` fallback is introduced. Concrete `mpf_class` overloads preserve input/result precision policy. Complex `mpfc_class` overloads are composed from the real GMP-only functions and follow principal branches. |
| Random support | `gmp_randclass`, `random_mpf_expr`, `get_z_bits()`, `get_z_range()`, `get_f()` | `gmp_randclass` is a non-copyable owner for `gmp_randstate_t`. `get_f(mp_bitcnt_t)` and `get_f(mpf_class const&)` return immediate `mpf_class` values. Bare `get_f()` returns `random_mpf_expr`, which evaluates through the normal floating expression assignment path and therefore uses the left-hand side precision on existing-object assignment. |
| Package config | `gmpxx_mkIIConfig.cmake`, `gmpxx_mkIIConfigVersion.cmake`, `gmpxx_mkIITargets.cmake` | Installed consumers can use `find_package(gmpxx_mkII CONFIG REQUIRED)` and link `gmpxx_mkII::gmpxx_mkII`. The config locates GMP without embedding build-tree paths. |

## Operator Summary

| Operator Family | Status | Covered Operand Shapes |
|---|---:|---|
| Binary `+` | Done through Phase 5 | `mpf_class`, `mpz_class`, `mpq_class`, expression, and scalar combinations except scalar/scalar |
| Binary `-` | Done through Phase 5 | `mpf_class`, `mpz_class`, `mpq_class`, expression, and scalar combinations except scalar/scalar |
| Binary `*` | Done through Phase 5 | `mpf_class`, `mpz_class`, `mpq_class`, expression, and scalar combinations except scalar/scalar; integer power-of-two mpf scaling uses `mpf_mul_2exp` |
| Binary `/` | Done through Phase 5 | `mpf_class`, `mpz_class`, `mpq_class`, expression, and scalar combinations except scalar/scalar; `mpz/mpz` remains integer division |
| Binary shifts | Done after Phase 6 | `value << integer` and `value >> integer` are implemented for `mpz_class`, `mpq_class`, `mpf_class`, direct wrapper operands, and expression operands. `mpz_class` also supports `<<=` and `>>=`. `mpz >> n` uses floor division by `2^n`, matching GMP legacy wrapper behavior for negative values. |
| Binary bitwise | Done after Phase 6 | `&`, `|`, `^`, and unary `~` are implemented for `mpz_class`, scalar-mixed mpz operands, and mpz-result expression operands where applicable. Scalar/scalar bitwise expressions remain native C++. |
| Unary `-` | Done through Phase 5 | `-mpf_class`, `-mpz_class`, `-mpq_class`, `-expression`, and `-(-x)` simplification |
| Unary `+` | Done through Phase 5 | `+mpf_class`, `+mpz_class`, `+mpq_class`, and `+expression` |
| Complex arithmetic | Initial implementation | `gmpxx::mpfc_class` supports expression-template `+`, `-`, `*`, `/`, unary `-`, compound assignment, and real-operand promotion for `mpf_class` and supported scalar operands. |
| Scalar arithmetic | Done through Phase 5 | Signed integer, unsigned integer, `float`, and `double` operands normalize to `int64_t`, `uint64_t`, and `double`. |
| `mpz_class` / `mpq_class` operands | Done through Phase 5 | mpz/mpq leaves participate in ET arithmetic and mixed mpf/mpz/mpq expressions. |
| Compound assignment | Done through Phase 5 | `mpf_class`, `mpz_class`, `mpq_class`, expression, cross-wrapper expression, and scalar RHS forms preserve left-hand side object state where applicable. `mpz_class` supports `%=` and bitwise compound assignment; all wrapper types support integer shift compound assignment. |
| Native mpz addmul fusion | Done for Phase 3 | Direct `mpz_class += mpz*mpz`, `mpz_class -= mpz*mpz`, and integral-scalar variants use `mpz_addmul`, `mpz_submul`, `mpz_addmul_ui`, or `mpz_submul_ui` where valid. |
| Comparisons | Done for Phase 4A | `cmp()`, `==`, `!=`, `<`, `<=`, `>`, and `>=`; `operator<=>` remains out of scope. |
| Stream output | Done for Phase 4B | Concrete wrappers and expression nodes support `operator<<`; expression output evaluates once and then streams the concrete result. |
| Stream input | Done for Phase 4B | Concrete wrappers support `operator>>`; expression input remains intentionally unsupported. |
| User-defined literals | Done for Phase 5 | `_mpz`, `_mpq`, and `_mpf` are available under `gmpxx::literals` and at global scope for source compatibility with GMP's C++ binding. |
| Defaults/base policy | Done for Phase 5 | `gmpxx_defaults` exposes precision queries and retains thread-local default-base get/set helpers; no-base string parsing itself follows GMP base-0 autodetection. |
| Package config | Done for Phase 5 | Install-tree package config supports `find_package(gmpxx_mkII CONFIG REQUIRED)`. |
| Random support | Done after Phase 5 | `gmp_randclass` is a non-copyable owner for GMP random state and exposes the GMP C++ random generation surface for mpz/mpf values. Bare `get_f()` is expression/proxy based for destination-precision-preserving assignment. |
| Basic mpf math functions | Done after Phase 5 | `sqrt(mpf_class)`, `abs(mpf_class)`, legacy `neg(mpf_class)`, `mpf_class::set_epsilon()`, and GMP-only `mpf_remainder()` are available. Remaining special functions are deferred. |
| GMP-only transcendental functions | Done for Phase 6 | `pi`, `const_pi`, `e`, `const_e`, `log_two`, `const_log2`, `inv_log_two`, `log_ten`, `const_log10`, `pi_over_two`, `pi_over_four`, `two_pi`, `log`, `log2`, `log10`, `log1p`, `exp`, `exp2`, `exp10`, `expm1`, `sin`, `cos`, `tan`, `asin`, `acos`, `atan`, `atan2`, `sinh`, `cosh`, `tanh`, `asinh`, `acosh`, `atanh`, and `pow` are available for concrete `mpf_class` values. |

## GMP C++ Binding Checklist

Source: GMP 6.3.0 manual
`docs/reference/upstream/gmp-6.3.0/generated/gmp.txt`, section 12
(`C++ Class Interface`), especially sections 12.2 through 12.5.

| Upstream Binding Area | GMP C++ Surface | Current Status | Notes for `gmpxx_mkII` |
|---|---|---:|---|
| Namespace and public class names | `mpz_class`, `mpq_class`, `mpf_class`, `gmp_randclass` | Implemented | Public C++ API is under namespace `gmpxx`. The CMake package target remains `gmpxx_mkII::gmpxx_mkII`. |
| Common arithmetic operators | `+`, `-`, `*`, `/`, unary `+`, unary `-`, compound assignment | Implemented | Expression-template implementation covers concrete wrappers, expressions, and supported scalar operands except scalar/scalar hijacking. |
| Comparisons | `cmp()`, `==`, `!=`, `<`, `<=`, `>`, `>=` | Implemented | Immediate comparisons evaluate expression operands once and compare through GMP values, not strings or a universal `double` fallback. |
| String and stream I/O | `get_str()`, `set_str()`, `operator<<`, `operator>>` | Implemented | Phase 4B implements concrete wrapper string/stream APIs and expression stream output. No-base string APIs follow GMP base-0 autodetection. |
| User-defined literals | `_mpz`, `_mpq`, `_mpf` | Implemented | Available under `gmpxx::literals` and at global scope for GMP `gmpxx.h` compatibility. |
| `mpz_class` constructors | Standard integer/floating/GMP wrapper constructors, string constructors, `const mpz_t` constructor | Partial | Integer constructors and assignments cover standard signed/unsigned integral widths without `int`/`int32_t`/`uint32_t` ambiguity. `double`, `mpf_class`, `mpq_class`, raw `mpz_t`/`mpz_srcptr`, string, expression, copy, and move construction are implemented. Unlike upstream `gmpxx.h`, normalized `long long` operands are intentionally supported through the ABI-stable scalar policy. |
| `mpz_class` arithmetic semantics | Integer arithmetic, truncating `/` and `%` | Implemented | `mpz/mpz` division uses truncating integer division; modulo uses GMP integer remainder semantics. |
| `mpz_class` conversion queries | `get_d()`, `get_si()`, `get_ui()`, `fits_*_p()` | Implemented | Implemented for compatibility with the GMP C++ binding conversion surface. |
| `mpz_class` helpers | `abs`, `sgn`, `sqrt`, `gcd`, `lcm`, `factorial`, `primorial`, `fibonacci`, `swap` | Implemented | Free-function helpers and swap are implemented. Upstream-style static forms `mpz_class::factorial`, `mpz_class::primorial`, and `mpz_class::fibonacci` are restored. Negative factorial/primorial inputs throw `std::domain_error`; oversized helper inputs throw `std::bad_alloc`; Fibonacci follows GMP's negative-index sign convention. |
| `mpq_class` constructors | Integer numerator/denominator, `mpz_class`, `mpf_class`, `double`, string, `const mpq_t` constructor | Partial | Integer, `mpz_class`, `mpf_class`, `double`, raw `mpq_t`/`mpq_srcptr`, string, expression, copy, and move construction are implemented. |
| `mpq_class` canonicalization | `canonicalize()`, canonical string construction/input | Implemented | Public construction, `set_str()`, stream input, and arithmetic maintain canonical rational values where required. |
| `mpq_class` numerator/denominator access | `get_num()`, `get_den()`, `get_num_mpz_t()`, `get_den_mpz_t()` | Implemented | Copy-returning numerator/denominator access and raw mutable/const access are implemented. Mutable raw access is a low-level compatibility surface; callers must restore canonical form with `canonicalize()` after mutation. |
| `mpq_class` helpers | `abs`, `cmp`, `get_d()`, `get_str()`, `set_str()`, `sgn`, `swap` | Implemented | Covered by Phase 4A/4B and later compatibility additions. |
| `mpf_class` constructors | Default, value constructors, value+precision constructors, wrapper conversion constructors, string constructors, `const mpf_t` constructors | Partial | Default, precision-tag, bool, integer+precision, `double`, string+precision, raw `mpf_t`/`mpf_srcptr`, `mpz_class`/`mpq_class` construction with default or explicit precision, expression, copy with default or explicit precision, and move construction are implemented. |
| `mpf_class` assignment | Assignment from values preserves destination precision | Implemented | Existing-object assignment from expressions, strings, and scalar values preserves the left-hand side precision. |
| `mpf_class` precision control | `get_prec()`, `set_prec()`, `set_prec_raw()` | Implemented | Public precision access and GMP-compatible precision mutators are exposed. `set_prec_raw()` has the same upstream restriction: callers must restore the allocated precision before destruction or full precision reset. |
| `mpf_class` conversion queries | `get_d()`, `get_si()`, `get_ui()`, `fits_*_p()` | Implemented | Implemented for compatibility with the GMP C++ binding conversion surface. |
| `mpf_class` helpers | `abs`, `ceil`, `floor`, `trunc`, `hypot`, `sgn`, `sqrt`, `swap` | Implemented | Basic concrete GMP helpers are implemented without MPFR/MPC or `double` fallback. |
| Random state class | `gmp_randclass`, `seed()`, `get_z_bits()`, `get_z_range()`, `get_f()` | Partial | Wrapper-owned random state, seeding, integer random generation, and floating random generation are implemented. Bare `get_f()` is intentionally expression/proxy based so assignment can use destination precision. The upstream varargs `gmp_randclass(gmp_randalg_t, ...)` constructor surface is not fully mirrored. |
| C++ binding limitations | Manual limitations around template expressions and unsupported C++11 types | Deliberate differences | The wrapper uses C++20 concepts and ABI-normalized scalar leaves. `long double` and scalar/scalar ET overloads remain rejected. Compiler `__int128` is supported only as a direct `mpz_class` construction/assignment compatibility surface, not as a scalar expression leaf. |

## Known Divergences from GMP `gmpxx.h`

| Area | `gmpxx_mkII` Behavior | Upstream `gmpxx.h` Behavior / Compatibility Impact | Current Disposition |
|---|---|---|---|
| Default floating precision | `gmpxx_mkII` snapshots a requested default precision through `gmpxx_defaults` and thread-local state; normal wrapper construction does not depend on GMP's global `mpf_set_default_prec()` state. | Upstream follows GMP's global default precision model. Code that mutates `mpf_set_default_prec()` directly and expects later C++ wrapper construction to follow it will not be portable to `gmpxx_mkII`; use `gmpxx_defaults::set_initial_default_prec()` before the target thread first touches the wrapper default. | Deliberate thread-safety and reproducibility policy. |
| Legacy default initializer objects | Legacy global initializer object behavior is not restored. | Upstream exposes legacy initialization hooks around default precision/base behavior. | Deliberate omission; use `gmpxx_defaults`. |
| Language and overload model | Header requires C++20 concepts, rejects scalar/scalar expression-template operators, and normalizes scalar leaves to ABI-stable `int64_t`, `uint64_t`, or `double`. | Upstream `gmpxx.h` supports older C++ modes and has a different template/SFINAE surface. Code relying on scalar/scalar overload hijacking or pre-C++20 compilation is outside the `gmpxx_mkII` target. | Deliberate v2.0.0 design boundary. |
| Math-function expression precision | Direct arithmetic expression assignment to an existing `mpf_class` preserves the destination precision, but free functions such as `sqrt()` and `hypot()` materialize an `mpf_class` result before assignment. | Upstream expression templates can keep `sqrt(1/f)` and similar function calls inside the expression tree, so assignment can evaluate the function at the destination precision. This affects `cxx/t-prec.cc` function-expression cases. | Deliberate difference unless the full math/transcendental function surface is expression-template-enabled consistently. |
| `long double` scalar expressions | `long double` is not a scalar expression leaf. | Upstream has broader machine scalar conversion surfaces in some contexts. | Deliberate exclusion until a precise GMP-only policy is specified. |
| Compiler 128-bit integers | `__int128` and `unsigned __int128` are accepted for direct `mpz_class` construction/assignment and compatibility comparisons where available, but not as expression scalar leaves. | Upstream does not define an ABI-stable expression policy for compiler extension integer widths. | Deliberate direct-conversion-only extension. |
| `gmp_randclass` constructors and `get_f()` precision | Random state ownership, seeding, `get_z_bits()`, `get_z_range()`, default/MT/LC initialization forms, and `get_f()` are implemented. Bare `get_f()` returns an expression/proxy so assignment can preserve destination precision. `get_f(prec)` and `get_f(mpf)` return immediate `mpf_class` values at the requested precision. | Upstream includes a fuller `gmp_randclass(gmp_randalg_t, ...)` varargs constructor surface. Its random floating expression forms are tied to GMP's global default precision and expression assignment behavior, so upstream default-precision checks expect `mpf_get_default_prec()` for some default and `get_f(prec)` assignment cases. | Partial compatibility; varargs constructor mirroring remains open, and wrapper default precision remains deliberately independent of GMP's global default. |
| Dropped v1 bridge surface | Fortran bridge APIs from the eager v1.0.0 header are not part of v2.0.0. | Not an upstream `gmpxx.h` surface, but a compatibility difference from this project's earlier header. | Not planned for v2.0.0. |

## Test Summary

| Test Target | Status | Coverage |
|---|---:|---|
| `test_abi_fingerprint` | Present | `scalar_normalize_t` ABI categories, `gmpxx_expr`, `phase0_operand`, `std::common_type` compatibility decay for wrappers and expression nodes, `std::numeric_limits` surface for GMP wrapper types, operator constraints rejecting scalar/scalar ET operands, and diagnostic-only expression type names. |
| `test_exception_support` | Present | Compiler `try`/`throw`/`catch` support and wrapper exception propagation for invalid string construction. |
| `test_headers` | Present | Standalone compilation of the public `gmpxx_mkII.h` header without relying on prior standard-library includes. |
| `test_construction_copy` | Present | `mpf_class`, `mpz_class`, and `mpq_class` default construction, bool construction, raw `mpf_t` construction, copy/move construction, C++11 noexcept compatibility surface, mpf copy-with-explicit-precision construction, copy/move assignment, mpf move-assignment precision-match and precision-mismatch behavior, member/free `swap`, unambiguous mpz integral construction/assignment, and mpf precision-preserving integral/double/string construction and assignment cases. |
| `test_mpf_math_functions` | Present | `sqrt`, `abs`, legacy `neg`, `ceil`, `floor`, `trunc`, `hypot`, scalar mixed `hypot` forms from `t-ops2f`, `mul_2exp`, `div_2exp`, `set_epsilon`, `mpf_remainder`, sign queries, and exact mpz/mpq helper functions match GMP behavior and preserve precision where applicable. |
| `test_mpf_transcendent_functions` | Present | `pi`, `const_pi`, `log_two`, `const_log2`, `log`, `log2`, `log10`, `log1p`, `exp`, `expm1`, `sin`, `cos`, `tan`, `atan`, `atan2`, and `pow` compile-time surface, upstream-derived reference literals, precision-doubling checks, near-zero/near-one paths, trigonometric reduction and axis cases, special values, identities, precision policy, and domain errors. |
| `test_mpf_extended_transcendent_functions` | Present | Extended constants (`e`, `log_ten`, `inv_log_two`, `pi_over_two`, `pi_over_four`, `two_pi`), inverse trigonometric functions, hyperbolic functions, inverse hyperbolic functions, `exp2`, `exp10`, compile-time surface, identities, domain errors, and precision preservation. |
| `test_type_conversions` | Present | `mpz_class` integer/double/string/base construction and assignment, string assignment failure safety, raw `mpz_t`/`mpq_t` construction, compiler 128-bit integer construction/assignment where available, wrapper-to-wrapper conversion construction and assignment among `mpf_class`, `mpz_class`, and `mpq_class`, legacy mixed-wrapper expression conversion and overload-resolution coverage from `t-mix`, floating-result expression `get_prec()` compatibility, explicit bool conversion, `mpf/mpz/mpq` scalar conversion queries, fit predicates, `mpq_class` integer/mpz/double/string/base construction, scalar/string assignment, and canonicalization, `mpf_class(mpz_class/mpq_class, precision)`, `mpf_class` wrapper assignment precision preservation, and mutable/const mpq numerator/denominator accessors. |
| `test_numeric_equivalence` | Present | Bit-exact comparison against raw GMP `mpf_t` reference calculations for unary and binary operations, nested expressions, mixed precisions, positive/negative/zero values, string construction, and double construction. |
| `test_alloc_count` | Present | Registers GMP memory hooks before object construction and verifies allocation counts for `dst = a + b`, `dst = a + b + c`, `dst = a + b + c + d`, and `dst = (a+b) * (c+d)` as `0, 0, 0, 1`. |
| `test_alias_safety` | Present | Self-alias and mixed-alias expression assignment cases compare against independent raw GMP references. |
| `test_thread_safety` | Present | Thread-local default precision lazy snapshots, isolation from GMP global default precision, `set_initial_default_prec()` before thread spawn, and snapshot immutability after first thread-local touch. |
| `test_scalar_arithmetic` | Present | Scalar arithmetic for signed integers, unsigned integers, `float`, and `double` in both operand orders, increment/decrement operators for mpz/mpq/mpf wrappers, including `INT64_MIN`, `UINT64_MAX`, precision 8, and expression/scalar composition. |
| `test_scalar_alloc_count` | Present | Allocation counts for scalar fast paths and double slow paths: `a + 5LL`, `a + 5.0`, `a + b + 5LL`, and `dst += 5LL`. |
| `test_compound_assign` | Present | `+=`, `-=`, `*=`, and `/=` with `mpf_class`, expression, scalar, self-alias, mixed-alias, precision-preservation cases, plus `t-ops3` coverage for wrapper shift compounds, mpz `%=`/bitwise compounds, and cross-wrapper expression RHS forms. |
| `test_long_width_dispatch` | Present | Normal LP64 and simulated LLP64 `uint64_t` dispatch, including large integers and `INT64_MIN`. |
| `test_long_width_dispatch_llp64` | Present | Same source compiled with `GMPXX_MKII_TEST_LLP64_PATH` to force the slow branch in the normal test suite. |
| `test_precision_policy` | Present | Construction and `.eval()` use expression precision; existing-object arithmetic expression assignment and compound assignment preserve destination precision; `mpf_class::set_prec()` and `set_prec_raw()` follow GMP precision mutation semantics. Function-call expression cases from `cxx/t-prec.cc` remain a documented divergence. |
| `test_unary_minus_simplification` | Present | Unary `+`, unary `-`, and mpz `~` cases for mpz/mpq/mpf with independently chosen values, plus `-(-mpf)` and `-(-(expression))` identity behavior while preserving assignment precision. |
| `test_power_of_two_fusion` | Present | Integer power-of-two multiplication/division, negative signed scalars including `INT64_MIN`, generic non-power cases, scalar-left division, and compound `*=`, `/=` precision preservation. |
| `test_mpz_arithmetic` | Present | mpz arithmetic, scalar mixing, truncating integer division, `%=` modulo, shift and compound-shift operators, scalar-mixed bitwise and complement operators, ET composition including independently chosen nested product/add/subtract shapes, self-alias, compound assignment, unary operators, exact integer helpers, static helper compatibility forms, helper exception policy, and negative Fibonacci semantics. |
| `test_mpq_arithmetic` | Present | mpq arithmetic, scalar mixing, shift operators, canonicalization, ET composition, mpz-expression promotion inside mixed mpq expressions, compound assignment, and unary operators. |
| `test_mixed_type_arithmetic` | Present | mpf×mpz, mpf×mpq including `t-ops2f` mpf/mpq arithmetic cases, mpz×mpq, legacy-compatible mpz/mpq plus double result typing, `t-ops2qf` direct mpf/mpq shift and high-precision double-minimum cases, result type checks, mpf shift operators, and mixed precision policy. |
| `test_mpfc_arithmetic` | Present | `gmpxx::mpfc_class` construction, real/imag accessors and mutators, lazy arithmetic, real operands, division, existing-object assignment precision preservation, equality comparisons, free `real`/`imag`, `conj`, `norm`, `abs`, `arg`, `polar`, and deterministic `std::complex<double>` arithmetic smoke coverage. |
| `test_mpfc_io` | Present | `gmpxx::mpfc_class` `std::complex`-style `(real,imag)` stream output/input, whitespace handling, failure safety across early and late parse failures, expression stream output, destination precision preservation, locale decimal-point behavior, strict rejection of real-only input forms, and scientific/fixed/showpos formatting. |
| `test_mpfc_transcendent_functions` | Present | `gmpxx::mpfc_class` complex `sqrt`, `exp`, `log`, trigonometric, inverse trigonometric, hyperbolic, inverse hyperbolic functions, expression inputs, real-axis cases, and principal square-root behavior. |
| `test_mpz_mpq_alloc_count` | Present | Test-only wrapper constructor counters for mpz/mpq/mpf temporaries in mixed-expression paths, including legacy-compatible mpz/mpq plus double paths that avoid mpf temporaries. |
| `test_mpz_addmul_fusion` | Present | Compile-time fusable-shape checks, fused-path counters, runtime GMP-equivalence checks, scalar sign and `INT64_MIN` cases, alias cases, and non-fused expression checks. |
| `test_mpz_addmul_alloc_count` | Present | Wrapper temporary and fused-counter checks for direct mpz addmul/submul and integral-scalar fast paths. |
| `test_mpz_addmul_alloc_count_llp64` | Present | Same allocation-count source compiled with `GMPXX_MKII_TEST_LLP64_PATH` to verify width-fallback scalar temporaries. |
| `test_comparisons` | Present | Compile-time comparison constraints, exact mpz/mpq comparisons, mpf and mixed comparisons including `t-ops2f` mpf/mpq relation cases, expression comparisons, scalar and compiler 128-bit integer edge cases, operator consistency with `cmp()`, and integer-division comparison semantics. |
| `test_io_and_strings` | Present | `get_str()`, `set_str()`, `to_string()`, raw `print_mpz`/`print_mpq`/`print_mpf`, raw GMP pointer stream input/output, wrapper stream input/output, iostream syntax compatibility, independently chosen stream formatting regressions, mpf locale decimal-point input/output, expression stream output, failure safety, precision preservation, base flags, legacy-style partial input consumption, width/fill adjustment, and GMP-allocated string freeing. |
| `test_user_defined_literals` | Present | `_mpz`, `_mpq`, and `_mpf` literal construction, large string and numeric literals, rational canonicalization, direct mpf text parsing, base-0 literal parsing, and raw numeric literal base independence. |
| `test_defaults_policy` | Present | Default precision getters, thread-local precision snapshot behavior, independence from GMP global default precision, default-base get/set, no-base string base-0 autodetection, stream/base independence, invalid-base errors, and thread-local base behavior. |
| `test_random` | Present | `gmp_randclass` construction modes, deleted copy/move semantics, deterministic seeding, `get_z_bits`, `get_z_range`, immediate `get_f(prec)`/`get_f(mpf)` generation, construction from `get_f(prec)`, existing-object assignment precision preservation, bare `get_f()` expression/proxy assignment preserving destination precision, documented default-precision divergence from upstream random floating checks, and LC initialization failure handling. |
| `test_gmpxx_mkII` | Present | Ported legacy compatibility coverage for constructors, assignment, arithmetic, comparisons, string/base handling, precision behavior, conversion helpers, math functions including `mpf_remainder`, random examples, and stream output. Blocks that depend on still-unsupported legacy APIs are temporarily disabled in-source with TODO comments. |
| `test_package_config` | Present | Installs the project into a temporary prefix, configures an external consumer with `find_package(gmpxx_mkII CONFIG REQUIRED)`, builds it, and runs it. |
| `GMPXX_MKII_NOPRECCHANGE` build | Present | The new extended transcendental target and the existing transcendental target pass when expression construction precision is the thread-local default instead of max operand precision; run the full maintained suite before release. |
| Environment override check | Present manually | `GMPXX_MKII_DEFAULT_PREC=1024 ctest --test-dir build --output-on-failure` passes. |
| Clang coverage | Present | Clang passes both default and `GMPXX_MKII_NOPRECCHANGE=ON` Phase 5 builds. |
| TSan coverage | Present for T4 | ThreadSanitizer build passes `test_thread_safety`. |
| ASan/UBSan coverage | Present | GCC 15.2.0 AddressSanitizer/UndefinedBehaviorSanitizer build passes all thirty Phase 6 tests before the extended transcendental split; rerun for the new target before release. |

## Verified Build Matrix

| Compiler / Build | Result | Notes |
|---|---:|---|
| GCC 15.2.0, default | Partial rerun | The new `test_mpf_extended_transcendent_functions` target and existing `test_mpf_transcendent_functions` target pass. |
| GCC 15.2.0, `GMPXX_MKII_NOPRECCHANGE=ON` | Partial rerun | The new `test_mpf_extended_transcendent_functions` target and existing `test_mpf_transcendent_functions` target pass. |
| GCC 15.2.0, `GMPXX_MKII_TEST_LLP64_PATH` | Pass | All thirty tests pass with the slow path forced before the extended transcendental split. |
| Clang, default | Pass | All thirty tests pass before the extended transcendental split. |
| Clang, `GMPXX_MKII_NOPRECCHANGE=ON` | Pass | All thirty tests pass before the extended transcendental split. |
| GCC 15.2.0, TSan | Pass | `test_thread_safety` passes. |
| GCC 15.2.0, ASan/UBSan | Pass | All thirty tests pass before the extended transcendental split. |
| C++17 direct include | Fails as intended | Header `static_assert(__cplusplus >= 202002L, ...)` fires. |

## Missing Feature Summary

| Category | Missing Items | Planned Phase / Notes |
|---|---|---|
| Scalar arithmetic | Additional scalar types beyond the Phase 1 normalized set | Phase 1 supports `int64_t`, `uint64_t`, and `double` leaves only. |
| Extended native integer fusion | Unary-minus addmul/submul normalization, multiplication-chain fusion, and distributive expansion such as `(b+c)*d` | Deferred beyond Phase 3. Direct `mpz_addmul`/`mpz_submul` compound-assignment fusion is implemented. |
| Remaining math functions | Expression-aware math overloads and special functions beyond the current concrete GMP-only surface, such as gamma/erf/Bessel families | Future GMP-only work; no MPFR/MPC fallback. Basic concrete GMP math helpers, concrete `mpf_class` transcendental functions, inverse trig/hyperbolic helpers, `mpf_remainder`, and exact integer helpers are implemented. |
| Remaining GMP C++ binding compatibility | Member/static integer helper forms and full upstream `gmp_randclass(gmp_randalg_t, ...)` constructor compatibility | Tracked in the GMP C++ binding checklist above. These are compatibility surface gaps, not expression-template core gaps. |
| Extended literal parsing | Additional literal forms beyond `_mpz`, `_mpq`, and `_mpf` | The Phase 5 UDL surface is intentionally limited to supported integer, rational, and floating text/machine literal forms. |
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
thread-local compatibility setting. No-base string constructors and string APIs
parse through GMP's base-0 autodetection, matching `gmpxx.h`. Stream formatting
remains controlled by iostream flags, not by the wrapper default base.

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
