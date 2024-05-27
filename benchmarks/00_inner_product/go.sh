uname -a
cat /proc/cpuinfo | grep 'model name' | head -1
echo
executables=(
    "inner_product_gmp_10_naive"
    "inner_product_gmp_11_openmp"
    "inner_product_gmp_12_mpblas_orig"
    "inner_product_gmp_12_mpblas_mkII"
    "inner_product_gmp_12_mpblas_mkIISR"
    "inner_product_gmp_12_mpblas_compat"
    "inner_product_gmp_13_mpblas_openmp_orig"
    "inner_product_gmp_13_mpblas_openmp_mkII"
    "inner_product_gmp_13_mpblas_openmp_mkIISR"
    "inner_product_gmp_13_mpblas_openmp_compat"
)
for exe in "${executables[@]}"; do
    COMMAND_LINE="./$exe 100000000 1024"
    echo $COMMAND_LINE
    $COMMAND_LINE
    if [ -f gmon.out ]; then
        mv gmon.out "gmon_${exe}.out"
        gprof ./$exe "gmon_${exe}.out" > "gprof_${exe}.txt"
    fi
    echo
done
