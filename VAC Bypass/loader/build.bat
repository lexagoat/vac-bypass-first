@echo off

cd ..
mkdir build\.loader
cd build\.loader
cmake -G "Ninja" ../../loader/
cmake --build .
copy loader.exe ..\loader.exe /Y
pause