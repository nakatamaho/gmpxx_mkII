#!/usr/bin/env bash
#
# Copyright (c) 2026
#      Nakata, Maho
#      All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
# OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
# OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.

set -euo pipefail

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
repo_dir="$(cd "${script_dir}/.." && pwd)"

build_dir="${1:-${repo_dir}/build_phase5}"
precision="${2:-512}"
rdot_n="${3:-100000}"
raxpy_n="${4:-${rdot_n}}"
rgemv_m="${5:-400}"
rgemv_n="${6:-400}"
rgemm_m="${7:-120}"
rgemm_k="${8:-120}"
rgemm_n="${9:-120}"
output_dir="${10:-${script_dir}/results}"

mkdir -p "${output_dir}"
log_file="${output_dir}/benchmark_$(date +%Y%m%d_%H%M%S).log"

benchmark_dir="${build_dir}/benchmarks"
if [[ ! -d "${benchmark_dir}" ]]; then
    echo "Benchmark directory not found: ${benchmark_dir}" >&2
    echo "Build first: cmake --build ${build_dir} -j" >&2
    exit 1
fi

run_one() {
    local label="$1"
    shift
    local exe="${benchmark_dir}/$1"
    shift

    if [[ ! -x "${exe}" ]]; then
        echo "Executable not found: ${exe}" >&2
        exit 1
    fi

    echo "COMMAND ${label} ${exe} $*"
    /usr/bin/time -f "WALL_SECONDS %e" "${exe}" "$@"
    echo
}

{
    uname -a
    if [[ -r /proc/cpuinfo ]]; then
        grep 'model name' /proc/cpuinfo | head -1
    else
        uname -m
    fi
    echo "BENCHMARK_PARAMS precision=${precision} rdot_n=${rdot_n} raxpy_n=${raxpy_n} rgemv_m=${rgemv_m} rgemv_n=${rgemv_n} rgemm_m=${rgemm_m} rgemm_k=${rgemm_k} rgemm_n=${rgemm_n}"
    echo

    run_one "Rdot native" gmp_native_benchmark_00_Rdot "${rdot_n}" "${precision}"
    run_one "Rdot gmpxx" gmpxx_benchmark_00_Rdot "${rdot_n}" "${precision}"
    run_one "Rdot gmpxx_mkII" gmpxx_mkii_benchmark_00_Rdot "${rdot_n}" "${precision}"

    run_one "Raxpy native" gmp_native_benchmark_01_Raxpy "${raxpy_n}" "${precision}"
    run_one "Raxpy gmpxx" gmpxx_benchmark_01_Raxpy "${raxpy_n}" "${precision}"
    run_one "Raxpy gmpxx_mkII" gmpxx_mkii_benchmark_01_Raxpy "${raxpy_n}" "${precision}"

    run_one "Rgemv native" gmp_native_benchmark_02_Rgemv "${rgemv_m}" "${rgemv_n}" "${precision}"
    run_one "Rgemv gmpxx" gmpxx_benchmark_02_Rgemv "${rgemv_m}" "${rgemv_n}" "${precision}"
    run_one "Rgemv gmpxx_mkII" gmpxx_mkii_benchmark_02_Rgemv "${rgemv_m}" "${rgemv_n}" "${precision}"

    run_one "Rgemm native" gmp_native_benchmark_03_Rgemm "${rgemm_m}" "${rgemm_k}" "${rgemm_n}" "${precision}"
    run_one "Rgemm gmpxx" gmpxx_benchmark_03_Rgemm "${rgemm_m}" "${rgemm_k}" "${rgemm_n}" "${precision}"
    run_one "Rgemm gmpxx_mkII" gmpxx_mkii_benchmark_03_Rgemm "${rgemm_m}" "${rgemm_k}" "${rgemm_n}" "${precision}"
} 2>&1 | tee "${log_file}"

python3 "${script_dir}/plot.py" "${log_file}" --output-dir "${output_dir}"
