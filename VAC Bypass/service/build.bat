@echo off

cd ..
mkdir build\.service
cd build\.service
cmake -G "Ninja" ../../service/
cmake --build .
copy service.exe ..\service.exe /Y
pause