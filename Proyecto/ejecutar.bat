@echo off

if not exist "output\app.exe" (
    echo ERROR: El archivo output\app.exe no existe!
    echo Debes compilar primero usando compilar.bat
    pause
    exit /b 1
)

echo Ejecutando programa...
echo.
output\app.exe

pause
