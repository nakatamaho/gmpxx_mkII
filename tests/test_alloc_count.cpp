// SPDX-License-Identifier: BSD-2-Clause

#include "gmpxx_mkII.h"

#include <atomic>
#include <cassert>
#include <cstdlib>

namespace {

std::atomic<int> alloc_count{0};

void* count_alloc(std::size_t n) {
    ++alloc_count;
    return std::malloc(n);
}

void* count_realloc(void* p, std::size_t, std::size_t n) {
    return std::realloc(p, n);
}

void count_free(void* p, std::size_t) {
    std::free(p);
}

void presize(mpf_class& x, std::uint64_t requested) {
    if (static_cast<std::uint64_t>(x.get_prec()) !=
        gmpxx_detail::effective_mpf_prec(requested)) {
        mpf_set_prec(x.get_mpf_t(), gmpxx_detail::checked_mp_bitcnt(requested));
    }
}

}  // namespace

int main() {
    mp_set_memory_functions(count_alloc, count_realloc, count_free);

    constexpr std::uint64_t requested = 256;
    mpf_class a("1.5", gmpxx_detail::checked_mp_bitcnt(requested));
    mpf_class b("2.5", gmpxx_detail::checked_mp_bitcnt(requested));
    mpf_class c("3.5", gmpxx_detail::checked_mp_bitcnt(requested));
    mpf_class d("4.5", gmpxx_detail::checked_mp_bitcnt(requested));
    mpf_class dst(gmpxx_detail::checked_mp_bitcnt(requested));
    presize(dst, requested);

    alloc_count = 0;
    dst = a + b;
    assert(alloc_count.load() == 0);

    alloc_count = 0;
    dst = a + b + c;
    assert(alloc_count.load() == 0);

    alloc_count = 0;
    dst = a + b + c + d;
    assert(alloc_count.load() == 0);

    alloc_count = 0;
    dst = (a + b) * (c + d);
    assert(alloc_count.load() == 1);

    return 0;
}
