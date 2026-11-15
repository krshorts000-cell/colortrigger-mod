@echo off
:: Quick build script for Color Trigger Generator mod
:: Run from the project root directory

set BUILD_DIR=build
if not exist %BUILD_DIR% mkdir %BUILD_DIR%
cd %BUILD_DIR%

cmake .. -G "Visual Studio 17 2022" -A x86 ^
    -DCMAKE_BUILD_TYPE=Release ^
    -DGEODE_SDK="%GEODE_SDK%"

cmake --build . --config Release --parallel %NUMBER_OF_PROCESSORS%

echo.
echo Build complete. .geode file is in: %BUILD_DIR%\RelWithDebInfo\
pause
