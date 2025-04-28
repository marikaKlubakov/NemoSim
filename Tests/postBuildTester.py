import subprocess
import sys
import os

# Define the path to the executable
curr_WD = os.getcwd()
exe_path = os.path.join(os.getcwd(), "..\\LIFNetworkSimulation.exe")

print("Start")

# Hard-coded list of XML files
xml_files = [
    "../Tests/SNN/LIF/test1/test.xml",
    "../Tests/SNN/LIF/test2/test.xml"
    # Add more files as needed
]

# Expected output files
output_files = ["Iins.txt", "vms.txt", "Vouts.txt"]

build_should_fail = False  # Track if comparisons fail

# Iterate over the XML files and run the executable for each file
for xml_path in xml_files:
    working_directory = os.path.dirname(xml_path)
    xml_file = os.path.basename(xml_path)

    os.chdir(working_directory)
    print("Current working directory:", os.getcwd())

    print(f"Preparing to run executable for {xml_file}...")
    try:
        result = subprocess.run([exe_path, xml_file, "input.txt"], capture_output=True, text=True)
        print(f"Finished running {xml_file}:")
        print("Output:", result.stdout)
        print("Error:", result.stderr)
    except subprocess.TimeoutExpired:
        print(f"Execution for {xml_file} timed out.")
        continue
    except Exception as e:
        print(f"An error occurred: {e}")
        continue

    print("-" * 40)

    # Compare each output file with its reference file
    for output_file in output_files:
        ref_file = output_file.replace(".txt", "_ref.txt")  # Reference file name

        if os.path.exists(output_file) and os.path.exists(ref_file):
            print(f"Comparing {output_file} with {ref_file}...")
            with open(output_file, "r") as out_f, open(ref_file, "r") as ref_f:
                out_lines = out_f.readlines()
                ref_lines = ref_f.readlines()

            differences = [i for i in range(min(len(out_lines), len(ref_lines))) if out_lines[i] != ref_lines[i]]

            if differences:
                print(f"Differences found in {output_file}:")
                for i in differences:
                    print(f"Line {i+1}:\nOutput: {out_lines[i].strip()}\nExpected: {ref_lines[i].strip()}\n")
                build_should_fail = True  # Flag the build to fail
            else:
                print(f"{output_file} matches reference file.")
        else:
            print(f"Skipping comparison: {output_file} or {ref_file} missing.")
            build_should_fail = True  # Fail if expected files are missing

    os.chdir(curr_WD)

print("All tasks completed.")

if build_should_fail:
    print("Build failed due to output mismatches.")
    sys.exit(1)  # Make build fail
else:
    print("Post-build test completed successfully.")
    sys.exit(0)  # Ensure clean exit when everything matches
