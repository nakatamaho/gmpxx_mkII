import re
import matplotlib.pyplot as plt
import sys

# Check if the command line argument for the file path is provided
if len(sys.argv) < 2:
    print("Usage: python script_name.py path_to_log_file")
    sys.exit(1)

# Path to the log file from the command line argument
file_path = sys.argv[1]

# Read file contents
with open(file_path, 'r') as file:
    lines = file.readlines()

# Extract CPU model and execution parameters from the log file
cpu_model_line = lines[1].strip() if len(lines) > 1 else "Unknown CPU Model"
dimension_line = lines[3].strip() if len(lines) > 3 else None

# Extract dimensions and precision
if dimension_line:
    parts = dimension_line.split()
    if len(parts) >= 2:
        dim = parts[-2]  # Second to last element
        prec = parts[-1]  # Last element
        dim = f"dim={dim}"
        prec = f"prec={prec}bit"
    else:
        print("Error: Dimension and precision data is malformed or not enough elements.")
        sys.exit(1)
else:
    print("Error: No dimension and precision data found.")
    sys.exit(1)

# Clean up the model name and remove non-ASCII characters
cpu_model = re.sub(r'\b(AMD|Intel|Threadripper|\(TM\)|\(R\)|Processor)\b', '', cpu_model_line).strip()
cpu_model = re.sub(r'[^\x00-\x7F]', '', cpu_model)  # Remove non-ASCII characters
cpu_model = re.sub(r'\s+', ' ', cpu_model).strip()  # Replace multiple spaces with a single space

# Define the pattern to extract operations and their elapsed times
pattern = re.compile(r'(\./inner_product_gmp_\d+_\w+) \d+ \d+\nElapsed time: ([\d.]+) s')

# Extract data using the defined pattern
data = pattern.findall(''.join(lines))

# Organize data, removing 'inner_product' from the operation names
operations = [op.replace('inner_product_', '') for op, _ in data]
times = [float(time) for _, time in data]

# Determine colors based on operation types
colors = []
for op in operations:
    if 'mkIISR' in op:
        colors.append('red')
    elif 'mkII' in op:
        colors.append('green')
    elif 'orig' in op:
        colors.append('blue')
    else:
        colors.append('gray')  # Default color for operations that do not match any condition

# Filter data for "openmp" operations
openmp_operations = [op for op in operations if 'openmp' in op]
openmp_times = [times[i] for i, op in enumerate(operations) if 'openmp' in op]

# Plotting all operations
plt.figure(figsize=(14, 8))
bars = plt.bar(operations, times, color=colors)
plt.xlabel('Operation')
plt.ylabel('Elapsed Time (s)')
plt.title(f'Elapsed Time for Various GMP Operations on {cpu_model} ({dim}, {prec})')
plt.xticks(rotation=90)
for bar, time in zip(bars, times):
    yval = bar.get_height()
    plt.text(bar.get_x() + bar.get_width()/2, yval, round(time, 2), ha='center', va='bottom')
plt.tight_layout()
plt.savefig('all_operations_elapsed_times_chart.pdf')
plt.close()

# Plotting only "openmp" operations if any exist
if openmp_operations:
    plt.figure(figsize=(10, 6))
    plt.bar(openmp_operations, openmp_times, color=colors)
    plt.xlabel('Operation')
    plt.ylabel('Elapsed Time (s)')
    plt.title(f'Elapsed Time for OpenMP GMP Operations on {cpu_model} ({dim}, {prec})')
    plt.xticks(rotation=90)
    plt.tight_layout()
    plt.savefig('openmp_operations_elapsed_times_chart.pdf')
    plt.close()
