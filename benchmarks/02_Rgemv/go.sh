uname -a
cat /proc/cpuinfo | grep 'model name' | head -1
echo
executables=(
    "Rgemv_gmp_C_native_01"
    "Rgemv_gmp_C_native_openmp_01"
    "Rgemv_gmp_kernel_01_orig"
    "Rgemv_gmp_kernel_01_mkII"
    "Rgemv_gmp_kernel_01_mkIISR"
    "Rgemv_gmp_kernel_openmp_01_orig"
    "Rgemv_gmp_kernel_openmp_01_mkII"
    "Rgemv_gmp_kernel_openmp_01_mkIISR"
)
for exe in "${executables[@]}"; do
    COMMAND_LINE="/usr/bin/time ./$exe 4000 4000 512"
    echo $COMMAND_LINE
    $COMMAND_LINE
    if [ -f gmon.out ]; then
        mv gmon.out "gmon_${exe}.out"
        gprof ./$exe "gmon_${exe}.out" > "gprof_${exe}.txt"
    fi
    echo
done

