# AGENTS.md

Guidance for AI coding agents (Codex CLI, Claude Code, Cursor, etc.)
working in this repository. Humans should read `README.md` instead.

## Project

`gmpxx_mkII` — a single-header C++20 expression-template wrapper around
GMP floating-point numbers (`mpf_t`), GMP integers (`mpz_t`), and GMP
rationals (`mpq_t`).

This repository is **GMP-only**. Do not introduce MPFR or MPC into the core
library, tests, examples, or build system unless the maintainer explicitly
requests it for a separate experiment.

Public API lives in **one** file: `gmpxx_mkII.h`. Do not split it
without prior discussion in an issue.

License: **BSD-2-Clause**. Every new source file must carry the
SPDX header `// SPDX-License-Identifier: BSD-2-Clause`.

## Build & Test

The project uses CMake + ctest. The canonical sequence is:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j
ctest --test-dir build --output-on-failure
```

Required toolchain:
- C++20 compiler (GCC ≥ 12, Clang ≥ 15)
- GMP ≥ 6.2
- CMake ≥ 3.20

Before opening a PR, all of the following ctest targets must pass:
`test_construction`, `test_arithmetic`, `test_mixed_scalar`,
`test_compound`, `test_precision`.

If you add a new public-facing capability, add a corresponding
`test_<feature>` target. Do not extend an existing test file with
unrelated coverage; one feature per test binary keeps regression
triage cheap.

Classify tests by behavior before adding new targets. Group tests that
exercise the same public capability or regression class into the same
`test_<feature>` binary when that keeps the target coherent. Create a
new target only when the coverage is a distinct feature, policy, or
failure mode.

### Test completeness and symmetry

When adding or changing functionality, inspect the surrounding tests
for missing cases before considering the task complete. If symmetry or
coverage is missing, add the tests in the same change.

- For floating-result APIs that accept `MpfExpr`, cover every relevant
  public input category: `mpf_class`, `mpz_class`, `mpq_class`, and
  composed expressions.
- For native integer/rational APIs, cover `mpz_class` and `mpq_class`
  separately. Do not assume that coverage for one GMP type proves the
  other, because canonicalization, parsing, and exact division policies
  differ.
- For operations where sign matters, cover both positive and negative
  inputs for each relevant category. Integer-only cases should still
  include negative integers when the operation's behavior differs by
  sign or when symmetry would otherwise be misleading.
- For expression-template APIs, include at least one composed
  expression input, not only leaf objects.
- For assignment paths, check that assigning into an existing
  `mpf_class` preserves the left-hand side precision.
- For mixed scalar operations, cover both operand orders when both are
  supported (`mpf_class op scalar` and `scalar op mpf_class`) plus
  the corresponding compound assignment if it exists.
- For stream and conversion APIs, cover representative formatting or
  base-policy cases plus the remaining unsupported cases in
  `STATUS.md`.

Do not treat a direct translation of an MPFR-centered test as
sufficient by itself. Adapt it to this project's GMP-only API and then
check whether the resulting test matrix has obvious gaps.

When porting coverage or behavior from GMP's legacy `gmpxx.h`, preserve
the public compatibility intent unless the new design explicitly
documents a difference. Translate tests to the local `mpf_class`,
`mpz_class`, and `mpq_class` implementation instead of routing through
MPFR, MPC, or `double`.

When you discover a test gap, report it explicitly in your final
response. State whether you filled it in the current change or left it
as a known gap, and update `STATUS.md` when the gap affects documented
coverage or missing-feature status.

## Implementation workflow

When modifying or adding behavior, check whether the change applies to
all three public numeric types: `mpf_class`, `mpz_class`, and
`mpq_class`.

- If the same capability is meaningful for more than one type, either
  implement and test each relevant type or document why a type is
  intentionally excluded.
- Keep result-type policy explicit. Floating expressions should produce
  `mpf_class` results; exact integer/rational operations should preserve
  native `mpz_class` or `mpq_class` results only where that behavior is
  part of the public API or explicitly requested.
- Do not use MPFR as an implementation shortcut for precision,
  rounding, parsing, formatting, transcendental functions, or reference
  comparisons in normal tests.
- After changing implemented functionality, tests, scope, or known
  gaps, update `STATUS.md` in the same change.
- After writing or changing program code, check whether `STATUS.md`
  and `README.md` still describe the current implementation accurately.
  Update them in the same change when the code changes supported
  features, build/test instructions, precision/default policies, known
  limitations, or documented coverage.

## Architectural invariants

These are not stylistic preferences. Violating them silently breaks
correctness or lifetime safety. Reviewers reject patches that change
these without an explicit rationale in the commit message.

### Storage policy (`ExprStorage`)

- **Leaf RAII types** (`mpf_class`, `mpz_class`, `mpq_class`) are
  stored in expression nodes by `const&`.
- **Intermediate expression nodes** (`BinaryExpr<Op,L,R>`,
  `ScalarExpr<S>`, unary negation) are stored **by value**.

Storing intermediates by reference produces dangling references the
moment an expression is bound to `auto`. Storing leaves by value
forces unnecessary copies of GMP objects, especially `mpf_t`, which is
heap-allocating.

### Precision propagation

- Evaluation precision of any floating expression node = `std::max` of
  its children's `get_prec()`.
- Assignment to an existing `mpf_class` **preserves the LHS
  precision**. The RHS is evaluated at the LHS precision, not at its
  own.
- The `prec_tag` constructor is the only way to set a precision
  numerically; the bare `mpf_class(int)` constructor must remain
  unambiguous as "construct from integer value at default precision".
  Do not collapse the two.
- GMP `mpf_t` does not have MPFR-style explicit rounding modes. Do not
  add fake rounding-mode parameters or silently emulate MPFR semantics.

### Concepts

- `MpfExpr` requires both `eval(mpf_class&)` and
  `get_prec() -> mp_bitcnt_t`. Anything that participates in operator
  overloading on the GMP floating-point side must satisfy this.
- `Scalar` is `std::is_arithmetic && !std::is_same_v<T,bool>`. Do not
  widen this to include user-defined numeric types; route those
  through the appropriate GMP expression concept instead.

### Temporaries

Use the project's sanctioned RAII temporary helpers for intermediate
GMP objects during `eval`. Do not introduce ad-hoc `mpf_t`, `mpz_t`,
or `mpq_t` locals with manual `mpf_init`/`mpf_clear`,
`mpz_init`/`mpz_clear`, or `mpq_init`/`mpq_clear` pairs — they leak on
exception and make move-safety review harder.

If a helper is missing for a new temporary category, add the helper
first and test its move/destruction behavior before using it in
expression evaluation.

## Pitfalls (read this before writing GMP code)

### GMP ADL silent double fallback

`mpf_class` and friends may expose conversion paths to built-in
floating-point types for compatibility with GMP's legacy C++ wrapper.
If you call a free function (`abs`, `floor`, `ceil`, `trunc`, `fmod`,
`copysign`, `log`, `exp`, `sin`, `cos`, `pow`, …) on a GMP type and
the corresponding overload is **missing from our headers**, overload
resolution may fall through to `<cmath>` via conversion to `double`.
The result is silently truncated to 53-bit IEEE-754 precision.
Symptom: relative error capped at ~2⁻⁵³ regardless of the working
precision.

When adding any new operation that touches `mpf_class`, `mpz_class`, or
`mpq_class`:

1. Grep the header for an existing overload taking the GMP type.
2. If absent and GMP provides a direct API, add an overload that uses
   the direct GMP API (`mpf_abs`, `mpz_abs`, `mpq_abs`, etc.).
3. If GMP does not provide the operation directly, either implement a
   GMP-only algorithm with precision-stress tests or leave the operation
   unsupported. Do **not** satisfy it by calling MPFR, MPC, or `<cmath>`.
4. Add a precision-stress test that would fail under a 53-bit
   truncation (i.e. compute something whose true relative error is
   `< 2⁻⁵³` and assert it).

This is not a hypothetical class of bug; it is the single most common
regression mode in this style of wrapper.

### No MPFR escape hatch

GMP `mpf_t` arithmetic has different semantics from MPFR. It has
user-controlled precision but no per-operation rounding-mode argument
and no correctly-rounded transcendental library. Therefore:

- Do not include `<mpfr.h>` or link `libmpfr` from this repository.
- Do not use MPFR for reference calculations in unit tests unless the
  test is explicitly marked as an external comparison test and is not
  required for the normal GMP-only test suite.
- Do not implement `log`, `exp`, `sin`, `cos`, `atan`, `pow`, or
  similar functions by converting through `double`.
- If a transcendental function is required, implement it as a GMP-only
  algorithm with documented precision policy and tests that expose
  double fallback.

### Compound assignment fast paths

`+=`, `-=`, `*=`, `/=` against supported scalar types must dispatch to
the specialised GMP entry points when available (`mpf_add_ui`,
`mpf_sub_ui`, `mpf_mul_ui`, `mpf_div_ui`, `mpz_add_ui`, `mpq_mul`, …)
rather than constructing unnecessary temporaries.

The naive path works, but allocates. Benchmarks under
`tests/test_compound` would not catch the allocation; a microbenchmark
must be added if you change this dispatch.

### GMP object moves

Moving `mpf_class`, `mpz_class`, or `mpq_class` must leave the source
in a state where its destructor is safe and does not free memory now
owned by the destination. Never leave two wrappers owning the same GMP
limb allocation.

Do not "optimise" move construction or move assignment by copying the
raw GMP struct without auditing ownership transfer, moved-from state,
self-assignment, and exception safety. Double-free and use-after-free
are the failure modes.

### Rational canonicalization

Every public `mpq_class` value must be canonical unless a narrow
internal helper explicitly documents otherwise. String constructors,
manual numerator/denominator construction, and low-level assignments
must check whether `mpq_canonicalize` is required.

Never compare, hash, or serialize rationals before canonicalization.

## Coding style

- 4-space indent, no tabs.
- Brace style: K&R / Stroustrup; opening brace on the same line for
  functions and control flow.
- `snake_case` for functions and variables, `PascalCase` for types
  and concepts, `UPPER_SNAKE` for macros.
- No `using namespace std;` at namespace scope. `using` aliases inside
  function bodies are fine.
- Preserve source compatibility with the public names expected from
  GMP's C++ wrapper (`mpf_class`, `mpz_class`, `mpq_class`) unless a
  design note explicitly says otherwise.
- Internals should live under a clearly private `detail` namespace.
  Do not expose helper types as accidental public API.
- Header guards: `#pragma once`. We do not target compilers that lack
  it.
- `clang-format` is not currently enforced via CI. Match the
  surrounding code.

## Commit messages

Subject line: imperative mood, ≤ 72 chars, no trailing period,
prefixed with the area when relevant
(`gmpxx_mkII.h:`, `tests/test_precision:`, `cmake:`, …).

Body: bullet list of concrete changes, wrapped at 72 columns. State
the *invariant* preserved or the *bug* fixed, not the diff. Example:

```
gmpxx_mkII.h: fix dangling ref in BinaryExpr<Op, BinaryExpr, …>

- ExprStorage<BinaryExpr<…>> was specialised to const& by mistake;
  intermediate nodes must be stored by value (see AGENTS.md).
- Add test_precision regression case `chained_three_op_lifetime`
  that triggers the dangling read under -fsanitize=address.
```

Write commit logs in **English**. The maintainer reads them, and so
do downstream packagers.

## What agents should not do without asking

- Add a new dependency (header-only or otherwise).
- Add MPFR or MPC as a dependency.
- Split `gmpxx_mkII.h` into multiple headers.
- Change the precision-propagation rule, even "to match
  Boost.Multiprecision" or similar.
- Introduce CI configuration. The maintainer runs CI manually on
  multiple distros (Debian, Ubuntu, Alpine, Rocky, openSUSE) and
  prefers to wire it up himself.
- Reformat unrelated code in the same commit as a functional change.

## Useful one-liners for agents

Run a single test with verbose GMP output:

```bash
ctest --test-dir build -R test_precision -V
```

Re-configure from scratch (after CMake list edits):

```bash
rm -rf build && cmake -S . -B build && cmake --build build -j
```

Run all tests under AddressSanitizer (catches the dangling-ref class
of bug above):

```bash
cmake -S . -B build-asan -DCMAKE_CXX_FLAGS="-fsanitize=address,undefined -g -O1"
cmake --build build-asan -j
ctest --test-dir build-asan --output-on-failure
```

Always run the ASan build before sending a PR that touches expression
template machinery or the `ExprStorage` specialisations.

## Upstream reference material

Local upstream reference material for GMP lives under:

- `docs/reference/upstream/gmp-6.3.0/`

The reference directory uses this layout:

- `source/`
  - copied upstream Texinfo, Info, HTML, and adjacent documentation files
- `generated/`
  - generated plain text, HTML, and Info files produced by `makeinfo`
- `curated/`
  - project-specific Markdown notes for implementing and reviewing
    `gmpxx_mkII`

Read curated Markdown first. Use generated text or HTML manuals to verify
upstream semantics when needed. Do not rely on memory for GMP API behavior.

The upstream documentation files under `source/` and `generated/` are
reference material and may have licenses different from the BSD-2-Clause
license used for the `gmpxx_mkII` library source.

## GMP reference material

Local GMP reference material lives in:

- `docs/reference/upstream/gmp-6.3.0/source/gmp.texi`
- `docs/reference/upstream/gmp-6.3.0/generated/gmp.txt`
- `docs/reference/upstream/gmp-6.3.0/generated/gmp.html`
- `docs/reference/upstream/gmp-6.3.0/curated/gmp-wrapper-reference.md`

When modifying any of the following, read the curated GMP wrapper reference
first:

- `mpf_class`
- `mpz_class`
- `mpq_class`
- GMP object lifetime management
- expression-template storage and evaluation
- precision propagation for `mpf_class`
- assignment preserving destination precision
- string construction and parsing
- rational canonicalization
- scalar interoperation
- stream formatting and base policies
- conversion between `mpf_class`, `mpz_class`, and `mpq_class`

Use the generated GMP text or HTML manual to verify upstream GMP semantics when
needed.

Important rules:

- every initialized `mpf_t` must be cleared exactly once
- every initialized `mpz_t` must be cleared exactly once
- every initialized `mpq_t` must be cleared exactly once
- moved-from wrapper objects must remain destructible
- assignment from floating expressions must preserve destination precision
- expression precision is the maximum precision across the expression tree
- string constructors must check GMP parse failures
- rationals must be canonicalized where required
- internal conversions must not use decimal string round-trips unless that
  behavior is explicitly part of the public formatting/parsing API
- scalar overloads must not accept `bool`

## Reference update scripts

Reference material is updated by this script:

- `tools/update-gmp-reference.sh`

Run it from the corresponding upstream documentation directory, for example:

```sh
cd "$HOME/mplapack/external/gmp/work/install/gmp-6.3.0/doc"
"$HOME/gmpxx_mkII/tools/update-gmp-reference.sh" .
```
