// square root calculation
#include <iostream>
#include <iomanip>
#include <cmath>
#include "gmpxx_mkII.h" // Adjust the path to the header file appropriately

int main() {
    int decimal_precision = 50;                                                         // You can change this value to adjust the precision
    int bit_precision = static_cast<int>(std::ceil(decimal_precision * std::log2(10))); // Calculate bit precision from decimal
    mpfr::defaults::set_default_prec(bit_precision);

    // Initialization and precision setting
    mpfr::mpfr_class x(1.0); // initial guess
    mpfr::mpfr_class prev_x;
    mpfr::mpfr_class two(2.0); // Constant 2

    std::cout << std::fixed << std::setprecision(decimal_precision); // Set output to fixed point notation with desired digits

    std::cout << "Calculating sqrt(2) in " << decimal_precision << " decimal precision" << std::endl;
    // Calculating sqrt(2) using Newton's method
    int iteration = 0;
    do {
        prev_x = x;              // Store current x value for comparison
        x = (x + two / x) / two; // Newton's method formula
        std::cout << "Iteration" << std::setw(14) << iteration + 1 << ": " << x << std::endl;
        iteration++;
    } while (mpfr::abs(x - prev_x) > mpfr::mpfr_class(pow(10, -decimal_precision))); // Continue until convergence

    // Comparison with mpfr::sqrt
    mpfr::mpfr_class sqrt2 = mpfr::sqrt(two); // Calculate sqrt(2) using MPFR
    std::cout << "Result using mpfr::sqrt: " << sqrt2 << std::endl;

    return 0;
}
