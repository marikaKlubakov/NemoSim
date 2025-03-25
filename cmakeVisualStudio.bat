@echo off
set CMAKE_PATH="c:\Program Files\CMake\bin\cmake.exe"
if not exist %CMAKE_PATH% (
    set CMAKE_PATH="c:\Program Files  (x86)\CMake\bin\cmake.exe"
)
%CMAKE_PATH% ./LIFNetwork -B_build64 -G "Visual Studio 16 2019" -T host=x64

%CMAKE_PATH% --build _build64 --target Nemo --config Debug
:end




