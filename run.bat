@echo off
cd /d %~dp0
cd "Debug"
start ServerPrototype.exe
cd ..
cd "GUI\bin\x86\Debug"
start WpfApp1.exe
cd ..
cd ..
cd ..
cd ..