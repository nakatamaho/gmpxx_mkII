# GMP wrapper reference for gmpfrxx_mkII

This file is a project-specific, curated reference extracted from the GMP
manual. It is not a replacement for the upstream manual.

Primary upstream source:

- `../source/gmp.texi`

Generated references:

- `../generated/gmp.txt`
- `../generated/gmp.html`
- `../generated/gmp.info`

## Rules for gmpfrxx_mkII

### `mpz_class` lifetime

- `mpz_class` must own exactly one initialized `mpz_t`.
- Every initialized `mpz_t` must be cleared exactly once.
- Copy construction and copy assignment must create independent GMP integers.
- Move construction and move assignment must not cause double-clear.
- Moved-from objects must remain destructible.

### `mpq_class` lifetime

- `mpq_class` must own exactly one initialized `mpq_t`.
- Every initialized `mpq_t` must be cleared exactly once.
- Copy construction and copy assignment must create independent GMP rationals.
- Move construction and move assignment must not cause double-clear.
- Moved-from objects must remain destructible.

### Rational canonicalization

- `mpq_class` values created or modified through numerator/denominator-level
  operations must be canonicalized when required.
- String construction for rationals must leave the object in a valid GMP
  rational state.
- Do not assume non-canonical rationals compare or print as expected.

### String construction

- `mpz_class` string construction should use GMP integer input APIs directly.
- `mpq_class` string construction should use GMP rational input APIs directly.
- Check return values from GMP string parsing functions.
- Constructors must not leave partially initialized objects in an unsafe state.

### Conversion to MPFR

- Conversion from `mpz_class` or `mpq_class` to `mpfr_class` should use MPFR/GMP
  conversion APIs, not decimal string round-trips.
- Conversion must respect the destination or temporary MPFR precision policy of
  `gmpfrxx_mkII`.

### Expression-template participation

- `mpz_class` and `mpq_class` participate in the `MpfrExpr` interface.
- Their `eval` operations must produce MPFR values through direct GMP-to-MPFR
  conversion paths.
- Their `get_prec` behavior must be consistent with expression precision
  propagation in `gmpfrxx_mkII`.

## Review checklist

Before changing GMP wrapper code, check:

- no uninitialized `mpz_t` or `mpq_t` is passed to GMP
- no initialized `mpz_t` or `mpq_t` is leaked
- no initialized `mpz_t` or `mpq_t` is cleared twice
- moved-from objects are still valid for destruction
- string constructors check parse failures
- rationals are canonicalized where required
- expression temporaries cannot dangle
- tests cover the changed behavior
