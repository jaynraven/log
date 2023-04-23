rmdir /S /Q build
rmdir /S /Q install

mkdir build\x86
mkdir build\x64

cd build\x86
cmake -A Win32 ..\..
@REM cmake --build . --config Debug
@REM cmake --build . --config Release
@REM cmake --install .

@REM cd ..\x64
@REM cmake -A x64 ..\..
@REM cmake --build . --config Debug
@REM cmake --build . --config Release
@REM cmake --install .