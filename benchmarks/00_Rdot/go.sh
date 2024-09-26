uname -a
cat /proc/cpuinfo | grep 'model name' | head -1
echo
executables=(
    "Rdot_gmp_C_native_01"
    "Rdot_gmp_C_native_openmp_01"
    "Rdot_gmp_kernel_01_orig"
    "Rdot_gmp_kernel_01_mkII"
    "Rdot_gmp_kernel_01_mkIISR"
    "Rdot_gmp_kernel_02_orig"
    "Rdot_gmp_kernel_02_mkII"
    "Rdot_gmp_kernel_02_mkIISR"
    "Rdot_gmp_kernel_03_orig"
    "Rdot_gmp_kernel_03_mkII"
    "Rdot_gmp_kernel_03_mkIISR"
    "Rdot_gmp_kernel_04_orig"
    "Rdot_gmp_kernel_04_mkII"
    "Rdot_gmp_kernel_04_mkIISR"
    "Rdot_gmp_kernel_openmp_01_orig"
    "Rdot_gmp_kernel_openmp_01_mkII"
    "Rdot_gmp_kernel_openmp_01_mkIISR"
    "Rdot_gmp_kernel_openmp_02_orig"
    "Rdot_gmp_kernel_openmp_02_mkII"
    "Rdot_gmp_kernel_openmp_02_mkIISR"
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
