uname -a
cat /proc/cpuinfo | grep 'model name' | head -1
echo
executables=(
    "inner_product_gmp_10_naive"
    "inner_product_gmp_11_openmp"
    "inner_product_gmp_12_0_mpblas_orig"
    "inner_product_gmp_12_0_mpblas_mkII"
    "inner_product_gmp_12_0_mpblas_mkIISR"
    "inner_product_gmp_12_1_mpblas_orig"
    "inner_product_gmp_12_1_mpblas_mkII"
    "inner_product_gmp_12_1_mpblas_mkIISR"
    "inner_product_gmp_12_2_mpblas_orig"
    "inner_product_gmp_12_2_mpblas_mkII"
    "inner_product_gmp_12_2_mpblas_mkIISR"
    "inner_product_gmp_13_0_mpblas_openmp_orig"
    "inner_product_gmp_13_0_mpblas_openmp_mkII"
    "inner_product_gmp_13_0_mpblas_openmp_mkIISR"
    "inner_product_gmp_13_1_mpblas_openmp_orig"
    "inner_product_gmp_13_1_mpblas_openmp_mkII"
    "inner_product_gmp_13_1_mpblas_openmp_mkIISR"
)
for exe in "${executables[@]}"; do
    COMMAND_LINE="/usr/bin/time ./$exe 500000000 512"
    echo $COMMAND_LINE
    $COMMAND_LINE
    if [ -f gmon.out ]; then
        mv gmon.out "gmon_${exe}.out"
        gprof ./$exe "gmon_${exe}.out" > "gprof_${exe}.txt"
    fi
    echo
done
