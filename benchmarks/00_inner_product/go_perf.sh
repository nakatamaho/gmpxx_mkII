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
    COMMAND_LINE="sudo perf record -g ./$exe 500000000 512"
        echo $COMMAND_LINE
#    $COMMAND_LINE 2>&1 | tee perf_record_output_${exe}.txt
    $COMMAND_LINE 2>&1

    COMMAND_LINE2="sudo perf report"
    echo $COMMAND_LINE2
    $COMMAND_LINE2 2>&1 | tee perf_output_output_${exe}.txt

    echo
done
