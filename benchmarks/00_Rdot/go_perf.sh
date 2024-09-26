uname -a
cat /proc/cpuinfo | grep 'model name' | head -1
echo

FLAMEGRAPH_DIR="/home/docker/FlameGraph"

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

args="100000000 512"
for exe in "${executables[@]}"; do
    echo "Profiling $exe"
    start_time=$(date +%s%N)
    sudo perf record -o perf.data_${exe} -g -- ./$exe $args 
    end_time=$(date +%s%N)
    duration=$(( (end_time - start_time) / 1000000 ))
    echo "Execution time: $duration ms"

    sudo perf script -i perf.data_${exe} > out_${exe}.perf
    cat out_${exe}.perf | $FLAMEGRAPH_DIR/stackcollapse-perf.pl | $FLAMEGRAPH_DIR/flamegraph.pl > flamegraph_${exe}.svg
    echo "Flamegraph for $exe generated."

    sudo perf annotate -i perf.data_${exe} > annotation_${exe}.txt
    echo "Annotation for $exe saved."
    sudo perf report -i perf.data_${exe} --stdio > report_${exe}.txt
    echo "Perf report for $exe generated and saved to report_${exe}.txt."
    echo
done
