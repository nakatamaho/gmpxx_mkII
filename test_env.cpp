#include <iostream>
#include <cstdlib>
#include <gmpxx.h>

mpf_class a;

int main() {

    std::cout << "Default precision: " << a.get_prec() << std::endl;

    return 0;
}
