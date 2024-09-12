# gmpxx_mkII.h: A GMP C++ Wrapper for High-Precision Calculations

## Overview

`gmpxx_mkII.h` is a C++ class designed to facilitate high-precision arithmetic operations using the GMP library, licensed under the 2-clause BSD license. GMP is a C library for multiple-precision floating-point computations. This class provides a convenient, alternative C++ wrapper around the GMP library's functionalities.

## Features

- **High-Level Source Compatibility:** Highly compatible with `gmpxx.h`, ensuring a smooth transition from the original GMP C++ wrapper.
- **Provides three modes**: mkII mode (default), mkIISR mode (no-precision-change-mode), and compatibility mode (does not use namespace and no enhancements).
- **Enhanced C++ Usability and Interface**: `gmpxx_mkII.h` improves upon the original `gmpxx.h` by removing template limitations and avoiding the use of complex macros or lazy evaluations. This straightforward coding style not only enhances clarity and maintainability but also addresses the interface limitations previously noted in the GMP C++ Interface Limitations. (https://gmplib.org/manual/C_002b_002b-Interface-Limitations).
- **Introduction of namespace**: To avoid conflicts and enhance code organization, we have introduced a namespace in `gmpxx_mkII.h`. However, if needed, this namespace can be easily disabled.
- **Comprehensive Mathematical Functions:** This includes implementations of trigonometric functions (sin, cos, tan, asin, acos, atan etc) and transcendental functions (log, exp), enhancing the library's utility for complex calculations.
- **Header-Only Library:** This library eliminates the need for the `libgmpxx` library, simplifying integration and deployment.
- **Quality assurance:** Enhanced with our own rigorous tests and building on the foundation of proven GMP tests
- **Licensing:** Distributed under the 2-clause BSD License, offering flexibility for both open-source and proprietary use.
- **Platform Support:** Optimized for 64-bit Linux and macOS in LP64 environments, ensuring reliable performance across major systems.
- **Original Development:** Written entirely from scratch, providing a clean, optimized, and maintainable code base.

## Requirements

- 64bit linux and 64bit macOS (LP64 environments)
- C++17 or higher

## Installation

To install this library, first clone the repository and then use the make install command:

```bash
git clone https://github.com/nakatamaho/gmpxx_mkII.git
cd gmpxx_mkII
sudo make PREFIX=/usr/local install
```

This will copy `gmpxx_mkII.h` to the appropriate location on your system.

## Usage

To effectively use `gmpxx_mkII.h` in your C++ projects, you must adjust your include directives and possibly namespace usage depending on the mode you choose to operate in.
The default precision is 512 bits = 154 decimal significant digits. We also introduced `namespace gmp` to avoid conflicts.

- **mkII mode (Default)**: This mode provides enhanced functionalities and optimizations over the original `gmpxx.h`.
  
  ```cpp
  #include <gmpxx_mkII.h>
  using namespace gmp;  // Simplifies access to the library's functionalities
  ```
  
  This setup allows you to use all the functions and classes in `gmpxx_mkII.h` without prefixing them with `gmp::`.

- **mkIISR mode (No-Precision-Change Mode)**: Designed for scenarios where precision settings must be explicitly controlled, this mode assumes changes to precision are made only through environment variables or directly at runtime during the main function's execution, ensuring that the library does not make any automatic adjustments.
  
  ```cpp
  #include <gmpxx_mkII.h>
  #define ___GMPXX_MKII_NOPRECCHANGE___  // Enable mkIISR mode at the preprocessor level
  using namespace gmp;
  ```
  
  In this mode, make sure to define `___GMPXX_MKII_NOPRECCHANGE___` before including the header file to activate the specific functionalities.

- **Compatibility Mode**:
  ```cpp
  #include <gmpxx_mkII.h>
  ```
  Compatibility Mode is designed for those who require strict backward compatibility with older versions of the `gmpxx.h` library. When compiling your project, ensure to include the flags `-D___GMPXX_POSSIBLE_BUGS___` and `-D___GMPXX_STRICT_COMPATIBILITY___` to activate this mode. This setup avoids using namespaces and maintains behavior consistent with earlier library versions.

In Compatibility Mode, do not use `using namespace gmp;` to avoid namespace conflicts. This mode ensures that your existing code that relies on older `gmpxx.h` features works without modifications.

## Link

When linking your project with `gmpxx_mkII.h`, removing the -lgmpxx link option used with the original GMP C++ wrapper is advisable. While keeping it may not cause immediate harm, removing it ensures that you link specifically against the correct library version provided by gmpxx_mkII, avoiding potential conflicts or ambiguities.

## Improvements from original gmpxx.h

### Performance Improvements

In gmpxx_mkII.h, we have observed significant performance improvements, particularly in single-core computations for inner products. When operating in mkIISR mode, enhancements of up to 25% compared to gmpxx.h have been documented. This increase in efficiency is primarily attributed to the elimination of complex macro expansions, which are not required in the new implementation. However, in multi-core scenarios, the performance of gmpxx_mkII.h is roughly equivalent to that of the original gmpxx.h.

For detailed benchmarking results, particularly those conducted on a Ryzen 3970X 32-core processor, please refer to the following [benchmark report](https://github.com/nakatamaho/gmpxx_mkII/blob/main/benchmarks/00_inner_product/all_operations_Ryzen_3970X_32-Core_500000000_512.pdf).

While our benchmarking efforts have been extensive, we acknowledge that not all performance areas have been thoroughly explored yet. Notably, in some cases, such as matrix-matrix operations, the performance has been observed to be  inferior to that of the original `gmpxx.h`. Further investigation and optimization are underway to address these discrepancies.

### Enhanced Mathematical Functions

One of the major enhancements introduced with `gmpxx_mkII.h` over the original `gmpxx.h` is the significant expansion of available mathematical functions. These functions include:

- **Logarithmic Functions:** `log`, `log2`, `log10`
- **Exponential and Power Functions:** `exp`, `pow`
- **Trigonometric Functions:** `cos`, `sin`, `tan`, `acos`, `asin`, `atan`, `atan2`
- **Hyperbolic Functions:** `cosh`, `sinh`, `tanh`, `acosh`, `asinh`, `atanh`

`log` and `atan` are implemented using the Arithmetic-Geometric Mean (AGM) method, while `exp`, `cos`, and `sin` are implemented using Taylor series expansions. The other functions are combinations of these implementations.
Implemented by referring to the implementation of MPFR. For details, see the [MPFR documentation](https://www.mpfr.org/mpfr-current/mpfr.pdf).

### No C++ Interface Limitations

`gmpxx_mkII.h` expands the capabilities of the standard GMP C++ bindings, removing the restrictions detailed in the [GMP C++ Interface Limitations] (https://gmplib.org/manual/C_002b_002b-Interface-Limitations).

#### Example: Calculating Minimum Values

Here is a simple example demonstrating how to use `gmpxx_mkII.h` to perform arithmetic operations and comparisons more intuitively:
```cpp
mpf_class a = 1.0, b = 2.0, c = 3.0, d;
d = std::min(a, b + c);
```
This operation might seem straightforward, but it was not supported directly in the original `gmpxx.h` due to its limitations in handling such expressions. In the case of `gmpxx.h`, you would need to explicitly "cast" the operation as follows:
```cpp
mpf_class a = 1.0, b = 2.0, c = 3.0, d;
d = std::min(a, mpf_class(b + c));
```

### Configuring Precision at Runtime Using Environment Variables

`gmpxx_mkII.h` allows dynamically configuring the default precision settings for arithmetic operations via environment variables. This feature will enable users to adjust precision without modifying the source code. Here's how you can use these environment variables:

- **GMPXX_MKII_DEFAULT_PREC**: This environment variable sets the default precision for all calculations. Specify the precision as the number of bits. For example, setting this variable to "128" would configure the default precision to 128 bits.
  
  ```bash
  export GMPXX_MKII_DEFAULT_PREC=128
  ```
or
  ```bash
  GMPXX_MKII_DEFAULT_PREC=128 ./a.out
  ```
- **GMPXX_MKII_DEFAULT_PREC_RAW**: This environment variable offers a direct way to set a specific raw precision level that overrides the default settings. This might be used for testing or to ensure consistent behavior across different environments.

  ```bash
  export GMPXX_MKII_DEFAULT_PREC_RAW=256
  ```
  ```bash
  GMPXX_MKII_DEFAULT_PREC_raw=128 ./a.out
  ```
In any case, the default precision is 512 bits = 154 decimal significant digits.

## Compatibility Differences from Original gmpxx.h

`gmpxx_mkII.h` introduces several modifications to behavior and functionality compared to the original `gmpxx.h`. These changes are designed to improve predictability and align more closely with standard C++ practices, but they also affect how certain operations are handled:

- **No Binary Compatibility**:
  `gmpxx_mkII.h` does not maintain binary compatibility with `gmpxx.h`. This means binaries compiled with the original library cannot be directly replaced with those compiled using `gmpxx_mkII.h`. This allows for architectural improvements and the introduction of new features.

- **Difference in Precision Handling in Evaluations**:
  ```cpp
  const int small_prec = 64, medium_prec = 128, very_large_prec = 256;
  mpf_set_default_prec(medium_prec);
  mpf_class f(3.0, small_prec);
  mpf_class g(1 / f, very_large_prec);
  ```
  Unlike `gmpxx.h`, where the precision of an expression is lazily evaluated upon assignment, `gmpxx_mkII.h` evaluates expressions immediately with the precision of the assigned variable. In `gmpxx_mkII.h`, the expression `1/f` is evaluated with `small_prec`, then transferred to `g` at `very_large_prec`, ensuring more predictable behavior.

- **Difference in Precision Assignment in Random Number Generation**:
  ```cpp
  const int medium_prec = 128, large_prec = 512;
  mpf_set_default_prec(medium_prec);
  gmp_randclass r1(gmp_randinit_default);
  mpf_class f(0, large_prec);
  f = r1.get_f();
  ```
  In `gmpxx_mkII.h`, random numbers are generated at the default precision (`medium_prec`), not the precision of the variable `f` (`large_prec`) as in `gmpxx.h`. This modification aligns with typical C++ assignment behaviors, where the right-hand side of an assignment does not adapt to the left side's attributes.

## Quality assurance

To ensure the reliability and stability of `gmpxx_mkII.h`, we have implemented a comprehensive quality assurance process. In addition to our suite of custom tests, we have successfully ported and passed the tests originally included with GMP, with minimal modifications.

## Contributing

Contributions to the gmpxx_mkII.h are welcome. Please submit pull requests or open issues to suggest improvements or report bugs.

## License

This project is licensed under the 2-clause BSD License.

## ChatGPT4 (memo)

* https://chat.openai.com/c/e88b85d9-dbca-4bdc-bfb3-52cff97ddae4
* https://chat.openai.com/c/268fe353-3a3f-44ea-8519-987b674d7d12
* https://chat.openai.com/c/46a93858-ea63-462f-b392-b9ed3a717454
* https://chat.openai.com/c/e472fda8-1397-44de-bef5-09acb143dbcf
