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
    content = file.read()

# Define the pattern to extract operations and their elapsed times
pattern = re.compile(r'(\\./inner_product_gmp_\\d+_[\\w]+) \\d+ \\d+\\nElapsed time: ([\\d.]+) s')

# Extract data using the defined pattern
data = pattern.findall(content)

# Organize data, removing 'inner_product' from the operation names
operations = [op.replace('inner_product_', '') for op, _ in data]
times = [float(time) for _, time in data]

# Plotting
plt.figure(figsize=(14, 8))
plt.bar(operations, times, color='lightblue')
plt.xlabel('Operation')
plt.ylabel('Elapsed Time (s)')
plt.title('Elapsed Time for Various GMP Operations in Inner Product Calculations')
plt.xticks(rotation=90)
plt.tight_layout()

# Save the plot to a PDF file
plt.savefig('elapsed_times_chart.pdf')
plt.close()
