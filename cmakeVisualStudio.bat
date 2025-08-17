@echo off
@echo off
set CMAKE_PATH="c:\Program Files\CMake\bin\cmake.exe"
if not exist %CMAKE_PATH% (
    set CMAKE_PATH="c:\Program Files (x86)\CMake\bin\cmake.exe"
)

rem Generate Visual Studio solution with the correct executable name
%CMAKE_PATH% ./ -B_build64 -G "Visual Studio 17 2022" -T host=x64

rem Build the NEMOSIM executable
%CMAKE_PATH% --build _build64 --target NEMOSIM --config Release
:end
