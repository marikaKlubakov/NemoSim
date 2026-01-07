#!/bin/csh

# Always use the script location as base (repo root)
set BASE_DIR = "$0:h"
cd "$BASE_DIR"
set BASE_DIR = `pwd`

pwd
ls -l

source cmakeLinux.csh

# Paths
set OUTPUT_DIR = "$BASE_DIR/_Build64"
set TESTS_DIR  = "$BASE_DIR/Tests"
set DOC_DIR    = "$BASE_DIR/Doc"
set SRC_DIR    = "$BASE_DIR/Src"

# Get date in YYMMDD format
set DATESTAMP = `date "+%y%m%d"`

# Zip name
set ZIP_NAME = "NemoSim_${DATESTAMP}.zip"
set ZIP_PATH = "$BASE_DIR/$ZIP_NAME"

# Clean old zip
if (-e "$ZIP_PATH") then
    rm -f "$ZIP_PATH"
endif

# Temp package folder
set TEMP_DIR = "$BASE_DIR/temp_package"
if (-d "$TEMP_DIR") then
    rm -rf "$TEMP_DIR"
endif
mkdir -p "$TEMP_DIR"

# Copy files to package
cp "$OUTPUT_DIR/NEMOSIM" "$TEMP_DIR/"
cp "$DOC_DIR/readme.txt" "$TEMP_DIR/"
cp "$SRC_DIR/Common/tinyxml2.h" "$TEMP_DIR/"
cp "$SRC_DIR/LIFNetwork/plot_vm_to_dt.py" "$TEMP_DIR/"
cp "$SRC_DIR/LIFNetwork/input_creator.py" "$TEMP_DIR/"
cp "$SRC_DIR/BIUNetwork/plot_vn_to_dt.py" "$TEMP_DIR/"

# Copy Tests excluding postBuildTester.py
mkdir -p "$TEMP_DIR/Tests"
rsync -av --exclude='postBuildTester.py' "$TESTS_DIR/" "$TEMP_DIR/Tests/"

# Create zip
cd "$TEMP_DIR"
/usr/bin/zip -r "$ZIP_PATH" *

# Clean up
cd "$BASE_DIR"
rm -rf "$TEMP_DIR"

echo "Package created at: $ZIP_PATH"
