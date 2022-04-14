@echo off

cd ..
mkdir build\.bypass
cd build\.bypass
cmake -G "Ninja" ../../bypass/
cmake --build .
copy vac.dll ..\vac.dll /Y
pause