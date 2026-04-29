# Copyright (c) 2026
#      Nakata, Maho
#      All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
# OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
# OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.

function(add_gmpxx_mkii_benchmark name)
    add_executable(gmpxx_mkii_benchmark_${name} ${name}.cpp)
    target_link_libraries(gmpxx_mkii_benchmark_${name}
        PRIVATE gmpxx_mkII::gmpxx_mkII)

    add_executable(gmpxx_benchmark_${name} ${name}.cpp)
    target_compile_definitions(gmpxx_benchmark_${name}
        PRIVATE USE_ORIGINAL_GMPXX)
    target_link_libraries(gmpxx_benchmark_${name} PRIVATE GMP::GMPXX GMP::GMP)
endfunction()

function(add_gmp_native_benchmark name)
    add_executable(gmp_native_benchmark_${name} ${name}_native.cpp)
    target_link_libraries(gmp_native_benchmark_${name} PRIVATE GMP::GMP)
endfunction()

add_gmpxx_mkii_benchmark(00_Rdot)
add_gmpxx_mkii_benchmark(01_Raxpy)
add_gmpxx_mkii_benchmark(02_Rgemv)
add_gmpxx_mkii_benchmark(03_Rgemm)

add_gmp_native_benchmark(00_Rdot)
add_gmp_native_benchmark(01_Raxpy)
add_gmp_native_benchmark(02_Rgemv)
add_gmp_native_benchmark(03_Rgemm)
