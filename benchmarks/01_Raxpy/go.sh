uname -a
cat /proc/cpuinfo | grep 'model name' | head -1
echo
executables=(
    "Raxpy_gmp_C_native_01"
    "Raxpy_gmp_C_native_openmp_01"
    "Raxpy_gmp_kernel_01_orig"
    "Raxpy_gmp_kernel_01_mkII"
    "Raxpy_gmp_kernel_01_mkIISR"
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

