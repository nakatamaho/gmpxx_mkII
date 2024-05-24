uname -a
cat /proc/cpuinfo | grep 'model name' | head -1
./inner_product_gmp_10_naive                10000000 1024
./inner_product_gmp_11_openmp               10000000 1024
./inner_product_gmp_12_mpblas_orig          10000000 1024
./inner_product_gmp_12_mpblas_mkII          10000000 1024
./inner_product_gmp_12_mpblas_compat        10000000 1024
./inner_product_gmp_12_mpblas_mkIISR        10000000 1024
./inner_product_gmp_13_mpblas_openmp        10000000 1024
./inner_product_gmp_13_mpblas_openmp_compat 10000000 1024
./inner_product_gmp_13_mpblas_openmp_mkII   10000000 1024
./inner_product_gmp_13_mpblas_openmp_mkIISR 10000000 1024

