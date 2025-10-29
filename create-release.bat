@echo off
setlocal
cd
dir
call "./cmakeVisualStudio.bat"
:: Set paths
set OUTPUT_DIR=%~dp0/_build64/Release
set TESTS_DIR=%~dp0Tests

:: Get date in DDMMYY format
for /f %%i in ('powershell -NoProfile -Command "Get-Date -Format yyMMdd"') do set DATESTAMP=%%i

:: Set ZIP name with date
set ZIP_NAME=NemoSim_%DATESTAMP%.zip
set ZIP_PATH=%~dp0%ZIP_NAME%


:: Clean up old zip if exists
if exist "%ZIP_PATH%" del "%ZIP_PATH%"

:: Create a temporary folder for packaging
set TEMP_DIR=%~dp0temp_package
if exist "%TEMP_DIR%" rd /s /q "%TEMP_DIR%"
mkdir "%TEMP_DIR%"

:: Copy files to package
copy "%OUTPUT_DIR%\NEMOSIM.exe" "%TEMP_DIR%"
::copy "%~dp0\Src\Doc\readme.txt" "%TEMP_DIR%"
copy "%~dp0\Src\Common\tinyxml2.h" "%TEMP_DIR%"
copy "%~dp0\Src\LIFNetwork\plot_vm_to_dt.py" "%TEMP_DIR%"
copy "%~dp0\Src\LIFNetwork\input_creator.py" "%TEMP_DIR%"
copy "%~dp0\Src\BIUNetwork\plot_vn_to_dt.py" "%TEMP_DIR%"

robocopy "%TESTS_DIR%" "%TEMP_DIR%\Tests" /E
del /f /q "%TEMP_DIR%\Tests\postBuildTester.py"


:: Create zip file (requires PowerShell)
powershell -Command "Compress-Archive -Path '%TEMP_DIR%\\*' -DestinationPath '%ZIP_PATH%'"

:: Clean up
rd /s /q "%TEMP_DIR%"

echo Package created at: %ZIP_PATH%
endlocal
