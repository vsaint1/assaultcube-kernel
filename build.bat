@ECHO off
set ARG=%1
set ARG2=%2

if %ARG% == "driver" AND %ARG2% == "--debug" call driver_debug

if %ARG% == "driver" AND %ARG2% == "--release" call driver_release

:driver_debug
    cmake -S . -B ./build 

    cmake --build ./build --config Debug -j 14

    signtool sign /fd SHA256 bin/Debug/driver.sys

    exit

:driver_release
    cmake -S . -B ./build 

    cmake --build ./build --config Release -j 14 

    signtool sign /fd SHA256 bin/Release/driver.sys

    exit


:usage
    echo "Usage: build.bat <driver|usermode> [--debug|--release]"
    echo "Example: build.bat driver --debug"
    exit
