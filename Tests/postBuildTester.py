import subprocess

import os

# Define the path to the executable
exe_path = os.path.join(os.getcwd(),"..\\LIFNetworkSimulation.exe")
print(f"start")
# Hard-code the list of XML files
xml_files = [
    "./SNN/LIF/test1.xml"
    # Add more files as needed
]

# Iterate over the XML files and run the executable for each file
for xml_path in xml_files:
    working_directory = os.path.dirname(xml_path)
    xml_file = os.path.basename(xml_path)
    os.chdir(working_directory)

    print("Current working directory:", os.getcwd())

    print(f"Preparing to run executable for {xml_file}...")
    try:
        result = subprocess.run([exe_path, xml_file])
        print(f"Finished running {xml_file}:")
        print("Output:", result.stdout)
        print("Error:", result.stderr)
    except subprocess.TimeoutExpired:
        print(f"Execution for {xml_file} timed out.")
    except Exception as e:
        print(f"An error occurred: {e}")
    print("-" * 40)

print("All tasks completed.")
