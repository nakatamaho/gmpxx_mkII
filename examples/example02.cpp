// examples/example02.cpp
#include "gmpxx_mkII.h"
#include <iostream>
#include <iomanip>

int main() {
    gmpxx::mpf_class num(2.0);
    gmpxx::mpf_class result;
    result = sqrt(num);
    std::cout << "Square root of 2.0 = " << std::setprecision(50) << result << std::endl;
    return 0;
}
