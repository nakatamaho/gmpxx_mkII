import re
import matplotlib.pyplot as plt
import sys
import glob  # Import the glob module to handle file patterns

# Check if the command line argument for the file path pattern is provided
if len(sys.argv) < 2:
    print("Usage: python script_name.py path_to_log_files_pattern")
    sys.exit(1)

# Read all file paths from command line arguments
file_paths = sys.argv[1:]

print(f"Files to process: {file_paths}")

# You can loop through file_paths to process each file.
for file_path in file_paths:
    print(f"Processing file: {file_path}")
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
        else:
            print("Error: Dimension and precision data is malformed or not enough elements.")
            sys.exit(1)
    else:
        print("Error: No dimension and precision data found.")
        sys.exit(1)

    # Clean up the model name and remove non-ASCII characters
    cpu_model = re.sub(r'\b(AMD|Intel|Threadripper|\(TM\)|\(R\)|Processor)\b', '', cpu_model_line).strip()
    cpu_model = re.sub(r'[^\x00-\x7F]', '', cpu_model)  # Remove non-ASCII characters
    cpu_model = cpu_model.replace('model name', '').strip()  # Remove 'model name:' prefix after regex clean-up
    cpu_model = re.sub(r'\d+(th|rd|st|nd)\b', '', cpu_model)  # Remove ordinal suffixes like 13th, 2nd, etc.
    cpu_model = cpu_model.replace('(R)', '').strip()
    cpu_model = cpu_model.replace('(TM)', '').strip()
    cpu_model = cpu_model.replace('CPU', '').strip()
    cpu_model = cpu_model.replace('Gen', '').strip()
    cpu_model = re.sub(r':', '', cpu_model)  # Remove :
    cpu_model = re.sub(r'\s+', ' ', cpu_model).strip()  # Replace multiple spaces with a single space

    cpu_model_filename = cpu_model.replace(' ', '_')  # Prepare CPU model for filename usage

    # Define the pattern to extract operations and their elapsed times
    pattern = re.compile(r'(\./inner_product_gmp_\d+_\w+) \d+ \d+\nElapsed time: ([\d.]+) s')

    # Extract data using the defined pattern
    data = pattern.findall(''.join(lines))

    # Organize data, removing 'inner_product' from the operation names
    operations = [op.replace('./inner_product_gmp_', '') for op, _ in data]
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

    plt.xlabel('Operation', fontsize=14, fontweight='bold')
    plt.ylabel('Elapsed Time (s)', fontsize=14, fontweight='bold')
    plt.title(f'Elapsed Time for Various GMP Operations on {cpu_model} (dim={dim}, prec={prec})', fontsize=16, fontweight='bold')
    plt.xticks(rotation=60, fontsize=12, fontweight='bold')
    plt.yticks(fontsize=12, fontweight='bold', ha='right')

    for bar, time in zip(bars, times):
        yval = bar.get_height()
        plt.text(bar.get_x() + bar.get_width()/2, yval, round(time, 2), ha='center', va='bottom', fontsize=16,fontweight='bold')
    plt.tight_layout()
    filename = f'all_operations_{cpu_model_filename}_{dim}_{prec}.pdf'
    plt.savefig(filename)
    plt.close()

    # Plotting only "openmp" operations if any exist
    if openmp_operations:
        plt.figure(figsize=(12, 6))
        plt.bar(openmp_operations, openmp_times, color=colors)
        plt.xlabel('Operation', fontsize=14, fontweight='bold')
        plt.ylabel('Elapsed Time (s)', fontsize=14, fontweight='bold')
        plt.title(f'Elapsed Time for OpenMP GMP Operations on {cpu_model} (dim={dim}, prec={prec})', fontsize=16, fontweight='bold')
        plt.xticks(rotation=90, fontsize=12, fontweight='bold')
        plt.yticks(fontsize=12, fontweight='bold')

        plt.tight_layout()
        filename_openmp = f'openmp_operations_{cpu_model_filename}_{dim}_{prec}.pdf'
        plt.savefig(filename_openmp)
        plt.close()
