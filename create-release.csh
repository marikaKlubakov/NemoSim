#!/bin/csh

# Get the directory of the script
set BASE_DIR = `pwd`

# Set paths
set OUTPUT_DIR = "$BASE_DIR/build"
set TESTS_DIR = "$BASE_DIR/Tests"

# Get date in YYMMDD format
set DATESTAMP = `date "+%y%m%d"`

# Set ZIP name with date
set ZIP_NAME = "NemoSim_${DATESTAMP}.zip"
set ZIP_PATH = "$BASE_DIR/$ZIP_NAME"

# Clean up old zip if exists
if (-e "$ZIP_PATH") then
    rm -f "$ZIP_PATH"
endif

# Create a temporary folder for packaging
set TEMP_DIR = "$BASE_DIR/temp_package"
if (-d "$TEMP_DIR") then
    rm -rf "$TEMP_DIR"
endif
mkdir -p "$TEMP_DIR"

# Copy files to package
cp "$OUTPUT_DIR/NEMOSIM" "$TEMP_DIR/"
cp "$BASE_DIR/Doc/readme.txt" "$TEMP_DIR/"
cp "$BASE_DIR/tinyxml2.h" "$TEMP_DIR/"
cp "$BASE_DIR/LIFNetwork/plot_vm_to_dt.py" "$TEMP_DIR/"
cp "$BASE_DIR/LIFNetwork/input_creator.py" "$TEMP_DIR/"
cp "$BASE_DIR/BIUNetwork/plot_vn_to_dt.py" "$TEMP_DIR/"

# Copy Tests directory excluding postBuildTester.py
mkdir -p "$TEMP_DIR/Tests"
rsync -av --exclude='postBuildTester.py' "$TESTS_DIR/" "$TEMP_DIR/Tests/"

# Create zip file
cd "$TEMP_DIR"
/usr/bin/zip -r "$ZIP_PATH" *

# Clean up
cd "$BASE_DIR"
rm -rf "$TEMP_DIR"

echo "Package created at: $ZIP_PATH"
