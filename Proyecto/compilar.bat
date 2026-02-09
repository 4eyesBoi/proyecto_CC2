@echo off
setlocal enabledelayedexpansion

REM Crear carpeta output si no existe
if not exist "output" mkdir output

REM Compilar - usando la ruta completa a g++
echo Compilando proyecto...
"C:\Familia\David\Programas\Dev-c\Dev-Cpp\MinGW64\bin\g++.exe" -std=c++11 -static -static-libgcc -static-libstdc++ -Wall -g src/app.cpp -o output/app.exe -lcomctl32 -lgdi32 -luser32

if errorlevel 1 (
    echo.
    echo ERROR: Compilacion fallida!
    echo.
    pause
    exit /b 1
)

echo.
echo Compilacion exitosa!
echo Ejecutable creado en: output\app.exe
echo.
pause
