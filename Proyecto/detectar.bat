@echo off
REM Script para detectar automáticamente dónde está g++
REM y crear un archivo de configuración

setlocal enabledelayedexpansion

echo.
echo ========================================
echo Detectando ubicacion de g++...
echo ========================================
echo.

REM Rutas comunes donde puede estar g++
set "rutas[0]=C:\Familia\David\Programas\Dev-c\Dev-Cpp\MinGW64\bin\g++.exe"
set "rutas[1]=C:\Program Files\mingw-w64\x86_64-8.1-posix-seh-rt_v6-rev0\mingw64\bin\g++.exe"
set "rutas[2]=C:\Program Files (x86)\MinGW\bin\g++.exe"
set "rutas[3]=C:\MinGW\bin\g++.exe"
set "rutas[4]=C:\msys64\mingw64\bin\g++.exe"
set "rutas[5]=C:\dev-cpp\MinGW64\bin\g++.exe"

set "encontrado=0"
set "ruta_correcta="

for /l %%i in (0,1,5) do (
    if exist "!rutas[%%i]!" (
        echo ✓ Encontrado g++ en: !rutas[%%i]!
        set "ruta_correcta=!rutas[%%i]!"
        set "encontrado=1"
        goto done
    )
)

:done
if !encontrado! equ 1 (
    echo.
    echo ✅ g++ detectado correctamente
    echo Ruta: !ruta_correcta!
    echo.
    pause
) else (
    echo.
    echo ❌ No se encontró g++ en las rutas comunes
    echo.
    echo Por favor:
    echo 1. Abre una terminal de administrador
    echo 2. Ejecuta: where g++
    echo 3. Copia la ruta que aparece
    echo 4. Abre compilar.bat con un editor
    echo 5. Reemplaza la ruta en el comando g++
    echo.
    pause
    exit /b 1
)
