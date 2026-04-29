// SPDX-License-Identifier: BSD-2-Clause

#include "gmpxx_mkII.h"

#include <cassert>
#include <stdexcept>

namespace {

inline void throw_expr() {
    throw std::invalid_argument("Test");
}

void test_basic_exception_support() {
    bool caught = false;
    try {
        throw_expr();
    } catch (std::invalid_argument&) {
        caught = true;
    }
    assert(caught);
}

void test_wrapper_exception_support() {
    bool caught = false;
    try {
        (void)mpz_class("not-an-integer");
    } catch (std::invalid_argument const&) {
        caught = true;
    }
    assert(caught);
}

}  // namespace

int main() {
    test_basic_exception_support();
    test_wrapper_exception_support();
    return 0;
}
