// calculation of pi using Gauss-Legendre algorithm

#include <iostream>
#include <iomanip>
#include <cmath>        // For std::pow function
#include "mpfr_class.h" // Set the correct path to the mpfr_class definition header

int main() {
    // Set initial values
    int decimal_precision = 100;                                                            // You can change this value to adjust the precision
    int bit_precision = static_cast<int>(std::ceil(decimal_precision * std::log2(10))) * 2; // Calculate bit precision from decimal, * 2 is for sure
    mpfr::defaults::set_default_prec(bit_precision);
    std::cout << std::fixed << std::setprecision(decimal_precision); // Set output to fixed-point notation with 50 digits of precision

    int iteration = 0;
    mpfr::mpfr_class one(1.0); // Second parameter sets the precision in bits
    mpfr::mpfr_class two(2.0);
    mpfr::mpfr_class four(4.0);
    mpfr::mpfr_class a(one), b(one / mpfr::sqrt(two)), t(0.25), p(one);
    mpfr::mpfr_class a_next, b_next, t_next, pi, pi_previous;
    mpfr::mpfr_class epsilon = mpfr::pow(mpfr::mpfr_class(10.0), mpfr::mpfr_class(-decimal_precision)); // Define the tolerance level for convergence
    bool converged = false;

    while (!converged) {
        iteration++;
        a_next = (a + b) / two;
        b_next = mpfr::sqrt(a * b);
        t_next = t - p * mpfr::pow(a - a_next, two);
        p = two * p;

        // Update values for the next iteration
        a = a_next;
        b = b_next;
        t = t_next;

        // Calculate pi
        pi_previous = pi;
        pi = mpfr::pow(a + b, two) / (four * t);

        // Check for convergence
        if (mpfr::abs(pi - pi_previous) < epsilon) {
            converged = true;
        }

        std::cout << "Iteration " << iteration << ": π" << pi << std::endl;
    }

    std::cout << "Converged after " << iteration << " iterations." << std::endl;
    // Comparison with mpfr::sqrt
    mpfr::mpfr_class _pi = mpfr::const_pi(); // Calculate const_pi() using MPFR
    std::cout << "Result by this calculation:  " << pi << std::endl;
    std::cout << "Result using mpfr::const_pi: " << _pi << std::endl;

    return 0;
}
