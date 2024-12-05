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

    # Extract OS from the first word of lines[0]
    os_name = lines[0].strip().split()[0] if lines else "Unknown OS"

    # Extract CPU model and execution parameters from the log file
    cpu_model_line = lines[1].strip() if len(lines) > 1 else "Unknown CPU Model"

    dimension_line = lines[3].strip() if len(lines) > 3 else None
    # Extract dimensions and precision
    if dimension_line:
        parts = dimension_line.split()
        if len(parts) >= 2:
            dim = parts[-2]  # Second to last element
            dimx = parts[-3]  # Second to last element
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

    # Prepare OS and CPU model for filename usage
    os_cleaned = os_name.replace(' ', '_')  # Replace spaces with underscores in OS name
    cpu_model_cleaned = cpu_model.replace(' ', '_')  # Replace spaces with underscores in CPU model

    os_cpu_model_filename = f"{os_cleaned}_{cpu_model_cleaned}"  # Combine OS and CPU model

    # Define the pattern to extract operations and their elapsed times
    pattern = re.compile(
    r'/usr/bin/time\s+(\./Rgemv_gmp_\w+)\s+\d+\s+\d+\s+\d+\n'
    r'Elapsed time:\s+([\d.]+)\s+s\n'
    r'MFLOPS:\s+([\d.]+)',
    re.MULTILINE
    )

    # Extract data using the defined pattern
    data = pattern.findall(''.join(lines))

    # Organize data, removing './Rgemv_gmp_' from the operation names
    operations = [op.replace('./Rgemv_gmp_', '') for op, _, _ in data]
    times = [float(time) for _, time, _ in data]
    flops = [float(flops) for _, _, flops in data]

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
    openmp_flops = [flops[i] for i, op in enumerate(operations) if 'openmp' in op]

    openmp_colors = []
    for op in openmp_operations:
        if 'mkIISR' in op:
            openmp_colors.append('red')
        elif 'mkII' in op:
            openmp_colors.append('green')
        elif 'orig' in op:
            openmp_colors.append('blue')
        else:
            openmp_colors.append('gray')  # Default color for operations that do not match any condition

    # Filter data for operations that do not contain "openmp"
    singlecore_operations = [op for op in operations if 'openmp' not in op]
    singlecore_times = [times[i] for i, op in enumerate(operations) if 'openmp' not in op]
    singlecore_flops = [flops[i] for i, op in enumerate(operations) if 'openmp' not in op]

    # Determine colors for singlecore operations
    singlecore_colors = []
    for op in singlecore_operations:
        if 'mkIISR' in op:
            singlecore_colors.append('red')
        elif 'mkII' in op:
            singlecore_colors.append('green')
        elif 'orig' in op:
            singlecore_colors.append('blue')
        else:
            singlecore_colors.append('gray')  # Default color for operations that do not match any condition

    # Plotting singlecore operations
    plt.figure(figsize=(15, 8))
    bars = plt.bar(singlecore_operations, singlecore_flops, color=singlecore_colors)

    plt.xlabel('Operation', fontsize=14, fontweight='bold')
    plt.ylabel('MFLOPS', fontsize=16, fontweight='bold')
    formatted_dim = "{:,}".format(int(dim))
    formatted_dimx = "{:,}".format(int(dimx))
    plt.title(f'MFLOPS for Various GMP Operations on {cpu_model} (dim=({formatted_dimx}, {formatted_dim}), prec={prec})', fontsize=16, fontweight='bold')
    plt.xticks(rotation=55, fontsize=12, fontweight='bold', ha='right')
    plt.yticks(fontsize=12, fontweight='bold')
    plt.ylim(0, max(singlecore_flops) * 1.1)

    for bar, flop in zip(bars, singlecore_flops):
        yval = bar.get_height()
        plt.text(bar.get_x() + bar.get_width()/2, yval, f"{flop:.1f}", ha='center', va='bottom', fontsize=16, fontweight='bold')

    # Adjust the subplot parameters to give more space at the bottom
    plt.subplots_adjust(bottom=0.4, right=0.75)

    # Add legend bars on the right side
    legend_labels = ['native C', 'orig(gmpxx.h)', 'mkII(gmpxx_mkII.h)', 'mkIISR(gmpxx_mkII.h)']
    legend_colors = ['gray', 'blue', 'green', 'red' ]
    for color, label in zip(legend_colors, legend_labels):
        plt.plot([], [], color=color, label=label, linewidth=10)
    legend = plt.legend(loc='center left', bbox_to_anchor=(1, 0.5), fontsize=12, frameon=False)
    for text in legend.get_texts():
        text.set_fontweight('bold')

    filename = f'singlecore_operations_{os_cpu_model_filename}_{dimx}_{dim}_{prec}.pdf'
    plt.savefig(filename)
    filename = f'singlecore_operations_{os_cpu_model_filename}_{dimx}_{dim}_{prec}.png'
    plt.savefig(filename)
    plt.close()

    # Plotting only "openmp" operations if any exist
    if openmp_operations:
        plt.figure(figsize=(17, 9))
        openmp_bars = plt.bar(openmp_operations, openmp_flops, color=openmp_colors)

        plt.bar(openmp_operations, openmp_flops, color=openmp_colors)
        plt.xlabel('Operation', fontsize=14, fontweight='bold')
        plt.ylabel('MFLOPS', fontsize=17, fontweight='bold')
        plt.title(f'MFLOPS for OpenMP GMP Operations on {cpu_model} (dim=({formatted_dimx}, {formatted_dim}), prec={prec})', fontsize=16, fontweight='bold')
        plt.xticks(rotation=55, fontsize=12, fontweight='bold', ha='right')
        plt.yticks(fontsize=12, fontweight='bold')
        plt.ylim(0, max(openmp_flops) * 1.1)
        for bar, flop in zip(openmp_bars, openmp_flops):
            yval = bar.get_height()
            plt.text(bar.get_x() + bar.get_width()/2, yval, f"{flop:.1f}", ha='center', va='bottom', fontsize=15, fontweight='bold')

        plt.subplots_adjust(bottom=0.4, right=0.75)
        for color, label in zip(legend_colors, legend_labels):
            plt.plot([], [], color=color, label=label, linewidth=10)

        legend = plt.legend(loc='center left', bbox_to_anchor=(1, 0.5), fontsize=14, frameon=False)
        for text in legend.get_texts():
            text.set_fontweight('bold')

        filename_openmp = f'openmp_operations_{os_cpu_model_filename}_{dimx}_{dim}_{prec}.pdf'
        plt.savefig(filename_openmp, bbox_inches='tight')
        filename_openmp = f'openmp_operations_{os_cpu_model_filename}_{dimx}_{dim}_{prec}.png'
        plt.savefig(filename_openmp, bbox_inches='tight')
        plt.close()
