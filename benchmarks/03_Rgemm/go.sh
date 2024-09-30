uname -a
cat /proc/cpuinfo | grep 'model name' | head -1
echo
executables=(
    "Rgemm_gmp_C_native_01"
    "Rgemm_gmp_C_native_02"
    "Rgemm_gmp_C_native_openmp_01"
    "Rgemm_gmp_C_native_openmp_02"
    "Rgemm_gmp_kernel_01_orig"
    "Rgemm_gmp_kernel_01_mkII"
    "Rgemm_gmp_kernel_01_mkIISR"
    "Rgemm_gmp_kernel_02_orig"
    "Rgemm_gmp_kernel_02_mkII"
    "Rgemm_gmp_kernel_02_mkIISR"
    "Rgemm_gmp_kernel_openmp_01_orig"
    "Rgemm_gmp_kernel_openmp_01_mkII"
    "Rgemm_gmp_kernel_openmp_01_mkIISR"
    "Rgemm_gmp_kernel_openmp_02_orig"
    "Rgemm_gmp_kernel_openmp_02_mkII"
    "Rgemm_gmp_kernel_openmp_02_mkIISR"
    "Rgemm_gmp_kernel_openmp_03_orig"
    "Rgemm_gmp_kernel_openmp_03_mkII"
    "Rgemm_gmp_kernel_openmp_03_mkIISR"
)
for exe in "${executables[@]}"; do
    COMMAND_LINE="/usr/bin/time ./$exe 500 500 500 512"
    echo $COMMAND_LINE
    $COMMAND_LINE
    if [ -f gmon.out ]; then
        mv gmon.out "gmon_${exe}.out"
        gprof ./$exe "gmon_${exe}.out" > "gprof_${exe}.txt"
    fi
    echo
done

