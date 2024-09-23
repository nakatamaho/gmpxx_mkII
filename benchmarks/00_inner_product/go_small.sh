uname -a
cat /proc/cpuinfo | grep 'model name' | head -1
echo
executables=(
    "inner_product_gmp_C_native_10"
    "inner_product_gmp_C_native_11_openmp"
    "inner_product_gmp_kernel_01_orig"
    "inner_product_gmp_kernel_01_mkII"
    "inner_product_gmp_kernel_01_mkIISR"
    "inner_product_gmp_kernel_02_orig"
    "inner_product_gmp_kernel_02_mkII"
    "inner_product_gmp_kernel_02_mkIISR"
    "inner_product_gmp_kernel_03_orig"
    "inner_product_gmp_kernel_03_mkII"
    "inner_product_gmp_kernel_03_mkIISR"
    "inner_product_gmp_kernel_04_orig"
    "inner_product_gmp_kernel_04_mkII"
    "inner_product_gmp_kernel_04_mkIISR"
)
for exe in "${executables[@]}"; do
    COMMAND_LINE="/usr/bin/time ./$exe 100000000 512"
    echo $COMMAND_LINE
    $COMMAND_LINE
    if [ -f gmon.out ]; then
        mv gmon.out "gmon_${exe}.out"
        gprof ./$exe "gmon_${exe}.out" > "gprof_${exe}.txt"
    fi
    echo
done
