# gmpxx_mkII.h: A GMP C++ Wrapper for High-Precision Calculations

## Overview

`gmpxx_mkII.h` is a C++ class designed to facilitate high-precision arithmetic operations using the GMP library, which is licensed under the 2-clause BSD license. GMP is a C library for multiple-precision floating-point computations. This class provides a convenient, alternative C++ wrapper around the GMP library's functionalities.

## Features

- **High-Level Source Compatibility:** Highly compatible with `gmpxx.h`, ensuring a smooth transition from the original GMP C++ wrapper.
- **Provides three modes**: mkII mode (default), mkIISR mode (no-precision-change-mode; usually faster than original `gmpxx.h`), and compatibility mode (does not use namespace and no enhancements)
- **Comprehensive Mathematical Functions:** This includes implementations of trigonometric functions (sin, cos, tan, asin, acos, atan) and transcendental functions (log, exp), enhancing the library's utility for complex calculations.
- **Straightforward Coding Style:** Implemented as typical classes without using (very complex) macros or lazy evaluation, prioritizing clarity and maintainability in the code.
- **Header-Only Library:** This library eliminates the need for the `libgmpxx` library, simplifying integration and deployment.
- **Licensing:** Distributed under the 2-clause BSD License, offering flexibility for both open-source and proprietary use.
- **Platform Support:** Optimized for 64-bit Linux and macOS in LP64 environments, ensuring reliable performance across major systems.
- **Original Development:** Written entirely from scratch, providing a clean, optimized, and maintainable code base.

## Requirements

- C++17 or higher

## Installation

To install this library, first clone the repository and then use the make install command:

```bash
git clone https://github.com/nakatamaho/gmpxx_mkII.git
sudo make PREFIX=/usr/local install
```

This will copy `gmpxx_mkII.h` to the appropriate location on your system.

## Usage

To effectively use `gmpxx_mkII` in your C++ projects, you must adjust your include directives and possibly namespace usage depending on the mode you choose to operate in.

- ** mkII mode (Default)**: This mode provides enhanced functionalities and optimizations over the original `gmpxx.h`.
  
  ```cpp
  #include <gmpxx_mkII.h>
  using namespace gmp;  // Simplifies access to the library's functionalities
  ```
  
  This setup allows you to use all the functions and classes in `gmpxx_mkII` without needing to prefix them with `gmp::`.

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
  Compatibility Mode is designed for those who require strict backward compatibility with older versions of the `gmpxx.h` library. When compiling your project, ensure to include the flags `-D___GMPXX_POSSIBLE_BUGS___` and `-D___GMPXX_STRICT_COMPATIBILITY___` to activate this mode. This setup avoids the use of namespaces and maintains behavior consistent with earlier library versions.

In Compatibility Mode, do not use `using namespace gmp;` to avoid namespace conflicts. This mode ensures that your existing code that relies on older `gmpxx.h` features works without modifications.

## Link

When linking your project with `gmpxx_mkII.h`, it is advisable to remove the -lgmpxx link option that was used with the original GMP C++ wrapper. While keeping it may not cause immediate harm, removing it ensures that you are linking specifically against the correct library version provided by gmpxx_mkII, avoiding any potential conflicts or ambiguities.

## Improvements from original gmpxx.h


## Contributing

Contributions to the gmpxx_mkII.h are welcome. Please submit pull requests or open issues to suggest improvements or report bugs.

## Supported Platforms

- 64-bit Linux and macOS in LP64 environments

## License

This project is licensed under the 2-clause BSD License.

## ChatGPT4 (memo)

* https://chat.openai.com/c/e88b85d9-dbca-4bdc-bfb3-52cff97ddae4
* https://chat.openai.com/c/268fe353-3a3f-44ea-8519-987b674d7d12
* https://chat.openai.com/c/46a93858-ea63-462f-b392-b9ed3a717454
* https://chat.openai.com/c/e472fda8-1397-44de-bef5-09acb143dbcf
