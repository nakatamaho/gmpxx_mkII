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

find_package(OpenMP)

function(configure_eager_benchmark target subdir)
    target_include_directories(${target} PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/${subdir}")
    set_target_properties(${target} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${subdir}")
    if(target MATCHES "openmp" AND OpenMP_CXX_FOUND)
        target_link_libraries(${target} PRIVATE OpenMP::OpenMP_CXX)
    endif()
endfunction()

function(add_mkii_variant subdir source base suffix)
    set(target "${base}_${suffix}")
    add_executable(${target} "${subdir}/${source}")
    configure_eager_benchmark(${target} ${subdir})
    target_link_libraries(${target} PRIVATE gmpxx_mkII::gmpxx_mkII)
    if(suffix STREQUAL "mkII_NOPRECCHANGE")
        target_compile_definitions(${target} PRIVATE GMPXX_MKII_NOPRECCHANGE)
    endif()
endfunction()

function(add_gmpxx_variant subdir source base)
    set(target "${base}_orig")
    add_executable(${target} "${subdir}/${source}")
    configure_eager_benchmark(${target} ${subdir})
    target_compile_definitions(${target} PRIVATE USE_ORIGINAL_GMPXX)
    target_link_libraries(${target} PRIVATE GMP::GMPXX GMP::GMP)
endfunction()

function(add_kernel_variants subdir source base)
    add_gmpxx_variant(${subdir} ${source} ${base})
    add_mkii_variant(${subdir} ${source} ${base} mkII)
    add_mkii_variant(${subdir} ${source} ${base} mkII_NOPRECCHANGE)
endfunction()

function(add_native_benchmark subdir source target)
    add_executable(${target} "${subdir}/${source}")
    configure_eager_benchmark(${target} ${subdir})
    target_link_libraries(${target} PRIVATE gmpxx_mkII::gmpxx_mkII GMP::GMP)
endfunction()

add_native_benchmark(00_Rdot Rdot_gmp_C_native_01.cpp
    Rdot_gmp_C_native_01)
add_native_benchmark(00_Rdot Rdot_gmp_C_native_openmp_01.cpp
    Rdot_gmp_C_native_openmp_01)
add_kernel_variants(00_Rdot Rdot_gmp_kernel_01.cpp Rdot_gmp_kernel_01)
add_kernel_variants(00_Rdot Rdot_gmp_kernel_02.cpp Rdot_gmp_kernel_02)
add_kernel_variants(00_Rdot Rdot_gmp_kernel_03.cpp Rdot_gmp_kernel_03)
add_kernel_variants(00_Rdot Rdot_gmp_kernel_04.cpp Rdot_gmp_kernel_04)
add_kernel_variants(00_Rdot Rdot_gmp_kernel_openmp_01.cpp
    Rdot_gmp_kernel_openmp_01)
add_kernel_variants(00_Rdot Rdot_gmp_kernel_openmp_02.cpp
    Rdot_gmp_kernel_openmp_02)

add_native_benchmark(01_Raxpy Raxpy_gmp_C_native_01.cpp
    Raxpy_gmp_C_native_01)
add_native_benchmark(01_Raxpy Raxpy_gmp_C_native_openmp_01.cpp
    Raxpy_gmp_C_native_openmp_01)
add_kernel_variants(01_Raxpy Raxpy_gmp_kernel_01.cpp Raxpy_gmp_kernel_01)
add_kernel_variants(01_Raxpy Raxpy_gmp_kernel_02.cpp Raxpy_gmp_kernel_02)
add_kernel_variants(01_Raxpy Raxpy_gmp_kernel_openmp_01.cpp
    Raxpy_gmp_kernel_openmp_01)
add_kernel_variants(01_Raxpy Raxpy_gmp_kernel_openmp_02.cpp
    Raxpy_gmp_kernel_openmp_02)

add_native_benchmark(02_Rgemv Rgemv_gmp_C_native_01.cpp
    Rgemv_gmp_C_native_01)
add_native_benchmark(02_Rgemv Rgemv_gmp_C_native_openmp_01.cpp
    Rgemv_gmp_C_native_openmp_01)
add_kernel_variants(02_Rgemv Rgemv_gmp_kernel_01.cpp Rgemv_gmp_kernel_01)
add_kernel_variants(02_Rgemv Rgemv_gmp_kernel_02.cpp Rgemv_gmp_kernel_02)
add_kernel_variants(02_Rgemv Rgemv_gmp_kernel_openmp_01.cpp
    Rgemv_gmp_kernel_openmp_01)
add_kernel_variants(02_Rgemv Rgemv_gmp_kernel_openmp_02.cpp
    Rgemv_gmp_kernel_openmp_02)

add_native_benchmark(03_Rgemm Rgemm_gmp_C_native_01.cpp
    Rgemm_gmp_C_native_01)
add_native_benchmark(03_Rgemm Rgemm_gmp_C_native_02.cpp
    Rgemm_gmp_C_native_02)
add_native_benchmark(03_Rgemm Rgemm_gmp_C_native_openmp_01.cpp
    Rgemm_gmp_C_native_openmp_01)
add_native_benchmark(03_Rgemm Rgemm_gmp_C_native_openmp_02.cpp
    Rgemm_gmp_C_native_openmp_02)
add_kernel_variants(03_Rgemm Rgemm_gmp_kernel_01.cpp Rgemm_gmp_kernel_01)
add_kernel_variants(03_Rgemm Rgemm_gmp_kernel_02.cpp Rgemm_gmp_kernel_02)
add_kernel_variants(03_Rgemm Rgemm_gmp_kernel_03.cpp Rgemm_gmp_kernel_03)
add_kernel_variants(03_Rgemm Rgemm_gmp_kernel_openmp_01.cpp
    Rgemm_gmp_kernel_openmp_01)
add_kernel_variants(03_Rgemm Rgemm_gmp_kernel_openmp_02.cpp
    Rgemm_gmp_kernel_openmp_02)
add_kernel_variants(03_Rgemm Rgemm_gmp_kernel_openmp_03.cpp
    Rgemm_gmp_kernel_openmp_03)
