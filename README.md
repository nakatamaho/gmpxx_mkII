# Yet Another GMP C++ Wrapper for High-Precision Calculations

## Overview

`gmpxx_mkII` is a C++ class designed to facilitate high-precision arithmetic operations using the GMP library, which is licensed under the 2-clause BSD license. GMP is a C library for multiple-precision floating-point computations. This class provides a convenient, alternative C++ wrapper around the GMP library's functionalities.

### Features

- High-level source compatibility with `gmpxx.h`
- Header-only library; libgmpxx is not needed anymore.
- Implementation of trigonometric (sin, cos, tan, asin, acos, atan, etc.) and transcendental functions (log, exp)
- Licensed under the 2-clause BSD License
- Written from scratch

## Requirements

- C++17 or higher

## Installation

To install this library, first clone the repository and then use the make install command:

```
bash
git clone https://github.com/nakatamaho/gmpxx_mkII.git ; sudo make PREFIX=/usr/local install
```

This will copy `gmpxx_mkII.h` to the appropriate location on your system.

## Usage

To use `gmpxx_mkII` in your C++ projects, you need to change the include directive from the original GMP wrapper:

Replace:

```cpp
#include <gmpxx.h>
```

With:

```cpp
#include <gmpxx_mkII.h>
```

Additionally, to simplify access to its functionalities, you can include the namespace directive in your code:

```cpp
using namespace gmp;
```

This will allow you to use all the functions and classes in `gmpxx_mkII` without prefixing them with `gmp::`.


# Building Your Project

To compile a project that uses mpfr_class, link against the MPFR library:

# Documentation

For more detailed documentation, refer to the MPFR library's official documentation.

# Contributing

Contributions to the mpfr_class are welcome. Please submit pull requests or open issues to suggest improvements or report bugs.

# License

This project is licensed under the 2-clause BSD License.

# ChatGPT4

* https://chat.openai.com/c/e88b85d9-dbca-4bdc-bfb3-52cff97ddae4
* https://chat.openai.com/c/268fe353-3a3f-44ea-8519-987b674d7d12
* https://chat.openai.com/c/46a93858-ea63-462f-b392-b9ed3a717454
* https://chat.openai.com/c/e472fda8-1397-44de-bef5-09acb143dbcf
