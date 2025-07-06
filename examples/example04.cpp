// calculation of pi using Gauss-Legendre algorithm

#include <iostream>
#include <iomanip>
#include <cmath>        // For std::pow function
#include "gmpxx_mkII.h" // Set the correct path to the mpf_class definition header

int main() {
    // Set initial values
    int decimal_precision = 100;                                                            // You can change this value to adjust the precision
    int bit_precision = static_cast<int>(std::ceil(decimal_precision * std::log2(10))) * 2; // Calculate bit precision from decimal, * 2 is for sure
    gmpxx::gmpxx_defaults::this_thread::set_precision(bit_precision);
    std::cout << std::fixed << std::setprecision(decimal_precision); // Set output to fixed-point notation with 50 digits of precision
    std::cout << "calcuate pi using arithmetic-geometric mean" << std::endl;

    int iteration = 0;
    gmpxx::mpf_class one(1.0); // Second parameter sets the precision in bits
    gmpxx::mpf_class two(2.0);
    gmpxx::mpf_class four(4.0);
    gmpxx::mpf_class a(one), b(one / gmpxx::sqrt(two)), t(0.25), p(one);
    gmpxx::mpf_class a_next, b_next, t_next, pi, pi_previous;
    gmpxx::mpf_class epsilon = gmpxx::pow(gmpxx::mpf_class(10.0), gmpxx::mpf_class(-decimal_precision)); // Define the tolerance level for convergence
    bool converged = false;

    while (!converged) {
        iteration++;
        a_next = (a + b) / two;
        b_next = gmpxx::sqrt(a * b);
        t_next = t - p * gmpxx::pow(a - a_next, two);
        p = two * p;

        // Update values for the next iteration
        a = a_next;
        b = b_next;
        t = t_next;

        // Calculate pi
        pi_previous = pi;
        pi = gmpxx::pow(a + b, two) / (four * t);

        // Check for convergence
        if (gmpxx::abs(pi - pi_previous) < epsilon) {
            converged = true;
        }

        std::cout << "Iteration " << iteration << ": π=" << pi << std::endl;
    }

    std::cout << "Converged after " << iteration << " iterations." << std::endl;
    // Comparison with gmpxx::sqrt
    gmpxx::mpf_class _pi = gmpxx::const_pi(); // Calculate const_pi() using MPFR
    std::cout << "Result by this calculation:  " << pi << std::endl;
    std::cout << "Result  using gmpxx::const_pi: " << _pi << std::endl;

    return 0;
}
