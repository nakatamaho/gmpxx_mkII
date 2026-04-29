#!/usr/bin/env python3
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

import argparse
import pathlib
import re
import sys

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt


COMMAND_RE = re.compile(r"^COMMAND\s+(\w+)\s+(\w+)\s+(\S+)\s+(.*)$")
MFLOPS_RE = re.compile(r"^MFLOPS:\s+([0-9.eE+-]+)")
PARAM_RE = re.compile(r"(\w+)=([^\s]+)")


def clean_cpu_model(line):
    line = line.replace("model name", "").replace(":", " ")
    line = re.sub(r"\b(AMD|Intel|Threadripper|Processor|CPU)\b", "", line)
    line = line.replace("(R)", "").replace("(TM)", "")
    line = re.sub(r"[^\x00-\x7F]", "", line)
    return re.sub(r"\s+", " ", line).strip() or "unknown_cpu"


def filename_token(text):
    return re.sub(r"[^A-Za-z0-9_.-]+", "_", text).strip("_") or "unknown"


def parse_log(path):
    lines = path.read_text(encoding="utf-8").splitlines()
    os_name = lines[0].split()[0] if lines else "UnknownOS"
    cpu_name = clean_cpu_model(lines[1]) if len(lines) > 1 else "unknown_cpu"
    params = {}
    rows = []
    current = None

    for line in lines:
        if line.startswith("BENCHMARK_PARAMS "):
            params = dict(PARAM_RE.findall(line))
            continue

        match = COMMAND_RE.match(line)
        if match:
            current = {
                "kernel": match.group(1),
                "variant": match.group(2),
                "command": match.group(3),
                "args": match.group(4),
            }
            continue

        match = MFLOPS_RE.match(line)
        if match and current is not None:
            current["mflops"] = float(match.group(1))
            rows.append(current)
            current = None

    return os_name, cpu_name, params, rows


def variant_color(variant):
    if "C_native_openmp" in variant:
        return "dimgray"
    if "C_native" in variant:
        return "gray"
    if variant.endswith("_orig"):
        return "blue"
    if variant.endswith("_mkII"):
        return "green"
    if variant.endswith("_mkII_NOPRECCHANGE"):
        return "red"
    if "openmp" in variant:
        return "orange"
    return "black"


def plot_kernel(rows, kernel, title_suffix, output_base):
    kernel_rows = [row for row in rows if row["kernel"] == kernel]
    if not kernel_rows:
        return

    labels = [row["variant"] for row in kernel_rows]
    values = [row["mflops"] for row in kernel_rows]
    colors = [variant_color(row["variant"]) for row in kernel_rows]

    plt.figure(figsize=(max(9, 0.72 * len(labels)), 6))
    bars = plt.bar(labels, values, color=colors)
    plt.ylabel("MFLOPS", fontsize=13, fontweight="bold")
    plt.title(f"{kernel} benchmark {title_suffix}", fontsize=13, fontweight="bold")
    plt.xticks(rotation=20, ha="right", fontsize=11, fontweight="bold")
    plt.yticks(fontsize=11, fontweight="bold")
    plt.ylim(0, max(values) * 1.15 if max(values) > 0 else 1)

    for bar, value in zip(bars, values):
        plt.text(
            bar.get_x() + bar.get_width() / 2,
            bar.get_height(),
            f"{value:.2f}",
            ha="center",
            va="bottom",
            fontsize=10,
            fontweight="bold",
        )

    plt.tight_layout()
    plt.savefig(f"{output_base}_{kernel}.png", dpi=150)
    plt.savefig(f"{output_base}_{kernel}.pdf")
    plt.close()


def plot_summary(rows, title_suffix, output_base):
    labels = [f"{row['kernel']}\n{row['variant']}" for row in rows]
    values = [row["mflops"] for row in rows]
    colors = [variant_color(row["variant"]) for row in rows]

    plt.figure(figsize=(max(12, 0.58 * len(labels)), 7))
    bars = plt.bar(labels, values, color=colors)

    plt.ylabel("MFLOPS", fontsize=13, fontweight="bold")
    plt.title(f"GMP benchmark summary {title_suffix}", fontsize=13,
              fontweight="bold")
    plt.xticks(rotation=65, ha="right", fontsize=8, fontweight="bold")
    plt.yticks(fontsize=11, fontweight="bold")
    plt.ylim(0, max(values) * 1.15 if max(values) > 0 else 1)
    for bar, value in zip(bars, values):
        plt.text(
            bar.get_x() + bar.get_width() / 2,
            bar.get_height(),
            f"{value:.1f}",
            ha="center",
            va="bottom",
            fontsize=7,
            rotation=90,
        )
    plt.tight_layout()
    plt.savefig(f"{output_base}_summary.png", dpi=150)
    plt.savefig(f"{output_base}_summary.pdf")
    plt.close()


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("logs", nargs="+", type=pathlib.Path)
    parser.add_argument("--output-dir", type=pathlib.Path, default=pathlib.Path("."))
    args = parser.parse_args()

    args.output_dir.mkdir(parents=True, exist_ok=True)
    for log in args.logs:
        os_name, cpu_name, params, rows = parse_log(log)
        if not rows:
            print(f"No benchmark rows found in {log}", file=sys.stderr)
            continue

        precision = params.get("precision", "unknown")
        title_suffix = f"on {cpu_name} (prec={precision})"
        output_base = args.output_dir / (
            f"{log.stem}_{filename_token(os_name)}_{filename_token(cpu_name)}"
        )

        plot_summary(rows, title_suffix, output_base)
        for kernel in ["Rdot", "Raxpy", "Rgemv", "Rgemm"]:
            plot_kernel(rows, kernel, title_suffix, output_base)


if __name__ == "__main__":
    main()
