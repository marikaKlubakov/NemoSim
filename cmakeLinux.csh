
# Set the full paths to the compilers


# Navigate to the project directory

set tinyxml2_dir = "/path/to/tinyxml2"

# Create a build directory if it doesn't exist

mkdir build

# Navigate to the build directory
cd build

# Run CMake to configure the project
cmake -DCMAKE_C_COMPILER=$CC -DCMAKE_CXX_COMPILER=$CXX ..

make
