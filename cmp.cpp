#if defined USE_ORIGINAL_GMPXX
#include <gmpxx.h>
#else
#include "gmpxx_mkII.h"
using namespace gmp;
#endif

#include <ostream>
#include <iostream>
#include <algorithm>

int main() {
    // Initialize mpf_class variables
    mpf_class a = 1.0;
    mpf_class b = 2.0;
    mpf_class c = 3.0;
    mpf_class d;

    // Calculate the minimum between b and c, then find the maximum between a and the result
    d = std::min(a, b+c);

    // Output the result
    std::cout << "The maximum of " << a << " and the minimum of " << b << " and " << c << " is " << d << std::endl;

    return 0;
}
