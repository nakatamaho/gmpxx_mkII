// square root calculation
#include <iostream>
#include <iomanip>
#include <cmath>
#include "gmpxx_mkII.h" // Adjust the path to the header file appropriately

int main() {
    int decimal_precision = 50;                                                         // You can change this value to adjust the precision
    int bit_precision = static_cast<int>(std::ceil(decimal_precision * std::log2(10))); // Calculate bit precision from decimal
    mpf_set_default_prec(bit_precision);

    // Initialization and precision setting
    gmpxx::mpf_class x(1.0); // initial guess
    gmpxx::mpf_class prev_x;
    gmpxx::mpf_class two(2.0); // Constant 2

    std::cout << std::fixed << std::setprecision(decimal_precision); // Set output to fixed point notation with desired digits

    std::cout << "Calculating sqrt(2) in " << decimal_precision << " decimal precision" << std::endl;
    // Calculating sqrt(2) using Newton's method
    int iteration = 0;
    do {
        prev_x = x;              // Store current x value for comparison
        x = (x + two / x) / two; // Newton's method formula
        std::cout << "Iteration" << std::setw(14) << iteration + 1 << ": " << x << std::endl;
        iteration++;
    } while (gmpxx::abs(x - prev_x) > gmpxx::mpf_class(pow(10, -decimal_precision))); // Continue until convergence

    // Comparison with gmpxx::sqrt
    gmpxx::mpf_class sqrt2 = gmpxx::sqrt(two); // Calculate sqrt(2) using GMP
    std::cout << "Result using gmpxx::sqrt:  " << sqrt2 << std::endl;

    return 0;
}
