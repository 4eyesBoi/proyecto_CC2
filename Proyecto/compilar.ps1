# Script de Compilación para Algoritmos
# Ejecutar en PowerShell: .\compilar.ps1

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "Compilando Navegador de Algoritmos..." -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# Crear carpeta output si no existe
if (-not (Test-Path "output")) {
    Write-Host "📁 Creando carpeta output..." -ForegroundColor Yellow
    New-Item -ItemType Directory -Path "output" -Force | Out-Null
}

# Ruta a g++
$gppPath = "C:\Familia\David\Programas\Dev-c\Dev-Cpp\MinGW64\bin\g++.exe"

# Verificar que g++ existe
if (-not (Test-Path $gppPath)) {
    Write-Host "❌ ERROR: No se encontró g++ en: $gppPath" -ForegroundColor Red
    Write-Host ""
    Write-Host "Por favor verifica la ruta o instala MinGW" -ForegroundColor Yellow
    Read-Host "Presiona Enter para cerrar"
    exit 1
}

# Compilar
Write-Host "⚙️  Compilando código..." -ForegroundColor Yellow
& $gppPath -std=c++11 -static -static-libgcc -static-libstdc++ -Wall -g src/app.cpp -o output/app.exe -lcomctl32 -lgdi32 -luser32

if ($LASTEXITCODE -eq 0) {
    Write-Host ""
    Write-Host "✅ ¡Compilación exitosa!" -ForegroundColor Green
    Write-Host "📍 Ejecutable: output\app.exe" -ForegroundColor Green
    Write-Host ""
    Write-Host "Para ejecutar: .\output\app.exe" -ForegroundColor Cyan
    Write-Host ""
}
else {
    Write-Host ""
    Write-Host "❌ Error en la compilación" -ForegroundColor Red
    Write-Host ""
    exit 1
}
