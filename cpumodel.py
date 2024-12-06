import re
import subprocess

def get_cpu_model_name():
    try:
        # Run the command and get the output
        result = subprocess.run(
            ["cat", "/proc/cpuinfo"],
            stdout=subprocess.PIPE,
            text=True,
            check=True
        )
        
        # Extract the first 'model name' line
        lines = [line for line in result.stdout.split("\n") if "model name" in line]
        cpu_model_line = lines[0].split(":")[1].strip() if lines else "Unknown CPU Model"

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

        # Replace spaces with underscores
        cpu_model_filename = cpu_model.replace(' ', '_')

        return cpu_model_filename
    except subprocess.CalledProcessError:
        return "Error: Unable to retrieve CPU model"

if __name__ == "__main__":
    cpu_model = get_cpu_model_name()
    print(cpu_model)
